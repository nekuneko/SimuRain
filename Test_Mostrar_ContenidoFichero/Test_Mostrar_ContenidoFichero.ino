/*
  SD card file dump

 This example shows how to read a file from the SD card using the
 SD library and send it over the serial port.

 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

 created  22 December 2010
 by Limor Fried
 modified 9 Apr 2012
 by Tom Igoe
 modified 12 Dec 2018
 by NekuNeko

 This example code is in the public domain.
 */

#include <SPI.h>
#include <SD.h>

// Global constants
const String fileName = "datalog.txt";
#define PIN_SDCARD_SS   5   // OUTPUT: microSD card's chip select


void testExiste ()
{
  if (SD.exists(fileName))
    Serial.println("El fichero donde guardar los eventos existe");
  else
    Serial.println("El fichero donde guardar los eventos no existe");
}


void setup() 
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(PIN_SDCARD_SS)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

  testExiste();

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(fileName);

  // if the file is available, write to it:
  if (dataFile) {
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else
    Serial.println("error opening file");
  
} // setup()


void loop() {
} // loop()