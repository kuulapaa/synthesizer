
uint16_t freqs[] = {
 4186,4435,4699,4978,5274,5588,5920,6272,6645,7040,7459,7902 }; // twice the frequencies of 7th octave
 
 
 // Real piano 7th octave frequencies (about), bit shift right to get 7 and so on...
 // 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951};

void setup() {
  // put your setup code here, to run once:
  DDRD = 0xFF; // output in PORT D (8-bits)
  timer0_init();
}

void timer0_init()
{
  TCCR0A = 0x02;  //CTC
  TCCR0B = 0x02; //clkIO/8
  OCR0A = 75; //-> interrupts handled at 33.3kHz (TOO HIGH, BUT IS GIVING THE RIGHT FREQUENCY OUTPUT NOW WITH 16-BIT PHASE ACCUMULATORS...) Can handle at least 6+ generators
  TIMSK0 = 0x02; // enable timer interrupt comp A
}


uint16_t triangle;  // Generators 1 to 8?
uint16_t triangle2;
uint16_t triangle3;
uint16_t sawtooth;
uint16_t sawtooth2;
uint16_t sawtooth3;

uint16_t triangle_out;
uint16_t triangle2_out;
uint16_t triangle3_out;

void loop(void)
{
  // Here we would do MIDI input and some channel arrangements with circular buffer
}

ISR(TIMER0_COMPA_vect)
{
    //TRIANGLE GEN
    uint16_t output = 0;
    
    triangle += (freqs[0]>>4);  // Add frequency value in every interrupt. Bit shift right gives lower octave
    if (triangle > 32768) triangle_out = ((0xFFFF ^ triangle)<<1); // XOR to ramp down
    else triangle_out = (triangle<<1);
    output = (triangle_out>>0);  //
    
    //TRIANGLE GEN2
    triangle2 += (freqs[1]>>3);
    if (triangle2 > 32768) triangle2_out = ((0xFFFF ^ triangle2)<<1);
    else triangle2_out = (triangle2<<1);
 //   output += (triangle2_out>>3);

    //TRIANGLE GEN3
    triangle3 += (freqs[4]>>5);
    if (triangle3 > 32768) triangle3_out = ((0xFFFF ^ triangle3)<<1);
    else triangle3_out = (triangle3<<1);
 //   output += (triangle3_out>>3);
    
    //SAW GEN
    sawtooth += (freqs[2]>>2);
 //   output += (sawtooth>>3);

    //SAW GEN2
    sawtooth2 += freqs[3]>>2;
 //   output += (sawtooth2 >> 3);

    //SAW GEN3
    sawtooth3 += freqs[4]>>5;
 //   output += (sawtooth3 >> 3);


    PORTD = (output>>8);  // Just output the damn thing and bit shift so it can fit 8 bits
}
