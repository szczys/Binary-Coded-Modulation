#define F_CPU 1000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

unsigned char pwmValues[8] = {0x00,0x02,0x04,0x08,0x10,0x20,0x40,0x80}; //Exponential Approximation
volatile unsigned char bcmBuffer[8];
unsigned char BCMtracker = 0;

void calcBCM(void) {
  for (unsigned char i=0; i<8; i++){ //Cycle though each bit of each pwmValues
    unsigned char tempValue = 0;
    for (unsigned char j=0; j<8; j++){
      tempValue >>= 1; //Shift right so as not to overwrite the last bit
      if (pwmValues[j] & (1<<i)) tempValue |= (1<<7); //Always set MSB because we're shifting right
    }
    bcmBuffer[i] = tempValue;  
  }
}

int main(void) 
{
  //Setup IO
  DDRD = 0xFF;
  PORTD = 0xFF;

  //Prewind the Binary Coded Modulation buffer
  calcBCM();
  //PORTD = bcmBuffer[7];
  //while(1){}

  //Setup Timer
  cli();		//disable all interrupts
  TCCR0A |= (1<<WGM01);	//Use CTC mode
  TCCR0B |= (1<<CS02);	//Start timer with 256 prescaler
  TIMSK0 |= (1<<OCIE0A);	//Enable the compare A interrupt
  OCR0A = 0x01;		//Set to compare on first timer tick
  sei();		//enable all interrupts

  while(1)
  {
    //Loop Forever
  }
}

ISR(TIMER0_COMPA_vect)	//Compare match interrupt handler
{

  //Update LEDs
  PORTD = bcmBuffer[BCMtracker];
  
  //Set interrupt for next BCM delay value
  if (BCMtracker == 0) OCR0A = 0x01;
  else OCR0A <<= 1;

  //Increment the BCM tracking index
  BCMtracker ++;
  BCMtracker &= 7;	//Flip back to zero when it gets to 8  
}
