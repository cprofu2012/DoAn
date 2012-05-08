/******************************************************************************
    DTMF_remote.c     DTMF decoder using RomanBlack's triplet algorithm
    Copyright:        Open-Source, Mar 2011, www.RomanBlack.com/DTMF/DTMF_alg.htm
    MCU:              P16F675
    Dev.Board:        EasyPIC6
    Oscillator:       HS 8MHz xtal
    Compiler:         MikroC v7.0 (or MikroC PRO 4.0)

  This is a very simple but functional DTMF decoder using RomanBlack's
  DTMF triplet decoder algorithm. For simplicity it only decodes 2 DTMF
  tones, the telephone "1" key and the "0" key. If you want to decode all the
  keys the code is included.
  DTMF tones are detected on the comparator input, as a 1bit DAC.
  All the PIC does is continually check for DTMF tones using software
  decoding of the signal on the comparator input. 

  Audio input hardware; Audio through a 0.1uF or 1uF capacitor to GP1,
  and a 4k7 or 10k resistor from GP1 to ground. This tested perfectly
  in hardware with DTMF waveforms anywhere from 0.4v to 2.6v peak to peak,
  somewhere about 1v peak to peak is typical.

  Detects DTMF 697 1209 (Key "1") = output ON
  Detects DTMF 941 1336 (Key "0") = output OFF
  
  PIC 12F675 pins;
   GP0  output, main output (drives a relay etc)
   GP1  input, comparator, DTMF audio in
   GP2  output, (drives a LED or speaker for confirmation beep/flash)
   GP3  input, not used
   GP4  osc 8MHz xtal
   GP5  osc 8MHz xtal
******************************************************************************/
// global vars
unsigned char i;

unsigned char cap;        // used to capture TMR0 for period testing
unsigned char cap_last;

unsigned char per;        // holds the last 3 periods values
unsigned char per1;
unsigned char per2;

unsigned char Dtone[12];  // stores points scores for the 12 DTMF tones 
unsigned char Dhigh;      // will hold the highest score
unsigned char Dbest;      // which DTMF tone was scored best

unsigned char debounce;
unsigned char output_last;


//=============================================================================
//  MAKE BEEPS
//=============================================================================
void make_beeps(unsigned char beeps)
{
  //-------------------------------------------------------
  // this makes a number of beeps (like 1 or 2 beeps) and then repeats
  // it three times. This is output on GP2, and can be a LED or
  // a speaker etc to show a confimration that the DTMF decoder has
  // changed the output pin. ie for DTMF remote control use.
  //-------------------------------------------------------
  unsigned char beep_count;
  unsigned char bloop;
  
  // repeat the beeping three times with a few seconds between
  for(i=0; i<3; i++)
  {
    beep_count = beeps;
    while(beep_count)   // loop and make X beeps
    {
      // the beep is 1000 Hz and 250mS long, made by toggling a pin  
      for(bloop=0; bloop<250; bloop++)
      {
        GPIO.F2 = 1;    // 1mS per cycle
        Delay_uS(500);
        GPIO.F2 = 0;
        Delay_uS(500);
      }  
      Delay_mS(300);  // small delay between beeps
      beep_count--;
    }
    Delay_mS(2500);   // long delay before repeating beeps
  }
}
//-----------------------------------------------------------------------------


//=============================================================================
//  COMP DEBOUNCE
//=============================================================================
void comp_debounce(void)
{
  //-------------------------------------------------------
  // this is a critical part of the DTMF decoding, it makes
  // sure the DTMF signal received by the comparator input LO for a
  // period of >220uS and then is HI for >220uS. This performs a
  // synchronisation effect where the periods are synchronised to
  // the most important and reliable features of the DTMF waveform.
  // To keep the 220uS period accurate it tests TMR0 bit0 which
  // toggles every 8 uS. So 220uS / 8uS = 27 cycles
  //-------------------------------------------------------
  // loop here until comparator input has been LO for 220uS
  debounce = 0;
  while(1)
  {
    while(!TMR0.F0);    // wait for TMR0 bit 0 to go HI
    if(CMCON.F6)  debounce = 0;  // test comparator
    else          debounce++;
    if(debounce >= 27) break;
    while(TMR0.F0);     // wait for TMR0 bit 0 to go LO
    if(CMCON.F6)  debounce = 0;  // test comparator
    else          debounce++;
    if(debounce >= 27) break;
  }
  // loop here until comparator input has been LO for 220uS
  debounce = 0;
  while(1)
  {
    while(!TMR0.F0);    // wait for TMR0 bit 0 to go HI
    if(!CMCON.F6)  debounce = 0;  // test comparator
    else          debounce++;
    if(debounce >= 27) break;
    while(TMR0.F0);     // wait for TMR0 bit 0 to go LO
    if(!CMCON.F6)  debounce = 0;  // test comparator
    else          debounce++;
    if(debounce >= 27) break;
  }
}
//-----------------------------------------------------------------------------


