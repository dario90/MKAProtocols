# include "stm32f4d_helper.h"
# include "polarssl/entropy.h"

void PUT32 ( unsigned int, unsigned int );
void PUT16 ( unsigned int, unsigned int );
void PUT8 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
unsigned int GET16 ( unsigned int );

volatile int counter = 0;
volatile int writer = 0; 
volatile int reader = 0;
volatile int uart_buf[UART_BUF_MAX_DIM];

void clock_init ( void )
{
    unsigned int ra;

    //enable HSE
    ra=GET32(RCC_CR);
    ra&=~(0xF<<16);
    PUT32(RCC_CR,ra);
    ra|=1<<16;
    PUT32(RCC_CR,ra);
    while(1)
    {
        if(GET32(RCC_CR)&(1<<17)) break;
    }
    PUT32(RCC_CFGR,0x0000D801); //PPRE2 /2 PPRE1 /8 sw=hse
    //slow flash accesses down otherwise it will crash
    PUT32(FLASH_ACR,0x00000105);
    //8MHz HSE, 168MHz pllgen 48MHz pll usb
    //Q 7 P 2 N 210 M 5 vcoin 1 pllvco 336 pllgen 168 pllusb 48
    ra=(7<<24)|(1<<22)|(((2>>1)-1)<<16)|(210<<6)|(5<<0);
    PUT32(RCC_PLLCFGR,ra);
    // enable pll
    ra=GET32(RCC_CR);
    ra|=(1<<24);
    PUT32(RCC_CR,ra);
    //wait for pll lock
    while(1)
    {
        if(GET32(RCC_CR)&(1<<25)) break;
    }
    //select pll
    PUT32(RCC_CFGR,0x0000D802); //PPRE2 /2 PPRE1 /8 sw=pllclk
    //if you didnt set the flash wait states you may crash here
    //wait for it to use the pll
    while(1)
    {
        if((GET32(RCC_CFGR)&0xC)==0x8) break;
    }
 }

// initialize random number generator
// RNG only present on stm32f4d07
void rnd_init ( void )
{
    unsigned int ra;

    ra=GET32(RCC_AHB2ENR);
    ra|=1<<6;
    PUT32(RCC_AHB2ENR,ra);

    ra=GET32(RNG_CR);
    ra|=4;
    PUT32(RNG_CR,ra);
}

void init() {
	unsigned int ra;

	ra=GET32(RCCBASE+0x30);
    ra|=1<<3; //enable port D
    PUT32(RCCBASE+0x30,ra);

	/*
    ra=GET32(RCCBASE+0x40);
    ra|=1<<3; //enable TIM5
    PUT32(RCCBASE+0x40,ra);

    PUT16(TIM5BASE+0x00,0x0000);
    PUT16(TIM5BASE+0x00,0x0001);
	*/

    //d12 = d15 output
    ra=GET32(GPIODBASE+0x00);
    ra&=0x00FFFFFF;
    ra|=0x55000000;
    PUT32(GPIODBASE+0x00,ra);
    //push pull
    ra=GET32(GPIODBASE+0x04);
    ra&=0xFFFF0FFF;
    PUT32(GPIODBASE+0x04,ra);
}

void uart1_init() {
	unsigned int ra;

	ra=GET32(RCC_AHB1ENR);
    ra|=1<<1; //enable port B
    PUT32(RCC_AHB1ENR,ra);

	ra=GET32(RCC_APB2ENR);
    ra|=1<<4; //enable USART1
    PUT32(RCC_APB2ENR,ra);

    //PB6 USART1_TX
    //PB7 USART1_RX --disabled--
	
	ra=GET32(GPIOB_MODER);
    ra|= (2<<12);
    //ra|= (2<<14);
    PUT32(GPIOB_MODER,ra);
    ra=GET32(GPIOB_OTYPER);
    ra&=(1<<6);
    //ra&=(1<<7);
    PUT32(GPIOB_OTYPER,ra);
    ra=GET32(GPIOB_AFRL);
    ra|=(7<<24);
    //ra|=(7<<28);
    PUT32(GPIOB_AFRL,ra);

    // divisor 136 fractional divisor 11
    PUT32(USART1_BRR,(136<<4)|(11<<0));
    //PUT32(USART1_CR1,(1<<13)|(1<<3)|(1<<2));
	PUT32(USART1_CR1,(1<<13)|(1<<3));
}

void uart6_init() {
	unsigned int ra;

	ra=GET32(RCC_AHB1ENR);
    ra|=1<<2; //enable port C
    PUT32(RCC_AHB1ENR,ra);

	ra=GET32(RCC_APB2ENR);
    ra|=1<<5; //enable USART6
    PUT32(RCC_APB2ENR,ra);

    //PC7 USART6_RX 
	
	ra=GET32(GPIOC_MODER);
    ra|= (2<<14);
    PUT32(GPIOC_MODER,ra);
    ra=GET32(GPIOC_OTYPER);
    ra&=(1<<7);
    PUT32(GPIOC_OTYPER,ra);
    ra=GET32(GPIOC_AFRL);
    ra|=(8<<28);
    PUT32(GPIOC_AFRL,ra);

    // divisor 136 fractional divisor 11
    PUT32(USART6_BRR,(136<<4)|(11<<0));
	PUT32(USART6_CR1,(1<<13)|(1<<2)|(1<<5));
	PUT32(0xE000E108,0x00000080); // enable USART6 interrupt
}

