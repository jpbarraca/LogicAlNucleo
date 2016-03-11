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
#include <algorithm>

#define TRIGGER_PARALLEL 0
#define TRIGGER_SERIAL 1
#define TRIGGER_DISABLED 0xFF

#define TRIGGER_MODE_SERIAL 0
#define TRIGGER_MODE_PARALLEL 1

#define FLAGS_DEMUX 1
#define FLAGS_FILTER 2
#define FLAGS_CHANNEL_GROUPS 0x3C
#define FLAGS_EXTERNAL 0x40
#define FLAGS_INVERTED 0x80
#define FLAGS_TEST 0x400


#define SUMP_ORIGINAL_FREQ  (100000000)
#define SYSTEM_CLOCK_MULT (SystemCoreClock / 1000000)
#define BUFFER_SIZE 32768
#define MAX_FREQUENCY 10000000

__attribute((section("AHBSRAM0"),aligned))  uint8_t  main_buffer[BUFFER_SIZE];

#define likely(x) __builtin_expect((x),1)

Sampler::Sampler(Serial *sp)
{
    pc = sp;
    bufferSize = BUFFER_SIZE;
    buffer =  main_buffer;

    //Setup port
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);
    GPIOB->OSPEEDR = 3;         // High Speed
    GPIOB->BSRR = 0xFFFF0000;   // No Special pins
    GPIOB->MODER = 0;           // Input
    GPIOB->PUPDR = 0;           // No pull up or pull down

    reset();
}

void Sampler::reset()
{
    buffer_index = 0;
    setTriggerMask(0);
    setTriggerValue(0);
    setTriggerState(0);
    setSamplingDivider(11);
    setFlags(0);
    setSampleNumber(bufferSize);
    setSamplingDelay(0);
}

uint32_t Sampler::getMaxFrequency(){
    return MAX_FREQUENCY;
}

uint32_t Sampler::getBufferSize(){
    return bufferSize;
}

void Sampler::setSamplingDivider(uint32_t divider)
{
    //Max speed is 10Mhz
    if(divider < 9)
        divider = 9;

    samplingPeriod = (divider+1)*10;
}

void Sampler::setSampleNumber(uint32_t s)
{
    sampleNumber = min((uint32_t) bufferSize, s);
}

void Sampler::setSamplingDelay(uint16_t s)
{
    sampleDelay = s;
}

void Sampler::setTriggerMask(uint32_t s)
{
    triggerMask = s;
}
void Sampler::setTriggerValue(uint32_t s)
{
    triggerValue = s & 0xFF;
}

void Sampler::setTriggerState(uint8_t state){
    triggerState = state;
}

void Sampler::setFlags(uint32_t s)
{
    flags = s;
}

void Sampler::runTest()
{
    //TODO
}

void Sampler::start()
{

    int32_t snum = sampleNumber;


    if(sampleDelay > 0){
        wait_us(sampleDelay);
    }

    if(triggerState == 1){
        while((GPIOB->IDR & triggerMask) != triggerValue);
    }
    
    //10Mhz (Close but not really...)
    if(samplingPeriod == 100){
        while(likely(snum >= 4)){
            buffer[snum - 1] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\n");
            
            buffer[snum - 2] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\n");
            buffer[snum - 3] = GPIOB->IDR;  
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\n");
            buffer[snum - 4] = GPIOB->IDR;  
            snum -= 4;
        }
    //5Mhz (Almost)
     }else  if(samplingPeriod == 200){
        while(likely(snum >= 4)){
            buffer[snum - 1] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            
            buffer[snum - 2] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            
            buffer[snum - 3] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            
            buffer[snum - 4] = GPIOB->IDR;
            snum -= 4;
        }
    //2Mhz (True)
     }else if(samplingPeriod == 500){
        while(likely(snum >= 4)){
            
            buffer[snum - 1] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP");
            
            buffer[snum - 2] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\n\nNOP\nNOP\nNOP\nNOP\nNOP");
            
            buffer[snum - 3] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\n\nNOP\nNOP\nNOP\nNOP\nNOP");
            
            buffer[snum - 4] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\n");
            snum -= 4;
        }
        //1Mhz (True)
     }else if(samplingPeriod == 1000){
        while(likely(snum >= 4)){

            buffer[snum - 1] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP");
            
            buffer[snum - 2] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP");
            
            buffer[snum - 3] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP");
            
            buffer[snum - 4] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\n");

            snum -= 4;
        }
    //Others
     }else {
        uint32_t c = samplingPeriod/1000.0;
        while(likely(snum >= 4)){
            buffer[snum - 1] = GPIOB->IDR;
            wait_us(c);
            buffer[snum - 2] = GPIOB->IDR;
            wait_us(c);
            buffer[snum - 3] = GPIOB->IDR;
            wait_us(c);
            buffer[snum - 4] = GPIOB->IDR;
            wait_us(c);
            snum -= 4;
        }
    }
}

void Sampler::arm()
{
    if (flags & FLAGS_TEST) {
        PwmOut pwm0(PB_0);
        pwm0.period_us(1);
        pwm0.write(0.5);

        PwmOut pwm1(PB_1);
        pwm1.period_us(10);
        pwm1.write(0.5);

        PwmOut pwm2(PB_3);
        pwm2.period_us(100);
        pwm2.write(0.5);

        PwmOut pwm3(PB_4);
        pwm3.period_us(500);
        pwm3.write(0.5);

        PwmOut pwm4(PB_5);
        pwm4.period_ms(1);
        pwm4.write(0.5);
        
        PwmOut pwm5(PB_6);
        pwm5.period_ms(10);
        pwm5.write(0.5);

        PwmOut pwm6(PB_7);
        pwm6.period_ms(100);
        pwm6.write(0.5);

        start();

        pwm0.write(0);
        pwm1.write(0);
        pwm2.write(0);
        pwm3.write(0);
        pwm4.write(0);
        pwm5.write(0);
        pwm6.write(0);
    }else{
        start();
    }

    for(uint16_t i = 0;i < sampleNumber; i++)
    {
        pc->putc(buffer[i]);
        while(!pc->writeable());
    }
}

void Sampler::stop()
{
}
