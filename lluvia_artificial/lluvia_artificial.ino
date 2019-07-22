// This code needs an Almoraima v2.x board installed on an Arduino Leonardo
// Author: Javier Villaverde Ramallo
// Date:   2018.12.20
#include <Wire.h>     // I2C comunication
#include <SPI.h>      // SPI comunication
#include <SD.h>       // <---- Standard Library Embedded in Arduino. File > Examples > SD (Archivo > Ejemplos > SD).
#include <RTClib.hpp> // Library by @gutierrezps https://github.com/gutierrezps/RTClib mod of Adafruit's https://learn.adafruit.com/adafruit-ds3231-precision-rtc-breakout/overview
                      // This RTC library allow compare dates.
#include <cactus_io_DS18B20.h> // Adafruit Prewired Temperature DS18B20 Sensor. For hookup details using this sensor then visit
                               // http://cactus.io/hookups/sensors/temperature-humidity/ds18b20/hookup-arduino-to-ds18b20-temperature-sensor 

// Global constants
#define mySerial        Serial    // Switchs between Serial (USB) / Serial1 (Bluetooth) to print data
#define RELAY1_STATUS   HIGH      // HIGH Abierto, LOW Cerrado
#define RELAY2_STATUS   LOW       // HIGH Abierto, LOW Cerrado

// save measures every....
const int16_t everyDays    = 0;
const int8_t  everyMinutes = 0;
const int8_t  everyHours   = 0;
const int8_t  everySeconds = 2;

const String fileName = "datalog.txt";      // Name of the file Where register the Event.
const float  flowCalibrationFactor = 4.5;   // Sensor: Flow, calibration. The hall-effect flow sensor outputs approximately 4.5 pulses per second per litre/minute of flow.

const String daysOfTheWeek[7] = {"Domingo", "Lunes", "Martes", "Miércoles", "Jueves", "Viernes", "Sábado"}; // For Date

// Pin definitions
// Note: By default relays are connected to 12v, so they are CLOSED and they don't permit water flow.
//      PIN_LED_POWER   +5V   // ALWAYS ON
#define PIN_LED_INFO    12    // OUTPUT: INFO LED,   HIGH ON, LOW OFF 
#define PIN_LED_STATUS  11    // OUTPUT: STATUS LED, HIGH ON, LOW OFF 
#define PIN_RELAY_1     10    // OUTPUT: LEFT  RELAY, WATER INPUT 1: HIGH OPEN; LOW CLOSED
#define PIN_RELAY_2     9     // OUTPUT: RIGHT RELAY, WATER INPUT 2: HIGH OPEN; LOW CLOSED
#define PIN_SENSOR_AUX  A8    // INPUT: A8 (D8). Aux sensor analog input for future use.
#define PIN_SENSOR_FLOW 7     // INPUT_PULLUP:  Flow sensor interrupt pin. 
#define PIN_SENSOR_PH   A7    // INPUT: A7 (D6).       pH meter v1.1 sensor Analog output to Arduino Analog Input A7 on Digital pin 6
#define PIN_SDCARD_SS   5     // OUTPUT: microSD card's chip select
#define PIN_SENSOR_TEMP A6    // INPUT: A6 (D4). DS18S20 temperature sensor Analog output to Arduino Analog Input A6 on Digital pin 4
//      I2C_SCL_RTC     3     // I2C communicatin for Real Time Clock 
//      I2C_SDA_RTC     2     // I2C communicatin for Real Time Clock
//      SERIAL1_TX      1     // Serial1 Bluetooth Communication
//      SERIAL2_RX      0     // Serial1 Bluetooth Communication
// SPI_MISO_MICRO_SD    MISO  // SPI communication for Micro SD
// SPI_MOSI_MICRO_SD    MOSI  // SPI communication for Micro SD
// SPI_SCK_MICRO_SD     SCK   // SPI communication for Micro SD      