int uart_init () {
    uart1_init();
	uart6_init();
	return 0;
}

void uart6_handler() {
	int data,sr;

	sr = GET32(USART6_SR);
	if (sr & (1<<3))
	{
		turnOnLed(RED);
		while (1) {}
	}
	else {
		data = GET32(USART6_DR);
		if (counter < UART_BUF_MAX_DIM) {
			if ((writer < UART_BUF_MAX_DIM)) {
				uart_buf[writer] = data;
				writer++; 
			}
			else {
				writer = 0;
				uart_buf[writer] = GET32(USART6_DR);
			}
			counter++;
		}
	}
}

void enableUserButton() {
	unsigned int ra;

	ra = GET32(RCCBASE+0x30);
    ra |= 1; //enable port A
    PUT32(RCCBASE+0x30,ra);

	ra = GET32(GPIOABASE+0x00);
    ra &= 0xFFFFFFFB;
    PUT32(GPIOABASE+0x00,ra);
}

void checkUserButton() {
	while (!(GET32(GPIOABASE+0x10) & 0x00000001))
		continue;
}

void timdelay ( void )
{
	unsigned int ra;
	unsigned int rb;
	rb=GET32(TIM5BASE+0x24);
	while(1)
	{
		ra=GET32(TIM5BASE+0x24);
		if((ra-rb)>=(168000000*4)) break;
	}
}

int random(void *data, unsigned char *output, unsigned int len, unsigned int *olen) 
{
	unsigned int ra;

    while(1)
    {
        ra=GET32(RNG_SR);
        if(ra & 1) break;
        if(ra & 0x66)
        {
			turnOnLed(RED);
            return(POLARSSL_ERR_ENTROPY_SOURCE_FAILED);
        }
    }
    if(ra & 0x66)
    {
		turnOnLed(RED);
        return(POLARSSL_ERR_ENTROPY_SOURCE_FAILED);
    }
	ra = GET32(RNG_DR);
	if ((int) len <= 4) {
		memcpy(output,&ra,len);
		*olen = len;
	}
	else {
		memcpy(output,&ra,4);
		*olen = 4;
	}
    return(0);	
}

int random2(void *data, unsigned char *output, unsigned int len, unsigned int *olen) 
{
    int i = 0;
    char random_bytes[4] = {0xE3,0xA5,0x76,0xDC};

	while ((i < 4) && (i < (int) len)) {
		output[i] = random_bytes[i];
		i++;
	}
	
	*olen = (size_t) i;
	return 0;
}

void turnOnLed(int led) {
	switch (led) {
		case GREEN:
			PUT32(GPIODBASE+0x18,0xE0001000);
			break;
		case RED:
			PUT32(GPIODBASE+0x18,0xB0004000);
			break;
		case ORANGE:
			PUT32(GPIODBASE+0x18,0xD0002000);
			break;
		case BLUE:
			PUT32(GPIODBASE+0x18,0x70008000);
			break;
	}
}

void uart_putc ( unsigned int x )
{
    while (( GET32(USART1_SR) & (1<<7)) == 0) continue;
    PUT32(USART1_DR,x);
}

unsigned int uart_getc ( void )
{
	/*
	unsigned int sr,val;

    while (1) {
        sr = GET32(USART6_SR);
        if (sr & (1 << 5)) {
			if ((sr & (1 << 1)) || (sr & (1 << 4))) {
				GET32(USART6_DR);
				continue;
			}
            else {
				val = GET32(USART6_DR);
                return(val);
			}
        }
    }
	*/
	int data;	

	while (counter == 0) continue;
	data = uart_buf[reader];
	reader++;
	if (reader == UART_BUF_MAX_DIM) reader = 0;
	counter--;	
	return data;
}

void uart_string (unsigned char *s,unsigned char len)
{
	unsigned char *cur = s;

    while ((cur-s) < len) {
        uart_putc(*cur);
		cur++;
	}
}

void uart_getstring(unsigned char *buf,unsigned int len) {
	int i = 0;
	unsigned int rec;
	
	while (i < len) {
		rec = uart_getc();
		//if ((rec != 0x00) && (rec != 0xFF)) {
			buf[i] = rec;
			i++;
         
		//}
	}
}

void *memcpy(void *dest, const void *src, unsigned int n)
{
	char *dp = dest;
    const char *sp = src;
    while (n--)
        *dp++ = *sp++;
    return dest;
}

unsigned int strlen(const char *s) 
{
    const char *p = s;
    while (*s) ++s;
    return s - p;
}

void *memset(void *s, int c, unsigned int n)
{
    unsigned char* p=s;
    while(n--)
        *p++ = (unsigned char)c;
    return s;
}

static const unsigned char charmap[] = {
	'\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
	'\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
	'\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
	'\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
	'\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
	'\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
	'\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
	'\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
	'\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	'\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
	'\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	'\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
	'\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
	'\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
	'\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
	'\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
	'\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
	'\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
	'\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
	'\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
	'\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
	'\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
	'\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
	'\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
	'\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
	'\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
	'\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
	'\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};

int strcasecmp(const char *s1, const char *s2)
{
	register const unsigned char *cm = charmap, 
		*us1 = (const unsigned char *)s1, *us2 = (const unsigned char *)s2;

	while (cm[*us1] == cm[*us2++])
		if (*us1++ == '\0')
			return (0);
	return (cm[*us1] - cm[*--us2]);
}
