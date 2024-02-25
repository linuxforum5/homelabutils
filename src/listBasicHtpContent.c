/**
 * List homelab BASIC .htp file
 * HTP fájlformátum
 * - 256 x 0x00
 * - 0xA5
 * - name with ending 0 character max 64 bytes length
 * - start address 2 bytes ( low, high ordered )
 * - length 2 bytes ( low, high ordered )
 * - data with 1 byte crc sum code
 * - 1 byte crc
 * - 0x00
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "getopt.h"
#include <libgen.h>
// #include "lib/fs.h"
// #include "lib/htp.h"
// #include "lib/basic.h"
// #include "lib/params.h"
#include "h3/tokens.h"

#define VM 0
#define VS 3
#define VB 'b'

#define BASIC_START 0x40A0
#define MAX_LINE_LENGTH 1000

int verbose = 0;
int force = 0;
int space = 0;
/********************************************************************************************************************
 * HTP functions
 ********************************************************************************************************************/
unsigned char crc = 0;

typedef struct {
    uint16_t HM;              // 0x4016
    uint16_t end_of_bas;      // 0x4018
    uint16_t end_of_var;      // 0x401A
    uint16_t start_of_bas;    // 0x4030
} DCB;

unsigned char read_byte( FILE *input ) {
    unsigned char c = fgetc( input );
    crc += c;
    return c;
}

uint16_t read_word( FILE *input ) { return read_byte( input ) + 256 * read_byte( input ); }
uint16_t read_word2( FILE *input ) { return read_byte( input ) *256 + read_byte( input ); }

DCB read_DCB( FILE *input ) {
    DCB dcb;
    unsigned long pos = ftell( input );
    if ( verbose ) printf( "Input file pos of DCB start: %04X\n", pos );
    dcb.HM = read_word( input );
    if ( verbose ) printf( "DCB.HM = %04X\n", dcb.HM );
    dcb.end_of_bas = read_word( input );
    if ( verbose ) printf( "DCB.end_of_bas = %04X\n", dcb.end_of_bas );
    dcb.end_of_var = read_word( input );
    if ( verbose ) printf( "DCB.end_of_var = %04X\n", dcb.end_of_var );
    int addr;
    for( addr=0x401C; addr<0x4030; addr+=2 ) read_word( input );
    dcb.start_of_bas = read_word( input );
    if ( verbose ) printf( "DCB.start_of_bas = %04X\n", dcb.start_of_bas );
    for( addr += 2; addr < 0x40A0; addr++ ) read_byte( input );
    for( ; addr < dcb.start_of_bas; addr++ ) read_byte( input ); // SKIP SYSTEM
    pos = ftell( input );
    if ( verbose ) printf( "Input file pos after DCB end: %04X\n", pos );
    return dcb;
}

void print_hexout( FILE* hexout, uint16_t line_number, FILE* input ) {
    fprintf( hexout, "%d ", line_number );
    unsigned long pos = ftell( input );
    for( unsigned char c=fgetc( input ); c!= 0x60; c=fgetc( input ) ) {
        fprintf( hexout, "%02X ", c );
    }
    fprintf( hexout, "\n" );
    fseek( input, pos, SEEK_SET );
}

int flushVar( FILE *output, int varType ) {
    if ( varType == 2 ) fprintf( output, "%c", '$' );
    return 0;
}