// Global variables
DS18B20 ds(PIN_SENSOR_TEMP);    // Sensor: Temperature class
RTC_DS3231 rtc;                 // Sensor: RTC class
DateTime now;                   // Time: now
DateTime future;                // Time: Timestamp in the future



volatile byte flowPulseCount;   // Sensor: Flow, interruption counter.
float flowRate;                 // Sensor: Flow, litres per minute
unsigned int flowMilliLitres;   // Sensor: Flow, mililitres per mitute
unsigned long totalMilliLitres; // Sensor: Flow, accumulated millilitres


// Interrupt Service Routine for Flow Sensor
void flowPulseCounter ()
{
  ++flowPulseCount; // Increment global pulse counter variable
}


// Blinks a led for a duration milliseconds period
void blink (int pin, int duration)
{
  digitalWrite(pin, HIGH);
  delay(duration);
  
  digitalWrite(pin, LOW);
  delay(duration);
}


void setup() 
{
  // Opens Serial USB
    mySerial.begin(9600);
    //while(!mySerial) // Wait until serial opens, comment to not to wait at startup
      //blink(PIN_LED_INFO, 1000);
    mySerial.println("lluvia_artificial.ino");
   
 // Pin configuration
 // Note: By default relays are connected to 12v, so they are CLOSED and they don't permit water flow.
 // pinMode(A5, NC);                  // A5 - Arduino UNO: I2C SCL; Arduino Leonardo: NC
 // pinMode(A4, NC);                  // A4 - Arduino UNO: I2C SDA; Arduino Leonardo: NC
 // pinMode(A3, NC);                  // A3 
 // pinMode(A2, NC);                  // A2 
 // pinMode(A1, NC);                  // A1 
 // pinMode(13, NC);                  // D13                Arduino UNO: SPI SCK;  Arduino Leonardo: NC
    pinMode(PIN_LED_INFO,   OUTPUT);  // D12 - LED INFO     Arduino UNO: SPI MISO; Arduino Leonardo: NC
    pinMode(PIN_LED_STATUS, OUTPUT);  // D11 - LED STATUS   Arduino UNO: SPI MOSI; Arduino Leonardo: NC
    pinMode(PIN_RELAY_1,    OUTPUT);  // D10 - Relay 8B -> ULN2803A Darlington Transistor Array -> 8C -> RIGHT RELAY, WATER INPUT 2: HIGH OPEN; LOW CLOSED
    pinMode(PIN_RELAY_2,    OUTPUT);  // D9  - Relay 7B -> ULN2803A Darlington Transistor Array -> 7C -> LEFT  RELAY, WATER INPUT 1: HIGH OPEN; LOW CLOSED
    pinMode(PIN_SENSOR_AUX,  INPUT);  // D8  - (A8) Sensor: AUX
    pinMode(PIN_SENSOR_FLOW, INPUT_PULLUP);  // D7 - Sensor: Flow (INTERRUPT)
    pinMode(PIN_SENSOR_PH,   INPUT);  // D6 - (A7)Sensor: PH   
 // pinMode(PIN_SDCARD_SS,  OUTPUT);  // D5 - SPI Chipselect: MicroSD, initialized on SD begin function.
 // pinMode(PIN_SENSOR_TEMP, INPUT);  // D4 - (A6) Sensor: Temp, initialized on DS18B20 constructor. 
 // pinMode(3,  NC);                  // D3 - Arduino Leonardo: I2C SCL  
 // pinMode(2,  NC);                  // D2 - Arduino Leonardo: I2C SDA  
 // pinMode(1,  NC);                  // D1 - Arduino Leonardo: Serial1 
 // pinMode(0,  NC);                  // D0 - Arduino Leonardo: Serial1
   
 
 // Initialization SD
    mySerial.print("Initializing SD card...");
    // see if the card is present and can be initialized:
    if (!SD.begin(PIN_SDCARD_SS)) {
      mySerial.println("ERROR: Card failed, or not present");
      while(true)
        blink(PIN_LED_INFO, 250);
    }
    mySerial.println("SD card initialized.");


  // Initialization RTC
    mySerial.print("Initializing RTC...");
    digitalWrite(PIN_LED_INFO, HIGH);
    if (! rtc.begin()) {
      mySerial.println("ERROR: Couldn't find RTC");
      while(true)
        blink(PIN_LED_INFO, 250);
    }
    
    // Set time automatically if needed.
    if (rtc.lostPower()) {
      mySerial.println("RTC lost power, lets set the time!");
      // following line sets the RTC to the date & time this sketch was compiled
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      // This line sets the RTC with an explicit date & time, for example to set
      // January 21, 2014 at 3am you would call:
      // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }
    mySerial.println("rtc initialized.");


  // Initialization Temperature sensor
    mySerial.println("Maxim Integrated DS18B20 Temperature Sensor | cactus.io");
    ds.getSerialNumber();

    
  // Initialization Global variables
    now = future = rtc.now(); // RTC:  Get time for first time
    

  // Pin Initialization
    digitalWrite(PIN_RELAY_1,    RELAY1_STATUS);  // Relé entrada de agua 1 HIGH, ABIERTO
    digitalWrite(PIN_RELAY_2,    RELAY2_STATUS);  // Relé entrada de agua 2 LOW,  CERRADO
    digitalWrite(PIN_LED_STATUS, HIGH);           // Led Verde encendido, todo OK
    digitalWrite(PIN_LED_INFO,   LOW);            // Led Azul apagado,    todo OK


  // System Started!
    mySerial.println("System started!");
    saveEventToSD("System started!");
} // setup()


// Global:
//    variables:  flowRate, flowMilliLitres, totalMilliLitres
void loop() 
{
  float temp_C;    // Sensor: Temperature, value ºC
  float phValue;   // Sensor: Ph, value

  if (itsMesureTime())
  {
    // Medir caudal del sensor YF-S201
    measureFlow();              // AVAILABLE DATA EACH SECOND
    
    // Medir PH del sensor 
    phValue = measurePH();      // AVAILABLE DATA INSTANT
    
    // Leer temperatura del sensor DS18B20
    ds.readSensor();            // AVAILABLE DATA INSTANT
    temp_C = ds.getTemperature_C();


    // Construct Event  to register on microSD card
    String event = "Caudal: ";
    event += String(flowRate);
    event += "L/min, ";
    event += String(flowMilliLitres);
    event += "mL/min";
    event += ",\tCaudal acumulado: ";
    event += String((totalMilliLitres/1000));
    event += "L, ";
    event += String(totalMilliLitres);
    event += "mL";
    event += ",\tPH: ";
    event += String(phValue);
    event += ",\t Temperatura: ";
    event += String(temp_C);
    event += " ºC";

    saveEventToSD(event);
  } 
} // loop()


// Devuelve True si ha pasado el tiempo especificado, False en caso contrario
// Global:
//    constantes: int16_t everyDays, int8_t everyHours, int8_t everyMinutes, int8_t everySeconds
//    variables:  DateTime now, DateTime future, RTC_DS3231 rtc
bool itsMesureTime (void)
{
  now = rtc.now(); // Get the Time

  if (now >= future)
  {
    future = now + TimeSpan(everyDays, everyHours, everyMinutes, everySeconds);
    return true;
  }
    return false;
}


// Global:
//    constantes: PIN_SENSOR_FLOW, flowCalibrationFactor
//    variables:  flowPulseCount, flowRate, flowMilliLitres, totalMilliLitres
void measureFlow ()
{
    // Reset the pulse counter so we can start incrementing again
    flowPulseCount = 0;

    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    unsigned long oldTime = millis();
    
    // The Hall-effect sensor is connected to pin PIN_SENSOR_FLOW which uses interrupt (View Arduino Documentation).
    // Configured to trigger on a FALLING state change (transition from HIGH state to LOW state)
    attachInterrupt(digitalPinToInterrupt(PIN_SENSOR_FLOW), flowPulseCounter, FALLING);

    // Wait for 1 second (measuring sensor)
    while ((millis() - oldTime) <= 1000);
  
    // Disable the interrupt while calculating flow rate and sending the value to the host
    detachInterrupt(digitalPinToInterrupt(PIN_SENSOR_FLOW));

    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - oldTime)) * flowPulseCount) / flowCalibrationFactor;
    
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;
    
    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
} // measureFlow()


