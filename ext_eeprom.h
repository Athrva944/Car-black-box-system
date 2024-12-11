#ifndef ext_eeprom
#define ext_eeprom

#define SLAVE_READ		0xA1
#define SLAVE_WRITE		0xA0

void write_ext_eeprom(unsigned char address1,  unsigned char data);
unsigned char read_ext_eeprom(unsigned char address1);
//void init_ds1307(void);

#endif