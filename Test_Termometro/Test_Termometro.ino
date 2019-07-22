/* Example sketch for Maxim Integrated DS18B20 temperature sensor
   Written by cactus.io, and requires the cactus_io_DS18B20 library.
   This sketch was tested using the Adafruit Prewired DS18B20 Sensor.
   For hookup details using this sensor then visit
   http://cactus.io/hookups/sensors/temperature-humidity/ds18b20/hookup-arduino-to-ds18b20-temperature-sensor
*/
#include "cactus_io_DS18B20.h" 

// Global constants
#define mySerial        Serial  // Switchs between Serial (USB) / Serial1 (Bluetooth) to print data
#define RELAY1_STATUS   HIGH    // HIGH Abierto, LOW Cerrado
#define RELAY2_STATUS   LOW     // HIGH Abierto, LOW Cerrado
#define DELAY_TIME      1000    // ms

#define PIN_LED_INFO    12      // OUTPUT: INFO LED,   HIGH ON, LOW OFF 
#define PIN_LED_STATUS  11      // OUTPUT: STATUS LED, HIGH ON, LOW OFF 
#define PIN_RELAY_1     10      // OUTPUT: LEFT  RELAY, WATER INPUT 1: HIGH OPEN; LOW CLOSED
#define PIN_RELAY_2     9       // OUTPUT: RIGHT RELAY, WATER INPUT 2: HIGH OPEN; LOW CLOSED
#define PIN_SENSOR_TEMP A6      // INPUT: A6 (D4). DS18S20 temperature sensor Analog output to Arduino Analog Input A6 on Digital pin 4
#define PIN_SENSOR_AUX  A8      // INPUT: A8 (D8). Aux sensor analog input for future use.


// Global variables
DS18B20 ds(PIN_SENSOR_TEMP);  // Creates DS18B20 object


void setup(void) 
{
  // Initialize a serial connection for reporting values to the host
    mySerial.begin(115200);
    while(!mySerial);
    mySerial.println("Test_Termometro.ino");


  // Initialize DS18B20 temperature sensor
    mySerial.println("Maxim Integrated DS18B20 Temperature Sensor | cactus.io");
    mySerial.print("DS18B20 Serial Number: ");
    
    ds.readSensor();
    // we pass the serial number byte array into the printSerialNumber function
    printSerialNumber(ds.getSerialNumber());

  
  // Pin initialization
    pinMode(PIN_RELAY_1,      OUTPUT);            // Relé entrada de agua 1
    pinMode(PIN_RELAY_2,      OUTPUT);            // Relé entrada de agua 2
    pinMode(PIN_LED_STATUS,   OUTPUT);            // Led Estado Verde
    pinMode(PIN_LED_INFO,     OUTPUT);            // Led Estado Azul
//  pinMode(PIN_SENSOR_TEMP,  INPUT);             // Sensor TEMP analog input (initialized on DS18B20 constructor)
    pinMode(PIN_SENSOR_AUX,   INPUT);             // Sensor AUX  analog input
    
    digitalWrite(PIN_RELAY_1,    RELAY1_STATUS);  // Relé entrada de agua 1 HIGH, ABIERTO
    digitalWrite(PIN_RELAY_2,    RELAY2_STATUS);  // Relé entrada de agua 2 LOW,  CERRADO
    digitalWrite(PIN_LED_STATUS, HIGH);           // Led Verde encendido, todo OK
    digitalWrite(PIN_LED_INFO,   LOW);            // Led Azul apagado,    todo OK


  // Print relay status
    mySerial.println("Estado inicial de las válvulas:");
    mySerial.print("\tVálvula 1: ");
      if (digitalRead(PIN_RELAY_1) == HIGH)
        mySerial.println("HIGH");
      else
        mySerial.println("LOW");
    mySerial.print("\tVálvula 2: ");
      if (digitalRead(PIN_RELAY_2) == HIGH)
        mySerial.println("HIGH");
      else
        mySerial.println("LOW"); 

    mySerial.println("\n\nTemp (C)\tTemp (F)");
} // setup()


void loop(void) 
{  
    ds.readSensor();
  
    mySerial.print(ds.getTemperature_C()); mySerial.print(" *C\t");
    mySerial.print(ds.getTemperature_F()); mySerial.println(" *F");
  
    delay(DELAY_TIME); // Add some delay just here to slow down the output.
} // loop()


// We call this function to display the DS18B20 serial number. 
// It takes an array of bytes for printing
void printSerialNumber(byte *addr) 
{
    byte i;
  
    for( i = 0; i < 8; i++) {
        mySerial.print("0x");
        if (addr[i] < 16) {
          mySerial.print('0');
        }
        mySerial.print(addr[i], HEX);
        if (i < 7) {
          mySerial.print(", ");
        }
    }
} // printSerialNumber()