// Returns phValue
// Global:
//    constantes: PIN_SENSOR_PH
float measurePH ()
{
    int buf[10],temp;
    float phValue = 0;
    unsigned long int avgValue = 0; //Store the average value of the sensor feedback
    
    // Get 10 sample value from the sensor for smooth the value
    for(int i=0;i<10;i++) 
    { 
      buf[i]=analogRead(PIN_SENSOR_PH);
      delay(10);
    }
    
    // Sort the analog from small to large
    for(int i=0;i<9;i++) 
      for(int j=i+1;j<10;j++)
        if(buf[i]>buf[j]) {
          temp=buf[i];
          buf[i]=buf[j];
          buf[j]=temp;
        }
  
    // Take the average value of 6 center sample
    for(int i=2;i<8;i++) 
      avgValue+=buf[i];
  
    phValue = (float)avgValue*5.0/1024.0/6.0; //convert the analog into millivolt
    phValue = 3.5*phValue;                    //convert the millivolt into pH value
  
    return phValue;
} // measurePH()


// dateTime callback for add date's modification of a file on SDCARD
// Global:
//    variables: DateTime now, rtc
void dateTime(uint16_t* date, uint16_t* time) 
{
     now = rtc.now();
  
    // return date using FAT_DATE macro to format fields
    *date = FAT_DATE(now.year(), now.month(), now.day());
  
    // return time using FAT_TIME macro to format fields
    *time = FAT_TIME(now.hour(), now.minute(), now.second());
}


