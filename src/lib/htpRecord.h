#include <stdint.h>

/**
 * A HTP fájl szerkezete:
 * - Első htp rekord
 *     256 x 0 bájt. ha az első, és legalább 128 bájt, ha a későbbi. Mondjuk legyen 200.
 *     0xA5
 *     név karakterek, maximum 65, de lehet 0 darab is, azonban nem tartalmazhat 0 értékű bájtot
 *     név rekordot lezáró 0 bájt
 *     loadAddress low
 *     loadAddress high
 *     byte_counter low
 *     byte_counter high
 *     /// Innentől kezdődően a kiírt karakterek értékét egy CRC-ben összegezzük, aminek az alsó bájtját a végén kiírjuk
 *     bytes payload
 *     crc byte
 *     0, ha ez volt az utolsó rekord, és 1, ha jönnek még tovűábbi rekordok
 * - Következő rekord
 *     kevesebb bevezető 0, és a név lehet üres, de a lezáró 0 kell. különben ugyanaz
 */

typedef struct { // AS név nem tartozik a htp definícióhoz, mivel csak az elsőnek van. Ez a fájl tulajdonsága. Ugyanígy, hogy első vagy sem, az sem tartozik ide.
    uint16_t load_address;
    uint16_t size;
    unsigned char type; // B : basic, R : Ram
    FILE *src;          // A forrásfájl. Nem feltétlen van. Például automatikusan generált BASIC loader esetén lehet, hogy nincs is.
} HtpRecordDef;

void write_htp_header( FILE *htp, int counter, HtpRecordDef htpRecord, const char *htp_name );
void crc_fputc( char byte, FILE *f );
void crc_write_word( uint16_t word, FILE *htp );
void close_fix_htp_block( FILE *htp, int is_last_block ); // crc and next block bytes only
void close_htp_block( FILE *htp, int is_last_block ); // with fseek
void close_htp_file( FILE *htp );
// void write_htp_block_name( FILE *htp, const char *htp_name );
void htp_dec_crc_and_byte_counter( unsigned char byte );
uint16_t htp_get_byte_counter();
