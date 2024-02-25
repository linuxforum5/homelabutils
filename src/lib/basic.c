/**
 * Homelab 3 basic functions
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "htp.h"
#include "fs.h"
#include "basic.h"
#include "params.h"
#include "labels.h"
#include <ctype.h>

#define BASIC_START 0x40A0
#define MAX_LINE_LENGTH 1000

int last_line_number = -1; // For auto line number

char txt_to_homelab( char c ) {
    switch( c ) {
        case '{' : return '{'; break;
        default: return c;
    }
}

void homelab_characterset_conversion( char *line, int line_length ) {
    for( int i=0; i<line_length; i++ ) {
        line[i] = txt_to_homelab( line[i] );
    }
}

/**
 * A tokentábla eleje az 1C3C címen van, de az első hasznos adat az 1C3D címen
 * A tokenek a 0x0C3D - 0x0D3D címen kezdődnek. 0x102 (256) byte
 * A token utolsó karakterének a 7. bitje 1-es, a többi karakternél ez a bit 0.
 * A token kódja az ebben a táblázabna elfoglalt indexe 0-ától számozva + 128.
 * Például a ^ token kódja : 0x80. Ez az első token a listában, tehát az indexe 0. Ehhez jön még a +128.
 *         a + token kódja : 43 0x81. Ez a második token a listában, tehát az indexe 1. Ehhez jön még a +128.
 */
const unsigned char tokens[] = { 0xde, 0xab, 0xad,
0xaa, 0xaf, 0xbe, 0xbc, 0xbd, 0x50, 0xe9, 0x52, 0x65, 0xed, 0x43, 0xf2, 0x48, 0xed, 0x49, 0x6e,0x6b, 0x65, 0xf9, 0x61, 0x6e, 0xe4, 0x6f, 0xf2, 0x63, 0x75, 0xf2, 0x44, 0x69, 0xed, 0x45, 0x6e,
0xe4, 0x46, 0x6f, 0xf2, 0x47, 0x6f, 0x74, 0xef, 0x47, 0x6f, 0x73, 0x75, 0xe2, 0x49, 0xe6, 0x43,0x6f, 0x6e, 0xf4, 0x44, 0x61, 0x74, 0xe1, 0x4c, 0x69, 0x73, 0xf4, 0x52, 0x65, 0x61, 0xe4, 0x4e,
0x65, 0x78, 0xf4, 0x4f, 0xee, 0x50, 0x6f, 0x6b, 0xe5, 0x45, 0x78, 0xf4, 0x52, 0x65, 0x74, 0x75,0x72, 0xee, 0x53, 0x61, 0x76, 0xe5, 0x74, 0x68, 0x65, 0xee, 0x4c, 0x6f, 0x61, 0xe4, 0x52, 0x65,
0x73, 0x74, 0x6f, 0x72, 0xe5, 0x73, 0x74, 0x65, 0xf0, 0x50, 0x6f, 0xf0, 0x4e, 0x65, 0xf7, 0x74,0xef, 0x42, 0x65, 0x65, 0xf0, 0x50, 0x72, 0x69, 0x6e, 0xf4, 0x52, 0x75, 0xee, 0x4d, 0x6f, 0xee,
0x6e, 0x6f, 0xf4, 0x50, 0x6c, 0x6f, 0xf4, 0x43, 0x61, 0x6c, 0xec, 0x49, 0x6e, 0x70, 0x75, 0xf4,0x73, 0x74, 0x72, 0xa4, 0x6c, 0x66, 0x74, 0xa4, 0x72, 0x67, 0x68, 0xa4, 0x63, 0x68, 0x72, 0xa4,
0x6d, 0x69, 0x64, 0xa4, 0x69, 0x6e, 0xf4, 0x61, 0x73, 0xe3, 0x6c, 0x65, 0xee, 0x61, 0x62, 0xf3,0x73, 0x67, 0xee, 0x63, 0x6f, 0xf3, 0x70, 0x65, 0x65, 0xeb, 0x73, 0x71, 0xf2, 0x72, 0x6e, 0xe4,
0x73, 0x69, 0xee, 0x74, 0x61, 0xee, 0x75, 0x73, 0xf2, 0x76, 0x61, 0xec, 0x61, 0x74, 0xee, 0x70,0x6f, 0x69, 0x6e, 0xf4, 0x65, 0x78, 0xf0, 0x6c, 0x6f, 0xe7, 0x8d, 0x8d, 0x8d, 0x66, 0x72, 0xe5,
// 0x44, 0xe4, 0x43, 0xee, 0x50, 0xf0, 0x4f, 0xe4, 0x55, 0xe6, 0x53, 0xee, 0x54, 0xed, 0x55, 0xf3,0x2f, 0xb0, 0x49, 0xf1, 0x4f, 0xed, 0x4f, 0xf6, 0x53, 0xec, 0x42, 0xf3, 0x4f, 0xeb
0x4B, 0x65, 0xF9, 0x4E, 0x65, 0xF7, 0x45, 0x78, 0xF4, 0x45, 0x73, 0x61, 0x76, 0xE5, 0x45, 0x6C, 0x6F, 0x61, 0xE4, 0x4D, 0x65, 0x72, 0x67, 0xE5, 0x56, 0x65, 0x72, 0x69, 0x66, 0xF9, 0x45, 0x64,
0x69, 0xF4, 0x52, 0x65, 0x70, 0x65, 0x61, 0xF4, 0x55, 0x6E, 0x74, 0x69, 0xEC, 0x44, 0x65, 0x6C, 0x65, 0x74, 0xE5, 0x47, 0x6F, 0x73, 0x75, 0x62, 0xA3, 0x47, 0x6F, 0x74, 0x6F, 0xA3, 0x52, 0x65,
0x74, 0x75, 0x72, 0xEE, 0x66, 0x6F, 0x72, 0x6D, 0x24, 0xA8, 0x68, 0x65, 0x78, 0x24, 0xA8, 0x64, 0x65, 0x63, 0xA8, 0x6D, 0x69, 0x6E, 0xA8, 0x6D, 0x61, 0x78, 0xA8, 0x66, 0x73, 0x77, 0xA8, 0x6D,
0x6F, 0x64, 0xA8, 0x50, 0x72, 0xE7, 0x66, 0x72, 0x61, 0xA8, 0x73, 0x74, 0x72, 0x69, 0x6E, 0x67, 0x24, 0xA8, 0xA5, 0xA4, 0x76, 0x61, 0x72, 0xA8, 0x72, 0x6F, 0x75, 0x6E, 0x64, 0xA8
// ,0x20, 0x65,0x72, 0x72, 0x6f, 0x72, 0x21, 0x8d, 0x0d, 0x42, 0x72, 0x65, 0x61, 0x6b, 0x8d, 0x21, 0xc2, 0x11,0x11, 0x52, 0x40, 0xcd, 0xa3, 0x1f, 0xed, 0xb0, 0x2a, 0x18, 0x40, 0x3e, 0x80, 0xbe, 0x20, 0x0c
 };
