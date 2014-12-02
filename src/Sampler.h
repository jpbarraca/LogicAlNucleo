#ifndef SAMPLER_H
#define SAMPLER_H
#include "mbed.h"

class Sampler{

public:

    Sampler(Serial*);

    void start();
    void arm();
    void stop();
    void reset();
    void runTest();

    //Getters and Setters
    uint32_t getBufferSize();
    uint32_t getMaxFrequency();

    void setSamplingDivider(uint32_t);
    void setSampleNumber(uint32_t);
    void setSamplingDelay(uint16_t);
    void setTriggerMask(uint32_t);
    void setTriggerValue(uint32_t);
    void setFlags(uint32_t);


private:
    uint8_t *buffer;
    uint16_t buffer_index;
    uint8_t buffer_rle_value;
    uint8_t buffer_rle_count;

    uint32_t samplingPeriod;
    uint32_t sampleNumber;
    uint32_t sampleDelay;
    uint32_t triggerMask;
    uint32_t triggerValue;
    uint32_t flags;

    uint32_t bufferSize;

    Serial *pc;
};
#endif