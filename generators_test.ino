
uint16_t freqs[] = {
 4186,4435,4699,4978,5274,5588,5920,6272,6645,7040,7459,7902 }; // twice the frequencies of 7th octave
 
 
 // Real piano 7th octave frequencies (about), bit shift right to get 7 and so on...
 // 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951};

#define OSCILLATORS 2
#define ATTEN 2
#define MOD_O 1   // oscillator generating the ring modulation
#define RINGM 1

const uint8_t SAW = 1;
const uint8_t TRI = 2;
const uint8_t PUL = 3;

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
  gens[0].freq = freqs[0]>>7;
  gens[0].wf = TRI;

  gens[1].freq = freqs[3]>>1;
  gens[1].wf = SAW;

  gens[2].freq = freqs[1]>>4;
  gens[2].wf = TRI;

  gens[3].freq = freqs[5]>>5;
  gens[3].wf = TRI;

  gens[4].freq = freqs[2]>>1;
  gens[4].wf = PUL;

   gens[5].freq = freqs[2]>>1;
  gens[5].wf = SAW;
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
  uint16_t output = 0;
  for (uint8_t i=0; i<OSCILLATORS; i++) {
     
    if ((gens[i].wf) == TRI) {                                  // Triangle waveform generator, XOR to get ramp down
      gens[i].phase += gens[i].freq;
      if (RINGM) {
        uint16_t bit_mask = (1<<15);                            // use MSB for ring modulation as in real SID
        uint16_t msb_m = gens[MOD_O].phase & bit_mask;
        gens[i].phase = (gens[i].phase & (~bit_mask)) | msb_m;  // substitute MSB of oscillator accumulator with MSB of carrier signal MSB
      }
      if (gens[i].phase & _BV(15)) {
        output += (((0xFFFF ^ gens[i].phase)<<1)>>ATTEN);
      }
      else {
        output += ((gens[i].phase<<1)>>ATTEN);
      }
    }
  
    if ((gens[i].wf) == SAW) {              // Sawtooth waveform generator
      gens[i].phase += gens[i].freq;
      //output += ((gens[i].phase)>>ATTEN);
    }
    
    if ((gens[i].wf) == PUL){
      gens[i].phase += gens[i].freq;        // Pulse waveform generator
      if (!(gens[i].phase & _BV(15))) {
        output += (0xFFFF>>ATTEN);
      }
    }
  }
  if (output >= 0xFFF0) while(1) {}         // Halt if output overdriven
  PORTD = (output>>8);                      // Just output the damn thing and bit shift so it can fit 8 bits
}
