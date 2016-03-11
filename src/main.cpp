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

#define printChar(v) pc.putc(v); while(!pc.writeable());

#define printUInt(v)\
    printChar(BYTE4(v));\
    printChar(BYTE3(v));\
    printChar(BYTE2(v));\
    printChar(BYTE1(v));


#define printString(v)\
    for(unsigned int i =0;i<strlen(v);i++) printChar(v[i]);

Serial pc(USBTX, USBRX);
Sampler sampler(&pc);

DigitalOut led(LED2);

inline void blink(unsigned int onTime,unsigned int offTime, unsigned int num){
    for(unsigned int i=0;i<num;i++){
        led = 1;
        wait_ms(onTime);
        led = 0;
        wait_ms(offTime);
    }
}

void handleSerial()
{
    uint8_t cmd_buffer[5];
    uint8_t cmd_index = 0;
    memset(cmd_buffer, 0, sizeof(cmd_buffer));
    
    led = 0;

    //Looping through the serial.   
    while (1) {
        led = 0;

        while(!pc.readable());
        
        led = 1;        
        cmd_buffer[cmd_index] = pc.getc();
        switch(cmd_buffer[0]) {
            case SUMP_RESET : {
                sampler.reset();
                break;
            }
            case SUMP_QUERY:  {
                printString("1ALS");
                break;
            }
            case SUMP_GET_METADATA: {
                uint32_t bufferSize = sampler.getBufferSize();
                uint32_t maxFrequency = sampler.getMaxFrequency();

                while(!pc.writeable());
                //NAME
                printChar(0x01);
                printString("LogicalNucleo");
                printChar(0x00);

                //SAMPLE MEM
                printChar(0x21);
                printUInt(bufferSize);

                //DYNAMIC MEM
                printChar(0x22);
                printUInt(0);

                //SAMPLE RATE
                printChar(0x23);
                printUInt(maxFrequency);

                //Number of Probes
                printChar(0x40);
                printChar(0x08);

                //Protocol Version
                printChar(0x41);
                printChar(0x02);
            
                //END
                printChar(0x00);
                break;
            }
            case SUMP_TEST:{
                sampler.runTest();
                break;    
            }
            case SUMP_ARM: {
                sampler.arm();
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
                cmd_index ++;
                if(cmd_index < 5)
                    continue;

                uint16_t readCount  = 1 + *((uint16_t*)(cmd_buffer + 1));
                uint16_t delayCount = * ((uint16_t*)(cmd_buffer + 3));
                sampler.setSampleNumber(4 * readCount);
                sampler.setSamplingDelay(4 * delayCount);
                break;
            }
            case SUMP_SET_DIVIDER: {
                cmd_index ++;
                if(cmd_index < 5)
                    continue;
                
                uint32_t divider = *((uint32_t *)(cmd_buffer + 1));
                sampler.setSamplingDivider(divider);
                break;
            }
            case SUMP_SET_TRIGGER_MASK:{
                cmd_index ++;
                if(cmd_index < 5)
                    continue;
                sampler.setTriggerMask(*(uint32_t *)(cmd_buffer + 1));
                break;
            }
            case SUMP_SET_TRIGGER_VALUES:{
                cmd_index ++;
                if(cmd_index < 5)
                    continue;
                
                sampler.setTriggerValue(*(uint32_t *)(cmd_buffer + 1));
                break;
            }
            case SUMP_SET_TRIGGER_CONF:{
                cmd_index ++;
                if(cmd_index < 5)
                    continue;

                uint16_t serial = (*((uint16_t*)(cmd_buffer + 1)) & 0x04) > 0 ? 1 : 0;
                uint16_t state = (*((uint16_t*)(cmd_buffer + 1)) & 0x08) > 0 ? 1 : 0;
                if(serial)
                    sampler.setTriggerState(0);//Not supported
                else
                    sampler.setTriggerState(state); 

                break;
            }
            case SUMP_SET_FLAGS:{
                cmd_index ++;
                if(cmd_index < 5)
                    continue;
                
                sampler.setFlags(*(uint32_t *)(cmd_buffer + 1));
                break;
            }
            default: {
            }
        }

        cmd_index = 0;
        memset(cmd_buffer, 0, sizeof(cmd_buffer));
    }
}



int main()
{
    pc.baud(115200);

    //Flush it
    while(pc.readable() == 1)
        pc.getc();

    blink(50,100,5);

    handleSerial();
}
