#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "ssd1306.h"
//Macros utilizadas.
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
#define periodo 4096
#define int_irq(gpio_pin) gpio_set_irq_enabled_with_callback(gpio_pin, GPIO_IRQ_EDGE_FALL, true, gpio_irq_handler);
//Variáveis Utilizadas.
const uint8_t b[2] = {5, 22};
const float divisor = 16.0;
uint slice_led_b, slice_led_r;
static volatile uint64_t last_time;
static volatile bool on_off_1 = 0;
static volatile bool on_off_2 = 0;
static volatile bool f_t_1 = 1;
static volatile bool f_t_2 = 1;
uint16_t vrx_value, vry_value;
volatile uint8_t border = 0;
volatile uint8_t l_border = 2;
bool light = 0;
ssd1306_t ssd;
//Protótipo das funções.
void ledinit(); //Responsável por iniciar os leds.
void botinit(); //Responsável por iniciar os botões.
void i2cinit(); //Responsável por iniciar o i2c.
void pwm_setup(uint led, uint *slice, uint16_t leveli); //Responsável por organizar a inicialização do pwm.
void joyinit(); //inicialização do joystick.
void joy_definition(); //Definição de como o joystick vai funcionar ao ler o adc e determinar a intensidade via pwm.
uint16_t media(uint8_t input); //Responsável por deixar a leitura do ADC mais precisa ao fazer a média de 10 leituras.
void oledinit(); //Responsável por iniciar o display.
void oleddis(); //Responsável por configurar o display para funcionar em conjunto com o joystick(ADC) e pwm.
void gpio_irq_handler(uint gpio, uint32_t events); //Eventos ao acionar a interrupção por qualquer um dos botões ativos.
//Função principal
int main(){
stdio_init_all();
i2cinit();
ledinit();
botinit();
joyinit();
oledinit();
pwm_setup(blue_led, &slice_led_b, 0);
pwm_setup(red_led, &slice_led_r, 0);
int_irq(botao_a);
int_irq(botao_j);
    while (true) {
        vrx_value = media(adc_channel_1);
        vry_value = 4095 - media(adc_channel_0);
        joy_definition();
        oleddis();
        printf("Valor digital dos eixos. Eixo Y: %d. Eixo X:%d\n", vry_value, vrx_value);
        sleep_ms(25);
    }
}

//Funções

void ledinit(){
    for(uint8_t i = 11 ; i <= 13; i++ ){    
        gpio_init(i);
        gpio_set_dir(i, 1);
        gpio_put(i, 0);
    }
}
void botinit(){
    for(uint8_t i = 0 ; i <= 1 ; i++){
        gpio_init (b[i]);
        gpio_set_dir(b[i], 0);
        gpio_pull_up(b[i]);
    }
}   

void i2cinit(){
    i2c_init(i2c_PORT, 400*1000);
        for(uint8_t i = 14 ; i < 16; i++){
            gpio_set_function(i, GPIO_FUNC_I2C);
            gpio_pull_up(i);
        }
}

void pwm_setup(uint led, uint *slice, uint16_t leveli){
        gpio_set_function(led, GPIO_FUNC_PWM);
        *slice = pwm_gpio_to_slice_num(led);
        pwm_set_clkdiv(*slice, divisor);
        pwm_set_wrap(*slice, periodo);
        pwm_set_gpio_level(led, leveli);
        pwm_set_enabled(*slice, true);          
    
}

void joyinit(){
    adc_init();
    adc_gpio_init(VRY);
    adc_gpio_init(VRX);
}

void joy_definition(){
    uint16_t intensidade_b = (vrx_value * periodo) / 4095;
    uint16_t intensidade_r = (vry_value * periodo) / 4095;

        if(vrx_value <= 2620 && vrx_value >= 1200)pwm_set_gpio_level(red_led, 0);
        else pwm_set_gpio_level(red_led, intensidade_b);
        if(vry_value <= 2750 && vry_value >= 1200)pwm_set_gpio_level(blue_led, 0);
        else pwm_set_gpio_level(blue_led, intensidade_r);
}

uint16_t media(uint8_t input){
uint16_t media_adc = 0;
    for(uint8_t i = 0; i <= 9; i++){
        adc_select_input(input);
        sleep_us(2);
        media_adc += adc_read();
    }
return media_adc / 10;
}

void oledinit(){
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, i2c_PORT);
    ssd1306_config(&ssd);
}

void oleddis(){
    light = !light;
    uint16_t coluna_x = (vrx_value * WIDTH) / 4095;
    uint16_t linha_y = (vry_value * HEIGHT) / 4095;
        if(coluna_x > WIDTH - 8) coluna_x = WIDTH - 8;
        if(linha_y > HEIGHT - 8) linha_y = HEIGHT - 8;
    ssd1306_fill(&ssd, false);
    ssd1306_rect(&ssd, linha_y, coluna_x, 8, 8, true, true);
        if(border == 1) ssd1306_rect(&ssd, 4, 4, 124, 60, on_off_2, false);
        if(border == 2) ssd1306_rect(&ssd, 4, 4, 124, 60, !light, false);
    ssd1306_send_data(&ssd);
}

void gpio_irq_handler(uint gpio, uint32_t events){
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    if(gpio == botao_a || gpio == botao_j){
        if(current_time - last_time > 300000){
            if(gpio == botao_a){
                pwm_set_enabled(slice_led_b, !f_t_1);
                pwm_set_enabled(slice_led_r, !f_t_2);

                f_t_1 = !f_t_1;
                f_t_2 = !f_t_2;
                
                on_off_1 = !on_off_1;
                (on_off_1 == 1) ? printf("PWM Desativado.\n") : printf("PWM Ativado.\n");
            }
            if (gpio == botao_j){
                gpio_put(11, !gpio_get(11));
                (on_off_2 == 1) ? printf("Led Verde Ligado.\n"): printf("Led Verde Desligado.\n");
                on_off_2 = !on_off_2;
                    if(border == 0){ //responsável por permitir alternância de bordas ao ligar o led verde.
                        if(l_border == 1){
                            border = 2;
                        }
                        else border = 1;
                        l_border = border;
                    } else border = 0;
                }
            }
    }
        last_time = current_time;
}