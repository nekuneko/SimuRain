#include <Wire.h>
#include <SPI.h>
#include <SD.h>       // <---- Standard Library Embedded in Arduino. File > Examples > SD (Archivo > Ejemplos > SD).
#include "RTClib.hpp" // Library by @gutierrezps https://github.com/gutierrezps/RTClib mod of Adafruit's https://learn.adafruit.com/adafruit-ds3231-precision-rtc-breakout/overview
                      // This RTC library allow compare dates.

// Global constants
#define PIN_SDCARD_SS   5   // OUTPUT: microSD card's chip select

const String daysOfTheWeek[7] = {"Domingo", "Lunes", "Martes", "Miércoles", "Jueves", "Viernes", "Sábado"};


// Global variables
RTC_DS3231 rtc;
DateTime now;
DateTime future;


void setup() 
{
    Serial.begin(115200);
    //while(!Serial);

    // Initialization SD
    Serial.print("Initializing SD card...");

    // see if the card is present and can be initialized:
    if (!SD.begin(PIN_SDCARD_SS)) {
      Serial.println("Card failed, or not present");
      while (1);  // don't do anything more
    }
    
    Serial.println("card initialized.");

    // Initialization RTC
    if (! rtc.begin()) {
      Serial.println("Couldn't find RTC");
      while (1); // don't do anything more
    }

    if (rtc.lostPower()) {
      Serial.println("RTC lost power, lets set the time!");
      // following line sets the RTC to the date & time this sketch was compiled
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      // This line sets the RTC with an explicit date & time, for example to set
      // January 21, 2014 at 3am you would call:
      // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }
    
    now = future = rtc.now();
    Serial.println("rtc initialized.");
    
} // setup()


void loop() 
{
  triggerEvent(0, 0, 0, 20, "Joaquin");  
} // loop()


void triggerEvent (int16_t days, int8_t hours, int8_t minutes, int8_t seconds, String Event)
{
  now = rtc.now(); // Get the Time

  if (now >= future)
  {
    saveEventToSD(Event);
    future = now + TimeSpan(days, hours, minutes, seconds);
  }
} // triggerEvent()


void saveEventToSD (String Event)
{
  // Get the Time
  now = rtc.now(); 

  // Construct Date
  String date = String(now.year()) + '/' + String(now.month()) + '/' + String(now.day()) + 
                " (" + daysOfTheWeek[now.dayOfTheWeek()] + ") " + 
                String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());

  // Construct Event
  String dateEvent = date + ' ' + Event;
  
  // Save To SD
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) 
  {
    dataFile.println(dateEvent);
    dataFile.close();
    
    // print to the serial port too:
    Serial.println(dateEvent);
  }
} // saveEventToSD()
