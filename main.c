/*
 ===============================================================================
 Name        : Assignment_4_I2C.c
 Author      : $(Daniel Glover, Lama Dong, Jessic)
 Version     : V1
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
#define I2C0SCLH (*(volatile unsigned int *)(0x4001C010))
#define I2C0SCLL (*(volatile unsigned int *)(0x4001C014))

#define I2C0CONSET (*(volatile unsigned int *)(0x4001C000))
#define I2C0CONCLR (*(volatile unsigned int *)(0x4001C018))
#define I2C0DAT (*(volatile unsigned int *)(0x4001C008))

const int tempReadAddr = 0b10011011; //both sensor has same address
int flagTemp = 0; //boolean to convert F to C
int flagSensor = 0; //boolean to read 2 sensors

void I2CInit();
void I2CStart();
void I2CWrite(int data);
int I2CRead(int ack);
void I2CStop();
void wait_ticks(unsigned long count);
int displayDigit(int d);
int convertToF(int temp);
int readTempSensorOne();
int readTempSensorTwo();
void regWrite(int slaveAddress, int regAddress, int data);
void writeToExpander(int digitA, int digitB);
int readSwitchOne();
int readSwitchTwo();
void disablePullUps();
void manipulation();


/**
 * the main function
 */
int main(void) {
	I2CInit();
	disablePullUps();
	while (1) {
		manipulation();
		wait_ticks(10000);
	}
	return 0;
}


/**
 *  I2C initialize
 */
void I2CInit() {
	PCONP |= (1 << 7); //resetting power control for I2C0
	PCLKSEL0 &= ~(1 << 15) | (1 << 14); // resetting clock selection
	PCLKSEL0 |= (0 << 15) | (0 << 14); //PCLK_peripheral = CCLK/4 = 4M/4
	// set high and low clk time for 1MHz/10 = 100kHz
	I2C0SCLH = 5;
	I2C0SCLL = 5;
	PINSEL1 |= (1 << 22) | (0 << 23); //enabling SDA0 on pin p0.27
	PINSEL1 |= (1 << 24) | (0 << 25); //enabling SCL0 on pin p0.28
	I2C0CONSET = (1 << 6); // I2C interface enable from table 385
}


/**
 * I2C Start Function
 */
void I2CStart() {
	I2C0CONSET = (1 << 3); //set SI while configuring other bits
	I2C0CONSET = (1 << 5); //set STA to start condition
	I2C0CONCLR = (1 << 3); //clear SI to active state machine
	while (((I2C0CONSET >> 3) & 1) == 0) {
		//wait for start condition to be set
	}
	I2C0CONCLR = (1 << 5); //clear STA
}


/**
 *I2C write function
 */
void I2CWrite(int data) {
	I2C0DAT = data;
	I2C0CONCLR = (1 << 3); //clear SI to standard data
	while (((I2C0CONSET >> 3) & 1) == 0) {
		//wait for write condition to be set
	}

	//check I2C0STAT for ack or nak
}


/**
 * I2C read function
 */
int I2CRead(int ack) {
	if (ack) {
		I2C0CONSET = (1 << 2);
	} else {
		I2C0CONCLR = (1 << 2);
	}
	//I2C0CONCLR = (1 << 2);
	I2C0CONCLR = (1 << 3); //clear SI to start read
	while (((I2C0CONSET >> 3) & 1) == 0) {
		//wait for read condition to be set
	}

	return I2C0DAT;
}

/**
 * I2C stop function
 */
void I2CStop() {
	I2C0CONSET = (1 << 4); //set STO to request stop condition
	I2C0CONCLR = (1 << 3); //clear SI to active state machine
	while (((I2C0CONSET >> 4) & 1) == 1) {
		//wait for stop condition to be set
	}
}


/**
 * wait function
 * @ param count passes integer to wait
 */
void wait_ticks(unsigned long count) {
	volatile int ticks;
	for (ticks = 0; ticks < count; ticks++) {

	}
}


/**
 * cases for each possible digit on 7-segment display
 * @param d passes digit to display
 */
int displayDigit(int d) {
	switch (d) {
	case 0:
		return 0x3F;
		break; //display 0
	case 1:
		return 0x06;
		break; //display 1
	case 2:
		return 0x5B;
		break;
	case 3:
		return 0x4F;
		break;
	case 4:
		return 0x66;
		break;
	case 5:
		return 0x6D;
		break;
	case 6:
		return 0x7D;
		break;
	case 7:
		return 0x07;
		break;
	case 8:
		return 0x7F;
		break;
	case 9:
		return 0x6F;
		break;
	default:
		return 0;
	}
}

