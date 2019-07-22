// Test to check if leds on Almoraima v2.x board works

// Global constans
//      LED_POWER ALWAYS ON
#define LED_ENTRADA_AGUA_1    10    
#define LED_ENTRADA_AGUA_2    9
#define LED_ESTADO            11
#define LED_INFO              12

#define DELAY_TIME      1000 // ms
#define DELAY_TIME_FADE   10 // ms


void setup() 
{
  pinMode(LED_ENTRADA_AGUA_1, OUTPUT);
  pinMode(LED_ENTRADA_AGUA_2, OUTPUT);
  pinMode(LED_ESTADO,         OUTPUT);
  pinMode(LED_INFO,           OUTPUT);
} // setup()


void loop() 
{
  //blink();
  //fade();
  on();
}

void on()
{
  digitalWrite(LED_ENTRADA_AGUA_1, HIGH);
  digitalWrite(LED_ENTRADA_AGUA_2, HIGH);
  digitalWrite(LED_ESTADO,         HIGH);
  digitalWrite(LED_INFO,           HIGH);
}

void blink ()
{
  digitalWrite(LED_ENTRADA_AGUA_1, HIGH);
  digitalWrite(LED_ENTRADA_AGUA_2, HIGH);
  digitalWrite(LED_ESTADO,         HIGH);
  digitalWrite(LED_INFO,           HIGH);
  delay(DELAY_TIME);

  digitalWrite(LED_ENTRADA_AGUA_1, LOW);
  digitalWrite(LED_ENTRADA_AGUA_2, LOW);
  digitalWrite(LED_ESTADO,         LOW);
  digitalWrite(LED_INFO,           LOW);
  delay(DELAY_TIME);
} // blink()


void fade ()
{
  for (int i=0; i<=255; ++i)
  {
    analogWrite(LED_ENTRADA_AGUA_1, i);
    analogWrite(LED_ENTRADA_AGUA_2, i);
    analogWrite(LED_ESTADO,         i);
    analogWrite(LED_INFO,           i);

    delay(DELAY_TIME_FADE);
  }

  for (int i=255; i>=0; --i)
  {
    analogWrite(LED_ENTRADA_AGUA_1, i);
    analogWrite(LED_ENTRADA_AGUA_2, i);
    analogWrite(LED_ESTADO,         i);
    analogWrite(LED_INFO,           i);

    delay(DELAY_TIME_FADE);
  }
} // fade()
