# Fortune Cookie Printer

A fun ESP32-based project that generates and prints unique fortunes using a thermal printer. The project uses a Markov chain algorithm to create novel, coherent fortunes based on a curated collection of wisdom quotes and traditional fortune cookie messages.

## Hardware Requirements

- ESP32 Development Board
- Mini Thermal Printer (TTL Serial, 58mm, compatible with Adafruit_Thermal library)
  - Similar to Adafruit Mini Thermal Receipt Printer
  - Uses TTL serial at 19200 baud
- Jumper Wires
- 5-9V Power Supply for the printer
- Optional: Project enclosure

## Wiring

1. **Thermal Printer Connection:**
   - RX → GPIO16 (ESP32)
   - TX → GPIO17 (ESP32)
   - GND → GND (ESP32)
   - VH → External 5-9V power supply
   - DTR → Not connected

2. **Circuit Trigger:**
   - Connect GPIO4 to GND to trigger a fortune print
   - GPIO4 has an internal pull-up resistor enabled
   - Can be connected through a button, switch, or any contact closure

## Software Requirements

This project is built using PlatformIO, which provides a powerful development environment and handles all dependencies automatically.

### Required Software
- VSCode with PlatformIO extension
- OR PlatformIO Core CLI

### Dependencies
- Arduino framework for ESP32
- Adafruit Thermal Printer Library

## Building and Uploading

1. Clone this repository:
   ```bash
   git clone https://github.com/yourusername/Fortune-Cookie.git
   cd Fortune-Cookie
   ```

2. Open the project in VSCode with PlatformIO:
   - Open VSCode
   - Click "Open Folder" and select the project directory
   - Wait for PlatformIO to initialize the project

3. Build and upload:
   - Click the "Upload" button in PlatformIO
   - OR use the command line:
     ```bash
     pio run -t upload
     ```

4. Monitor serial output (optional):
   ```bash
   pio device monitor
   ```

## Usage

1. Power up the ESP32 and thermal printer
2. Short GPIO4 to GND to trigger a fortune print:
   - Each time you make this connection, a new fortune will be printed
   - The circuit can remain shorted to continuously print fortunes (one every 2 seconds)
   - Perfect for buttons, switches, or other triggering mechanisms
3. The printer will output:
   - A decorative header "YOUR FORTUNE"
   - A uniquely generated fortune
   - A decorative footer and feed lines

## Technical Details

### Fortune Generation

The project uses a Markov chain algorithm to generate unique fortunes:
- Pre-loaded with 70+ carefully selected wisdom quotes and traditional fortune cookie messages
- Source fortunes cover themes like wisdom, success, happiness, relationships, and personal growth
- Analyzes word patterns and transitions to create new combinations
- Generates new, coherent sentences while maintaining fortune cookie style
- Prevents word repetition and ensures grammatical coherence
- Optimized for ESP32's memory constraints

### Memory Optimization

The implementation is carefully optimized for the ESP32:
- Fixed-size arrays to prevent heap fragmentation
- Efficient data structures for the Markov chain
- Minimal memory footprint for word storage
- Optimized string handling
- Uses about 8% of ESP32's RAM and 10% of Flash

### Printer Communication

- Uses Hardware Serial (UART2) for reliable printer communication
- 19200 baud rate for stable printing
- Implements word wrapping for clean text formatting (32 chars per line)
- Handles printer wake/sleep cycles efficiently
- Includes proper delays between continuous prints

## Operation Modes

1. **Single Print Mode:**
   - Momentarily connect GPIO4 to GND
   - Perfect for button or switch activation
   - Prints one fortune per trigger

2. **Continuous Print Mode:**
   - Keep GPIO4 connected to GND
   - Automatically prints a new fortune every 2 seconds
   - Useful for demonstrations or continuous operation
   - Can be stopped by disconnecting GPIO4 from GND

## Troubleshooting

1. **Printer not responding:**
   - Check power supply voltage (should be 5-9V)
   - Verify TX/RX connections aren't swapped
   - Ensure proper ground connection
   - Check if printer's red LED is on

2. **Garbled print output:**
   - Verify baud rate settings
   - Check power supply stability
   - Ensure good connections
   - Try reducing print speed

3. **Trigger not working:**
   - Verify GPIO4 connection to GND
   - Check serial monitor for debug messages
   - Ensure proper ground reference
   - Test with a direct wire connection

## License

This project is licensed under the MIT License - see the LICENSE file for details.
