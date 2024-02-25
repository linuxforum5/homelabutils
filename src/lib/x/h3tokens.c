/**
 * Homelab 3 basic specific data
 */

/**
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
0x44, 0xe4, 0x43, 0xee, 0x50, 0xf0, 0x4f, 0xe4, 0x55, 0xe6, 0x53, 0xee, 0x54, 0xed, 0x55, 0xf3,0x2f, 0xb0, 0x49, 0xf1, 0x4f, 0xed, 0x4f, 0xf6, 0x53, 0xec, 0x42, 0xf3, 0x4f, 0xeb
};

void list_tokens() {
    int code=128;
    int i=0;
    while( i<sizeof(tokens) ) {
        printf( "%d.\t%02X %d\t'", i, code, code );
        while( ( !(tokens[i] & 128) ) && ( i<sizeof(tokens) ) ) printf( "%c", tokens[i++] );
        printf( "%c' (%d)\n", tokens[i] & 127, tokens[i] );
        i++;
        code++;
    }
}

void write_htp_BCD4018( FILE *htp, FILE *txt, uint16_t load_address, FILE *BAS ) {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// BCD : from 0x4001 to 0x409F
    /// Write DCB header from 0x4018 to 0x409F
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
     crc_write_word( 0x0000, htp );                           // 4000-4001   : <-><------>// USR függvény kezdőcíme
     crc_write_word( 0x6000, htp );                           // 4002-4003   : 0306<-><------>// Input vektor. (A LOAD parancs befejezése után az itt megadott címre ugrik?)
     crc_write_word( 0x0534, htp );                           // 4004-4005   : 0534<-><------>// Output vektor
     crc_write_word( 0xE001, htp );                           // 4006-4007   : E001<-><------>// F1/F2 vector
     crc_fputc( 0xC8, htp );                                  // 4008        : C8<-><------>// SHIFT ALERT flag
     crc_fputc( 0x00, htp );                                  // 4009        : 00<-><------>// SCROLL atop flag
     crc_fputc( 0xC3, htp );                                  // 400A        : C3<-><------>// 32/64 betű flag
     crc_fputc( 0xC5, htp );                                  // 400B        : C5<-><------>// Buffer pointer
     crc_write_word( 0x1DC5, htp );                           // 400C-400D   : 1DC5<-><------>// RST 1 vector
     crc_write_word( 0x157C, htp );                           // 400E-400F   : 157C<-><------>// Input vektor
     crc_write_word( 0x7C15, htp );                           // 4010-4011   : 157C<-><------>// Print vektor
     crc_write_word( 0x0000, htp );                           // 4012-4013   : 0000<-><------>// Monitor pointer
     crc_write_word( 0xC119, htp );                           // 4013-4014   : c119<-><------>// Cursor position in RAM ( C001+0x28*Y + X )
*/
//    crc_write_word( 0x8000, htp )   ;                           // 4016 - 4017 : 8000<-><------>// HM
    long int dcb18_position = ftell( htp );                     //
    // Write 0x4018-0x40A0 DCB18- placeholder                   //
    for( int i=0; i<6; i++ ) fputc( 0, htp );       // Placeholder for:
                                                                // 4018-4019   :    0<-><------>// Start variables
                                                                // 401A-401B   :    0<-><------>// Start string variables
                                                                // 401C-401D   :    0<-><------>// End of string space

//     crc_write_word( 0x0004, htp );                           // 401E-401F   : 0004<-><------>// Primary acc
//     crc_write_word( 0x4A85, htp );                           // 4020-4021   : 4A85<-><------>// Primary acc
    for( int i=0x401E; i<0x4022; i++ ) crc_fputc( 0x00, htp );
!     crc_write_word( 0xFE9B, htp );                           // 4022-4023   : FE9B<-><------>// Random number Ennek kell, hogy értéke legyen, különben a következő véletlen szám is 0 lesz
!     crc_write_word( 0x2F71, htp );                           // 4024-4025   : 2F71<-><------>// Random number

//     crc_write_word( 0xFEFE, htp );                           // 4026-4027   : FEFE // ???
//     crc_write_word( 0x0009, htp );                           // 4028-4029   : 0009<-><------>// Auxiliary Accumulator
//     crc_write_word( 0x8133, htp );                           // 402A-402B   : 8133<-><------>// Auxiliary Accumulator
//     crc_write_word( 0x5D4F, htp );                           // 402C-402D   : 5DF4<-><------>// Temporary register
    for( int i=0x401E; i<0x4026; i++ ) crc_fputc( 0x00, htp );

    crc_write_word( 0x7FFF, htp );                           // 402E-402F   : 7FFF<-><------>// Current BASIC Line
    crc_write_word( 0x40A0, htp );                           // 4030 - 4031 : 40A0<----><------>// Start of BASIC program

