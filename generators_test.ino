
uint16_t freqs[] = {
 4186,4435,4699,4978,5274,5588,5920,6272,6645,7040,7459,7902 }; // twice the frequencies of 7th octave
 
 // Real piano 7th octave frequencies (about), bit shift right to get 7 and so on...
 // 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951};

#define OSCILLATORS 8

volatile struct voices {
  uint16_t freq;
  uint16_t phase;
  uint8_t wf;
  uint8_t volume;
} gens[OSCILLATORS];

enum {
  TRIANGLE,
  SAW,
};

uint8_t i;

void setup() {
  // put your setup code here, to run once:
  DDRD = 0xFF; // output in PORT D (8-bits)
  timer0_init();
  setup_voices();
}

void setup_voices() { // FOR TESTING
  gens[0].freq = freqs[0]>>4;
  gens[0].wf = SAW;
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
  for (i=0; i<OSCILLATORS; i++) { 
    switch(gens[i].wf) {
      case TRIANGLE:
      gens[i].phase += gens[i].freq;  // Add frequency value in every interrupt. Bit shift right gives lower octave
      if (gens[i].phase > 32768) output += (((0xFFFF ^ gens[i].phase)<<1)>>3); // XOR to ramp down
      else output += ((gens[i].phase<<1)>>3);
      break;

      case SAW:
      gens[i].phase += gens[i].freq;
      output += ((gens[i].phase)>>3);
      break;

      default:
      break;
    }
  }
  PORTD = (output>>8);  // Just output the damn thing and bit shift so it can fit 8 bits
}

/*
ISR(TIMER0_COMPA_vect)
{
    //TRIANGLE GEN
    uint16_t output = 0;
    
    triangle += (freqs[0]>>4);  // Add frequency value in every interrupt. Bit shift right gives lower octave
    if (triangle > 32768) triangle_out = ((0xFFFF ^ triangle)<<1); // XOR to ramp down
    else triangle_out = (triangle<<1);
    output = (triangle_out>>3);  //
    
    //TRIANGLE GEN2
    triangle2 += (freqs[1]>>3);
    if (triangle2 > 32768) triangle2_out = ((0xFFFF ^ triangle2)<<1);
    else triangle2_out = (triangle2<<1);
    output += (triangle2_out>>3);

    //TRIANGLE GEN3
    triangle3 += (freqs[4]>>5);
    if (triangle3 > 32768) triangle3_out = ((0xFFFF ^ triangle3)<<1);
    else triangle3_out = (triangle3<<1);
    output += (triangle3_out>>3);

    //TRIANGLE GEN4
    triangle4 += (freqs[5]>>4);
    if (triangle4 > 32768) triangle4_out = ((0xFFFF ^ triangle4)<<1);
    else triangle4_out = (triangle4<<1);
    output += (triangle4_out>>3);
    
    //SAW GEN
    sawtooth += (freqs[2]>>2);
    output += (sawtooth>>3);

    //SAW GEN2
    sawtooth2 += freqs[3]>>2;
    output += (sawtooth2 >> 3);

    //SAW GEN3
    sawtooth3 += freqs[4]>>5;
    output += (sawtooth3 >> 3);

     //SAW GEN4
    sawtooth4 += freqs[6]>>7;
    output += (sawtooth4 >> 3);



    PORTD = (output>>8);  // Just output the damn thing and bit shift so it can fit 8 bits
}
*/
