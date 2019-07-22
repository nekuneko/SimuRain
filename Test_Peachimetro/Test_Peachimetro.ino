/*
# This sample codes is for testing the pH meter V1.0.
 # Editor : Javier Villaverde Ramallo (@NekuNeko)
 # Date   : 2018.12.20
 # Ver    : 1.1
 # Product: pH meter
 # SKU    : SEN0161
*/

// Global constants
#define mySerial        Serial  // Switchs between Serial (USB) / Serial1 (Bluetooth) to print data
#define RELAY1_STATUS   HIGH    // HIGH Abierto, LOW Cerrado
#define RELAY2_STATUS   LOW     // HIGH Abierto, LOW Cerrado
#define DELAY_TIME      1000    // ms

#define PIN_LED_INFO    12      // OUTPUT: INFO LED,   HIGH ON, LOW OFF 
#define PIN_LED_STATUS  11      // OUTPUT: STATUS LED, HIGH ON, LOW OFF 
#define PIN_RELAY_1     10      // OUTPUT: LEFT  RELAY, WATER INPUT 1: HIGH OPEN; LOW CLOSED
#define PIN_RELAY_2     9       // OUTPUT: RIGHT RELAY, WATER INPUT 2: HIGH OPEN; LOW CLOSED
#define PIN_SENSOR_PH   A7      // INPUT: A7 (D6). pH meter v1.1 sensor Analog output to Arduino Analog Input A7 on Digital pin 6
#define PIN_SENSOR_AUX  A8      // INPUT: A8 (D8). Aux sensor analog input for future use.


void setup()
{
  // Initialize a serial connection for reporting values to the host
    mySerial.begin(115200);
    while(!mySerial);
    mySerial.println("Test_Peachimetro.ino");     // Test the serial monitor


  // Pin initialization
    pinMode(PIN_RELAY_1,      OUTPUT);            // Relé entrada de agua 1
    pinMode(PIN_RELAY_2,      OUTPUT);            // Relé entrada de agua 2
    pinMode(PIN_LED_STATUS,   OUTPUT);            // Led Estado Verde
    pinMode(PIN_LED_INFO,     OUTPUT);            // Led Estado Azul
    pinMode(PIN_SENSOR_PH,    INPUT);             // Sensor PH  analog input
    pinMode(PIN_SENSOR_AUX,   INPUT);             // Sensor AUX analog input
    
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


void loop()
{
    int buf[10],temp;
    float phValue = 0;              
    unsigned long int avgValue = 0;  // Store the average value of the sensor feedback

    // Get 10 sample value from the sensor for smooth the value
    for(int i=0;i<10;i++)       
    { 
      buf[i]=analogRead(PIN_SENSOR_PH);
      delay(10);
    }

    // Sort the analog from small to large
    for(int i=0;i<9;i++)        
    {
      for(int j=i+1;j<10;j++)
      {
        if(buf[i]>buf[j])
        {
          temp=buf[i];
          buf[i]=buf[j];
          buf[j]=temp;
        }
      }
    }

    // Take the average value of 6 center sample
    for(int i=2;i<8;i++)                      
      avgValue+=buf[i];
      
    phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
    phValue=3.5*phValue;                //convert the millivolt into pH value
    
    mySerial.print("\tpH:");  
    mySerial.println(phValue,2);
    
    delay(DELAY_TIME); // Add some delay just here to slow down the output.
} // loop()
