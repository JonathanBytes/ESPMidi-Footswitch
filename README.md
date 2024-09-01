# 🎚️ ESPMidi Footswitch! 🎛️ 🎸

A customizable MIDI Footswitch built on the ESP32 microcontroller. It uses foot buttons to detect foot presses and send MIDI messages to a connected device. The controller is highly customizable and can be configured to send any MIDI message, including note on/off, control change, program change, and more.

It has 2 buttons, 4 knobs, and an OLED display witch shows multiple information about the controller status.

## Features 🚀

- MIDI interface control with multiple banks and presets.
- LED indicators for bank selection.
- Support for buttons, knobs, and OLED display.
- Web-based configuration interface.
- Multicore implementation for real-time performance.

## The controller in deep 🤖

The ESPMidi controller is based on the ESP32 microcontroller. It uses the Control Surface library to handle MIDI messages and the WebServer library to create a web interface for configuration. The controller has two foot switches, four potentiometers, and an OLED display. The foot switches and the potentiometers are used to send MIDI Control Change (CC) messages.

The OLED display shows information about the controller status, such as the current bank and preset, as well as very fancy boot animations!

### Materials 🛠️

- ESP32 microcontroller. (I used the ESP32 DevKit V1)
- 2 momentary foot switches.
- 4 10k potentiometers.
- n leds. (Optional, future feature for bank selection and effects status)
- 0.91" OLED display 128x32px. (Optional, for showing controller information)
- Enclousure.

### Used Libraries 📚

These are the main libraries used in this project:

- [Control Surface](https://tttapa.github.io/Control-Surface-doc/Doxygen/index.html)
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
- [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library)
- Arduino-esp32 Wifi.h (built-in)
- Arduino-esp32 WebServer.h (built-in)

This is not a library, but I want to thank Wokwi for the amazing animations in the web interface. Check them out [here](https://wokwi.com/animatior).

## TODO List
