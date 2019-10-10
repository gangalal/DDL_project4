/*
 ===============================================================================
 Name        : midterm.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
 ===============================================================================
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>

#define FIO0DIR (*(volatile unsigned int *)0x2009C000)
#define FIO0PIN (*(volatile unsigned int *)0x2009C014)
#define PINMODE0 (*(volatile unsigned int *)0x4002C040) //remember if you use char * it won't work because we are shifting by bit 20 in line 35

int main(void) {

	FIO0DIR |= (1 << 4); // configure P0[4] as output
	FIO0DIR |= (1 << 5); // configure P0[5] as output

	FIO0DIR &= ~(1 << 10); // configure P0[10] as input
	FIO0DIR &= ~(1 << 11); // configure P0[11] as input

	FIO0PIN |= (1 << 4); //set P0[4] high
	FIO0PIN &= ~(1 << 5); //set P0[5] low

	//Pull-down resistor enabled
	PINMODE0 |= (3 << 20); //you can also do PINMODE0 |= (1<<20)|(1<<21) // here 3 is 11 i.e shifting 1 on bit 20 and 21
	//PINMODE0 |= (1<<20)| (1<<21);

	while (1) {
		/*if (((FIO0PIN >> 10) & 0x01) == 1) {
			FIO0PIN &= ~(1 << 4); //set low
		}

		if (((FIO0PIN >> 11) & 0x01) == 0) {
			FIO0PIN |= (1 << 5); //set high
		}*/ //just to test

		//problem c
		if ((((FIO0PIN >> 10) & 0x01) == 1) && (((FIO0PIN >> 11) & 0x01) == 0)) {
			FIO0PIN &= ~(1 << 4); //set low
		}

	}
	return 0;
}