/*
const unsigned char tokens0[] = { 0xde, 0xab, 0xad,
0xaa, 0xaf, 0xbe, 0xbc, 0xbd, 0x50, 0xe9, 0x52, 0x65, 0xed, 0x43, 0xf2, 0x48, 0xed, 0x49, 0x6e,0x6b, 0x65, 0xf9, 0x61, 0x6e, 0xe4, 0x6f, 0xf2, 0x63, 0x75, 0xf2, 0x44, 0x69, 0xed, 0x45, 0x6e,
0xe4, 0x46, 0x6f, 0xf2, 0x47, 0x6f, 0x74, 0xef, 0x47, 0x6f, 0x73, 0x75, 0xe2, 0x49, 0xe6, 0x43,0x6f, 0x6e, 0xf4, 0x44, 0x61, 0x74, 0xe1, 0x4c, 0x69, 0x73, 0xf4, 0x52, 0x65, 0x61, 0xe4, 0x4e,
0x65, 0x78, 0xf4, 0x4f, 0xee, 0x50, 0x6f, 0x6b, 0xe5, 0x45, 0x78, 0xf4, 0x52, 0x65, 0x74, 0x75,0x72, 0xee, 0x53, 0x61, 0x76, 0xe5, 0x74, 0x68, 0x65, 0xee, 0x4c, 0x6f, 0x61, 0xe4, 0x52, 0x65,
0x73, 0x74, 0x6f, 0x72, 0xe5, 0x73, 0x74, 0x65, 0xf0, 0x50, 0x6f, 0xf0, 0x4e, 0x65, 0xf7, 0x74,0xef, 0x42, 0x65, 0x65, 0xf0, 0x50, 0x72, 0x69, 0x6e, 0xf4, 0x52, 0x75, 0xee, 0x4d, 0x6f, 0xee,
0x6e, 0x6f, 0xf4, 0x50, 0x6c, 0x6f, 0xf4, 0x43, 0x61, 0x6c, 0xec, 0x49, 0x6e, 0x70, 0x75, 0xf4,0x73, 0x74, 0x72, 0xa4, 0x6c, 0x66, 0x74, 0xa4, 0x72, 0x67, 0x68, 0xa4, 0x63, 0x68, 0x72, 0xa4,
0x6d, 0x69, 0x64, 0xa4, 0x69, 0x6e, 0xf4, 0x61, 0x73, 0xe3, 0x6c, 0x65, 0xee, 0x61, 0x62, 0xf3,0x73, 0x67, 0xee, 0x63, 0x6f, 0xf3, 0x70, 0x65, 0x65, 0xeb, 0x73, 0x71, 0xf2, 0x72, 0x6e, 0xe4,
0x73, 0x69, 0xee, 0x74, 0x61, 0xee, 0x75, 0x73, 0xf2, 0x76, 0x61, 0xec, 0x61, 0x74, 0xee, 0x70,0x6f, 0x69, 0x6e, 0xf4, 0x65, 0x78, 0xf0, 0x6c, 0x6f, 0xe7, 0x8d, 0x8d, 0x8d, 0x66, 0x72, 0xe5,
0x44, 0xe4, 0x43, 0xee, 0x50, 0xf0, 0x4f, 0xe4, 0x55, 0xe6, 0x53, 0xee, 0x54, 0xed, 0x55, 0xf3,0x2f, 0xb0, 0x49, 0xf1, 0x4f, 0xed, 0x4f, 0xf6, 0x53, 0xec, 0x42, 0xf3, 0x4f, 0xeb };
*/
void list_tokens() {
    int code=128;
    int i=0;
    while( i<sizeof(tokens) ) {
        printf( "%d.\t%02X %d\t'", i, code, code );
        int i0 = i;
        // show token as characters
        while( ( !(tokens[i] & 128) ) && ( i<sizeof(tokens) ) ) printf( "%c", tokens[i++] & 127 ); // 0b01011111 
        printf( "%c' (%d)\thex: ", tokens[i] & 127, tokens[i] ); // 127
        // show token as hex
        i = i0;
        while( ( !(tokens[i] & 128) ) && ( i<sizeof(tokens) ) ) printf( " %02X", tokens[i++] & 127 );
        printf( " %02X\n", tokens[i] & 127, tokens[i] ); // 127
        i++;
        code++;
    }
}
/*
char charset_conversion_homelab_to_utf( char c ) {
    switch( c ) {
      case 0x04 : return '$'; break;
//      case 0x0A : return '*'; break;
//      case 0x0B : return '+'; break;
//      case 0x0D : return '-'; break;
      case 0x0F : return '/'; break;
      case 0x1E : return '>'; break;
      case 0x1C : return '<'; break;
      case 0x1D : return '='; break;
      default: return c;
    }
}

char charset_conversion_utf_to_homelab( char c ) {
    switch( c ) {
//      case '$' : return 0x04; break;
//      case '*' : return 0x0A; break;
//      case '+' : return 0x0B; break;
//      case '-' : return 0x0D; break;
      case '/' : return 0x0F; break;
      case '>' : return 0x1E; break;
      case '<' : return 0x1C; break;
      case '=' : return 0x1D; break;
      default: return toupper( c );
      // default: return c;
    }
}
*/
unsigned char get_token( unsigned char *string, int *string_length ) { // Visszaadja a szóhoz tartozó token kódot, vagy 0 kódot, ha nem tokenizálható.
    int current_token = 127; // A tokenszavak utolsó karakterének 7. bitje 1. A token azt mondja meg, hogy hányadik(+127) tokenszó a keresett. Az első szó az "^" Ennek kódja 128.
    int match_length = 0;
    int skip = 0; // Ha ez 1, akkor nem ellenőrzünk, hanem várjuk, míg vége lesz az aktuális tokennek, amiről tudjuk, hogy rossz
// printf( "Checked string: '%s'\n", string );
    for( int i=0; i<sizeof( tokens ); i++ ) {
        int last_token_character = tokens[ i ] > 127;
        if ( last_token_character ) current_token++; // Token last char. Increment token counter
        if ( skip ) {
            if ( last_token_character ) skip = 0;
        } else { // Ellenőrzésben vagyunk
            char token_character = tokens[ i ] & 127; // char token_character = last_token_character ? tokens[ i ] & 95 : tokens[ i ];
            if ( token_character >= 'a' && token_character <= 'z' ) token_character = toupper( token_character );
            if ( token_character == toupper( string[ match_length ] ) ) { // A keresett karakter jó
// printf( "??? Found token character: '%c'\n", token_character );
                match_length++; // Eggyel több illeszkedik
                if ( last_token_character ) { // Ez volt a token utolsó karaktere, megvagyunk
                    *string_length = match_length; // Az illesztett szó hossza
                    if ( verbose ) {
                        printf( "!!! Found token '" );
                        for( int p=0; p<match_length; p++ ) printf( "%c", string[ p ] );
                        printf( "' code=%02X (%d)\n", current_token, current_token );
                    }
                    return current_token; // Ez a token kódja
                } else if ( match_length >= *string_length ) { // Nincs több illeszthető keresett karakter, ez nem token
// printf( ":( No 1\n" );
                    return 0;
                } // else { // Különben folytatjuk.
            } else { // Nem illeszkedő karakter
                match_length = 0; // Kezdjük előről
                if ( !last_token_character ) skip = 1;
            }
        }
    }
// printf( ":( No 2\n" );
    return 0;
}

