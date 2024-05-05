#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#include <TinyGPS++.h>
TinyGPSPlus gps;

File myFile;

#define sdChipSelect 5

unsigned long lastPrint;
unsigned long bytesWritten = 0;

void setup()
{
    Serial.begin(115200);
    while (!Serial);
    Serial.println("hoge");

    Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX, TX pins for UART communication with GPS module

    while (Serial.available())
    {
        Serial.read();
    }

    Serial.println(F("Press any key to start logging."));

    while (!Serial.available())
    {
        ;
    }

    delay(100);

    while (Serial.available())
    {
        Serial.read();
    }

    Serial.println("Initializing SD card..");

    SPI.begin(18, 19, 23, 5); // Initialize SPI with SCK, MISO, MOSI, and CS pins
    if (!SD.begin(sdChipSelect))
    {
        Serial.println("Card failed, or not present. Freezing...");
        while (1);
    }
    Serial.println("SD card initialized.");

    myFile = SD.open("/GPS_Data.txt", FILE_WRITE);
    if (!myFile)
    {
        Serial.println(F("Failed to create GPS data file! Freezing..."));
        while (1);
    }

    Serial.println(F("Press any key to stop logging."));

    lastPrint = millis();
}

void loop()
{
    while (Serial2.available() > 0)
    {
        if (gps.encode(Serial2.read()))
        {
            if (gps.location.isValid())
            {
                String dataString = "";
                dataString += String(gps.location.lat(), 6);
                dataString += ",";
                dataString += String(gps.location.lng(), 6);
                dataString += ",";
                dataString += String(gps.altitude.meters());
                dataString += ",";
                dataString += String(gps.date.value());
                dataString += ",";
                dataString += String(gps.time.value());
                dataString += "\r\n";

                myFile.print(dataString);
                bytesWritten += dataString.length();
            }
        }
    }

    if (millis() > (lastPrint + 1000))
    {
        Serial.print(F("The number of bytes written to SD card is "));
        Serial.println(bytesWritten);
        lastPrint = millis();
    }

    if (Serial.available())
    {
        myFile.close();
        Serial.print(F("The total number of bytes written to SD card is "));
        Serial.println(bytesWritten);
        Serial.println(F("Logging stopped. Freezing..."));
        while (1);
    }
}
