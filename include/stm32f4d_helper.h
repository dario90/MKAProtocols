// definition of registers' addresses
#define RCCBASE 0x40023800
#define RCC_CR (RCCBASE+0x00)
#define RCC_PLLCFGR (RCCBASE+0x04)
#define RCC_CFGR (RCCBASE+0x08)
#define RCC_AHB1ENR (RCCBASE+0x30)
#define RCC_APB1ENR (RCCBASE+0x40)
#define RCC_AHB2ENR (RCCBASE+0x34)
#define RCC_APB2ENR (RCCBASE+0x44)

#define GPIODBASE 0x40020C00
#define TIM5BASE 0x40000C00
#define FLASH_ACR 0x40023C00
#define GPIOABASE 0x40020000
#define GPIOA_MODER (GPIOABASE+0x00)
#define USART1_BASE 0x40011000
#define USART1_SR (USART1_BASE+0x00)
#define USART1_DR (USART1_BASE+0x04)
#define USART1_BRR (USART1_BASE+0x08)
#define USART1_CR1 (USART1_BASE+0x0C)
#define USART1_CR2 (USART1_BASE+0x10)
#define USART1_CR3 (USART1_BASE+0x14)
#define USART1_GTPR (USART1_BASE+0x18)
#define USART2_BASE 0x40004400
#define USART2_SR (USART2_BASE+0x00)
#define USART2_DR (USART2_BASE+0x04)
#define USART2_BRR (USART2_BASE+0x08)
#define USART2_CR1 (USART2_BASE+0x0C)
#define USART2_CR2 (USART2_BASE+0x10)
#define USART2_CR3 (USART2_BASE+0x14)
#define USART2_GTPR (USART2_BASE+0x18)
#define USART3_BASE 0x40004800
#define USART3_SR (USART3_BASE+0x00)
#define USART3_DR (USART3_BASE+0x04)
#define USART3_BRR (USART3_BASE+0x08)
#define USART3_CR1 (USART3_BASE+0x0C)
#define USART3_CR2 (USART3_BASE+0x10)
#define USART3_CR3 (USART3_BASE+0x14)
#define USART3_GTPR (USART3_BASE+0x18)
#define USART6_BASE 0x40011400
#define USART6_SR (USART6_BASE+0x00)
#define USART6_DR (USART6_BASE+0x04)
#define USART6_BRR (USART6_BASE+0x08)
#define USART6_CR1 (USART6_BASE+0x0C)
#define USART6_CR2 (USART6_BASE+0x10)
#define USART6_CR3 (USART6_BASE+0x14)
#define USART6_GTPR (USART6_BASE+0x18)
#define GPIOA_AFRL (GPIOABASE+0x20)
#define GPIOA_OTYPER (GPIOABASE+0x04)
#define GPIOB_BASE 0x40020400
#define GPIOB_MODER (GPIOB_BASE+0x00)
#define GPIOB_AFRL (GPIOB_BASE+0x20)
#define GPIOB_AFRH (GPIOB_BASE+0x24)
#define GPIOB_OTYPER (GPIOB_BASE+0x04)
#define GPIOB_OSPEED (GPIOB_BASE+0x08)
#define GPIOB_PUPDR (GPIOB_BASE+0x0C)
#define GPIOC_BASE 0x40020800
#define GPIOC_MODER (GPIOC_BASE+0x00)
#define GPIOC_AFRL (GPIOC_BASE+0x20)
#define GPIOC_AFRH (GPIOC_BASE+0x24)
#define GPIOC_OTYPER (GPIOC_BASE+0x04)
#define GPIOC_OSPEED (GPIOC_BASE+0x08)
#define GPIOC_PUPDR (GPIOC_BASE+0x0C)

// --- only for stm32f4d07 ---
#define RNG_BASE 0x50060800
#define RNG_CR (RNG_BASE+0x00)
#define RNG_SR (RNG_BASE+0x04)
#define RNG_DR (RNG_BASE+0x08)
// --- only for stm32f4d07 ---

// LEDS
#define GREEN   0
#define RED     1
#define ORANGE  2  
#define BLUE    3

#define UART_BUF_MAX_DIM 1000

extern volatile int reader;
extern volatile int writer;
extern volatile int counter;
extern volatile int uart_buf[UART_BUF_MAX_DIM];

// initialize the clock of the board: 8MHz HSE, 168MHz pllgen 48MHz pll usb
// specific to stm32fd07, doesn't work on stm32f4d01
void clock_init (void);

// initialize random number generator
// RNG only present on stm32f4d07
void rnd_init (void);

// initialize the four user leds (useful for debugging)
// the initialization of TIM5 is commented, 
// it will be implemented on a separate function
void init(void);

// init uart peripheral
int uart_init ();

void enableUserButton();
void checkUserButton();

void timdelay ( void );

// random function to be added as an entropy source
// generates 4 random bytes through hardware RNG and store them in output
// in case of error it returns POLARSSL_ERR_ENTROPY_SOURCE_FAILED 
int random(void *data, unsigned char *output, unsigned int len, unsigned int *olen);
int random2(void *data, unsigned char *output, unsigned int len, unsigned int *olen);

void turnOnLed(int led);

// send one byte via uart
void uart_putc (unsigned int x);

// receive one byte via uart
unsigned int uart_getc (void);

// send len byes via uart
void uart_string (unsigned char *s,unsigned char len);

// receive len bytes via uart
void uart_getstring(unsigned char *buf,unsigned int len);

// --- IMPLEMENTATION OF NEEDED STANDARD FUNCITON OF THE LIBC ---
// Functions implemented so far: memcpy, strlen, memset, strcasecmp
void *memcpy(void *dest, const void *src, unsigned int n);
unsigned int strlen(const char *str);
void *memset(void *s, int c, unsigned int n);
int strcasecmp(const char *s1, const char *s2);
