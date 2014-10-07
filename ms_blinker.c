
#include <avr/io.h>
#include <avr/eeprom.h>

#define F_CPU 8000000UL

#include <util/delay.h>
#include <avr/interrupt.h>

//program with:
//avrdude -c usbtiny -p attiny2313 -U lfuse:w:0xe4:m -U flash:w:ms_blinker.hex

//uint16_t eepromWord __attribute__((section(".eeprom")));

volatile uint16_t counter = 0;
volatile uint16_t temp = 0;


ISR(TIMER1_COMPA_vect){
	counter++;
	TCNT1 = 0x00;
}

int main (void)
{

	//Initialization routine: Clear watchdog timer-- this can prevent several things from going wrong.
	MCUSR &= 0xF7;		//Clear WDRF Flag
	WDTCSR	= 0x18;		//Set stupid bits so we can clear timer...
	WDTCSR	= 0x00;

	//Data direction register: DDR's
	//Port A: 0, 1 are inputs.
	//Port B: 0-3 are outputs, B4 is an input.
	//Port D: 1-6 are outputs, D0 is an input.

	DDRA = 0U;
	DDRB = 15U;
	DDRD = 126U;

	PORTA = 3;	// Pull-up resistors enabled, PA0, PA1
	PORTB = 16;	// Pull-up resistor enabled, PA
	PORTD = 0;


	/* Visualize outputs:

	 L to R:

	 D2 D3 D4 D5 D6 B0 B1 B2 B3

	*/

    TCCR1B = 0x01; //run at system clock
    //trigger the interrupt vector TIMER1_OVF_vect when timer 1 is overflow
    TIMSK = _BV(OCIE1A);
    TCNT1 = 0x00;
	OCR1A = 8000;

    //sets the interruptions to enabled
    sei();

	counter = 1;
	uint8_t use_graycode = 1;
	uint8_t buttonwasdown = 0;

	for (;;)  // main loop
	{
		if(counter >=512)
			counter = 0;

		if(use_graycode == 1)
			temp = (counter >> 1) ^ counter;
		else
			temp = counter;
		PORTB = temp>>5 | 16;
		PORTD = temp<<2;

		if ((PINB & 16) == 0) {
			if(buttonwasdown==0){		// Check for button press
				if(use_graycode == 1)
					use_graycode = 0;
				else
					use_graycode = 1;
			}
			buttonwasdown = 1;
			TCCR1B = 0x03;
		}else {
			buttonwasdown = 0;
			TCCR1B = 0x01;
		}
	}

	return 0;
}
