# Sketch for the sensor

The weight of the sensor is defined in the setup function and can be changed.

How the sensor will function is that it is both transmitting its own data, and receiving data from other sensors. However, priority will be placed on its transmission since we do not want to lose any data. This is done through a interrupt mechanism. When there is a pulse on the hall sensor, the interrupt will be triggered and the sensor will not be receiving data from other sensors and will run the Interrupt Service Routine function instead.

Sending a reply after receiving data and waiting for a reply after transmitting data have been commented out in this sketch for now. They are useful things to have but the many transmissions has made the program quite messy. This part is to be improved.