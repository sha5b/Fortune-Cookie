# Fortune Cookie Printer

An ESP32-based Fortune Cookie printer that generates unique wisdom using a Markov chain algorithm and prints them on a thermal printer when triggered by a circuit closure.

## Features

- Generates unique fortunes using a Markov chain algorithm
- Large dataset of wisdom sayings for natural-sounding output
- Decorative printing with borders and formatting
- Simple circuit trigger mechanism
- Word-wrapped text for clean presentation

## Hardware Requirements

1. ESP32 Development Board
2. Thermal Printer (Compatible with Adafruit Thermal Printer Library)
3. Push Button or Contact Switch
4. 10kΩ Resistor (optional, if not using internal pullup)
5. Power Supply (5-9V, 2-3A) for the thermal printer

## Circuit Connections

### Thermal Printer
- Printer RX → ESP32 GPIO 17 (TX2)
- Printer TX → ESP32 GPIO 16 (RX2)
- Printer GND → ESP32 GND
- Printer VH → 5-9V power supply
- Printer VH GND → Power supply GND

### Circuit/Switch
- One terminal → ESP32 GPIO 4
- Other terminal → ESP32 GND
(The internal pullup resistor is enabled in the code, so no external resistor is needed)

## Software Setup

1. Install PlatformIO in VSCode
2. Clone this repository
3. Open the project in PlatformIO
4. Build and upload to your ESP32

## Operation

1. Power up the ESP32 and thermal printer
2. The system will initialize and build the Markov chain
3. When the circuit is closed (button pressed):
   - A new fortune is generated
   - The printer wakes up
   - Prints the fortune with decorative borders
   - Feeds paper and goes back to sleep

## Serial Monitor Output

You can monitor the operation through the serial monitor at 115200 baud:
- On startup: "Setup complete!" and initialization statistics
- On trigger: "Circuit closed - printing fortune!" and the generated text

## Troubleshooting

### Printer Issues
1. No Response
   - Check power supply voltage (5-9V) and current capacity (2-3A recommended)
   - Verify TX/RX connections aren't swapped
   - Ensure all grounds are connected

2. Garbled Text
   - Verify printer baud rate (19200)
   - Check TX/RX connections
   - Try adjusting printer heating settings

### Circuit Issues
1. Not Triggering
   - Check GPIO 4 connection
   - Verify switch/button connection to ground
   - Try external pullup resistor if issues persist

2. Multiple Triggers
   - Check for switch bounce
   - Increase debounce delay if needed

### Software Issues
1. Compilation Errors
   - Verify PlatformIO installation
   - Check library dependencies
   - Update platform and libraries

2. Strange Fortunes
   - Markov chain needs more source material
   - Check word splitting logic
   - Verify RAM usage isn't too high

## Customization

You can customize the project by:
1. Adding more fortune source material in `fortunes.h`
2. Adjusting Markov chain parameters in `main.cpp`
3. Modifying printer formatting and decoration
4. Changing the trigger mechanism or adding multiple inputs

## Technical Details

- Uses Hardware Serial 2 for printer communication
- Markov chain implementation with up to 200 nodes
- Each node can store up to 10 next-word transitions
- Word wrapping at 32 characters for standard thermal paper
- Internal pullup resistor used for circuit input

## Contributing

Feel free to:
1. Add more fortune cookie messages
2. Improve the Markov chain algorithm
3. Enhance the printing layout
4. Add new features

## License

This project is licensed under the MIT License - see the LICENSE file for details.
