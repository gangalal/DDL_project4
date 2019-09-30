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

#define PCONP (*(volatile unsigned char *)(0x400FC0C4))
#define PCLKSEL0 (*(volatile unsigned char *)(0x400FC1A8))
#define PINSEL1 (*(volatile unsigned char *)(0x4002C004))
#define I2CPADCFG (*(volatile unsigned char *)(0x4002C07C))


#define I2C0CONSET (*(volatile unsigned char *)(0x4001C000))
#define I2C0CONCLR (*(volatile unsigned char *)(0x4001C018))
#define I2C0DAT (*(volatile unsigned char *)(0x4001C008))


void I2CInit()
{
    PCONP |= (1<< 7); //resetting power control for I2C0
    PCLKSEL0 |= (0<<15)|(0<<14); // resetting clock selection
    PCLKSEL0 |= (1<<15)|(1<<14); //dividing CCLK by 8 *needs to review
    PINSEL1 |= (1<<22)|(1<<23); //enabling SDA0 on pin p0.27
    PINSEL1 |= (1<<24)|(1<<25); //enabling SDA1 on pin p0.28
    I2CPADCFG |= (0<<2); //configuring I2C0 register to standard mode
}




void I2CStart()
{
    I2C0CONSET = (1 << 3); //set SI while configuring other bits
    I2C0CONSET = (1 << 5); //set STA to start condition
    I2C0CONCLR = (1 << 3); //clear SI to active state machine
    while (((I2C0CONSET >> 3) & 1) == 0)
    {
        //wait for start condition to be set
    }
    I2C0CONCLR = (1 << 5); //clear STA
}

void I2CWrite(int data)
{
    I2C0DAT = data;
    I2C0CONCLR = (1 << 3); //clear SI to standard data
    while (((I2C0CONSET >> 3) & 1) == 0)
    {
        //wait for write condition to be set
    }

    //check I2C0STAT for ack or nak
}

int I2CRead(int ack)
{
    if (ack)
    {
        I2C0CONSET = (1 << 2);
    }
    else
    {
        I2C0CONCLR = (1 << 2);
    }
    I2C0CONCLR = (1 << 2);
    I2C0CONCLR = (1 << 3); //clear SI to start read
    while (((I2C0CONSET >> 3) & 1) == 0)
    {
        //wait for read condition to be set
    }

    return I2C0DAT;
}

void I2CStop()
{
    I2C0CONSET = (1 << 4); //set STO to request stop condition
    I2C0CONCLR = (1 << 3); //clear SI to active state machine
    while (((I2C0CONSET >> 3) & 1) == 0)
    {
        //wait for stop condition to be set
    }
}

int main(void)
{
	I2CInit();
    while (1)
    {

    }
    return 0;
}
