//Be name Allah

#ifndef TimerTwo_IS_IN
#define TimerTwo_IS_IN



#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// Arduino Mega
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define TimerTwo_PIN  10

// Teensy++
#elif defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)
#define TimerTwo_PIN  24

// Teensy 3.0 & 3.1
#elif defined(__MK20DX128__) || defined(__MK20DX256__)
#define TimerTwo_PIN  5

// Sanguino
#elif defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644__)
#define TimerTwo_PIN  15

// Arduino Uno, Duemilanove, Diecimila, etc
#else
#define TimerTwo_PIN  11
#endif



class TimerTwo
{
  private:
    static uint8_t pwmEnabled;
  public:
    static void (*onOverflow)(); // not really public, but I can't work out the 'friend' for the SIGNAL

  public:
    static void initialize(unsigned long microseconds = 100000);
    static void start();
    static void stop();
    static void setPeriod(unsigned long);
    static void attachInterrupt( void (*)() );
	static void detachInterrupt() {
      attachInterrupt(0);
    }
    static void enablePwm();
    static void disablePwm();
};

extern TimerTwo Timer2;

#endif