//=============================================================================
//  DECODE TRIPLETS
//=============================================================================
void decode_triplets(void)
{
  //-------------------------------------------------------
  // this is called once for each captured period, it checks the
  // latest period to see if matches one of the 24 periods in 
  // the DTMF signatures.
  // if so, it checks that period and the 2 periods before it to
  // see if they match a "triplet" and then allocate points.
  // This project only needs to test 2 DTMF tones, so tones
  // not used are commented out to save processing time and ROM.
  // These triplet tests can be optimised for speed or ROM size, this
  // current version was just thrown together for testing but is
  // fast enough.
  // For theory see; www.RomanBlack.com/DTMF/DTMF_alg.htm
  //-------------------------------------------------------
  
  // Tone 0 66 98  LLS LSL SLL (DTMF key "1")
  if(per >= 66-2 && per <= 66+2)  // ends S 
  {
    if(per2 >= 98-2 && per2 <= 98+2 && per1 >= 98-2 && per1 <= 98+2) Dtone[0]+=5; // LLS
  }
  if(per >= 98-2 && per <= 98+2)  // ends L 
  {
    if(per2 >= 66-2 && per2 <= 66+2 && per1 >= 98-2 && per1 <= 98+2) Dtone[0]+=5; // SLL
    if(per2 >= 98-2 && per2 <= 98+2 && per1 >= 66-2 && per1 <= 66+2) Dtone[0]+=5; // LSL
  }
  
  /*
  //-------------------------------------------------------
  // Tone 1 63 94  SLS LSL 
  if(per >= 63-1 && per <= 63+1)  // ends S 
  {
    if(per2 >= 63-1 && per2 <= 63+1 && per1 >= 94-1 && per1 <= 94+1) Dtone[1]+=7; // SLS
  }
  if(per >= 94-1 && per <= 94+1)  // ends L 
  {
    if(per2 >= 94-1 && per2 <= 94+1 && per1 >= 63-1 && per1 <= 63+1) Dtone[1]+=3; // LSL
  }
  
  //-------------------------------------------------------
  // Tone 2 61 90  SSL SLS LSS 
  if(per >= 61-1 && per <= 61+1)  // ends S 
  {
    if(per2 >= 61-1 && per2 <= 61+1 && per1 >= 90-1 && per1 <= 90+1) Dtone[2]+=1; // SLS
    if(per2 >= 90-1 && per2 <= 90+1 && per1 >= 61-1 && per1 <= 61+1) Dtone[2]+=6; // LSS
  }
  if(per >= 90-1 && per <= 90+1)  // ends L 
  {
    if(per2 >= 61-1 && per2 <= 61+1 && per1 >= 61-1 && per1 <= 61+1) Dtone[2]+=6; // SSL
  }
  
  //-------------------------------------------------------
  // Tone 3 58 86  SSL LSS 
  if(per >= 58-1 && per <= 58+1)  // ends S 
  {
    if(per2 >= 86-1 && per2 <= 86+1 && per1 >= 58-1 && per1 <= 58+1) Dtone[3]+=5; // LSS
  }
  if(per >= 86-1 && per <= 86+1)  // ends L 
  {
    if(per2 >= 58-1 && per2 <= 58+1 && per1 >= 58-1 && per1 <= 58+1) Dtone[3]+=5; // SSL
  }
  
  //-------------------------------------------------------
  // Tone 4 62 92  SSS SLL LLS 
  if(per >= 62-1 && per <= 62+1)  // ends S 
  {
    if(per2 >= 62-1 && per2 <= 62+1 && per1 >= 62-1 && per1 <= 62+1) Dtone[0]+=1; // SSS
    if(per2 >= 92-1 && per2 <= 92+1 && per1 >= 92-1 && per1 <= 92+1) Dtone[4]+=14; // LLS
  }
  if(per >= 92-1 && per <= 92+1)  // ends L 
  {
    if(per2 >= 62-1 && per2 <= 62+1 && per1 >= 92-1 && per1 <= 92+1) Dtone[4]+=14; // SLL
  }
  
  //-------------------------------------------------------
  // Tone 5 60 89  SLL LLS 
  if(per >= 60-1 && per <= 60+1)  // ends S 
  {
    if(per2 >= 89-1 && per2 <= 89+1 && per1 >= 89-1 && per1 <= 89+1) Dtone[5]+=5; // LLS
  }
  if(per >= 89-1 && per <= 89+1)  // ends L 
  {
    if(per2 >= 60-1 && per2 <= 60+1 && per1 >= 89-1 && per1 <= 89+1) Dtone[5]+=5; // SLL
  }
  
  //-------------------------------------------------------
  // Tone 6 58 85  SLL LLS LSL
  if(per >= 58-1 && per <= 58+1)  // ends S 
  {
    if(per2 >= 85-1 && per2 <= 85+1 && per1 >= 85-1 && per1 <= 85+1) Dtone[6]+=5; // LLS
  }
  if(per >= 85-1 && per <= 85+1)  // ends L 
  {
    if(per2 >= 58-1 && per2 <= 58+1 && per1 >= 85-1 && per1 <= 85+1) Dtone[6]+=5; // SLL
    if(per2 >= 85-1 && per2 <= 85+1 && per1 >= 58-1 && per1 <= 58+1) Dtone[6]+=3; // LSL
  }
  */
  
  //-------------------------------------------------------
  // Tone 7 55 82  SLS SSL LSS  (DTMF key "0")
  if(per >= 55-2 && per <= 55+2)  // ends S 
  {
    if(per2 >= 55-2 && per2 <= 55+2 && per1 >= 82-2 && per1 <= 82+2) Dtone[7]+=3; // SLS
    if(per2 >= 82-2 && per2 <= 82+2 && per1 >= 55-2 && per1 <= 55+2) Dtone[7]+=4; // LSS
  }
  if(per >= 82-2 && per <= 82+2)  // ends L 
  {
    if(per2 >= 55-2 && per2 <= 55+2 && per1 >= 55-2 && per1 <= 55+2) Dtone[7]+=4; // SSL
  }
  
  /*
  //-------------------------------------------------------
  // Tone 8 86 114  SLS SSL LSS 
  if(per >= 86-1 && per <= 86+1)  // ends S 
  {
    if(per2 >= 86-1 && per2 <= 86+1 && per1 >= 114-1 && per1 <= 114+1) Dtone[8]+=7; // SLS
    if(per2 >= 114-1 && per2 <= 114+1 && per1 >= 86-1 && per1 <= 86+1) Dtone[8]+=8; // LSS
  }
  if(per >= 114-1 && per <= 114+1)  // ends L 
  {
    if(per2 >= 86-1 && per2 <= 86+1 && per1 >= 86-1 && per1 <= 86+1) Dtone[8]+=8; // SSL
  }

  //-------------------------------------------------------
  // Tone 9 57 83  SLL LLS LSL
  if(per >= 57-1 && per <= 57+1)  // ends S 
  {
    if(per2 >= 83-1 && per2 <= 83+1 && per1 >= 83-1 && per1 <= 83+1) Dtone[9]+=9; // LLS
  }
  if(per >= 83-1 && per <= 83+1)  // ends L 
  {
    if(per2 >= 57-1 && per2 <= 57+1 && per1 >= 83-1 && per1 <= 83+1) Dtone[9]+=9; // SLL
    if(per2 >= 83-1 && per2 <= 83+1 && per1 >= 57-1 && per1 <= 57+1) Dtone[9]+=9; // LSL
  }
  
  //-------------------------------------------------------
  // Tone 10 54 80  LLS
  if(per >= 54-1 && per <= 54+1)  // ends S 
  {
    if(per2 >= 80-1 && per2 <= 80+1 && per1 >= 80-1 && per1 <= 80+1) Dtone[10]+=8; // LLS
  }
  
  //-------------------------------------------------------
  // Tone 11 52 77  SLS SLL LLS
  if(per >= 52-1 && per <= 52+1)  // ends S 
  {
    if(per2 >= 52-1 && per2 <= 52+1 && per1 >= 77-1 && per1 <= 77+1) Dtone[11]+=4; // SLS
    if(per2 >= 77-1 && per2 <= 77+1 && per1 >= 77-1 && per1 <= 77+1) Dtone[11]+=4; // LLS
  }
  if(per >= 77-1 && per <= 77+1)  // ends L 
  {
    if(per2 >= 52-1 && per2 <= 52+1 && per1 >= 77-1 && per1 <= 77+1) Dtone[11]+=4; // SLL
  }
  */
}
//-----------------------------------------------------------------------------