int read_bas_line( FILE *input, FILE *output, FILE* hexout, uint16_t *addr  ) {
    uint16_t length = 1;
    unsigned long pos = ftell( input );
    uint16_t line_number = read_word2( input );
    int last = 0;
    int REM_MODE = 0;
    if ( line_number >= 0x7FFF ) { // End of BASIC code
        if ( verbose ) printf( "Basic close line at pos 0x%04X Addr: 0x%04X\n", pos, *addr );
        for ( unsigned char byte = read_byte( input ); byte != 0x60 && !feof( input ); byte = read_byte( input ) ) length++;
        last = 1;
    } else {
        fprintf( output, "%d ", line_number );
        if ( hexout ) print_hexout( hexout, line_number, input );
        int varType = 0; // 0-még nincs első karakter, 1-első karakter[A-Z] azaz numeric, 2-első karakter [a-z] azaz string
        int idezojelben = 0;
        if ( verbose ) printf( "BAS LINE %d at pos 0x%04X Addr: 0x%04X\n", line_number, pos, *addr );
        for ( unsigned char byte = read_byte( input ); byte!=0x60 && !feof( input ); byte = read_byte( input ) ) {
//printf( "Read byte: 0x%02X (%c)\n", byte, byte );
            length++;
            if ( REM_MODE ) {
                fprintf( output, "%c", byte );
            } else if ( byte == 0x22 ) {
                if ( varType ) varType = flushVar( output, varType );
                idezojelben = 1-idezojelben;
                fprintf( output, "%c", byte );
            } else if ( idezojelben ) {
                fprintf( output, "%c", byte );
            } else if ( byte & 128 ) { // Token
                if ( varType ) varType = flushVar( output, varType );
                char* token = code_to_h3_token( byte );
                if ( verbose ) printf( "Found BASIC token '%s' for code 0x%02X in line %d\n", token, byte, line_number );
                if ( space ) fprintf( output, " " );
                fprintf( output, "%s", token );
                if ( space ) fprintf( output, " " );
                if ( byte == 0x89 ) REM_MODE = 1;
            } else if ( byte == 0x1F ) { // egy karakteres tömbnév második karaktere
                if ( varType ) { // Van típusa
                    if ( varType ) flushVar( output, varType );
                    fprintf( output, "%c", '(' );
                } else {
                    fprintf( stderr, "Invalid 2 DIM array!\n" ); exit(4);
                }
            } else if ( byte >= 'A' && byte <= 'Z' ) { // varCharCounter
                if ( !varType ) {
                    varType = 1; // numeric
                } else {
//                    varType = 0; // Egy stringnek is lehet kétbesűs változóneve
                }
                fprintf( output, "%c", byte );
            } else if ( byte >= 'a' && byte <= 'z' ) { // varCharCounter
                if ( !varType ) {
                    varType = 2; // String
                } else {
                    fprintf( stderr, "Invalid string second character\n" ); exit(4);
                    varType = 0;
                }
                fprintf( output, "%c", byte-32 );
            } else if ( byte >= 'A'-64 && byte <= 'Z'-64 ) { // Tömb második karaktere
                if ( varType ) {
                    fprintf( output, "%c", byte+64 );
                    varType = flushVar( output, varType );
                    fprintf( output, "%c", '(' );
                } else {
                    fprintf( stderr, "Invalid DIM array x!\n" );exit(4);
                }
            } else {
                if ( varType ) varType = flushVar( output, varType );
//                    if ( varCharCounter == 1 ) {
                fprintf( output, "%c", byte );
                varType = 0;
//                    } else {
//                        fprintf( stderr, "Invalid 3 DIM array!\n" );exit(4);
//                    }
            }
        }
        if ( varType ) varType = flushVar( output, varType );
        fprintf( output, "\n" );
    }
    *addr += length;
    return last;
}

void convert_htp_block( FILE *input, FILE *output, FILE *hexout, uint16_t length, int homelabVersion ) {
    DCB dcb = read_DCB( input );
    uint16_t addr = dcb.start_of_bas;
    int last = 0;
    while( addr < dcb.end_of_bas && !feof( input ) && !last ) {
        last = read_bas_line( input, output, hexout, &addr );
    }
    if ( addr != 0x40A0 && !force ) {
        fclose( output );
        fprintf( stderr, "A BASIC program nem a 0x40A0 címen kezdődik, hanem a 0x%04X címen! Konverzió egyelőre nincs implementálva.\n", addr );
        exit(4);
    }
}

void convertHls( FILE *input, FILE *output, FILE *hexout, int homelabVersion ) { // 0xC6-tól a 0x40A0 vagy 0x26-tól a 0x4000
    fseek( input, 0xC6, SEEK_SET );
    uint16_t addr = 0x40A0;
    int last = 0;
    while( !feof( input ) && !last ) {
        last = read_bas_line( input, output, hexout, &addr );
    }
}

void convertHtp( FILE *input, FILE *output, FILE *hexout, int homelabVersion ) {
    unsigned char c;
    for ( c = fgetc( input ); !c && !feof( input ); c = fgetc( input ) );
    if ( c==0xA5 ) { // End of prespace
        for ( c = fgetc( input ); c && !feof( input ); c = fgetc( input ) );
        if ( !feof( input ) && !c ) { // End of filename
            uint16_t start_addr = fgetc( input ) + 256 * fgetc( input );
            if ( start_addr == 0x4016 ) {
                uint16_t length = fgetc( input ) + 256 * fgetc( input );
                if ( verbose ) printf( "HTP Data block start address: 0x%04X\n", start_addr );
                if ( verbose ) printf( "HTP Data block length: 0x%04X (end address: 0x%04X)\n", length, start_addr+length );
                crc = 0;
                convert_htp_block( input, output, hexout, length, homelabVersion ); // Innen kezdődik a crc számítása
                unsigned long crc_pos = ftell( input );
                c = fgetc( input );
                if ( c == crc ) { // CRC ok
                    c = fgetc( input );
                    if ( !c ) { // End of htp
                        printf( "Conversion finished\n" );
                        return;
                    } else {
                        fprintf( stderr, "Multiblock htp not implemented\n" );
                        exit(4);
                    }
                } else {
                    fprintf( stderr, "HTP crc error: 0x%02X <> %02X. Input file pos: %04X\n", c, crc, crc_pos );
                    exit(4);
                }
            } else {
                fprintf( stderr, "Not a standard BASIC HTP file! Start address is not 0x4016! (0x%04X)\n", start_addr );
                exit(4);
            }
        } else {
            fprintf( stderr, "Invalid HTP filename end\n" );
            exit(4);
        }
    } else {
        fprintf( stderr, "Invalid HTP format: %02X\n", c );
        exit(4);
    }
}

