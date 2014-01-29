
This example shows how to configure external interrupt lines.
EXTI0 is connected to GPIOA pin 0.
In the interrupt routine a led connected to GPIOB pin 0 will be toggled.

After EXTI configuration, a software interrupt is generated on the EXTI0 
to toggle LED0.
After that, when rising edge is detected on EXTI0, an interrupt will be 
triggered and the LED0 will toggle.

To check the example you need an PUSH-BUTTON and an 10Kohm RESISTOR.
