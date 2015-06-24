// SparkFun MIDI Sheild and MIDI Breakout test code
// Defines bare-bones routines for sending and receiving MIDI data
// A0 pot0 CounterClockwise rotation 0..1023   pitch
// A1 pot1                                     velocity
// D2 button0 HIGH=off                         channel 0 note  
// D3 button1 LOW=on                           channel 1 note  
// D4 button2                                  channel 2 note
// D7 LEDRED  HIGH=off                          red LED  
// D6 LEDGRN  LOW=on                            green LED   
// TX MIDI out
// RX MIDI in (not used in _Out sketch)

// Original Written  02/16/2010
// Revision Out_Poly 06/17/2015

// defines for MIDI Shield components only
#define KNOB0  0
#define KNOB1  1

#define BUTTON2  2
#define BUTTON3  3
#define BUTTON4  4

#define LEDRED  7
#define LEDGRN  6

#define LEDOFF HIGH
#define LEDON  LOW

#define NOTEON 0x90
#define NOTEOFF 0x80
#define WAIT 0x00

#define BLINKCOUNT 10000

byte incomingByte;
// midi command components
byte cmd;       //left  hexit is command: 8 = note off; 9 = note on
byte chn;       //right hexit is channel  0..F 
byte cc;        // combined cmd | chn
byte note;      // 0..127
byte velocity;  // 0..127

byte ccLast[16];  //last midi command issued on a channel

int pot1;
int pot0;
int channel; // channel number 0..15 = midi channel 1..16
int bCount;


boolean blinkGrn;
boolean blinkRed;


byte byte1;
byte byte2;
byte byte3;

void setup() {

  pinMode(LEDRED,OUTPUT);   
  pinMode(LEDGRN,OUTPUT);

  pinMode(BUTTON2,INPUT);
  pinMode(BUTTON3,INPUT);
  pinMode(BUTTON4,INPUT);

  digitalWrite(BUTTON2,HIGH);
  digitalWrite(BUTTON3,HIGH);
  digitalWrite(BUTTON4,HIGH);

  for(int i = 0;i < 2;i++) // flash MIDI Sheild LED's on startup
  {
    digitalWrite(LEDRED,LEDOFF);  
    digitalWrite(LEDGRN,LEDON);
    delay(2000);
    digitalWrite(LEDRED,LEDON);  
    digitalWrite(LEDGRN,LEDOFF);
    delay(500);
  }
  digitalWrite(LEDRED,LEDOFF);   
  digitalWrite(LEDGRN,LEDOFF);

  //start serial with midi baudrate 31250
  Serial.begin(31250);   
  allNotesOff();
 
  //reset last-command-issued 
  for (byte c = 0; c<16; c++){
    ccLast[c]=0;  
  }
  bCount=BLINKCOUNT;  //still-alive blink interval
  blinkGrn = false;
  blinkRed = false;
}

void loop () {
  --bCount;
  if(bCount<0){
    blinkLed(LEDRED,1);
    blinkLed(LEDGRN,1);
    bCount=BLINKCOUNT;
  }
  
  //*************** MIDI OUT ***************//

  pot0 = analogRead(KNOB0);   //note
  pot1 = analogRead(KNOB1);   //velocity
  
  note = 127- (pot0/8);       // convert value to value 0-127
  velocity = 127-(pot1/8);    //reverse to increase clockwise 
  for (int btn = 2; btn<5;btn++){ //buttons 2,3,4
    chn = btn - 2;  //convert button number to MIDI channel   
    if(button(btn)) {
      Midi_Send_Cond( NOTEON,chn,note,velocity);
    }else{
       Midi_Send_Cond( NOTEOFF, chn,note,0);   
    }
  }

}

void allNotesOff(){
  byte chn, note, cc;
  for (chn=0;chn<16;++chn){
    cc=0x80 | chn;
    blinkLed(LEDGRN,1);
    for (note = 0; note<128; ++note){    
      Midi_Send(cc,note,0);
      blinkLed(LEDRED,1);
    }
  }
}

void Midi_Send(byte cmd, byte data1, byte data2) {
  Serial.write(cmd);
  Serial.write(data1);
  Serial.write(data2);
}

void Midi_Send_Cond( byte cmd, byte chn, byte data1, byte data2) {
  //send MIDI command only if not already sent
  //then remember it in global array ccLast
  //and blink a LED
  byte cc = cmd | chn;
  if (cc != ccLast[chn]) {
    Serial.write(cc);
    Serial.write(data1);
    Serial.write(data2);
    ccLast[chn] = cc;
    if (cmd == NOTEON) {blinkLed(LEDGRN,1); }
    else if (cmd == NOTEOFF) {blinkLed(LEDRED,1) ;}
  }
}

void blinkLed(byte pin, int rate) {
  digitalWrite(pin,LEDON);
  delay(rate);
  digitalWrite(pin,LEDOFF);
}

void blink(int times, int rate) {
  int grn=rate;
  int red=127-rate;
  for (int t=0; t<times;t++) {
    digitalWrite(LEDRED,LEDOFF);  
    digitalWrite(LEDGRN,LEDON);
    delay(rate);
    digitalWrite(LEDGRN,LEDOFF);
    digitalWrite(LEDRED,LEDON);  
    delay(rate);
  }
  digitalWrite(LEDRED,LEDOFF);  
  digitalWrite(LEDGRN,LEDOFF);
}

char button(char button_num)
{
  return (!(digitalRead(button_num)));
}
