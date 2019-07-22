#include <Wire.h>     // I2C comunication
#include <SPI.h>      // SPI comunication
#include <SD.h>       // <---- Standard Library Embedded in Arduino. File > Examples > SD (Archivo > Ejemplos > SD).
#include <RTClib.hpp> // Library by @gutierrezps https://github.com/gutierrezps/RTClib mod of Adafruit's https://learn.adafruit.com/adafruit-ds3231-precision-rtc-breakout/overview
                      // This RTC library allow compare dates.
#include <cactus_io_DS18B20.h> // Adafruit Prewired Temperature DS18B20 Sensor. For hookup details using this sensor then visit
                               // http://cactus.io/hookups/sensors/temperature-humidity/ds18b20/hookup-arduino-to-ds18b20-temperature-sensor 

// Global constants
const float  flowCalibrationFactor = 4.5;   // Sensor: Flow, calibration. The hall-effect flow sensor outputs approximately 4.5 pulses per second per litre/minute of flow.
const String fileName = "datalog.txt";      // Name of the file Where register the Event.

// The Event will trigger every....
const int16_t everyDays    = 0;
const int8_t  everyMinutes = 0;
const int8_t  everyHours   = 0;
const int8_t  everySeconds = 2;

const String daysOfTheWeek[7] = {"Domingo", "Lunes", "Martes", "Miércoles", "Jueves", "Viernes", "Sábado"}; // For Date

// Pin definitions

#define PIN_SENSOR_FLOW 0         // INPUT_PULLUP:  Flow sensor interrupt pin
#define PIN_RELAY_RIGHT 1         // RELE DERECHO, HIGH ABIERTO, LOW CERRADO
#define PIN_RELAY_LEFT  4         // RELE IZQUIERDO, HIGH ABIERTO, LOW CERRADO
#define PIN_SENSOR_TEMP 8         // INPUT:  Temperature sensor analog output to Arduino Analog Input on Digital 8.
#define PIN_LED3        9         // OUTPUT: LED3, HIGH ON, LOW OFF 
#define PIN_SDCARD_SS   10        // OUTPUT: microSD card's chip select
#define PIN_SENSOR_AUX  A0        // INPUT:  Aux sensor for future use.
#define PIN_SENSOR_PH   A1        // INPUT:  pH meter Analog output to Arduino Analog Input 1 (A1)
#define PIN_LED2        A2        // OUTPUT: LED2, HIGH ON, LOW OFF 
#define LED_BUILTIN     PIN_LED2  // LED_BUILTIN NOT CONNECTED TO 13
// POWER_LED        ->  VIN       // ALWAYS ON

// Global variables
DS18B20 ds(PIN_SENSOR_TEMP);    // Sensor: Temperature class
RTC_DS3231 rtc;                 // Sensor: RTC class
DateTime now;                   // Time: now
DateTime future;                // Time: Timestamp in the future

String date;                    // Event's date to register on microSD card
String event;                   // Event to register on microSD card
float temp_C;                   // Sensor: Temperature, value ºC
float phValue;                  // Sensor: Ph, value
volatile byte flowPulseCount;   // Sensor: Flow, interruption counter.
float flowRate;                 // Sensor: Flow, litres per minute
unsigned int flowMilliLitres;   // Sensor: Flow, mililitres per mitute
unsigned long totalMilliLitres; // Sensor: Flow, accumulated millilitres
unsigned long oldTime;          // Sensor: Flow, timer for measure each second.


// We call this function to display the DS18B20 temperature sensor serial number. It takes an array of bytes for printing
void printSerialNumber(byte *addr) {
    byte i;
  
    for( i = 0; i < 8; i++) {
        Serial.print("0x");
        if (addr[i] < 16) {
          Serial.print('0');
        }
        Serial.print(addr[i], HEX);
        if (i < 7) {
          Serial.print(", ");
        }
    }
    Serial.println();
}

// blink led for show errors
void blink (int pin, int duration, int direction)
{
  pinMode(pin, direction);

  while (true)
  {
    digitalWrite(pin, HIGH);
    delay(duration);

    digitalWrite(pin, LOW);
    delay(duration);
  }
}

// Error 0 - SD Card not present
// Error 1 - RTC not present
// Error 2 - Temperature sensor fail
void showSystemError (int myError)
{
  switch (myError)
  {
    case 0:
      blink(LED_BUILTIN, 250, OUTPUT);
      break;
    case 1:
      blink(LED_BUILTIN, 500, OUTPUT);
      break;
    case 2:
      blink(LED_BUILTIN, 1000, OUTPUT);
      break;
    default:
      break;
  }
}

