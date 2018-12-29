
uint16_t freqs[] = {
 4186,4435,4699,4978,5274,5588,5920,6272,6645,7040,7459,7902 }; // twice the frequencies of 7th octave
 
 
 // Real piano 7th octave frequencies (about), bit shift right to get 7 and so on...
 // 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951};

#define OSCILLATORS 1
#define SAW 1
#define TRI 2

volatile struct voices {
  uint16_t freq;
  uint16_t phase;
  uint8_t wf;
  uint8_t volume;
} gens[OSCILLATORS];


/*
enum {
  TRIANGLE,
  SAW,
};
*/

void setup() {
  // put your setup code here, to run once:
  DDRD = 0xFF; // output in PORT D (8-bits)
  timer0_init();
  setup_voices();
}

void setup_voices() { // FOR TESTING
  gens[0].freq = freqs[0]>>2;
  gens[0].wf = TRI;

  gens[1].freq = freqs[2]>>3;
  gens[1].wf = 4;
}

void timer0_init()
{
  TCCR0A = 0x02;  //CTC
  TCCR0B = 0x02; //clkIO/8
  OCR0A = 75; //-> interrupts handled at 33.3kHz (IS GIVING THE RIGHT FREQUENCY OUTPUT NOW WITH 16-BIT PHASE ACCUMULATORS...) Can handle at least 8+ generators
  TIMSK0 = 0x02; // start timer from zero when interrupt occurs
}

void loop(void)
{
  // Here we would do MIDI input and some channel arrangements with circular buffer
}

ISR(TIMER0_COMPA_vect)
{
  uint16_t output;
  for (uint8_t i=0; i<OSCILLATORS; i++) { 
    if ((gens[i].wf) == TRI) {
      gens[i].phase += gens[i].freq;  // Add frequency value in every interrupt. Bit shift right gives lower octave
      if (gens[i].phase & _BV(15)) {
      //if (gens[i].phase > 32768) {
        output += (((0xFFFF ^ gens[i].phase)<<1)>>3); // XOR to ramp down
      }
      else {
        output += ((gens[i].phase<<1)>>3);
      }
    }
    
    if ((gens[i].wf) == SAW) {
      gens[i].phase += gens[i].freq;
      output += ((gens[i].phase)>>3);
    }
    else {}
  }
  PORTD = (output>>8);  // Just output the damn thing and bit shift so it can fit 8 bits
}