int drop_comment( char * line, int line_length ) {
    for( int i=0; i<line_length; i++ ) {
        if ( comment_skip_enabled && ( skip_comments_character == 0 ) && ( line[i] == 0x27 ) && ( line[ i+1 ]== 0x22 ) ) { // 0x27=' és 0x22="
            line_length = i;
        } else if ( comment_skip_enabled && ( line[i] == 0x27 ) && ( line[ i+1 ]== 0x22 ) && ( line[ i+2 ]== skip_comments_character ) ) { // '"! mindenképp kihagyásra kerül!
            line_length = i;
        }
    }
    line[ line_length ] = 0;
    return line_length;
}

int get_line_from( FILE *f, unsigned char* line, int16_t *line_number, char* label, int max_line_length ) {
    *line_number = -1;
    label[0] = 0;
    int label_length = 0;
    int read_number = 1; // 1=read line number, 2=read label
    int line_length = 0;
    for ( unsigned char c = fgetc( f ); !feof( f ) && c!=10 && c!=13; c = fgetc( f ) ) {
        if ( read_number == 1 ) {
            if ( c>='0' && c<='9' ) {
                if ( *line_number == -1 ) *line_number = 0;
                *line_number = *line_number * 10 + c - '0';
            } else if ( *line_number == -1 && c <=' ' ) { // SKIP prefix spaces
            } else if ( c==' ' || c==9 ) { // SKIP line basic prefix spaces
            } else if ( c=='{' ) { // Begin label
                read_number = 2;
            } else { // BASIC text line
                read_number = 0;
            }
        } else if ( read_number == 2 ) { // Read label
            if ( c=='}' ) { // End label
                read_number = 1;
            } else if ( c > 32 ) {
                label[ label_length++ ] = c;
                if ( label_length >= MAX_LABEL_LENGTH ) label_length--;
                label[ label_length ] = 0;
            }
        }
        if ( !read_number ) {
            line[ line_length++ ] = c;
            if ( line_length == max_line_length ) {
                fprintf( stderr, "Line to length!\n" ); exit(1);
            }
        }
    }
    line_length = drop_comment( line, line_length );
    while( line_length && ( line[ line_length-1 ]==32 || line[ line_length-1 ]==9 ) ) line[ --line_length ] = 0; // Rtrim
    return line_length;
}

int read_label_write_number( const char* line, int i, int line_length, FILE *htp ) { // Címke beolvasása, és a htp fájlba a hozzá tartozó sorszám kiírása
    if ( line[ i ] == '{' ) {
        char label[ MAX_LABEL_LENGTH ];
        int j = 0;
        for( i++; ( i < line_length ) && ( line[i] != '}' ); i++ ) { // i = next_char start position
            label[ j++ ] = line[i];
        }
        label[j]=0;
        int16_t line_number = get_line_number( label );
        char numstr[10] = { 0 };
        sprintf( numstr, "%d", line_number );
        for( int i=0; numstr[i] && ( i < 10 ); i++ ) crc_fputc( numstr[i], htp );
        return i;
    } else {
        fprintf( stderr, "Internal error 1" );
        exit(4);
    }
}