// Interrupt Service Routine for Flow Sensor
void flowPulseCounter ()
{
  flowPulseCount++; // Increment global pulse counter variable
}

void measureFlow ()
{
    // Reset the pulse counter so we can start incrementing again
    flowPulseCount = 0;

    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    oldTime = millis();
    
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
}

void measurePH ()
{
  //*************************************************************************************
  // Medir PH: Codigo de Test_Peachimetro/phMeterSample/phMeterSample.ino
    int buf[10],temp;
    for(int i=0;i<10;i++) //Get 10 sample value from the sensor for smooth the value
    { 
      buf[i]=analogRead(PIN_SENSOR_PH);
      delay(10);
    }
  
    //sort the analog from small to large
    for(int i=0;i<9;i++) 
      for(int j=i+1;j<10;j++)
        if(buf[i]>buf[j]) {
          temp=buf[i];
          buf[i]=buf[j];
          buf[j]=temp;
        }
    
    unsigned long int avgValue = 0; //Store the average value of the sensor feedback
    for(int i=2;i<8;i++) //take the average value of 6 center sample
      avgValue+=buf[i];
    
    phValue = (float)avgValue*5.0/1024.0/6.0; //convert the analog into millivolt
    phValue = 3.5*phValue;                    //convert the millivolt into pH value
  //*************************************************************************************
}


// dateTime callback for add date's modification of a file on SDCARD
void dateTime(uint16_t* date, uint16_t* time) {
   now = rtc.now();

  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(now.year(), now.month(), now.day());

  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(now.hour(), now.minute(), now.second());
}

// Global: String date, String event 
void saveEventToSD ()
{
  /* La concatenación de cadenas probablemente desborda la pila de llamadas: no se imprimen las cadenas en Serial */        
  // Construct Date
  date = "";
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
  //Serial.print("saveEventToSD says, date: ");  Serial.println(date);
  //Serial.print("saveEventToSD says, event: "); Serial.println(event);
  
  // Save To SD
  SdFile::dateTimeCallback(dateTime); // https://forum.arduino.cc/index.php?topic=348562.0
  File dataFile = SD.open(fileName, FILE_WRITE);  // class for abstract File inside the microSD card
  while(!dataFile) 
  {
    Serial.println("saveEventToSD says, file failed to open, retrying in 1 sec...");
    delay(1000);
    
    SdFile::dateTimeCallback(dateTime);
    dataFile = SD.open(fileName, FILE_WRITE);
  }

  if (dataFile)
  {
  dataFile.print(date);
  dataFile.print(" - ");
  dataFile.println(event);
  dataFile.close();
  }
  else
    Serial.println("ERROR: esto no debería pasar en la vida, dataFile corrupt");
    
  // DEBUG Date + Event
  Serial.print("saveEventToSD says, savedEvent: ");
  Serial.print(date);
  Serial.print(" - ");
  Serial.println(event);   
}


// The Event will trigger every....
void checkIfTriggerEvent (int16_t everyDays, int8_t everyMinutes, int8_t everyHours, int8_t everySeconds)
{
  now = rtc.now(); // Get the Time

  // now is future, the time has come...
  if (now >= future)
  {
    saveEventToSD();
    future = now + TimeSpan(everyDays, everyHours, everyMinutes, everySeconds);
    //Serial.println("triggerEvent says: future updated ");
  }
  /*
  else // DEBUG: keep waiting the time...
    Serial.println("triggerEvent says: now is less ");
  */
}