//=============================================================================
//   MAIN
//=============================================================================
void main ()
{
  //-----------------------------------------------------
  // PIC 12F675  setup ports
  ANSEL = 0;            // ADC off
  CMCON =  0b00000100;  // GP1 is comp input pin
  VRCON =  0b10100000;  // CVref ON, CVref must be set to zero!

  TRISIO = 0b00001010;  // see page top for pins
  GPIO =   0b00000000;  // GP1 output OFF at startup
  WPU =    0b00000000;  // pin pullups; 1 = pullup ON
  
  OPTION_REG = 0b10000011; //  TMR0 at 1:16 prescaler

  output_last = 0;
  //-----------------------------------------------------
  // DTMF triplet decoder algorithm;
  // 1, debounce the comparator input so it must be HI and LO for 220uS each
  // 2. synchronise with that debounce to measure one input period
  // 3. after every period compare the last 3 periods to match a "triplet"
  // 4. allocate DTMF points for any triplet matches
  // 5. after 60 triplets are tested, see if there is a good winning points score
  // 6. if winning score is DTMF keys "0" or "1" set the output pin accordingly
  //-----------------------------------------------------
  // main run loop
  while(1)
  {
    //-----------------------------------------
    // clear the tone points accumulators before testing
    for(i=0; i<12; i++)  Dtone[i] = 0;  

    //-----------------------------------------
    // capture 63 periods and test 60 triplets, takes about 95mS total
    for(i=0; i<63; i++)
    {
      comp_debounce();          // sync with waveform
      cap = TMR0;               // capture timer
      per = (cap - cap_last);   // calc the period between timer captures
      per = (per >> 1);         // halve period to suit testing
      cap_last = cap;           // save capture for next test

      // after 3 initial captures we can start testing triplets
      if(i >= 3) decode_triplets();  // test triplet, and allocate points

      // store the previous periods ready for next triplet test
      per2 = per1;
      per1 = per;
    }
    //-----------------------------------------
    // now we have tested 60 triplets and allocated points, test which
    // of the 12 DTMF tones scored the most points
    Dhigh= 0;
    for(i=0; i<12; i++)
    {
      if(Dtone[i] > Dhigh)  // if this is the new best score
      {
        Dhigh = Dtone[i];   // record best score
        Dbest = i;          // and who owns it
      }
    }
    
    // now Dbest tells which DTMF tone had the best score,    
    // Dhigh is its score. A real good DTMF match score will be 120-180
    if(Dhigh > 60)    // looks like we found a real DTMF tone! 
    {
      if(Dbest == 0)    // DTMF "1" key, so turn output ON
      {
        GPIO.F0 = 1;    // change the output
        if(output_last == 0) make_beeps(2);  // 2 beeps to say it toggled
        output_last = 1;
      }
      if(Dbest == 7)    // DTMF "0" key, so turn output OFF
      {
        GPIO.F0 = 0;    // change the output
        if(output_last == 1) make_beeps(1);  // 1 beep to say it toggled
        output_last = 0;
      }
    }
  }
}
//-----------------------------------------------------------------------------




