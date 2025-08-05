# ESP32 Smart Door Light Controller

An ESP32-based smart lighting system that automatically controls RGBW LED strips based on door position detection using an ultrasonic sensor. Perfect for cabinet lighting, closet automation, or any application where you need lights to turn on when a door opens.

## Features

- **Automatic Door Detection**: Uses HC-SR04 ultrasonic sensor to detect door open/close
- **RGBW LED Control**: Full control over SK6812 RGBW LED strips
- **Web Interface**: Modern, responsive web UI for configuration
- **Live Updates**: Real-time door status and distance monitoring
- **WiFi Auto-Reconnect**: Automatic reconnection with visual status indicators
- **OTA Updates**: Over-the-air firmware updates
- **Adjustable Settings**: Configurable threshold, LED count, and colors
- **Noise Filtering**: Advanced signal filtering to prevent false triggers

## Hardware Requirements

- ESP32 Development Board (ESP32-S3-DevKitC-1 or compatible)
- HC-SR04 Ultrasonic Distance Sensor
- SK6812 RGBW LED Strip
- Power supply (5V for LED strip, 3.3V/5V for ESP32)
- Jumper wires and breadboard/PCB

## Wiring Diagram

| Component | ESP32 Pin | Notes |
|-----------|-----------|-------|
| HC-SR04 Trig | GPIO 12 | Trigger pin |
| HC-SR04 Echo | GPIO 14 | Echo pin |
| LED Strip Data | GPIO 27 | Data line to LED strip |
| LED Strip GND | GND | Common ground |
| LED Strip VCC | 5V | External 5V power recommended |

 **Important**: For reliable operation, use a logic level shifter between  (3.3V) and SK6812 data line (5V), or use a 330Ω resistor in series with the data line.

## Software Setup

### 1. Clone the Repository
```bash
git clone https://github.com/yourusername/esp32-door-light-controller.git
cd esp32-door-light-controller
```

### 2. Install PlatformIO
- Install [PlatformIO](https://platformio.org/) (VS Code extension recommended)
- Open the project folder in VS Code

### 3. Configure Settings
```bash
cp src/config.h.example src/config.h
```

Edit `src/config.h` with your settings:
```cpp
#define WIFI_SSID "Your_WiFi_Network"
#define WIFI_PASSWORD "Your_WiFi_Password"
#define OTA_PASSWORD "Your_OTA_Password"
// Adjust pin assignments and defaults as needed
```

### 4. Build and Upload
```bash
pio run --target upload
```

## Web Interface

After successful upload and WiFi connection:

1. Check the serial monitor for the ESP32's IP address
2. Open the IP in your web browser
3. Use the interface to:
   - Monitor door status (OPEN/CLOSED) in real-time
   - View current distance readings
   - Adjust LED colors using RGBW sliders
   - Set the number of LEDs
   - Configure the open threshold distance

## Configuration Options

### Distance Threshold
- Set the distance (in inches) at which the door is considered "open"
- Readings above this threshold OR exactly 0 (no echo) trigger the lights

### LED Settings
- **Count**: Number of LEDs in your strip (1-300)
- **Colors**: Individual control of Red, Green, Blue, and White channels (0-255)

### Advanced Features
- **Noise Filtering**: Uses a 5-sample rolling buffer to prevent false triggers
- **WiFi Monitoring**: Visual indicators (red flashing) when WiFi disconnects
- **Auto-Reconnect**: Automatic WiFi reconnection with status feedback

## API Endpoints

- `GET /` - Web interface
- `GET /status` - JSON status (door state, distance, current settings)
- `GET /set?param=value` - Update settings

Example status response:
```json
{
  "doorOpen": true,
  "distance": 3.45,
  "r": 255,
  "g": 0,
  "b": 0,
  "w": 0,
  "numLeds": 40,
  "thresh": 2.0
}
```

## Troubleshooting

### LEDs show wrong colors
- Your SK6812 strip may have different color ordering
- Try swapping R/G values in the `setAllLeds()` function
- Check wiring and ensure proper power supply

### False triggers/flashing
- Adjust the distance threshold
- Ensure stable mounting of the ultrasonic sensor
- Check for reflective surfaces that might cause interference

### WiFi connection issues
- Verify credentials in `config.h`
- Check 2.4GHz WiFi compatibility
- Monitor serial output for connection status

### OTA updates not working
- Ensure OTA password is correct
- Check that ESP32 and computer are on same network
- Use `pio run --target upload --upload-port IP_ADDRESS`

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Built with [PlatformIO](https://platformio.org/)
- Uses [Adafruit NeoPixel Library](https://github.com/adafruit/Adafruit_NeoPixel)
- Web server powered by [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)

## Support

If you find this project helpful, please consider giving it a ⭐ on GitHub!

For issues and questions, please use the [GitHub Issues](https://github.com/yourusername/esp32-door-light-controller/issues) page.
