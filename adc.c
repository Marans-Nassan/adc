#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "ssd1306.h"

#define botao_a 5
#define green_led 11
#define blue_led 12
#define red_led 13
#define i2c_SDA 14
#define i2c_SCL 15
#define i2c_PORT i2c1
#define botao_j 22
#define adc_channel_0  0
#define adc_channel_1  1
#define VRX 26
#define VRY 27
#define endereco 0x3c
uint8_t i;
const uint8_t b[2] = {5, 22};
#define periodo 4096
const float divisor = 16.0;
uint16_t led_level_b, red_level_r = 100;
uint s[2];
static volatile uint64_t last_time;
static volatile bool on_off_1 = 0;
static volatile bool on_off_2 = 0;


void ledinit(){
    for(i = 11 ; i <= 13; i++ ){    
        gpio_init(i);
        gpio_set_dir(i, 1);
        gpio_put(i, 0);
    }
}
void botinit(){
    for(uint8_t j = 0 ; j <= 1 ; j++){
        gpio_init (b[j]);
        gpio_set_dir(b[j], 0);
        gpio_pull_up(b[j]);
    }
}   

void i2cinit(){
    i2c_init(i2c_PORT, 400000);
        for(i = 14 ; i <= 15; i++){
            gpio_set_function(i, GPIO_FUNC_I2C);
            gpio_pull_up(i);
        }
}

void pwm_setup(){
    for(uint8_t p = 0, i = 12 ; i <= 13 ; p++, i++){
        gpio_set_function(i, GPIO_FUNC_PWM);
        s[p] = pwm_gpio_to_slice_num(i);
        pwm_set_clkdiv(s[p], divisor);
        pwm_set_wrap(s[p], periodo);
        pwm_set_gpio_level(i, 0);
        pwm_set_enabled(s[p], true);          
    }
}
void pwm(){
    adc_select_input(0);
    uint16_t vrx_v = adc_read();
    float dc = (vrx_v / 4095.0) * 100;
    pwm_set_gpio_level(s[0], vrx_v);
}

ssd1306_t ssd;
void oledinit(){
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, i2c_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

void oleddis(const char *dot){
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
    ssd1306_draw_string(&ssd, dot, 58, 25);
    ssd1306_send_data(&ssd);
}

void joy_init(){
    for(i = 26 ; i<= 27 ; i++){
        adc_gpio_init(i);
    }
}

void gpio_irq_handler(uint gpio, uint32_t events){
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    if(gpio == botao_a || gpio == botao_j && current_time - last_time > 300000){
        if(gpio == botao_a){
            pwm_set_enabled(s[0], !true);
            pwm_set_enabled(s[1], !true);
            on_off_1 = !on_off_1;
            if(on_off_1 == 1)printf("PWM Desativado.\n");
            else printf("PWM Ativado.\n");
        }
        if (gpio == botao_j){
            gpio_put(11, !gpio_get(11));
            on_off_2 = !on_off_2;
            if(on_off_2 == 1)printf("Led Verde Ligado.\n");
            else printf("Led Verde Desligado.\n");
        }
        last_time = current_time;
    }

}

#define int_irq(gpio_pin) gpio_set_irq_enabled_with_callback(gpio_pin, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

int main()
{
stdio_init_all();
ledinit();
botinit();
adc_init();
int_irq(botao_a);
int_irq(botao_j);
    while (true) {
        printf("Hello, world!\n");
        sleep_ms(100000);
    }
}