//    crc_fputc( 0x00, htp );                                  // 4032        :   01<-><------>// BASIC FLAG 5.bit=CR
//     crc_write_word( 0xFEFE, htp );                           // 4033-4034   : FEFE<-><------>// Start of graphics
//     crc_write_word( 0x41A3, htp );                           // 4035-4036   : 41A3<-><------>// Statement pointer for CONT
//     crc_write_word( 0x40A0, htp );                           // 4037-4038   : 40A0<-><------>// Next item for READ
//     crc_write_word( 0x00DC, htp );                           // 4039-403A   : 00DC<-><------>// Current line for CONT
?     crc_write_word( 0x8000, htp );                           // 403B-403E   : 8000<-><------>// Random number
    for( int i=0x4032; i<0x403D; i++ ) crc_fputc( 0x00, htp );
?    crc_fputc( 0x80, htp );                                     // 403D        :   80<><------>// HM Page : HM high byte

?   crc_fputc( BASIC_interrupt_403E, htp );                     // 403E        :    0<------><------>// BASIC interrupt és autostart? 1=tilt, 0=engedélyez

//     crc_fputc( 0*0x1D, htp );                              // 403F        :   1D<---><------>// Counter for cursor flashing
//     crc_fputc( 0*0x00, htp );                              // 4040        :    0 // ???
//     crc_write_word( 0x7FA4, htp );                           // 4041-4042   : 7FA4<-><------>// Auxiliary stack pointer
//     crc_fputc( 0*0x01, htp );                              // 4043        :   01<---><------>// RST 8
//     crc_fputc( 0*0x00, htp );                              // 4044        :    0<---><------>// RST 8
//     crc_fputc( 0*0x00, htp );                              // 4045        :    0<---><------>// RST 8
//     crc_fputc( 0*0x00, htp );                              // 4046        :    0<---><------>// RST 10
//     crc_fputc( 0*0x01, htp );                              // 4047        :   01<---><------>// RST 10
//     crc_fputc( 0*0x00, htp );                              // 4048        :    0<---><------>// RST 10
    for( int i=0x403F; i<0x4049; i++ ) crc_fputc( 0x00, htp );
     crc_write_word( 0x1581, htp );                           // 4049 - 404A : 1581 vagy 1582?<------>// Error vector
?     crc_fputc( 0*0x0D, htp );                              // 404B        :   0D<---><------>// Previous character

    crc_fputc( program_protection_404C, htp );                // 404C        :   10<---><------>// Security flag : Programvédelem 00=védi a programot, 10=nem védi

//     crc_write_word( 0*0x0, htp );                              // 404D - 404E :    0 // ???
//     crc_fputc( 0*0xD1, htp );                              // 404F        :   D1<---><------>// Error code
//     crc_write_word( 0*0xC350, htp );                           // 4050 - 4051 : C350><------>// Line number at Error
    for( int i=0x404D; i<0x4052; i++ ) crc_fputc( 0x00, htp );

    crc_write_word( 0x16B2, htp );                           // 4052 - 4053 : 0CA9<----><------>// Vector for BASIC interpreter.
    crc_write_word( 0x0167, htp );                           // 4054 - 4055 : 0E18<----><------>// Vector for listing
    crc_write_word( 0x1581, htp );                           // 4056 - 4057 : 1581<----><------>// Vector for Error              // 4056 : 81 15.
    crc_write_word( 0x1C3C, htp );                           // 4058 - 4059 : 1C3C<----><------>// Start of basic TOKENs
    for( int i=0x405A; i<0x40A0; i++ ) crc_fputc( 0x00, htp ); // 4058 - 406F :    0<--><------>// Stack for display generator   // 4058 : E9 FF E9 FB E9 F7 E9 F3 E9 EF E9 EB E9 E7 E9 E3 85 4D F2 02 76 4D 04 60
}

    encodeBasicFrom40A0( htp, txt, BAS );

    // Next the BASIC close line. Size is 3 bytes:
    crc_fputc( 0x80, htp );
    crc_fputc( 0x60, htp );
    crc_fputc( 0x0, htp );
    uint16_t end_address = load_address + htp_get_byte_counter() - 1 + 4;

void write_htp_BCD_2_4018( FILE *htp, FILE *txt, uint16_t load_address, FILE *BAS ) {
    // Back to dcb header
    fseek( htp, dcb18_position, SEEK_SET );
    crc_write_word( end_address, htp );         // 4018 Write DCB+0x18<----->// Start variables
    crc_write_word( end_address+1, htp );       // 401A Write DCB+0x1A<--->// Start string variables
    crc_write_word( end_address+2, htp );       // 401C Write DCB+0x1C<--->// End of string space
// B644 B744 B844
// BC44 E744 E844

// B240 B340 B440 -> B840 B940 BA40
}
