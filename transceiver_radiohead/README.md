# Sketch for the sensor
The weight of the sensor is defined in the _setup()_ function and can be changed.

How the sensor will function is that it is both transmitting its own data, and receiving data from other sensors. However, priority must be placed on its own transmission since we do not want to lose any data while receiving data from another sensor. To implement this priority, we use an interrupt mechanism. Whenever there is a pulse on the hall sensor, an interrupt will be triggered and the sensor will stop what it is doing, and will run the Interrupt Service Routine function _ISR()_ instead.

_ISR()_ first checks to see when the last interrupt was. If it was more than 3 seconds ago (ie. the hall sensor had gone more than 3 seconds without being triggered), then it signifies a new cycle. The following actions will be taken:
-   _pulseCount_ will be stored into a variable to be transmitted
-   The boolean _toTransmit_ will be set to _true_ so that the program will know to transmit the stored variable
-   _pulseCount_ will be reset to 0, since it is a new cycle

After that, regardless of whether it is a new cycle or not, _pulseCount_ is incremented and the last interrupt time is updated.

In the main loop, if the boolean _toTransmit_ is set, the stored variable will be transmitted. After transmitting, _toTransmit_ will be reset to _false_. 

However, if _toTransmit_ is _false_ (ie. there is nothing to transmit, since a cycle has yet to be started or the program is currently in the middle of a cycle), then the sensor will focus on receiving data from other sensors. It checks to see if there is an incoming message from a sensor. If there is, and it is from a sensor with a smaller weight, then this message will be re-transmitted out.
