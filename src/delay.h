
volatile unsigned int *DWT_CYCCNT   = (volatile unsigned int *)0xE0001004; //address of the register
volatile unsigned int *DWT_CONTROL  = (volatile unsigned int *)0xE0001000; //address of the register
volatile unsigned int *SCB_DEMCR        = (volatile unsigned int *)0xE000EDFC; //address of the register

static uint32_t __PrecisionTimingTarget = 0;

//Not perfect but better than wait_us at 1us scale
#define wait_ns(ns)   *DWT_CYCCNT = 0; \
                         __PrecisionTimingTarget = *DWT_CYCCNT + ns - 17;\
                        while(*DWT_CYCCNT < __PrecisionTimingTarget);


void EnablePrecisionTiming(){
    *SCB_DEMCR = *SCB_DEMCR | 0x01000000;
    *DWT_CYCCNT = 0;
    *DWT_CONTROL = *DWT_CONTROL | 1 ;
}
