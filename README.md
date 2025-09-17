# atem-tally

## Purpose
The purpose of this project is to build a custom tally light system that integrates with the Blackmagic Atem Switcher. This system will provide real-time visual indicator for the program and preview camera states. The physical tally lights are ESP32 microcontrollers that have LEDs connected to them. The program connects to the switcher via IP and waits for an event to occur. When an event occurs, the program polls the program and preview states of the atem switcher and updates the microcontroller color via MQTT.
 
## Architecture
This server connects to the ATEM via IP and polls the server waiting for an event to occur. When an event occurs, it then updates the state of the microcontroller LEDs via MQTT. The MQTT broker can either be run locally on the same host as this server (tally light server) or be hosted elsewhere. Each microcontroller subscribes to a topic that is associated with a camera. Therefore, there is no limit to how many tally lights can recieve updates for a specific camera

## Features
1. Get updates from ATEM Switcher
2. Wireless control of Tally Lights via MQTT
3. Server logs
4. MacOS system tray
