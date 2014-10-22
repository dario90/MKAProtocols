# include "stm32f4d_helper.h"
# include "KnxTpUart.h"

# define CRYP_BASE  0x50060000
# define CRYP_CR    (CRYP_BASE+0x00)
# define CRYP_SR    (CRYP_BASE+0x04)
# define CRYP_DIN   (CRYP_BASE+0x08)
# define CRYP_DOUT  (CRYP_BASE+0x0C)
# define CRYP_K3RR  (CRYP_BASE+0x3C)
# define CRYP_IV0LR (CRYP_BASE+0x40)

unsigned char key[16] = "dario secretkey";

void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );

void init_crypt();

int notmain() {
	int i;
	unsigned int ra;
	unsigned int data[4] = {1,2,3,4};
	unsigned char *p,enc_data[16];
	
	init_crypt();
	turnGPIO(PORTD,12,ON);
	
	// enable crypto processor
	ra = GET32(CRYP_CR);
	ra |= 1<<15;
	PUT32(CRYP_CR,ra);

	for(i = 0; i < 4; i++) {
		PUT32(CRYP_DIN,data[i]);
	}

	p = enc_data;
	for (i = 0; i < 4; i++) {
		while (GET32(CRYP_SR) & (1<<2)) {continue;};
		ra = GET32(CRYP_DOUT);
		memcpy(&ra,&p,4);
		p += 4;
	}
	
	sendData(0,0,0,enc_data,16,UART1);
	turnGPIO(PORTD,12,OFF);

	return 0;
}

void init_crypt() {
	unsigned int ra;

	ra=GET32(RCC_AHB2ENR);
    ra|=1<<4;
    PUT32(RCC_AHB2ENR,ra);

	// set up key size
	ra = GET32(CRYP_CR);
	ra &= 0xFFFFFCFF;
	PUT32(CRYP_CR,ra);
	
	// write key value
	memcpy((void *) CRYP_K3RR,key,sizeof(key));

	// configure datatype (byte), direction (encryption) and mode (CBC)
	ra = GET32(CRYP_CR);
	ra |= (5<<3)|(2<<6);
	PUT32(CRYP_CR,ra);

	// set up initialization vector 
	memset((void *) CRYP_IV0LR,0,16);

	// flush in and out fifos
	ra = GET32(CRYP_CR);
	ra |= 1<<14;
	PUT32(CRYP_CR,ra);
}
