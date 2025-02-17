# Projeto: Controle de LEDs, Joystick e Display OLED com Raspberry Pi Pico

## Descrição

Este projeto visa o controle de LEDs, a utilização de um conversor ADC em um joystick analógico e um display OLED SSD1306 utilizando um Raspberry Pi Pico. A comunicação entre os componentes é realizada via I2C, e os botões são gerenciados por interrupções com debounce. O joystick analógico é utilizado para controlar a intensidade dos LEDs via PWM e atualizar a posição de um cursor (quadrado) no display OLED.

## Objetivos

- Compreender a comunicação I2C em microcontroladores.
- Aplicar controle de LEDs via PWM.
- Compreender e utilizar ADC para ler entradas analógicas do joystick.
- Implementar interrupções para otimização da leitura dos botões.

## Componentes Utilizados

- Raspberry Pi Pico
- Display OLED SSD1306 (I2C)
- LEDs (Verde, Azul e Vermelho)
- Joystick analógico
- Botões de acionamento

## Configuração de Hardware

### Pinos Utilizados

#### Botões:
- Botão A: GPIO 5
- Botão J: GPIO 22

#### LEDs:
- LED Verde: GPIO 11
- LED Azul: GPIO 12
- LED Vermelho: GPIO 13

#### Joystick (ADC):
- Eixo X (VRX): GPIO 26 (ADC 0)
- Eixo Y (VRY): GPIO 27 (ADC 1)

#### Display OLED (I2C):
- SDA: GPIO 14
- SCL: GPIO 15

## Instalação e Compilação

1. Configure o ambiente de desenvolvimento do Raspberry Pi Pico (SDK do Pico e CMake).
2. Clone este repositório.
3. Compile o código utilizando CMake e o SDK do Pico.
4. Carregue o arquivo .uf2 gerado para o Raspberry Pi Pico.

## Funcionalidades

- Controle de LEDs individuais através dos botões.
- Ajuste da intensidade dos LEDs Azul e Vermelho com o joystick.
- Movimentação de um cursor no display OLED conforme a posição do joystick.
- Detecção de entrada analógica do joystick para gerar saídas PWM.
- Uso de interrupções para utilizar os botões.
- Botão A é responsável por definir o estado do pwm. (ativado ou desativado)
- Alterança de borda no display OLED e ativar ou desativar o led verde ao pressionar o botão J.

## Como Usar

### Inicialização
- Ao ligar, o display OLED é inicializado e os LEDs ficam apagados.

### Controles
- **Botão A**: Alterna a ativação do PWM nos LEDs Azul e Vermelho.
- **Botão J**: Alterna o estado do LED Verde e modifica a borda do display OLED.
- **Joystick**:
  - Movimenta o cursor no display OLED.
  - Controla a intensidade dos LEDs Azul e Vermelho via PWM.

## Motivção

Desenvolvido para fins educacionais no estudo de microcontroladores.

## Licença

The Unlicense

## Autor

Hugo Martins Santana (TIC370101267)