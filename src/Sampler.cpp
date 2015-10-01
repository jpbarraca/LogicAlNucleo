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
#include "delay.h"

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
    reset();
    EnablePrecisionTiming();
}

void Sampler::reset()
{
    buffer_index = 0;
    setTriggerMask(0);
    setTriggerValue(0);
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
    sampleNumber = min((uint32_t) bufferSize,s);
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

    uint16_t snum = sampleNumber;

    //Due to Loop Unrooling
    if(snum < 8)
        snum = 8;

    if(sampleDelay > 0){
        wait_us(sampleDelay);
    }

    buffer[snum] = GPIOB->IDR;
    while((buffer[snum] & triggerMask) != triggerValue){
        buffer[snum] = GPIOB->IDR;
    }

    //TODO: Migrate to a more generic approach through wait_ns
    //10Mhz
    if(samplingPeriod == 100){
        while(likely(snum >= 8)){
            buffer[snum] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\n");
            buffer[snum-1] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\n");
            buffer[snum-2] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\n");
            buffer[snum-3] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\n");
            buffer[snum-4] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\n");
            buffer[snum-5] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\n");
            buffer[snum-6] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\n");
            buffer[snum-7] = GPIOB->IDR;
            __asm volatile ( " NOP\n");
            snum-=8;
        }
    //5Mhz
     }else  if(samplingPeriod == 200){
        while(likely(snum >= 8)){
            buffer[snum] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            buffer[snum-1] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            buffer[snum-2] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            buffer[snum-3] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            buffer[snum-4] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            buffer[snum-5] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            buffer[snum-6] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            buffer[snum-7] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            snum-=8;
        }
    //2Mhz
     }else if(samplingPeriod == 500){
        while(likely(snum >= 8)){
            buffer[snum] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\n");

            buffer[snum-1] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\n");

            buffer[snum-2] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\n");

            buffer[snum-3] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\n");

            buffer[snum-4] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\n");

            buffer[snum-5] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\n");

            buffer[snum-6] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\n");

            buffer[snum-7] = GPIOB->IDR;
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\nNOP\n");
            __asm volatile ( " NOP\nNOP\nNOP\n");

            snum-=8;
        }
    //Others
     }else {
        uint32_t c = (samplingPeriod * SYSTEM_CLOCK_MULT)/1000.0;
        while(likely(snum> 8)){
            buffer[snum] = GPIOB->IDR;
            wait_ns(c);
            buffer[snum-1] = GPIOB->IDR;
            wait_ns(c);
            buffer[snum-2] = GPIOB->IDR;
            wait_ns(c);
            buffer[snum-3] = GPIOB->IDR;
            wait_ns(c);
            buffer[snum-4] = GPIOB->IDR;
            wait_ns(c);
            buffer[snum-5] = GPIOB->IDR;
            wait_ns(c);
            buffer[snum-6] = GPIOB->IDR;
            wait_ns(c);
            buffer[snum-7] = GPIOB->IDR;
            wait_ns(c);
            snum-=8;
        }
    }

}

void Sampler::arm()
{
    if (flags & FLAGS_TEST) {
        PwmOut pwm(D9);
        pwm.period_us((uint16_t) 50);
        pwm.pulsewidth_us((uint16_t) 20);

        start();

        pwm.period_us((uint16_t) 0);
        pwm.pulsewidth_us(0);
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
