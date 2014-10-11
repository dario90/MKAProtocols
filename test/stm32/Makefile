ARMGNU = arm-none-eabi

COPS = -I../../include -ggdb -Wall -nostdlib -nostartfiles -ffreestanding

OBJS = ../../lib/entropy.gcc.thumb.o	../../lib/entropy_poll.gcc.thumb.o	\
../../lib/ctr_drbg.gcc.thumb.o	../../lib/sha512.gcc.thumb.o	../../lib/aes.gcc.thumb.o	\
../../lib/bignum.gcc.thumb.o	../../lib/ecp.gcc.thumb.o	../../lib/ecdh.gcc.thumb.o \
../../lib/ecp_curves.gcc.thumb.o	../../lib/platform.gcc.thumb.o	\
../../lib/memory_buffer_alloc.gcc.thumb.o	../../lib/stm32f4d_helper.gcc.thumb.o	\
../../lib/util.gcc.thumb.o	../../lib/KnxTelegram.gcc.thumb.o	\
../../lib/KnxTpUart.gcc.thumb.o	../../lib/bd1.gcc.thumb.o	../../lib/bd2.gcc.thumb.o \
../../lib/mka.gcc.thumb.o

LIBGCC = /home/dario/gcc-arm-none-eabi-4_8-2014q2/lib/gcc/arm-none-eabi/4.8.4/armv7-m/libgcc.a

all: mka_test_server.gcc.thumb.bin    mka_test_client.gcc.thumb.bin	bd1_test_server.gcc.thumb.bin \
bd1_test_client.gcc.thumb.bin

vectors.o : vectors.s
	$(ARMGNU)-as vectors.s -o vectors.o

mka_test_server.gcc.thumb.o: mka_test_server.c $(OBJS)
	$(ARMGNU)-gcc $(COPS) -mthumb -c mka_test_server.c -o mka_test_server.gcc.thumb.o
mka_test_server.gcc.thumb.bin : memmap vectors.o mka_test_server.gcc.thumb.o $(OBJS)
	$(ARMGNU)-ld -o mka_test_server.gcc.thumb.elf -T memmap vectors.o mka_test_server.gcc.thumb.o  $(OBJS) $(LIBGCC)
	$(ARMGNU)-objcopy mka_test_server.gcc.thumb.elf mka_test_server.gcc.thumb.bin -O binary


mka_test_client.gcc.thumb.o: mka_test_client.c $(OBJS)
	$(ARMGNU)-gcc $(COPS) -mthumb -c mka_test_client.c -o mka_test_client.gcc.thumb.o
mka_test_client.gcc.thumb.bin : memmap vectors.o mka_test_client.gcc.thumb.o $(OBJS)
	$(ARMGNU)-ld -o mka_test_client.gcc.thumb.elf -T memmap vectors.o mka_test_client.gcc.thumb.o  $(OBJS) $(LIBGCC)
	$(ARMGNU)-objcopy mka_test_client.gcc.thumb.elf mka_test_client.gcc.thumb.bin -O binary

bd1_test_server.gcc.thumb.o: bd1_test_server.c $(OBJS)
	$(ARMGNU)-gcc $(COPS) -mthumb -c bd1_test_server.c -o bd1_test_server.gcc.thumb.o
bd1_test_server.gcc.thumb.bin : memmap vectors.o bd1_test_server.gcc.thumb.o $(OBJS)
	$(ARMGNU)-ld -o bd1_test_server.gcc.thumb.elf -T memmap vectors.o bd1_test_server.gcc.thumb.o  $(OBJS) $(LIBGCC)
	$(ARMGNU)-objcopy bd1_test_server.gcc.thumb.elf bd1_test_server.gcc.thumb.bin -O binary


bd1_test_client.gcc.thumb.o: bd1_test_client.c $(OBJS)
	$(ARMGNU)-gcc $(COPS) -mthumb -c bd1_test_client.c -o bd1_test_client.gcc.thumb.o
bd1_test_client.gcc.thumb.bin : memmap vectors.o bd1_test_client.gcc.thumb.o $(OBJS)
	$(ARMGNU)-ld -o bd1_test_client.gcc.thumb.elf -T memmap vectors.o bd1_test_client.gcc.thumb.o  $(OBJS) $(LIBGCC)
	$(ARMGNU)-objcopy bd1_test_client.gcc.thumb.elf bd1_test_client.gcc.thumb.bin -O binary

clean:
	rm *.o
	rm *.elf
	rm *.bin
