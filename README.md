# ESTC

DSR embedded system course.

## [led_test](led_test)

Simple test maded by dsr.

## [led_and_buttons](led_and_buttons)

Modification of led_test for 3-color LED and 2 buttons.
* On first button click color changing and LEDs slowly wake up (PWM on `for` cycle).
* On second button click LEDs off.
* `for` cylce used for normal buttons click. 

## [timers_and_interrupt](timers_and_interrupt)

Modification of led_and_buttons. It uses timer for changing color of led and interrupts for buttons actions.
* TIM2 used for changing color.
* On first button click color change to white.
* On second button click colors rotates backwards.
* TIM3 used for normal buttons click.

## [PWM](PWM)

Uses TIM1 PWM to change LEDs brightness.
* On first button click choose color red, green or blue. Base LED duplicate choosen color.
* On second button click britness of choosen color is increase.
* TIM2 used for normal first button click.
* Green led brightness fixed.
