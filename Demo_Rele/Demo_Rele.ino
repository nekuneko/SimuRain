/*
  Programa control 2 reles by ULN2803A Darlington Transistor Array
*/

// Global constants
#define PIN_RELAY_1   10
#define PIN_RELAY_2   9

#define TIME_DELAY 10000//ms
void setup() 
{
  Serial.begin(115200);
  while(!Serial);

  pinMode(PIN_RELAY_1, OUTPUT); // ULN2803A INPUT 8B -> 8C OUTPUT
  pinMode(PIN_RELAY_2, OUTPUT); // ULN2803A INPUT 7B -> 7C OUTPUT
  

  // LOW is OFF, Relé cerrado, no pasa agua
  digitalWrite(PIN_RELAY_1, LOW);
  digitalWrite(PIN_RELAY_2, LOW);
  Serial.println("Relays are LOW");
/*
  // HIGH is ON, Relé abierto, pasa agua
  digitalWrite(PIN_RELAY_1, HIGH);
  digitalWrite(PIN_RELAY_2, HIGH);
  Serial.println("Relays are HIGH");
  */
} // setup()


void loop() 
{
  digitalWrite(PIN_RELAY_1, HIGH);
  delay(TIME_DELAY);
  
  digitalWrite(PIN_RELAY_1, LOW);
  while(true);
  
} // loop()
