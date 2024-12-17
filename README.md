# Fortune Cookie Printer

A fun ESP32-based project that generates and prints unique fortunes using a thermal printer. The project uses a Markov chain algorithm to create novel, coherent fortunes based on a curated collection of wisdom quotes and traditional fortune cookie messages.

## Hardware Requirements

- ESP32 Development Board
- Thermal Printer (Compatible with Adafruit Thermal Printer Library)
- Jumper Wires
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
2. Short GPIO4 to GND to trigger a fortune print
   - Each time you make this connection, a new fortune will be printed
   - The circuit can remain shorted to continuously print fortunes (one every 2 seconds)
3. The printer will output:
   - A decorative header
   - Your generated fortune
   - A decorative footer

## Technical Details

### Fortune Generation

The project uses a Markov chain algorithm to generate unique fortunes:
- Pre-loaded with carefully selected wisdom quotes and traditional fortune cookie messages
- Analyzes word patterns and transitions
- Generates new, coherent sentences while maintaining fortune cookie style
- Prevents word repetition and ensures grammatical coherence
- Optimized for ESP32's memory constraints

### Memory Optimization

The implementation is carefully optimized for the ESP32:
- Fixed-size arrays to prevent heap fragmentation
- Efficient data structures for the Markov chain
- Minimal memory footprint for word storage
- Optimized string handling

### Printer Communication

- Uses Hardware Serial (UART2) for reliable printer communication
- 19200 baud rate for stable printing
- Implements word wrapping for clean text formatting
- Handles printer wake/sleep cycles efficiently

## Troubleshooting

1. **Printer not responding:**
   - Check power supply voltage (should be 5-9V)
   - Verify TX/RX connections aren't swapped
   - Ensure proper ground connection

2. **Garbled print output:**
   - Verify baud rate settings
   - Check power supply stability
   - Ensure good connections

3. **Trigger not working:**
   - Verify GPIO4 connection to GND
   - Check serial monitor for debug messages
   - Ensure proper ground reference

## License

This project is licensed under the MIT License - see the LICENSE file for details.
