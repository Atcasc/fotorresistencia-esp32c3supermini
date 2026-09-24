#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <driver/gpio.h>
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "hal/adc_types.h"
#include "hal/gpio_types.h"
#include "sdkconfig.h"


#define FOTORRESISTENCIA_ADC CONFIG_FOTORRESISTENCIA_ADC
#define DISPLAY0_GPIO CONFIG_DISPLAY0_GPIO
#define DISPLAY1_GPIO CONFIG_DISPLAY1_GPIO
#define DISPLAY2_GPIO CONFIG_DISPLAY2_GPIO
#define DISPLAY3_GPIO CONFIG_DISPLAY3_GPIO

static const char *TAG = "LDR";
static  uint8_t gpio0 = 0;
static  uint8_t gpio1 = 0;
static  uint8_t gpio2 = 0;
static  uint8_t gpio3 = 0;
static  uint16_t fotorresistencia = 0;
static adc_oneshot_unit_handle_t handler;
void configure_display(void)
  {
    gpio_reset_pin(DISPLAY0_GPIO);
    gpio_set_direction(DISPLAY0_GPIO,GPIO_MODE_OUTPUT);
    gpio_reset_pin(DISPLAY1_GPIO);
    gpio_set_direction(DISPLAY1_GPIO,GPIO_MODE_OUTPUT);
    gpio_reset_pin(DISPLAY2_GPIO);
    gpio_set_direction(DISPLAY2_GPIO,GPIO_MODE_OUTPUT);
    gpio_reset_pin(DISPLAY3_GPIO);
    gpio_set_direction(DISPLAY3_GPIO,GPIO_MODE_OUTPUT);
  }

void configure_fotorresistencia(void)
  {
	adc_oneshot_unit_init_cfg_t lector = {
    .unit_id = ADC_UNIT_1,//pines 32-29
	};
    adc_oneshot_new_unit(&lector,&handler);
    adc_oneshot_chan_cfg_t variable={
		.bitwidth=ADC_BITWIDTH_DEFAULT,
		.atten=ADC_ATTEN_DB_12
	};
    adc_oneshot_config_channel(handler,FOTORRESISTENCIA_ADC,&variable);
  }

void give_value_display(uint16_t valor)
  {
     uint8_t display = valor*10 / 4096;
     gpio_set_level(DISPLAY0_GPIO, display&1 ? 1 : 0);
     gpio_set_level(DISPLAY1_GPIO, display&2 ? 1 : 0);
     gpio_set_level(DISPLAY2_GPIO, display&4 ? 1 : 0);
     gpio_set_level(DISPLAY3_GPIO, display&8 ? 1 : 0);
	ESP_LOGI(TAG,"El valor del display es: %d",display);

  }


uint16_t receive_value_fotorresistencia(void)
  {
    uint16_t resultado;
    adc_oneshot_read(handler,FOTORRESISTENCIA_ADC ,&resultado);
	return resultado;
  }


void app_main(void)
{
	
    configure_display();
    configure_fotorresistencia();
    while(1){

    fotorresistencia = receive_value_fotorresistencia();
    give_value_display(fotorresistencia);
    sleep(1);
    }
}