int writeVarname( FILE *htp, char *varname, int varnamelength, int isString, int isArray ) {
    if ( varnamelength ) {
        if ( verbose ) {
            varname[2] = 0;
            if ( varnamelength == 1 ) varname[1] = 0;
            printf( "%s found: %s%c%c\n", isArray?"Array":"Variable", varname, isString?'$':' ' );
        }
        if ( isString ) varname[0] += 32; // 5. bit 1-be
        if ( isArray ) {
            if ( varnamelength == 1 ) {
                varnamelength = 2;
                varname[1] = 0x1F;
            } else {
                varname[1] -= 64;
            }
        }
        crc_fputc( varname[0], htp );
        if ( varnamelength > 1 ) crc_fputc( varname[1], htp );
    } else {
        fprintf( stderr, "Üres változónév nem írható ki!\n" ); exit(4);
    }
    return 0;
}
int num10( char c ) { return ( c >= '0' && c <= '9' ) || c == '.' || c == 'E'; }
int num16( char c ) { return ( c >= '0' && c <= '9'  ) || ( c >= 'A' && c <= 'F' ); }

#define CODE_MODE 0
#define SPACE_MODE 1
#define REM_MODE 2
#define LABEL_MODE 3
/**
 * Egy sor elkódolás sorfolytonos. Ha van TOKEN, akkor annak helyére a kódja kerül, különben maradnak a karakterek.
 * A változók neve is konvertálásra kerül:
 * - maximum 2 karakter hosszú lehet, és csak A-Z karakterekből állhat ([0x41-0x5A] [01000001-01011010]), azaz minden változónév karakterre igaz, hogy: 
 *   - 7. bit = 0
 *   - 6. bit = 1
 *   - 5. bit = 0
 * - Amennyiben a változónév végén $ karakter van, azt nem tároljuk, de a változó nevének első betűjének kódjához 32-t hozzáadunk (set 5. bit)
 * - Amennyiben a változó tömbb, úgy a második karakterének 6. bitjét töröljük. Amennyiben 1 karakteres, úgy a második karakter az 0x1F lesz
 * - Amennyiben a változó string típusú
 * A karakterek 7. bitje mindig 0, a tokenek 7. bitje mindig 1.
 * A sor felolvasása közben a szóközöket átugorjuk.
 * A sor felolvasása közben a következő állapotok lehetégesek:
 * - CODE_MODE : Ez a nyitó állapot. 
 *   Ebben az állapotban a szóközöket átugorjuk.
 *   Beolvassuk a következő TOKEN kódját, ha találunk.
 *   Ha találtunk TOKENT-t, akkor
 *     Ha ez a token a $ jel tokenje (0xE2), akkor 
 *       Ha az utoljára megtalált TOKEN az 0x8C volt (INKEY), kiírjuk a $ karaktert, mint karaktert
 *       Ha egy változónevet olvastunk utoljára, akkor annak módosítjuk a típusát
 *       Ha egyik sem, akkor kiírjuk a token-t
 *     Ha nem a $ token volt, akkor kiírjuk a tokent. Ha ez egy függvény volt, akkor zárójelnek kell következnie, amit lenyelünk.
 *   Ha nem találunk TOKEN-t, akkor
 *     Ha az aktuális karakter a " jel, akkor SPACE módba váltunk, és letároljuk az " karaktert.
 *     Ha változónév, akkor regisztráljuk a változót
 *     Ha számjegy, akkor  
 * - SPACE_MODE: "-jelig mindent egy az egyben kiírunk. "jelre visszaváltunk CODE_MODE-ba
 * Az aktuális karaktert 
 */
