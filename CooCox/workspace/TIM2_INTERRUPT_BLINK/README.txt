
This example shows how to initialize timer 2 peripheral to generate an interrupt at 1kHz.
Each time the interrupt is triggered, in the interrupt handler a variable is incremented
and when the variable reach 1000 the led on PB0 is toggled and the variable is reseted to zero.

