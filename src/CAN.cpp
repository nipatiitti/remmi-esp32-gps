#include "CAN.h"

#include <mcp_canbus.h>

#include "config.h"

// Define global variables declared in config.h
MCP_CAN CAN(SPI_CS_PIN);
uint32_t CAN_SEND_ID = 0x680;
uint32_t CAN_RECEIVE_ID = 0x30A;

void sendGPSData(float latitude, float longitude, TinyGPSTime time,
                 TinyGPSDate date, float speed, float altitude,
                 float trueCourse, float hdop, uint8_t satellites) {
    // Convert latitude and longitude to 32-bit fixed point (7 decimal places)
    int32_t latFixed = static_cast<int32_t>(latitude * 10000000.0);
    int32_t lonFixed = static_cast<int32_t>(longitude * 10000000.0);

    // Prepare CAN message for latitude and longitude
    uint8_t data1[8];
    data1[0] = (latFixed >> 24) & 0xFF;
    data1[1] = (latFixed >> 16) & 0xFF;
    data1[2] = (latFixed >> 8) & 0xFF;
    data1[3] = latFixed & 0xFF;
    data1[4] = (lonFixed >> 24) & 0xFF;
    data1[5] = (lonFixed >> 16) & 0xFF;
    data1[6] = (lonFixed >> 8) & 0xFF;
    data1[7] = lonFixed & 0xFF;

    // Send the first CAN message
    CAN.sendMsgBuf(CAN_SEND_ID, 0, 8, data1);

    // Convert time to HHMMSS.sss format
    uint32_t timeValue = (time.hour() * 10000000) + (time.minute() * 100000) +
                         (time.second() * 1000) + (time.centisecond() * 10);

    // Convert speed to 16-bit unsigned value (0.1 km/h resolution)
    uint16_t speedFixed = static_cast<uint16_t>(speed * 10.0);

    // Convert altitude to 16-bit signed value (0.1 m resolution)
    int16_t altitudeFixed = static_cast<int16_t>(altitude * 10.0);

    // Prepare CAN message for time, speed, and altitude
    uint8_t data2[8];
    data2[0] = (timeValue >> 24) & 0xFF;
    data2[1] = (timeValue >> 16) & 0xFF;
    data2[2] = (timeValue >> 8) & 0xFF;
    data2[3] = timeValue & 0xFF;
    data2[4] = (speedFixed >> 8) & 0xFF;
    data2[5] = speedFixed & 0xFF;
    data2[6] = (altitudeFixed >> 8) & 0xFF;
    data2[7] = altitudeFixed & 0xFF;

    // Send the second CAN message
    CAN.sendMsgBuf(CAN_SEND_ID + 1, 0, 8, data2);

    // Convert date to DDMMYY format
    uint32_t dateValue =
        (date.day() * 10000) + (date.month() * 100) + (date.year() % 100);

    // Convert true course to 16-bit signed value (0.1 degree resolution)
    int16_t trueCourseFixed = static_cast<int16_t>(trueCourse * 10.0);

    // Convert HDOP to 8-bit unsigned value (0.1 resolution)
    uint8_t hdopFixed = static_cast<uint8_t>(hdop * 10.0);

    // Prepare CAN message for date, validity, true course, HDOP, and satellites
    uint8_t data3[8];
    data3[0] = (dateValue >> 16) & 0xFF;
    data3[1] = (dateValue >> 8) & 0xFF;
    data3[2] = dateValue & 0xFF;
    data3[3] = 1;  // GPS Valid
    data3[4] = (trueCourseFixed >> 8) & 0xFF;
    data3[5] = trueCourseFixed & 0xFF;
    data3[6] = hdopFixed;
    data3[7] = satellites;

    // Send the third CAN message
    CAN.sendMsgBuf(CAN_SEND_ID + 2, 0, 8, data3);

    // Prepare CAN message for magnetic variation, DGPS station ID, DGPS update
    // time, FAA mode, fix quality, and WGS84 height
    uint8_t data4[8];
    data4[0] = 0;
    data4[1] = 0;
    data4[2] = 0;
    data4[3] = 0;
    data4[4] = 1;  // GPS FAA Mode
    data4[5] = 1;  // GPS Fix Quality
    data4[6] = 0;
    data4[7] = 0;

    // Send the fourth CAN message
    CAN.sendMsgBuf(CAN_SEND_ID + 3, 0, 8, data4);
}
