/* 
  This code uses interrupt timer with 1ms to get delay.
  1ms using timer1 and prescalar of 256.
  Calculations (for 1ms): 
  System clock 16 Mhz and Prescalar 256;
  Timer 1 speed = 16Mhz/256 = 62.5 Khz    
  Pulse time = 1/62.5 Khz =  16us  
  Count up to = 1ms / 16us = 63 (so this is the value the OCR register should have)
 */

#define LM35 A0

bool ledState = true;
volatile unsigned int delayCount; 
unsigned int delayValue = 250;   //Used to set delay in milli seconds.

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LM35, INPUT);
  //Serial.begin(9600);
  
  cli();                      //stop interrupts for till we make the settings
  /*1. First we reset the control register to amke sure we start with everything disabled.*/
  TCCR1A = 0;                 // Reset entire TCCR1A to 0 
  TCCR1B = 0;                 // Reset entire TCCR1B to 0
 
  /*2. We set the prescalar to the desired value by changing the CS10 CS12 and CS12 bits. */  
  TCCR1B |= B00000100;        //Set CS12 to 1 so we get prescalar 256  
  
  /*3. We enable compare match mode on register A*/
  TIMSK1 |= B00000010;        //Set OCIE1A to 1 so we enable compare match A 
  
  /*4. Set the value of register A to 31250*/
  OCR1A = 63;              //Finally we set compare register A to this value  
  sei();                     //Enable back the interrupts
}

void loop()
{
  unsigned int tempSensorValue = analogRead(LM35);    //Reading Value from A0.
  float voltage = tempSensorValue * (5.0f / 1023.0f); //Converting A0 value to corresponding voltage.
  float temperature = voltage * 100;                  //Converting voltage to temperature in celsius.
  //Serial.print("Temperature = ");
  //Serial.println(temperature);
  
  if(temperature < 30.0f)
  {
    delayValue = 250; //Set led blink delay to 250ms
  }
  else
  {
    delayValue = 500;//Set led blink delay to 500ms
  }
}

//With the settings above, this IRS will trigger each 1ms.
ISR(TIMER1_COMPA_vect){
  TCNT1  = 0;                  //First, set the timer back to 0 so it resets for next interrupt
  delayCount++;
  if(delayCount == delayValue)
  {
    ledState = !ledState;
    digitalWrite(13,ledState);
    delayCount = 0;
  }
}
