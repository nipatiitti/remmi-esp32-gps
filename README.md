# ESP32 OLED CAN GPS

A real-time GPS tracker and speedometer system built for the Seeed XIAO ESP32-C3 with OLED display and CAN bus communication capabilities.

## 🚀 Features

- **GPS Tracking**: Real-time GPS coordinate logging with speed measurement
- **OLED Display**: Live speed display with lap timing visualization
- **CAN Bus Communication**: Broadcasts GPS data over CAN network
- **Real-time Updates**: Continuous GPS data processing and display updates
- **Lap Time Tracking**: Visual representation of relative lap times
- **High Precision**: 7 decimal place GPS coordinate accuracy

## 📋 Hardware Requirements

- **Seeed XIAO ESP32-C3** development board
- **128x64 OLED Display** (SSD1306) connected via I2C
- **GPS Module** with UART communication (38400 baud)
- **MCP2515 CAN Bus Module** for CAN communication
- **Connecting wires** and breadboard/PCB

## 🔌 Pin Configuration

### GPS Module
- **RX Pin**: D3 (GPIO 4)
- **TX Pin**: D2 (GPIO 3)
- **Baud Rate**: 38400

### OLED Display (I2C)
- **SCL**: Pin 7 (GPIO 7)
- **SDA**: Pin 6 (GPIO 6)

### CAN Bus (SPI)
- **CS Pin**: D7 (GPIO 10)
- **CAN Speed**: 1 Mbps

## 📦 Dependencies

The project uses PlatformIO with the following libraries:

```ini
lib_deps = 
    olikraus/U8g2@^2.35.17          # OLED display library
    mikalhart/TinyGPSPlus@^1.0.3    # GPS parsing library
```

Additional included library:
- **Arduino_CAN_BUS_MCP2515** (included in `/lib/` folder)

## 🛠️ Installation & Setup

1. **Clone the repository**
   ```bash
   git clone <repository-url>
   cd esp-oled-can-gps
   ```

2. **Install PlatformIO**
   - Install [PlatformIO IDE](https://platformio.org/install/ide) or CLI

3. **Build and Upload**
   ```bash
   pio run --target upload
   ```

4. **Monitor Serial Output**
   ```bash
   pio device monitor
   ```

## 🏗️ Project Structure

```
├── platformio.ini          # PlatformIO configuration
├── src/
│   ├── main.cpp            # Main application logic
│   ├── CAN.cpp             # CAN bus communication
│   └── display.cpp         # OLED display functions
├── include/
│   ├── config.h            # Hardware configuration
│   ├── CAN.h               # CAN function declarations
│   ├── display.h           # Display function declarations
│   └── animation.h         # Animation definitions
├── lib/
│   └── Arduino_CAN_BUS_MCP2515-1.0.0/  # CAN bus library
└── test/
    └── test.csv            # Sample GPS NMEA data
```

## 📡 CAN Bus Protocol

The system broadcasts GPS data over CAN bus using the following message structure:

### Message 1 - Coordinates (ID: 0x680)
- **Bytes 0-3**: Latitude (32-bit fixed point, 7 decimal places)
- **Bytes 4-7**: Longitude (32-bit fixed point, 7 decimal places)

### Message 2 - Time Data (ID: 0x680 + 1)
- **Bytes 0-3**: Time and date information
- **Bytes 4-7**: Additional time data

### Message 3 - Speed & Navigation (ID: 0x680 + 2)
- **Speed**: GPS speed in km/h
- **Altitude**: GPS altitude in meters
- **Course**: True course in degrees
- **HDOP**: Horizontal dilution of precision
- **Satellites**: Number of satellites in view

## 📺 Display Features

The OLED display shows:
- **Large Speed Display**: Current speed in km/h (up to 99.9 km/h)
- **Lap Time Bar**: Visual representation of relative lap timing
- **Loading Screen**: GPS connection status
- **Direction Indicator**: Shows positive/negative lap time direction

## 🔧 Configuration

Key configuration parameters in `config.h`:

```cpp
#define SPI_CS_PIN D7           // CAN bus CS pin
uint32_t CAN_SEND_ID = 0x680;   // CAN transmission ID
uint32_t CAN_RECEIVE_ID = 0x30A; // CAN reception ID
```

## 🐛 Troubleshooting

### Common Issues

1. **CAN Bus Initialization Fails**
   - Check SPI connections
   - Verify CS pin configuration
   - Ensure proper power supply

2. **No GPS Signal**
   - Check antenna connection
   - Verify UART wiring (RX/TX)
   - Test in open sky conditions

3. **Display Not Working**
   - Verify I2C connections
   - Check display power supply
   - Confirm correct I2C address

### Serial Monitor Output

Monitor the serial output at 115200 baud for debugging:
- CAN bus initialization status
- GPS data parsing information
- System status messages

### Problems with uploading code?

Check if you are affected by [this](https://community.platformio.org/t/can-no-longer-upload-or-monitor-xiao-esp32-c3/40992) problem

## 📝 License

This project is part of the Remmi team development. Please check with the team for licensing information.

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## 📞 Support

For issues and questions:
- Check the troubleshooting section
- Review the serial monitor output
- Contact the Remmi team

---

**Built with ❤️ by the Remmi Team**