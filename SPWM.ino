#define pinSPWM_A 9
#define pinSPWM_B 10

#define frekuensiOutput 50
#define frekuensiSPWM 1000

volatile byte indexSPWM;
volatile float output;
int limitOutput;

void setup(){
  pinMode (pinSPWM_A, OUTPUT);
  pinMode (pinSPWM_B, OUTPUT);

  TCCR1A = (1 << COM1A1)|(1 << COM1B1)|(1 << WGM11);

  long cycles = (F_CPU / frekuensiSPWM / 2);
  uint8_t clockSelectBits;

  if (cycles < 0xFF) clockSelectBits = _BV(CS10);
  else if ((cycles >>= 3) < 0xFF) clockSelectBits = _BV(CS11);
  else if ((cycles >>= 3) < 0xFF) clockSelectBits = _BV(CS11)| _BV(CS10);
  else if ((cycles >>= 2) < 0xFF) clockSelectBits = _BV(CS12);
  else if ((cycles >>= 2) < 0xFF) clockSelectBits = _BV(CS12)| _BV(CS10);
  else cycles = 0xFF - 1, clockSelectBits = _BV(CS12)| _BV(CS10);

  TCCR1B = (1 << WGM13) | clockSelectBits;
  ICR1 = cycles;
  TIMSK1 = _BV(TOIE1);
  OCR1A = 0;
  OCR1B = 0;

  limitOutput = cycles;
}

void loop(){
  output = limitOutput / 2;
}

ISR(TIMER1_OVF_vect){
  int duty = 1.0 * sin(2.0 * PI * indexSPWM / (frekuensiSPWM / frekuensiOutput)) * output;

  if (duty >= 0)
  {
    OCR1AL = duty;
    OCR1BL = limitOutput + 1;
  }
  else
  {
    OCR1AL = limitOutput + 1;
    OCR1BL = 0 - duty;  
  }

  indexSPWM++;
  indexSPWM %= frekuensiSPWM / frekuensiOutput;
}