char encode_line( FILE *htp, char* line, int line_length ) {
    int mode = CODE_MODE; // CODE módban a kód értelmeződik tokenekre és egyéb paraméterekre és a space lenyelődik, de SPACE modban nincs értelmezés, olyankor a SPACE is kiírásra kerül.
    unsigned char last_token_code = 0; // A közvetlenül most beazonosított token kódja az aktuális előtt
    int constant = 0; // Konstans számrendszere, ha épp olvassuk
    char varname[3]; // Ebben tároljuk majd a megtalált változók nevének első két karakterét
//    int varIsString = 0; // Ha 1, akkor a változó string
//    int varIsArray = 0; // Ha 1, akkor a változó tömb
    int varnamelength = 0; // Ebben tároljuk majd a megtalált változó nevének hosszát. Ha nagyobb, mint 2, akkor már nem tárolunk
    for( int i = 0; i < line_length; i++ ) { // i = next_char start position
        char current_char = line[ i ];
        if ( mode == REM_MODE ) {
            crc_fputc( current_char, htp );
//        } else if ( mode == LABEL_MODE ) {
//            crc_fputc( current_char, htp );
//            if ( current_char == '}' ) mode = CODE_MODE;
        } else if ( mode == CODE_MODE ) {
            int length = line_length-i; // Hátralévő hossz
            while ( current_char == ' ' ) current_char = line[ ++i ]; // SPACE-ek átugrása
            unsigned char token_code = get_token( line + i, &length ); // i = current_char_start_position
            if ( token_code == 0xE2 ) { // $ token
                if ( last_token_code == 0x8C ) { // INKEY token
                    crc_fputc( '$', htp );
                } else if ( varnamelength ) { // Van változónév
                    if ( line[i+1] == '(' ) {
                        varnamelength = writeVarname( htp, varname, varnamelength, 1, 1 );
                        i++;
                    } else {
                        varnamelength = writeVarname( htp, varname, varnamelength, 1, 0 );
                    }
                } else { // mehet a tokenkiírás
                    crc_fputc( token_code, htp );
                    constant = 16;
                }
            } else if ( token_code == 0x89 ) { // REM
                crc_fputc( token_code, htp );
                mode = REM_MODE;
            } else if ( token_code ) { // Más token, tároljuk le, és ha függvény, ugorjuk át a ( karaktert
                if ( varnamelength ) varnamelength = writeVarname( htp, varname, varnamelength, 0, 0 );
                crc_fputc( token_code, htp );
                i += length - 1;
                if ( ( token_code >= 0xAF ) && ( token_code <= 0xC8 ) ) { // Function H2 és H3 esetén is
                    if ( line[ i + 1 ] == '(' ) {
                        i++; // Skip first '('
                    } else {
                        fprintf( stderr, "Missign '(' after funciton! : 0x%02X in %d. '%s'\n", token_code, i, line );
                        exit(4);
                    }
                }
                last_token_code = token_code;
                constant = 0;
            } else if ( current_char == '"' ) { // Idézőjelmód
                if ( varnamelength ) varnamelength = writeVarname( htp, varname, varnamelength, 0, 0 );
                // if ( varnamelength ) { fprintf( stderr, "Változónév után nem jöhet \": '%c' in '%s'\n", current_char, line ); exit(4); } // De, jöhet: INPUT A"Prompot"
                crc_fputc( current_char, htp );
                mode = SPACE_MODE;
                last_token_code = 0;
            } else if ( current_char == '(' ) {
                if ( varnamelength ) varnamelength = writeVarname( htp, varname, varnamelength, 0, 1 );
                else crc_fputc( current_char, htp );
            } else if ( current_char == ')' || current_char == ',' || current_char == ':' || current_char == '\'' || current_char == ';' ) { // Elfogadott egyéb karakterek
                if ( varnamelength ) varnamelength = writeVarname( htp, varname, varnamelength, 0, 0 );
                crc_fputc( current_char, htp );
                constant = 0;
//            } else if ( current_char == '{' ) { // For labels
//                if ( varnamelength ) varnamelength = writeVarname( htp, varname, varnamelength, 0, 0 );
//                crc_fputc( current_char, htp );
//                constant = 0;
//                mode = LABEL_MODE;
            } else if ( constant ) {
                if ( constant == 10 && num10( current_char ) ) crc_fputc( current_char, htp );
                else if ( constant == 16 && num16 ) crc_fputc( current_char, htp );
                else { fprintf( stderr, "Hibás konstans karakter: '%c' (0x%02X) (számrendszer=%d) pos=%d, length=%d\n", current_char, current_char, constant, i, line_length ); exit(4); }
            } else if ( current_char >= 'A' && current_char <= 'Z' ) { // Változónév karakter
                if ( varnamelength < sizeof( varname ) ) varname[ varnamelength ] = current_char;
                varnamelength++;
            } else if ( num10( current_char ) ) {
                constant = 10; // 10-es számrendszerbeli konstans
                crc_fputc( current_char, htp );
            } else {
                fprintf( stderr, "Ismeretlen forráskód karakter: '%c' in '%s'\n", current_char, line ); exit(4);
            }
        } else if ( mode == SPACE_MODE ) {
            last_token_code = 0;
            crc_fputc( current_char, htp );
            if ( current_char == 0x22 ) mode = CODE_MODE;
        } else {
            fprintf( stderr, "Ismeretlen értelmezési mód!" ); exit(1);
        }
    }
    if ( varnamelength ) varnamelength = writeVarname( htp, varname, varnamelength, 0, 0 );
}


void preload_labels( FILE *txt ) {
    clear_labels();
    unsigned char line[ MAX_LINE_LENGTH+1 ]; // +1 a lezáró 0 miatt, amire nem lenne szükség, de a biztosnág kedvéért inkább mindig odatesszük.
    unsigned char label[ MAX_LABEL_LENGTH ];
    int16_t line_number = -1;
    while( !feof( txt ) ) {
        int line_length = 0;
        if ( line_length = get_line_from( txt, line, &line_number, label, MAX_LINE_LENGTH ) ) {
            if ( line_number == -1 ) line_number = ( last_line_number == -1 ) ? 10 : last_line_number + 10;
            if ( label[0] ) set_label( label, line_number );
            last_line_number = line_number;
        }
    }
    fseek( txt, 0, SEEK_SET );
    last_line_number = -1;
}

unsigned char newline[ MAX_LINE_LENGTH+1 ];
int newline_length = 0;

unsigned char* labelness( unsigned char* line ) {
    unsigned char label[ MAX_LINE_LENGTH+1 ];
    int j=0;
    int in_label = 0;
    int l = 0;
    for( int i=0; (i<MAX_LINE_LENGTH) && line[i]; i++ ) {
        if ( line[i] == '{' ) {
            in_label = 1;
        } else if ( line[i] == '}' ) {
            label[ l ] = 0;
            int16_t line_number = get_line_number( label );
            char numstr[10] = { 0 };
            sprintf( numstr, "%d", line_number );
            for( int n=0; numstr[ n ] && ( n < 10 ); n++ ) newline[ j++ ] = numstr[ n ];
            in_label = 0;
            l = 0;
        } else if ( in_label ) {
            label[ l++ ] = line[ i ];
        } else {
            newline[ j++ ] = line[ i ];
        }
    }
    newline[ j ] = 0;
    newline_length = j;
printf( "Source line without label: '%s'\n", line );
// printf( "BASIC line with line number: '%s'\n", newline );
    return newline;
}

// Line: NextRowAddrL NextRowAddrH NumL NumH tokenized 0
// Program end: 0 0
void encodeBasicFrom40A0( FILE *htp, FILE *txt, FILE *BAS ) { // Encode text into src.bytes
    preload_labels( txt );
    uint16_t next_line_addr = BASIC_START;
    int16_t line_number = -1;
    unsigned char line[ MAX_LINE_LENGTH+1 ];
    unsigned char label[ MAX_LABEL_LENGTH ];
    while( !feof( txt ) ) {
        int line_length = 0;
        if ( line_length = get_line_from( txt, line, &line_number, label, MAX_LINE_LENGTH ) ) {
            if ( label[0] ) line_number = get_line_number( label );
            if ( line_number == -1 ) line_number = ( last_line_number == -1 ) ? 10 : last_line_number + 10;
            // if ( line_number == -1 ) line_number = last_line_number + 10;
// printf( "%d %s\n", line_number, line );
            last_line_number = line_number;
            crc_fputc( line_number / 256, htp );
            crc_fputc( line_number % 256, htp );
            line[ line_length ] = 0;

            if ( BAS ) fprintf( BAS, "%d %s\n", line_number, labelness( line ) );
            if ( verbose ) printf( "Converted line %d '%s'\n", line_number, line );

            homelab_characterset_conversion( line, line_length );
            // encode_line( htp, line, line_length );
            encode_line( htp, newline, newline_length );
            crc_fputc( 0x60, htp );
        } else {
            if ( verbose ) printf( "Skip empty source line\n" );
        }
    }
}

