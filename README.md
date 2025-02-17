## Water Cleaning Robot



This project allows you to control a water-cleaning robot via Wi-Fi using the ESP8266 Wi-Fi module. The robot can move in various directions and its speed can be adjusted. The system can be used for cleaning water in tanks, ponds, or other similar areas.

## Features:
Control robot's movement: forward, backward, left, right, and stop.
-Adjust robot's speed with predefined values.
-Web-based control interface through ESP8266 Web Server.
-Use of HTTP requests to control the movement and speed.
-Can be used for cleaning applications (pond cleaning, pool cleaning, etc.).
-Hardware Requirements:
-ESP8266 Wi-Fi module (e.g., NodeMCU, Wemos D1 Mini)
-L298N motor driver or similar motor control board
-DC motors (for robot movement)
-Power supply (battery or other suitable source for motors)
-Water pump (for cleaning function, optional based on your project setup)
-Waterproof housing for electronic components (to protect from water exposure)

## Software Requirements:
-Arduino IDE: Download and install from here.
-ESP8266 Board: Add the ESP8266 package in the Arduino IDE.
-Libraries:
  -ESP8266WiFi
  -WiFiClient
  -ESP8266WebServer
-Install Libraries:
  -Open the Arduino IDE.
  -Go to Sketch -> Include Library -> Manage Libraries.
  -Search for and install the following libraries:
    -ESP8266WiFi
    -WiFiClient
    -ESP8266WebServer
-Upload Instructions:
  -Connect the ESP8266:
  
    Connect your ESP8266 module (e.g., NodeMCU) to the computer using a USB cable.
    Ensure that you have selected the correct board type (NodeMCU or Wemos D1) and port from the Tools menu.
  -Upload the Code:
  
    Open the Arduino IDE and paste the provided code into the IDE.
    Click on the Upload button to upload the code to the ESP8266.
  -Monitor the Output:
    
    Open the Serial Monitor (set baud rate to 115200).
    The IP address of the ESP8266 will be displayed after the ESP8266 connects to Wi-Fi.
    Note down the IP address, as you will use it to control the robot via a browser.
## Future Improvements:
-Integrate a water pump control for automated cleaning.
-Add sensors to monitor water quality and adjust cleaning accordingly.
-Create a mobile app interface for easier control.

