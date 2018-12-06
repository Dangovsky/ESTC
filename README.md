# ESTC

DSR embedded system course.

## [led_test](led_test)

Simple test maded by dsr

## [led_and_buttons](led_and_buttons)

Modification of led_test for 3-color LED and 2 buttons.
* On first button click color changing and LEDs slowly wake up.
* On second button click LEDs off and first button become inactive.

## [timers_and_interrupt](timers_and_interrupt)

Modification of led_and_buttons. It uses timer for changing color of led and interrupts for buttons actions.
* 9 colors (minus white)
* On first button click color change to white.
* On second button click LEDs off.

## [PWM](PWM)

Uses timer 2 PWM to slowly change LEDs brightness.
* On first button click choose color red, green or blue. Base LED duplicate choosen color.
* On second button click britness of choosen color is increase.
