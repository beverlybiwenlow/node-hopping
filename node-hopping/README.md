# Implementing node hopping for LoRaWAN sensors

## Big idea:
If a sensor is out of range from the hub, there is no fail-safe mechanism to make sure the packet gets there and is not lost. The idea of node hopping is that data from the sensors further away from the hub can be received by sensors that are nearer, and then retransmitted out again. This allows the data to "hop" from the furthest sensors to nearer sensors and then finally to the hub.

How this is done is that each sensor will have a weight, depending on how far it is from the hub. The nearer a sensor is to the hub, the bigger its weight is. In order to keep data going in only one direction towards the hub and not away, sensors will only re-transmit data from other sensors that have a smaller weight than it does.

Although this could result in the same data packet reaching the hub multiple times (when data from the further sensors can already reach the hub but the nearer sensors retransmit it anyway), this is acceptable behaviour since the hub has a mechanism that can deal with this and remove any replicates.

Explanation of the sketch on the sensor is [here](transceiver).