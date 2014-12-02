/*
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 Author: Joao Paulo Barraca <jpbarraca@gmail.com>
*/

#include "mbed.h"
#include "Sampler.h"

#define SUMP_RESET 0x00
#define SUMP_ARM   0x01
#define SUMP_QUERY 0x02
#define SUMP_TEST   0x03
#define SUMP_GET_METADATA 0x04
#define SUMP_RLE_FINISH 0x05
#define SUMP_XON 0x11
#define SUMP_XOFF 0x13
#define SUMP_SET_TRIGGER_MASK 0xC0
#define SUMP_SET_TRIGGER_VALUES 0xC1
#define SUMP_SET_TRIGGER_CONF 0xC2
#define SUMP_SET_DIVIDER 0x80
#define SUMP_SET_READ_DELAY_COUNT 0x81
#define SUMP_SET_FLAGS 0x82


#define BYTE1(v) ((uint8_t)v & 0xff)         //LSB
#define BYTE2(v) ((uint8_t)(v >> 8) & 0xff)  //
#define BYTE3(v) ((uint8_t)(v >> 16) & 0xff) //
#define BYTE4(v) ((uint8_t)(v >> 24) & 0xff) //MSB

Serial pc(USBTX, USBRX);
uint8_t cmd_buffer[5];
uint8_t cmd_index = 0;
Sampler sampler(&pc);

void handleSerial()
{
    cmd_buffer[cmd_index++] = pc.getc();

    if((cmd_buffer[0] & 0x80) == 0){
        cmd_index = 0;
    }else if(cmd_index < 5){
        return;
    }

    switch(cmd_buffer[0]) {
        case SUMP_RESET : {
            sampler.reset();
            break;
        }

        case SUMP_QUERY:  {
            pc.printf("1ALS");
            break;
        }
        case SUMP_GET_METADATA: {
            uint32_t bufferSize = sampler.getBufferSize();
            uint32_t maxFrequency = sampler.getMaxFrequency();
            //NAME
            pc.putc(0x01);
            pc.putc('L');
            pc.putc('O');
            pc.putc('G');
            pc.putc('I');
            pc.putc('C');
            pc.putc('N');
            pc.putc('U');
            pc.putc('C');
            pc.putc('L');
            pc.putc('E');
            pc.putc('O');
            pc.putc(0);

            //SAMPLE MEM
            pc.putc(0x21);
            pc.putc(BYTE4(bufferSize));
            pc.putc(BYTE3(bufferSize));
            pc.putc(BYTE2(bufferSize));
            pc.putc(BYTE1(bufferSize));

            //SAMPLE RATE
            pc.putc(0x23);
            pc.putc(BYTE4(maxFrequency));
            pc.putc(BYTE3(maxFrequency));
            pc.putc(BYTE2(maxFrequency));
            pc.putc(BYTE1(maxFrequency));

            //Number of Probes
            pc.putc(0x40);
            pc.putc(0x08);

            //Protocol Version
            pc.putc(0x41);
            pc.putc(0x02);

            //END
            pc.putc(0);
            break;
        }
        case SUMP_TEST:{
            sampler.runTest();
            break;    
        }
        case SUMP_ARM: {
            sampler.start();
            break;
        }
        case SUMP_XON: {
            sampler.start();
            break;
        }
        case SUMP_XOFF: {
            sampler.stop();
            break;
        }
        case SUMP_SET_READ_DELAY_COUNT: {
            uint16_t readCount  = 1 + *((uint16_t*)(cmd_buffer+1));
            uint16_t delayCount = *((uint16_t*)(cmd_buffer+3));
            sampler.setSampleNumber(4*readCount);
            sampler.setSamplingDelay(4*delayCount);
            break;
        }
        case SUMP_SET_DIVIDER: {
            uint32_t divider = *((uint32_t*)(cmd_buffer+1));
            sampler.setSamplingDivider(divider);
            break;
        }
        case SUMP_SET_TRIGGER_MASK:{
            sampler.setTriggerMask(*(uint32_t*)(cmd_buffer+1));
            break;
        }
        case SUMP_SET_TRIGGER_VALUES:{
            sampler.setTriggerValue(*(uint32_t*)(cmd_buffer+1));
            break;
        }
        case SUMP_SET_TRIGGER_CONF:{
            break;
        }
        case SUMP_SET_FLAGS:{
            sampler.setFlags(*(uint32_t*)(cmd_buffer+1));
            break;
        }
        default: {
        }
    }

    cmd_index = 0;

}


int main()
{
    pc.baud(115200);
    //Looping through the serial. No interrupts.
    while (1) {
        if(pc.readable())
            handleSerial();
    }
}
