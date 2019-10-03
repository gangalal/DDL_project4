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

const int tempReadAddr = 0b10011011;
int displayA = 0;
int displayB = 0;

const int IODIRA = 0x00;
const int IODIRB = 0x01;
const int GPIOA = 0x12;
const int GPIOB = 0x13;

//const int A = 0x00;

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

void writeToExpander() {
	I2CStart();
	I2CWrite(0b01000000);
	I2CWrite(IODIRA); //write to the IODIR
	I2CWrite(0x00); //writing 0
	I2CWrite(IODIRB);
	I2CWrite(0x00);
	I2CWrite(GPIOA);
	I2CWrite(0b11111111);
	I2CWrite(GPIOB);
	I2CWrite(0b11111111);

	I2CStart();
	I2CWrite(0b01000001);
	int data = I2CRead(0);
	printf("expander %d\n", data);
	I2CStop();
}

void displayDigit(int d) {
	switch (d) {
	case 0:
		displayA = 0x3F;
		break; //display 0
	case 1:
		displayA = 0x06;
		break; //display 1
	case 2:
		displayA = 0x5B;
		break;
	case 3:
		displayA = 0x4F;
		break;
	case 4:
		displayA = 0x66;
		break;
	case 5:
		displayA = 0x6D;
		break;
	case 6:
		displayA = 0x7D;
		break;
	case 7:
		displayA = 0x07;
		break;
	case 8:
		displayA = 0x7F;
		break;
	case 9:
		displayA = 0x6F;
		break;
	default:
		displayA = 0;

	}
}

int main(void) {
	I2CInit();
	while (1) {
		I2CStart();
		printf("test\n");
		I2CWrite(0b10011011);
		printf("temperature sensor data %d\n", I2CRead(1));
		I2CStop();
		writeToExpander();

	}
	return 0;
}