void write_htp_basic_payload( FILE *htp, FILE *txt, uint16_t load_address, FILE *BAS ) {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Write DCB header from 0x4018
    /// A SAVE a 4016-tól a (4018)-ig ment
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 0B 02 75 │ 02 74 03 8B │ 05 54 00 AB │ 40 00 D7 A0 │ 40 45 F1 FF │ 00 00 A0 40 │ 00 00 00 33  ..@w8...@.@.@5@B`..u.t...T..@.נ @E.....@...3
00 00 00 00 │ 00 00 00 00 │ 00 00 C0 F9 │ 6B 20 18 05 │ 00 00 00 00 │ 3C 2D 20 04 │ 20 6B 20 3C │ 1C 00 00 00 │ 00 00 00 32 │ 30 32 34 43 │ 41 4C 4C 31  ............k ......<- . k <.......2024CALL1
36 36 34 34 │ 00 00 00 00 │ 00 00 00 00 │ 00 00 00 00 │ 00 00 00 00 │ 00 00 00 00 │ 00 00 00 00 │ 00 00 00 00 │ 00 00 00 00 │ 00 00 00 00 │ 00 00 00 00  6644........................................
00 00 00 00 │ 00 00 00 00 │ 00 00 60 07 │ E8 AD 31 36 │ 36 34 34 60 │ 80 60 07 E8 │ AD 31 36 36 │ 34 34 60 20 │ 66 72 65 65 │ 20 63 6F 6D │ 70 75 74 65  ..........`...16644`.`...16644` free compute
72 0D 20 20 │ 20 48 6F 6D │ 65 4C 61 62 │ 20 42 41 53 │ 49 43 20 72 │ 65 76 2E 33 │ 2E 31 0D 0D │ 00 00 00 00 │ 00 00 00 00 │ 00 00 00 00 │ 00 00 00 00  r.   HomeLab BASIC rev.3.1..................
00 00 00 00 │ 00 00 00 00 │ 00 00 00 00 │ 00 00 00 00 │ 00 00 00 2D │ 4E 53 2D F3 │ 11 67 41 1A │ FE 00 28 05 │ CD 14 41 18 │ F6 18 F1 21 │ 00 F0 3E 01  ...................-NS-..gA...(...A....!..>.
32 64 41 06 │ 40 0E 20 1A │ 13 CB 77 CA │ 34 41 CB B7 │ 32 65 41 1A │ 13 32 64 41 │ 3A 65 41 32 │ 63 41 3A 63 │ 41 FE 10 28 │ 09 D3 FF CB │ F7 CB FF 77  2dA.@. ...w.4A˷ 2eA..2dA:eA2cA:cA..(.......w
D3 7F 23 05 │ 20 05 0D 28 │ 18 06 40 3A │ 64 41 3D 32 │ 64 41 FE 00 │ 20 DC 3E 01 │ 32 64 41 C3 │ 20 41 00 00 │ 00 C9 FF FF │ FF 45 87 10 │ 8B AF BF 50  ..#. ..(..@:dA=2dA.. .>.2dA. A.......E.....P
00000268 02 8B FF 02 │ 8F 50 02 8B │ AF FF 02 87 │ 50 02 AB BF │ 97 50 02 8B │ BF 50 06 AF │ 87 10 8B FF │ 06 50 03 FF │ 03 50 05 8F │ FF 09 C0 03 │ AA BF C0 03  .....P......P....P...P.......P...P..........
00000294 BF 10 C0 05 │ 8B BF C0 04 │ BF C0 04 BF │ C0 06 AA C0 │ 03 FF 05 97 │ C0 03 AB FF │ 02 C0 06 10 │ FF 08 C0 03 │ AA BF C0 03 │ 97 C0 03 BC │ 10 C0 02 AF  ............................................
000002C0 C0 04 BF C0 │ 04 BF C0 02 │ 10 FC 02 B8 │ BE C0 03 FF │ 05 10 C0 03 │ 10 FF 02 C0 │ 03 BC C0 03 │ FF 08 C0 08 │ 95 C0 02 10 │ BF 95 C0 02 │ AA C0 04 10  ............................................
000002EC C0 04 BF C0 │ 02 50 04 BF │ C0 03 FF 05 │ C0 05 FF 02 │ C0 03 10 C0 │ 02 10 FF 08 │ C0 08 95 C0 │ 02 AA BF 95 │ C0 02 AA C0 │ 09 BF C0 06 │ BF C0 03 FF  .....P......................................
00000318 04 95 C0 02 │ BC C0 02 AA │ BF C0 06 10 │ FF 08 C0 03 │ AA BF C0 03 │ 95 C0 02 10 │ BF 95 C0 02 │ AA C0 02 10 │ C0 03 10 C0 │ 02 BF C0 02 │ AA FF 04 C0  ............................................
00000344 03 FF 04 10 │ C0 05 10 BF │ C0 03 BF 10 │ C0 02 AB FF │ 07 C0 03 AA │ BF C0 03 BD │ C0 03 10 C0 │ 03 BE C0 02 │ AA C0 03 95 │ C0 02 BF C0 │ 06 AB C0 06  ............................................
00000370 10 C0 07 AF │ C0 03 10 C0 │ 03 BE FF 07 │ C0 03 AA BF │ C0 03 BF BD │ 10 C0 03 10 │ BA BF C0 02 │ AA 10 80 10 │ 95 C0 02 BF │ C0 06 AA C0 │ 09 BA BF B5  ............................................
0000039C C0 02 AA C0 │ 06 B8 FF 13 │ BD BC BE FF │ FF FF 74 CF │ 03 BF 9F CF │ 02 AF 9F CF │ 02 AF BF CF │ 06 FF 02 9F │ CF 02 AF FF │ 04 CF 04 FF │ 03 CF 03 AF  ..............t.............................
000003C8 FF 03 CF 03 │ AF FF 04 9F │ 10 8B FF 09 │ 95 C0 03 BF │ 95 C0 02 AA │ C0 03 10 BF │ C0 03 10 C0 │ 02 AA BF C0 │ 03 FF 04 10 │ C0 04 10 BF │ 10 C0 04 AB  ............................................
000003F4 9F C0 05 AB │ FF 02 9F C0 │ 03 FF 09 95 │ C0 03 BF 95 │ C0 02 AA C0 │ 04 AF C0 03 │ BF 10 C0 02 │ 10 C0 02 BA │ FF 03 9F C0 │ 02 50 02 C0 │ 02 9F C0 02  .....................................P......
00000420 50 02 80 10 │ 95 C0 02 BC │ C0 02 10 FF │ 02 10 C0 03 │ FF 09 95 C0 │ 03 50 02 C0 │ 02 AA C0 04 │ 10 C0 03 BF │ B5 C0 04 10 │ FF 05 FC 02 │ BE 10 C0 02  P....................P......................
0000044C 95 C0 02 BF │ 95 C0 02 BD │ FC 02 9F C0 │ 02 10 BF 10 │ C0 04 FF 09 │ 95 C0 07 AA │ C0 08 FF 02 │ 10 C0 03 FF │ 08 10 C0 02 │ BA 95 C0 02 │ BF 95 C0 02  ............................................
00000478 FF 02 9F C0 │ 03 BE 10 80 │ 50 02 C0 02 │ FF 09 95 C0 │ 03 BC 10 C0 │ 02 AA C0 03 │ 10 C0 04 FF │ 03 C0 02 AA │ FF 06 9F 10 │ C0 02 B8 BF │ 95 C0 02 BF  ........P...................................
000004A4 95 C0 02 BF │ 97 C0 02 10 │ BE BF C0 07 │ FF 08 95 C0 │ 03 BF 95 C0 │ 02 AA C0 03 │ BD C0 04 FF │ 03 C0 02 AA │ FF 06 10 C0 │ 04 10 BD C0 │ 02 50 02 80  .........................................P..
000004D0 10 9F C0 05 │ 10 C0 07 FF │ 08 95 C0 03 │ BF 95 C0 02 │ AA C0 03 BF │ B5 C0 03 FF │ 03 C0 02 AA │ FF 05 97 C0 │ 06 BF 10 C0 │ 04 BA 95 C0 │ 06 FF 03 95  ............................................
000004FC C0 02 FF 0A │ FC 02 BE BF │ BD FC 02 BF │ BD BC BE FF │ 02 FC 02 BE │ FF 03 FC 02 │ BE FF 05 BD │ FC 06 FF 02 │ FC 03 BE BF │ BD FC 06 FF │ 04 FC 02 FF  ....
*/
/**
 *    #  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
 * 4000 7C 15 5C 03 83 02 00 06 1B A7 01 60 C9 06 E1 C9
 * 4010 7C 15 CD BE 00 FA FF CF A6 40 A7 40 A8 40 35 40
 * 4020 42 64 0B 02 75 02 01 00 B4 02 AD 02 A8 40 FF F7
 * 4030 A0 40 00 EB CF 00 00 00 00 00 00 00 33 00 00 00
 * 4040 00 00 00 00 00 00 00 C0 F9 6B 20 18 05 00 00 00
 * 4050 00 3C 2D 20 04 20 6B 20 3C 1C 00 00 00 00 00 00
 * 4060 4F 6B 20 48 6F 6D 65 4C 61 62 20 42 41 53 49 43
 * 4070 20 72 65 76 2E 33 2E 31 65 72 00 00 00 00 00 00
 * 4080 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 * 4090 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 */
/*
     crc_write_word( 0x6000, htp );                           // 4002-4003   : 0306<-><------>// Input vektor. A LOAD parancs befejezése után az itt megadott címre ugrik
     crc_write_word( 0x0534, htp );                           // 4004-4005   : 0534<-><------>// Output vektor
     crc_write_word( 0xE001, htp );                           // 4006-4007   : E001<-><------>// Start of alpha
     crc_fputc( 0xC8, htp );                                  // 4008        : C8<-><------>// DL
     crc_fputc( 0x00, htp );                                  // 4009        : 00<-><------>// GL
     crc_fputc( 0xC3, htp );                                  // 400A        : C3<-><------>// AL
     crc_fputc( 0xC5, htp );                                  // 400B        : C5<-><------>// ???
    crc_write_word( 0x06C9, htp );                            // 400C-400D   : C9 06 <-><------>// Interrupt vector
    crc_write_word( 0xC9E1, htp );                            // 400E-400F   : E1 C9 <-><------>// Input vektor
    crc_write_word( 0x7C15, htp );                            // 4010-4011   : 15 7C <-><------>// Print vektor
    crc_write_word( 0xBECD, htp );                            // 4012-4013   : CD BE <-><------>// Monitor pointer
    crc_write_word( 0xFA00, htp );                            // 4013-4014   : 00 FA <-><------>// Cursor position in RAM ( C001+0x28*Y + X )
*/

    // 16K alatti BASIC esetén 7FFF, 32K BASIC esetén, ami amúgy sem fér el 16K-n, lehet CFFF, 64K-nál meg FFFF
    crc_write_word( 0x7FFF, htp )   ;                         // 4016 - 4017 : FF CF <-><------>// HM                ||| FF FF 
    long int dcb18_position = ftell( htp );                     // Programvégemutatók kezdete. Ide fogunk visszatölteni (6 byte) a konvertálás után, ha már tudjuk a méretet
    // Write 0x4018-0x40A0 DCB18- placeholder                   //
    for( int i=0; i<6; i++ ) fputc( 0, htp );       // Placeholder for:
                                                                // 4018-4019   :    0<-><------>// End of BASIC text
                                                                // 401A-401B   :    0<-><------>// End of Variable table
                                                                // 401C-401D   :    0<-><------>// End of string space 1

//      crc_write_word( 0x4035, htp );                           //* 401E-401F   : 00 D7 <-><------>// Primary acc     ||| 35 40
//      crc_write_word( 0x6042, htp );                           //* 4020-4021   : 42 64 <-><------>// Primary acc     ||| 42 60
    for( int i=0x401E; i<0x4022; i++ ) crc_fputc( 0x00, htp );

//     crc_write_word( 0x020B, htp );                           // 4022-4023   : 0B 02 <-><------>// Auxilary stack 
//     crc_write_word( 0x0275, htp );                           // 4024-4025   : 75 02 <-><------>// Auxilary stack 
//     crc_write_word( 0x0001, htp );                           // 4026-4027   : 01 00 <-><------>// Auxilary stack 
//     crc_write_word( 0x02B4, htp );                           // 4028-4029   : B4 02 <-><------>// Auxilary stack 
//     crc_write_word( 0x02AD, htp );                           // 402A-402B   : AD 02 <-><------>// Auxilary stack 
//     crc_write_word( 0x40A8, htp );                           // 402C-402D   : A8 40<-><------>// End of string space 2
    for( int i=0x4022; i<0x402E; i++ ) crc_fputc( 0x00, htp );

    crc_write_word( 0x7FFF, htp );                           // 402E-402F   : FF F7 <-><------>// Current BASIC Line
    crc_write_word( 0x40A0, htp );                           // 4030 - 4031 : A0 40 <----><------>// Start of BASIC program

      crc_fputc( 0x00, htp );                                  //* 4032        :   00<-><------>// BASIC FLAG
      crc_write_word( 0xCFEB, htp );                           //* 4033-4034   : EB CF <-><------>// Auxiliary stack

    // crc_write_word( 0x41A3, htp );                           // 4035-4036   : 41A3<-><------>// Statement pointer for CONT
    // crc_write_word( 0x40A0, htp );                           // 4037-4038   : 40A0<-><------>// Next item for READ
    // crc_write_word( 0x00DC, htp );                           // 4039-403A   : 00DC<-><------>// Current line for CONT
    // crc_write_word( 0x3300, htp );                           // 403B-403C   : 33 00<-><------>// Random number
    // crc_write_word( 0x0000, htp );                           // 403D-403E   : 00 00<-><------>// Random number
    // crc_fputc( 0*0x1D, htp );                              // 403F-40     :   0<---><------>// RST 7
    // crc_write_word( 0x7FA4, htp );                         // 4041-4042   :   0<-><------>// free
    // crc_write_word( 0, htp );                              // 4043-44     :   0<---><------>// RST 6
    // crc_write_word( 0, htp );                              // 4045-46     :   0<---><------>// free

    for( int i=0x4035; i<0x4047; i++ ) crc_fputc( 0x01, htp );
    crc_write_word( 0xF9C0, htp );                              // 4047-48     : C0 F9 <---><------>// Display pointer
    crc_write_word( 0x206B, htp );                            // 4049 - 404A : 6B 20 <------>// Error vector
    crc_fputc( 0*0x18, htp );                                 // 404B        : 18<---><------>// Interrupt flags
    crc_fputc( program_protection_404C, htp );                // 404C        : 05<---><------>// Security flag : Programvédelem : 0 esetén védi a programot, !=0 esetén nem védi

//     crc_write_word( 0*0x0, htp );                              // 404D - 404E :    0 // ???
//     crc_fputc( 0*0xD1, htp );                              // 404F        :   D1<---><------>// Error code
//     crc_write_word( 0*0x00C3, htp );                           // 4050 - 4051 : 00 C3 <------>// Line number at Error
    for( int i=0x404D; i<0x4052; i++ ) crc_fputc( 0x00, htp );

    crc_write_word( 0x202D, htp );                           // 4052 - 4053 : 2D 20 <----><------>// Vector for BASIC interpreter.
    crc_write_word( 0x2004, htp );                           // 4054 - 4055 : 04 20 <----><------>// Vector for listing
    crc_write_word( 0x206B, htp );                           // 4056 - 4057 : 6B 20 <----><------>// Vector for Error
    crc_write_word( 0x1C3C, htp );                           // 4058 - 4059 : 3C 1C <----><------>// Start of token table
    for( int i=0x405A; i<0x40A0; i++ ) crc_fputc( 0x00, htp ); // 4058 - 406F :    0<--><------>// Stack for display generator   // 4058 : E9 FF E9 FB E9 F7 E9 F3 E9 EF E9 EB E9 E7 E9 E3 85 4D F2 02 76 4D 04 60

    encodeBasicFrom40A0( htp, txt, BAS );

    // Next the BASIC close line. Size is 5 bytes: 7F FF 91 3A 98 60
    crc_fputc( 0x7F, htp );
    crc_fputc( 0xFF, htp );
    crc_fputc( 0x91, htp );
    crc_fputc( 0x3A, htp );
    crc_fputc( 0x98, htp );
    crc_fputc( 0x60, htp );
    // Empty varyables tables:
    crc_fputc( 0x80, htp ); // <- 0x4018
    crc_fputc( 0x60, htp ); // <- 0x401A
    crc_fputc( 0x20, htp ); // <- 0x401C
    uint16_t end_address = load_address + htp_get_byte_counter() - 1 + 4; // Ez itt a jó érték, de miért is?
// printf( "!!! 4018 : %04X\n", end_address );
    // Back to dcb header
    fseek( htp, dcb18_position, SEEK_SET );
    crc_write_word( end_address, htp );		// 4018 Write DCB+0x18<----->// Start variables
    crc_write_word( end_address+1, htp );	// 401A Write DCB+0x1A<--->// Start string variables
    crc_write_word( end_address+2, htp );	// 401C Write DCB+0x1C<--->// End of string space
}