// Guarda un evento Fecha + Datos en un fichero de la microSD.
// Global: 
//    constantes: String[] daysOfTheWeek,
//    variables:  String date, String event, DateTime now
void saveEventToSD (const String& event)
{
    /* La concatenación de cadenas probablemente desborda la pila de llamadas: no se imprimen las cadenas en Serial */        
    // Construct Date to register on microSD card
    String date = "";
    date += String(now.year());
    date += "/";
    date += String(now.month());
    date += "/";
    date += String(now.day());
    date += " (";
    date += daysOfTheWeek[now.dayOfTheWeek()];
    date += ") ";
    date += String(now.hour());
    date += ":";
    date += String(now.minute());
    date += ":"; 
    date += String(now.second());
  
    // DEBUG Date & Event
    //mySerial.print("saveEventToSD says, date: ");  mySerial.println(date);
    //mySerial.print("saveEventToSD says, event: "); mySerial.println(event);
    
    // Save To SD
    SdFile::dateTimeCallback(dateTime); // https://forum.arduino.cc/index.php?topic=348562.0
    File dataFile = SD.open(fileName, FILE_WRITE);  // class for abstract File inside the microSD card
    while(!dataFile) 
    {
      mySerial.println("saveEventToSD says, file failed to open, retrying in 1 sec...");
      blink(PIN_LED_INFO, 1000);
      
      SdFile::dateTimeCallback(dateTime);
      dataFile = SD.open(fileName, FILE_WRITE);
    }
  
    if (dataFile)
    {
      dataFile.print(date);
      dataFile.print(" - ");
      dataFile.println(event);
      dataFile.close();

      // DEBUG Date + Event
      mySerial.print("saveEventToSD says, savedEvent: ");
      mySerial.print(date);
      mySerial.print(" - ");
      mySerial.println(event);
    }
    else
      mySerial.println("ERROR: esto no debería pasar en la vida, dataFile corrupt");
      

} // saveEventToSD()
