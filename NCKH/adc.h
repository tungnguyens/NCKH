#ifndef ADC_H_
#define ADC_H_

#include <stdio.h>
#include <stdint.h>

#define HAL_ADC_EOC         0x80    /* End of Conversion bit */
#define HAL_ADC_DEC_256     0x20    /* Decimate by 256 : 12-bit resolution */
#define HAL_ADC_REF_VOLT    0x80
#define PORT_VCC_ADC			LEDS_GREEN

int16_t read_adc(uint8_t channel); // P0.x (x = channel)
void adc_on(void);
void adc_off(void);

#endif /* ADC_H_ */
