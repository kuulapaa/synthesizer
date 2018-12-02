
uint16_t freqs[] = {
  2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951};
 // Real piano 7th octave frequencies (about), bit shift right to get 6 and so on...
 
void setup() {
  // put your setup code here, to run once:
  DDRD = 0xFF; // output in PORT D (8-bits)
  timer0_init();
}

void timer0_init()
{
  TCCR0A = 0x02;  //CTC
  TCCR0B = 0x02; //clkIO/8
  OCR0A = 37; //-> interrupts handled at 66.6kHz (TOO HIGH, BUT IS GIVING THE RIGHT FREQUENCY OUTPUT NOW WITH 16-BIT PHASE ACCUMULATORS...) Can handle at least 4 generators
  TIMSK0 = 0x02; // enable interrupt on compare match A
}


uint16_t triangle;  // Generators 1 to 8?
uint16_t triangle2;
uint16_t sawtooth;
uint16_t sawtooth2;

uint16_t triangle_out;
uint16_t triangle2_out;

void loop(void)
{
  // Here we would do MIDI input and some channel arrangements with circular buffer
}

ISR(TIMER0_COMPA_vect)
{
    //TRIANGLE GEN
    uint16_t output = 0;
    triangle += (freqs[0]>>0);  // Phase accumulator. Add frequency value in every interrupt. Bit shift right gives lower octave
    if (triangle > 32768) triangle_out = ((0xFFFF ^ triangle)<<1); // XOR to ramp down
    else triangle_out = (triangle<<1);
    output = (triangle_out>>2);  //
    
    //TRIANGLE GEN2
    triangle2 += (freqs[1]>>3);
    if (triangle2 > 32768) triangle2_out = ((0xFFFF ^ triangle2)<<1);
    else triangle2_out = (triangle2<<1);
    output += (triangle2_out>>2);
    
    //SAW GEN
    sawtooth += (freqs[2]>>2);
    output += (sawtooth>>2);

    //SAW GEN2
    sawtooth2 += freqs[3]>>2;
    output += (sawtooth2 >> 2);


    PORTD = (output>>8);  // Just output the damn thing and bit shift so it can fit 8 bits
}