void setup() 
{
  // Wait until serial opens
    Serial.begin(115200);
   // while(!Serial); // comment to not to wait at startup

 // Pin configuration
    pinMode(PIN_SENSOR_AUX, INPUT); // A0 - Sensor: AUX
    pinMode(PIN_SENSOR_PH, INPUT);  // A1 - Sensor: PH
    pinMode(PIN_LED2, OUTPUT);      // A2 - LED 2
 // pinMode(A3, NC);          // A3 
 // pinMode(A4, OUTPUT);      // A4 - Arduino UNO: I2C SDA; Arduino Leonardo: NC
 // pinMode(A5, OUTPUT);      // A5 - Arduino UNO: I2C SCL; Arduino Leonardo: NC
    pinMode(PIN_SENSOR_FLOW, INPUT); // D0 - Sensor: Flow    
    pinMode(PIN_RELAY_RIGHT, OUTPUT);       // D1 - Relay ULN2803A: 3B Darlington Transistor Array -> 3C -> BLUE_OUT (Relé Salida?)  -> S RELE DERECHO, HIGH ABIERTO, LOW CERRADO
 // pinMode(2,  NC);          // D2 - Arduino Leonardo: I2C SDA
 // pinMode(3,  NC);          // D3 - Arduino Leonardo: I2C SCL    
    pinMode(PIN_RELAY_LEFT, OUTPUT); // D4 - Relay ULN2803A: 2B Darlington Transistor Array -> 2C -> BLUE_OUT (Relé Entrada?) -> E  RELE IZQUIERDO, HIGH ABIERTO, LOW CERRADO
 // pinMode(5,  NC);          // D5 
 // pinMode(6,  NC);          // D6
 // pinMode(7,  NC);          // D7 
 // pinMode(8, INPUT);        // D8 - Sensor: Temp, initializated on DS18B20 Constructor. 
    pinMode(PIN_LED3, OUTPUT);      // D9 - LED 3
    pinMode(PIN_SDCARD_SS, OUTPUT); // D10 - SPI Chipselect: MicroSD
 // pinMode(11, OUTPUT);      // D11 - Arduino UNO: SPI MOSI; Arduino Leonardo: NC
 // pinMode(12, INPUT);       // D12 - Arduino UNO: SPI MISO; Arduino Leonardo: NC
 // pinMode(13, OUTPUT);      // D13 - Arduino UNO: SPI SCK;  Arduino Leonardo: NC


 // Initialization SD
    Serial.print("Initializing SD card...");
    // see if the card is present and can be initialized:
    if (!SD.begin(PIN_SDCARD_SS)) {
      Serial.println("ERROR: Card failed, or not present");
      // don't do anything more:
      showSystemError(0);
    }
    Serial.println("SD card initialized.");

  // Initialization RTC
    Serial.print("Initializing RTC...");
    if (! rtc.begin()) {
      Serial.println("ERROR: Couldn't find RTC");
      showSystemError(1);
    }

    // Set time automatically if needed.
    if (rtc.lostPower()) {
      Serial.println("RTC lost power, lets set the time!");
      // following line sets the RTC to the date & time this sketch was compiled
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      // This line sets the RTC with an explicit date & time, for example to set
      // January 21, 2014 at 3am you would call:
      // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }
    Serial.println("rtc initialized.");

  // Initialization Temperature sensor
    Serial.println("Maxim Integrated DS18B20 Temperature Sensor | cactus.io");
    Serial.print("DS18B20 Serial Number: ");
    printSerialNumber(ds.getSerialNumber()); // we pass the serial number byte array into the printSerialNumber function
    //showSystemError(2);
    
  // Get sensor values  
    now = future = rtc.now(); // RTC: Get time for first time
    ds.readSensor();              // TEMP: Get temperature

    flowPulseCount    = 0;
    flowRate          = 0.0;
    flowMilliLitres   = 0;
    totalMilliLitres  = 0;
    oldTime           = millis();

    // Pin Initialization
    digitalWrite(PIN_LED2, HIGH);  // LED 2: turn the LED on (HIGH is the voltage level)
    digitalWrite(PIN_LED3, HIGH);  // LED 3: turn the LED on (HIGH is the voltage level)
    digitalWrite(PIN_RELAY_LEFT, HIGH); // RELE IZQUIERDO ABIERTO
    digitalWrite(PIN_RELAY_RIGHT, LOW); // RELE DERECHO CERRADO, SI SE ABRE SE SALE EL AGUA SI HAY MUCHA PRESION
    
    event = "System started!";
    saveEventToSD();
}




void loop() 
{
  // Medir caudal del sensor YF-S201
  measureFlow();    // AVAILABLE DATA EACH SECOND
  
  // Medir PH del sensor 
  measurePH();      // AVAILABLE DATA INSTANT
  
  // Leer temperatura del sensor DS18B20
  ds.readSensor();  // AVAILABLE DATA INSTANT
  temp_C = ds.getTemperature_C();

  // Construct Event
  event = "Flow rates: ";
  event += String(int(flowRate));
  event += "L/min, ";
  event += String(flowMilliLitres);
  event += "mL/min, ";
  event += "Flow accumulated: ";
  event += String(totalMilliLitres);
  event += "mL, ";
  event += "PH: ";
  event += String(phValue);
  event += ", Temperatura: ";
  event += String(temp_C);
  event += " ºC";

  // Save the event to microSD if time has come: everyDays, everyHours, everyMinutes, everySeconds
  checkIfTriggerEvent(everyDays, everyMinutes, everyHours, everySeconds);
}
