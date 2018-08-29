//Be name Allah

#include <TimerTwo.h>

#include <avr/interrupt.h>

#define TIMER2_RESOLUTION 256UL  // Timer2 is 8 bit

TimerTwo Timer2;

static unsigned short pwmPeriod;
static unsigned char clockSelectBits;

void (*TimerTwo::onOverflow)() = 0;
uint8_t TimerTwo::pwmEnabled = 0;


#if defined(TIMER2_COMPA_vect)
ISR(TIMER2_COMPA_vect)
#elif defined(TIMER2_COMP_vect)
ISR(TIMER2_COMP_vect)
#else
#error "This board does not have a hardware timer which is compatible with TimerTwo"
void dummy_function(void)
#endif
{
  static bool once;
  once = !once;
  if (once == true) { // we work with half-cycles before the toggle

    static uint8_t inHandler = 0; // protect us from recursion if our handler enables interrupts

    if ( !inHandler && TimerTwo::onOverflow) {
      inHandler = 1;
      (*TimerTwo::onOverflow)();
      inHandler = 0;
    }
  
  }
}

void TimerTwo::attachInterrupt( void (*func)() )
{
  TimerTwo::onOverflow = func;
#if defined(TIMSK2)
  if ( func) TIMSK2 |= _BV(OCIE2A);
  else TIMSK2 &= ~_BV(OCIE2A);
#elif defined(TIMSK)
  if ( func) TIMSK |= _BV(OCIE2);
  else TIMSK &= ~_BV(OCIE2);
#endif
}

void TimerTwo::initialize(unsigned long microseconds) {
  TCCR2A = _BV(WGM21);
  TCCR2B = 0;
  setPeriod(microseconds);
}

void TimerTwo::start() {
  TCCR2B = clockSelectBits;
}

void TimerTwo::stop() {
  TCCR2B = 0;
}

void TimerTwo::setPeriod(unsigned long microseconds)
{
  const unsigned long cycles = (F_CPU / 1000000L) * microseconds / 2; // we work with half-cycles before the toggle
  if ( cycles <= TIMER2_RESOLUTION) {
    clockSelectBits = _BV(CS20);
    pwmPeriod = cycles - 1;
  } else if ( cycles <= TIMER2_RESOLUTION * 8) {
    clockSelectBits = _BV(CS21);
    pwmPeriod = cycles / 8 - 1;
  } else if ( cycles <= TIMER2_RESOLUTION * 32) {
    clockSelectBits = _BV(CS21) | _BV(CS20);
    pwmPeriod = cycles / 32 - 1;
  } else if ( cycles <= TIMER2_RESOLUTION * 64) {
    clockSelectBits = _BV(CS22);
    pwmPeriod = cycles / 64 - 1;
  } else if ( cycles <= TIMER2_RESOLUTION * 128) {
    clockSelectBits = _BV(CS22) | _BV(CS20);
    pwmPeriod = cycles / 128 - 1;
  } else if ( cycles <= TIMER2_RESOLUTION * 256) {
    clockSelectBits = _BV(CS22) | _BV(CS21);
    pwmPeriod = cycles / 256 - 1;
  } else if ( cycles <= TIMER2_RESOLUTION * 1024) {
    clockSelectBits = _BV(CS22) | _BV(CS21) | _BV(CS20);
    pwmPeriod = cycles / 1024 - 1;
  } else {
    clockSelectBits = _BV(CS22) | _BV(CS21) | _BV(CS20);
    pwmPeriod = TIMER2_RESOLUTION - 1;
  }

#if defined(TCCR2A)

#if defined(ASSR) && defined(AS2)
  ASSR &= ~_BV(AS2);    // use clock, not T2 pin
#endif
  OCR2A = pwmPeriod;
  TCCR2A = (_BV(WGM21) | ( TimerTwo::pwmEnabled ? _BV(COM2A0) : 0));
  TCCR2B = clockSelectBits;
#elif defined(TCCR2)
  TCCR2 = 0;
  TCNT2 = 0;
  ASSR &= ~_BV(AS2);    // use clock, not T2 pin
  OCR2 = pwmPeriod;
  TCCR2 = (_BV(WGM21) | ( TimerTwo::pwmEnabled ? _BV(COM20) : 0)  | clockSelectBits);
#endif

}

void TimerTwo::enablePwm()
{
  TimerTwo::pwmEnabled = 1;
#if defined(TCCR2A)
  TCCR2A |= _BV(COM2A0);
#elif defined(TCCR2)
  TCCR2 |= _BV(COM20);
#endif
}

void TimerTwo::disablePwm()
{
  TimerTwo::pwmEnabled = 0;
#if defined(TCCR2A)
  TCCR2A &= ~_BV(COM2A0);
#elif defined(TCCR2)
  TCCR2 &= ~_BV(COM20);
#endif
}