/********************************************************************************************************************
 * Parameter functions
 ********************************************************************************************************************/
char *lastBasename;

FILE* setInputFile( char* filename ) {
    FILE *file;
    if ( !( file = fopen( filename, "rb" ) ) ) {
        fprintf( stderr, "Error opening %s.\n", filename );
        exit(4);
    }
    int i;
    for( i=strlen( filename )-1; i>0 && filename[i]!='.'; i-- );
    if ( i && filename[i]=='.' ) filename[i]=0;
    lastBasename = filename;
    return file;
}

FILE* setOutputFile( char* filename, const char *ext ) {
    FILE *file;
    if ( !filename[0] ) {
        filename = lastBasename;
        int i;
        for( i=strlen( filename )-1; i>0 && filename[i]!='.'; i-- );
        if ( i && filename[i]=='.' ) filename[i]=0;
        char buffer[1000];
        sprintf( buffer, "%s.%s", filename, ext );

        if ( !( file = fopen( buffer, "wb" ) ) ) {
            fprintf( stderr, "Error create %s.\n", buffer );
            exit(4);
        }
    } else {
        if ( !( file = fopen( filename, "wb" ) ) ) {
            fprintf( stderr, "Error create %s.\n", filename );
            exit(4);
        }
    }
    return file;
}
/*
FILE* setHexOutputFile( char* filename ) {
    if ( !filename[0] ) filename = lastBasename;
    int i;
    for( i=strlen( filename )-1; i>0 && filename[i]!='.'; i-- );
    if ( i && filename[i]=='.' ) filename[i]=0;
    char buffer[1000];
    sprintf( buffer, "%s.%s", filename, "hex" );

    FILE *file;
    if ( !( file = fopen( buffer, "wb" ) ) ) {
        fprintf( stderr, "Error create %s.\n", buffer );
        exit(4);
    }
    return file;
}
*/
/********************************************************************************************************************
 * Main functions
 ********************************************************************************************************************/
void print_usage() {
    printf( "listBasicHtpContent v%d.%d%c (build: %s)\n", VM, VS, VB, __DATE__ );
    printf( "Copyright 2023 by László Princz\n");
    printf( "List a Homelab BASIC .htp file.\n");
    printf( "Usage:\n");
    printf( "listBasicHtpContent <source_bas_htp_file> [<dest_bas_txt_file>]\n");
    printf( "-H [2|3]               : Homelab 2 or 3 HTP file. Default is Homelab 3.\n");
    printf( "-h                     : This help screen.\n");
    printf( "-s                     : Read HLS fileformat instead htp.\n");
    printf( "-S                     : Space before and after tokens in output.\n");
    printf( "-f                     : Force convert, even if BASIC start address is not 0x40A0.\n");
    printf( "-v                     : Verbose output.\n");
    exit(1);
}

/**
 * Set the command parameters
 */
int main( int argc, char *argv[] ) {
    int opt = 0;
    int homelabVersion = 3;
    int htp = 1; //  if 0: hls
    while ( ( opt = getopt (argc, argv, "lvsS?fh:H:") ) != -1 ) {
        switch ( opt ) {
            case -1:
            case ':':
                break;

            case '?':
            case 'h':
                print_usage();
                break;

            case 'v': 
                verbose = 1;
                break;

            case 'f': 
                force = 1;
                break;

            case 's': 
                htp = 0;
                break;

            case 'S': 
                space = 1;
                break;

            case 'l':
                list_tokens();
                exit(0);
                break;

            case 'H':
                if ( !sscanf( optarg, "%d", &homelabVersion ) ) {
                    fprintf( stderr, "Error parsing argument for '-H'.\n");
                    exit(2);
                }
                if ( homelabVersion != 2 && homelabVersion != 3 ) {
                    fprintf( stderr, "Invalid homelab version for '-H'.\n");
                    exit(2);
                }
                break;
        }
    }

    FILE *input;
    FILE *output;
    FILE *hexout = 0;
    // optind is for the extra arguments 
    // which are not parsed 
    if ( optind < argc ) { // First end parameter is the input filename
        input = setInputFile( argv[optind++] );
        if ( optind < argc ) { // Second end parameter is the output filename
            output = setOutputFile( argv[optind++], "bas" );
            if ( optind < argc ) { // Third end parameter is the hexout filename
                hexout = setOutputFile( argv[optind++], "hex" );
            }
        } else {
            output = setOutputFile( "", "bas" );
        }
    } else {
        fprintf( stderr, "A htp fájl megadása kötelező!\n" ); exit(4);
    }
    if ( optind < argc ) { // First end parameter is the input filename
        fprintf( stderr, "Ismeretlen paraméter: '%s'\n", argv[optind++] ); exit(4);
    }

    if ( htp ) {
        convertHtp( input, output, hexout, homelabVersion );
    } else {
        convertHls( input, output, hexout, homelabVersion );
    }
    fclose( input );
    fclose( output );
    if ( hexout ) fclose( hexout );

    return 0;
}
