# Simple pedestrian traffic light system with Raspberry Pi Pico W and OLED SSD 1306

The system logic goes as following: 

* Red light starts signaling that pedestrians should not cross. However, they can press the button.
* If the button is pressed while the red light is active, the system immediately goes to yellow light, meaning attention.
* The system stays in yellow for 5s, if the button is pressed, then 10s in green, so pedrestrians should cross.
* If the button is not pressed, times are slightly different: 8s in red, 2s in yellow, 8s in green.

This is just a simple logic and for a more realistic scenario, one could adapt timmings as needed and do not allow the user press the button 
without red light has stayed in this state for some seconds. Because in the way it is implemented the pedestrians could trick the traffic 
and do not let any time the light be red, if the button is always pressed. 

The display shows the following depending on which state the system is: 