/**
 * converts C to F
 * @param temp passes Fahrenheit
 * @returns Celsius
 */
int convertToF(int temp) {
	return 1.8 * temp + 32;
}


/**
 * reads temperature sensor one
 */
int readTempSensorOne() {
	int temp;
	I2CStart();
	I2CWrite(tempReadAddr);
	temp = I2CRead(1);
	I2CStop();
	return temp;
}


/**
 * reads temperature sensor two
 */
int readTempSensorTwo() {
	int temp;
	I2CStart();
	I2CWrite(tempReadAddr);
	temp = I2CRead(1);
	I2CStop();
	return temp;
}


/**
 * Register write for I/O Expander
 * @param slaveAddress passes slave address
 * @param regAddress passes register directory
 * @param data passes data write in the slave
 */
void regWrite(int slaveAddress, int regAddress, int data) {
	I2CStart();
	I2CWrite(slaveAddress);
	I2CWrite(regAddress);
	I2CWrite(data);
	I2CStop();
}


/**
 * configure I/O Expander and write to 7-seg displays
 * @param digitA passes first digit
 * @param digitA passes second digit
 */
void writeToExpander(int digitA, int digitB) {
	regWrite(0b01000000, 0x00, 0b10000000); // IODIRA, set pins to output and switch pin to input
	regWrite(0b01000000, 0x01, 0b10000000); // IODIRB, set pins to output and switch pin to input
	regWrite(0b01000000, 0x12, displayDigit(digitA)); // GPIOA, set pins to logic high
	regWrite(0b01000000, 0x13, displayDigit(digitB)); // GPIOB, set pins to logic high
}


/**
 * reads status of switch one
 * @returns if true 10000111 else 00000111
 */
int readSwitchOne() {
	I2CStart();
	I2CWrite(0b01000000);
	I2CWrite(0x12);
	I2CStart();
	I2CWrite(0b01000001);
	int data = I2CRead(0);
	I2CStop();
	return data;
}


/**
 * reads status of switch two
 * @returns if true 10000111 else 00000111
 */
int readSwitchTwo() {
	I2CStart();
	I2CWrite(0b01000000);
	I2CWrite(0x13);
	I2CStart();
	I2CWrite(0b01000001);
	int data = I2CRead(0);
	I2CStop();
	return data;
}


/**
 *  Disable internal pull-up resistors for switch pins
 */
void disablePullUps() {
	regWrite(0b01000000, 0x0C, 0b00000000);
	regWrite(0b01000000, 0x0D, 0b00000000);
}


/**
 * manipulation is done here
 */
void manipulation() {
	int switchOneStatus = readSwitchOne();
	int switchTwoStatus = readSwitchTwo();

	if (((switchOneStatus >> 7) & 0x01) == 1) {
		flagTemp += 1;
	}

	if (((switchTwoStatus >> 7) & 0x01) == 1) {
		flagSensor += 1;
	}

	if (flagTemp % 2 > 0) {
		int tempInC = readTempSensorOne();
		int digitA = tempInC / 10;
		int digitB = tempInC % 10;
		printf("temperature from sensor one is %dC\n", tempInC);
		writeToExpander(digitA, digitB);
		wait_ticks(1000); //switch bounce
	}

	else if (flagSensor % 2 > 0) {
		int tempInF = convertToF(readTempSensorTwo());
		int digitA = tempInF / 10;
		int digitB = tempInF % 10;
		printf("temperature from sensor two is %dF\n", tempInF);
		writeToExpander(digitA, digitB);
		wait_ticks(1000); //switch bounce
	}

	else if ((flagSensor % 2 > 0) && (flagTemp % 2 > 0)) {
		int tempInC = readTempSensorTwo();
		int digitA = tempInC / 10;
		int digitB = tempInC % 10;
		printf("temperature from sensor two is %dC\n", tempInC);
		writeToExpander(digitA, digitB);
		wait_ticks(1000); //switch bounce
	} else {
		int tempInF = convertToF(readTempSensorOne());
		int digitA = tempInF / 10;
		int digitB = tempInF % 10;
		printf("temperature from sensor one is %dF\n", tempInF);
		writeToExpander(digitA, digitB);
		wait_ticks(1000); //switch bounce
	}
}
