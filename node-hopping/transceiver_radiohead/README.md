# Sketch for the sensor

The weight of the sensor is defined in the setup function and can be changed.

How the sensor will function is that it is both transmitting its own data, and receiving data from other sensors. However, priority must be placed on its transmission since we do not want to lose any data. To implement this priority, we use an interrupt mechanism. 

Whenever there is a pulse on the hall sensor, the interrupt will be triggered and the sensor will stop what it is doing, and will run the Interrupt Service Routine (ISR) function instead. The ISR updates a pulse count. If the hall sensor goes 3 seconds without being triggered, a transmit boolean will be set, the pulse count will be stored into a variable to be transmitted and will be reset to 0.

In the main loop, if the transmit boolean is set, the variable with the pulse count will be transmitted. After transmitting, this boolean will be reset. If the transmit boolean is not set (ie. there is nothing to transmit), then the sensor will focus on receiving data from other sensors. If there is an incoming message from a sensor with a smaller weight, then its data will be retransmitted.
