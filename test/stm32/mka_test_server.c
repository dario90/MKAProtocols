# include "mka.h"
# include "polarssl/entropy.h"
# include "polarssl/memory_buffer_alloc.h"
# include "stm32f4d_helper.h"
# include "KnxTpUart.h"
# include <string.h>

extern unsigned long _data_flash;
extern unsigned long _data_begin;
extern unsigned long _data_end;
extern unsigned long _bss_begin;
extern unsigned long _bss_end;
extern unsigned long _stack_end;

# define N      2
# define LEN    66
# define area   0
# define line   0
# define member 10

int notmain() {
	unsigned int olen;
	int i,ret;
	char personalization[] = "server";
	unsigned char data_sent[LEN],data_rec[(N-1)*LEN],buf[10000];
	mka_context ctx;
	ctr_drbg_context ctr_drbg;
    entropy_context entropy;
	ecp_group_id grp_id = POLARSSL_ECP_DP_SECP256R1;
	f_source_ptr callback = &random;

	// set up dynamic memory management
	memory_buffer_alloc_init(buf,sizeof(buf));

	// initializing random number generator
	entropy_init(&entropy);
	entropy_add_source(&entropy,callback,NULL,4);
    if ((ret = ctr_drbg_init(&ctr_drbg, entropy_func, &entropy,
                               (const unsigned char *) personalization,
                               strlen(personalization))) != 0)
	{
		turnOnLed(RED);
    	entropy_free( &entropy );
		return(ret);
	}

	inizializzaKnxTpUart(area,line,member);
	setListenToBroadcasts(true);
	addListenGroupAddress(1,1,1);

	if ((ret = mka_init(&ctx,grp_id)) != 0) {
		turnOnLed(RED);
		entropy_free(&entropy);
		return (ret);
	}

	if ((ret = mka_set_part(&ctx,N,&ctr_drbg)) != 0) {
		turnOnLed(RED);
		entropy_free(&entropy);
		return (ret);
	}

	for(i = 0; i <= N-2; i++) {	
		if ((ret = mka_make_broadval(&ctx,&olen,data_sent,LEN,&ctr_drbg)) != 0) {
			turnOnLed(RED);
			entropy_free(&entropy);
			return (ret);
		}
		
		sendData(0,0,0,data_sent,LEN);
		turnOnLed(GREEN);
		receiveData(data_rec,(N-1)*LEN);
		turnOnLed(BLUE);

		if ((ret = mka_acc_broadval(&ctx,data_rec,(N-1)*LEN,&ctr_drbg)) != 0) {
			turnOnLed(RED);
			entropy_free(&entropy);
			return (ret);
		}
	}

	if ((ret = mka_compute_key(&ctx,&olen,data_sent,LEN,&ctr_drbg)) != 0) {
		turnOnLed(RED);
		entropy_free(&entropy);
		return (ret);
	}
	
	turnOnLed(GREEN);

	entropy_free(&entropy);
	mka_free(&ctx);

	return 0;
}

void handler_reset(void)
{
    unsigned long *source;
    unsigned long *destination;
    // Copying data from Flash to RAM
    source = &_data_flash;
    for (destination = &_data_begin; destination < &_data_end;)
    {
        *(destination++) = *(source++);
    }   
    // default zero to undefined variables
    for (destination = &_bss_begin; destination < &_bss_end;)
    {
        *(destination++) = 0;
    }

	// init clock, leds, rng, uart
	clock_init();
	init();
	rnd_init();
	uart_init(); 
    notmain();
}
