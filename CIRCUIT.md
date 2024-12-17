# Circuit Connections for Fortune Cookie Printer

## Components Needed
1. ESP32 Development Board
2. Thermal Printer (Compatible with Adafruit Thermal Printer Library)
3. Push Button or Contact Switch
4. 10kΩ Resistor (optional, if not using internal pullup)

## Connections

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

## Notes
- The thermal printer requires a separate 5-9V power supply capable of providing enough current (2-3A recommended)
- Make sure all grounds (ESP32, printer, and power supply) are connected together
- The printer's TX/RX lines operate at 3.3V which is compatible with the ESP32
- The switch can be any type of contact closure (push button, reed switch, etc.)

## Testing
1. After uploading the code, open the Serial Monitor at 115200 baud
2. You should see "Setup complete!" when the ESP32 starts
3. When you close the circuit (press button/switch), you should see "Circuit closed - printing fortune!"
4. The printer should wake up, print a randomly generated fortune with decorative borders, and feed the paper

## Troubleshooting
1. If the printer doesn't respond:
   - Check power supply voltage and current capacity
   - Verify TX/RX connections aren't swapped
   - Ensure all grounds are connected
2. If fortunes don't print when circuit closes:
   - Check GPIO 4 connection
   - Verify switch/button is properly connected to ground
3. If text is garbled:
   - Verify printer baud rate matches code (19200)
   - Check TX/RX connections
