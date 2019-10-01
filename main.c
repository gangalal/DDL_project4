/*
 ===============================================================================
 Name        : project4.c
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

#define PCONP (*(volatile unsigned int *)(0x400FC0C4))
#define PCLKSEL0 (*(volatile unsigned int *)(0x400FC1A8))
#define PINSEL1 (*(volatile unsigned int *)(0x4002C004))
#define I2CPADCFG (*(volatile unsigned int *)(0x4002C07C))
#define I2C0SCLH (*(volatile unsigned int *)(0x4001C010))
#define I2C0SCLL (*(volatile unsigned int *)(0x4001C014))

#define I2C0CONSET (*(volatile unsigned int *)(0x4001C000))
#define I2C0CONCLR (*(volatile unsigned int *)(0x4001C018))
#define I2C0DAT (*(volatile unsigned int *)(0x4001C008))

const int tempReadAddr = 0x00;

void I2CInit() {
	PCONP |= (1 << 7); //resetting power control for I2C0
	PCLKSEL0 &= ~(1 << 15) | (1 << 14); // resetting clock selection
	PCLKSEL0 |= (0 << 15) | (0 << 14); //dividing CCLK by 8 *needs to review
	I2C0SCLH = 5;
	I2C0SCLL = 5;
	PINSEL1 |= (1 << 22) | (0 << 23); //enabling SDA0 on pin p0.27
	PINSEL1 |= (1 << 24) | (0 << 25); //enabling SCL0 on pin p0.28
	//I2CPADCFG |= (0<<2); //configuring I2C0 register to standard mode
	I2C0CONSET = (1 << 6);
}

void I2CStart() {
	I2C0CONSET = (1 << 3); //set SI while configuring other bits
	I2C0CONSET = (1 << 5); //set STA to start condition
	I2C0CONCLR = (1 << 3); //clear SI to active state machine
	while (((I2C0CONSET >> 3) & 1) == 0) {
		//wait for start condition to be set
	}
	I2C0CONCLR = (1 << 5); //clear STA
}

void I2CWrite(int data) {
	I2C0DAT = data;
	I2C0CONCLR = (1 << 3); //clear SI to standard data
	while (((I2C0CONSET >> 3) & 1) == 0) {
		//wait for write condition to be set
	}

	//check I2C0STAT for ack or nak
}

int I2CRead(int ack) {
	if (ack) {
		I2C0CONSET = (1 << 2);
	} else {
		I2C0CONCLR = (1 << 2);
	}
	I2C0CONCLR = (1 << 2);
	I2C0CONCLR = (1 << 3); //clear SI to start read
	while (((I2C0CONSET >> 3) & 1) == 0) {
		//wait for read condition to be set
	}

	return I2C0DAT;
}

void I2CStop() {
	I2C0CONSET = (1 << 4); //set STO to request stop condition
	I2C0CONCLR = (1 << 3); //clear SI to active state machine
	while (((I2C0CONSET >> 4) & 1) == 1) {
		//wait for stop condition to be set
	}
}

void writeToIO() {
	I2CStart();
	I2CWrite(0b01000000);
	I2Cwrite(0x00);
	I2CStop();
}

int main(void) {
	I2CInit();
	while (1) {
		I2CStart();
		printf("test\n");
		I2CWrite(0b10011011);
		printf("temperature sensor data %d\n", I2CRead(1));
		I2CStop();
	}
	return 0;
}
