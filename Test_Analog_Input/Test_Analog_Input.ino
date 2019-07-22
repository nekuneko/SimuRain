// Global constants
#define DELAY_TIME      1000  // ms

#define PIN_SENSOR_TEMP A6    // INPUT: A6 (D4). DS18S20 temperature sensor Analog output to Arduino Analog Input A6 on Digital pin 4
#define PIN_SENSOR_PH   A7    // INPUT: A7 (D6).       pH meter v1.1 sensor Analog output to Arduino Analog Input A7 on Digital pin 6
#define PIN_SENSOR_AUX  A8    // INPUT: A8 (D8). Aux sensor for future use.


void setup() 
{
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Test_Analog_Input.ino");
  
  pinMode(PIN_SENSOR_TEMP,  INPUT);
  pinMode(PIN_SENSOR_PH,    INPUT);
  pinMode(PIN_SENSOR_AUX,   INPUT);
} // setup()


void loop() 
{
  int analog_temp = analogRead(PIN_SENSOR_TEMP);
  int analog_ph   = analogRead(PIN_SENSOR_PH);
  int analog_aux  = analogRead(PIN_SENSOR_AUX);

  Serial.println("ANALOG_TEMP: " + String(analog_temp));
  Serial.println("ANALOG_PH:   " + String(analog_ph));
  Serial.println("ANALOG_AUX:  " + String(analog_aux));
  Serial.println();
  
  delay(DELAY_TIME); // Add some delay just here to slow down the output.
} // loop()
