#include "cc253x.h"
#include "sfr-bits.h"
#include "8051def.h"
#include "dev/leds.h"

#include "adc.h"

int16_t read_adc(uint8_t channel) {
	int16_t  reading = 0;
	uint8_t   i, resbits;
	uint8_t  adcChannel = 1;
	uint8_t  adcRef = HAL_ADC_REF_VOLT;

	if (channel < 8) {
    	for (i=0; i < channel; i++) {
	    	adcChannel <<= 1;
    	}
 	}
  APCFG |= adcChannel;
	resbits = HAL_ADC_DEC_256;
  ADCCON3 = channel | resbits | adcRef;
  while (!(ADCCON1 & HAL_ADC_EOC));
  APCFG &= (adcChannel ^ 0xFF);
  reading = (int16_t) (ADCL);
  reading |= (int16_t) (ADCH << 8);
  if (reading < 0){
   	reading = 0;
	}
	reading >>= 4;
	return reading;	
}
/*---------------------------------------------------------------------------*/
/* ON/OFF VCC cho Sensor Nhip tim */
void adc_on(void){
	//PRINTF("__adc_on\n");
	leds_on(PORT_VCC_ADC);
}

void adc_off(void){
	//PRINTF("__adc_off\n");
	leds_off(PORT_VCC_ADC);
}
/*---------------------------------------------------------------------------*/// ** bao anh