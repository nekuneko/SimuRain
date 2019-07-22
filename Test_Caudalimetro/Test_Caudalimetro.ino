/*
Liquid flow rate sensor -DIYhacking.com Arvind Sanjeev
modified 20.12.2018 by NeKuNeKo

Measure the liquid/water flow rate using this code. 
Connect Vcc and Gnd of sensor to arduino, and the 
signal line to arduino digital pin 7.
*/

// Global constants
// The hall-effect flow sensor outputs approximately 4.5 pulses per second per litre/minute of flow.
const float flowCalibrationFactor = 4.5;

#define mySerial        Serial  // Switchs between Serial (USB) / Serial1 (Bluetooth) to print data
#define RELAY1_STATUS   HIGH    // HIGH Abierto, LOW Cerrado
#define RELAY2_STATUS   LOW     // HIGH Abierto, LOW Cerrado
#define DELAY_TIME      1000    // ms

#define PIN_LED_INFO    12      // OUTPUT: INFO LED,   HIGH ON, LOW OFF 
#define PIN_LED_STATUS  11      // OUTPUT: STATUS LED, HIGH ON, LOW OFF 
#define PIN_RELAY_1     10      // OUTPUT: LEFT  RELAY, WATER INPUT 1: HIGH OPEN; LOW CLOSED
#define PIN_RELAY_2     9       // OUTPUT: RIGHT RELAY, WATER INPUT 2: HIGH OPEN; LOW CLOSED
#define PIN_SENSOR_FLOW 7       // INPUT_PULLUP:  Flow sensor interrupt pin. 


// Global variables
volatile byte flowPulseCount;  

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;


/* Interrupt Service Routine */
void flowPulseCounter()
{
  // Increment the pulse counter
  ++flowPulseCount;
}


void setup()
{
  // Initialize a serial connection for reporting values to the host
    mySerial.begin(115200);
    while(!mySerial);
    mySerial.println("Test_Caudalimtero.ino");

   
  // Pin initialization
    pinMode(PIN_RELAY_1,      OUTPUT);            // Relé entrada de agua 1
    pinMode(PIN_RELAY_2,      OUTPUT);            // Relé entrada de agua 2
    pinMode(PIN_LED_STATUS,   OUTPUT);            // Led Estado Verde
    pinMode(PIN_LED_INFO,     OUTPUT);            // Led Estado Azul
    pinMode(PIN_SENSOR_FLOW,  INPUT_PULLUP);      // Flow sensor interrupt
    
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
} // setup()


/**
 * Main program loop
 */
void loop()
{
  measureFlow();
    
  // Print the flow rate for this second in litres / minute
  mySerial.print("Flow rate: ");
  mySerial.print(int(flowRate));  // Print the integer part of the variable
  mySerial.print("L/min");
  mySerial.print("\t"); 		      // Print tab space

  // Print the cumulative total of litres flowed since starting
  mySerial.print("Output Liquid Quantity: ");        
  mySerial.print(totalMilliLitres);
  mySerial.println("mL"); 
  mySerial.print("\t"); 		      // Print tab space
	mySerial.print(totalMilliLitres/1000);
	mySerial.print("L");
    
  delay(DELAY_TIME); // Add some delay just here to slow down the output.
} // loop()


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
