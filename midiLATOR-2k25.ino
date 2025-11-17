// midiLATOR - calculator becomes MIDI controller
// Built with ESP-WROOM-32
// Based around AppleMIDI library, WiFi MIDI
// Use with RTPMIDI utility on Windows


#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

#define SerialMon Serial
#include <AppleMIDI_Debug.h>

#include <AppleMIDI.h>

/* Pins for each "row" of calculator buttons */
const int rowSix = 15;
const int rowThree = 2;
const int rowSeven = 0;
const int rowOne = 4;
const int rowFour = 16;
const int rowTwo = 17;
const int rowZero = 5;
const int rowEight = 18;
const int rowFive = 19;

/* Button status registers */
int buttonArray[35];

//Address row pins in an array
int rowPins[] = {rowZero, rowOne, rowTwo, rowThree, rowFour, rowFive, rowSix, rowSeven, rowEight};
// Note that this is setup to match the keypad matrix
// of a J BUrrows calculator. You might need to change
// it to suit your own gear.

//create array of MIDI notes mapped to buttons
int buttonNoteStatus[35];

//setup WiFi AP settings
char ssid[] = "ssid"; //  your network SSID (name)
char pass[] = "password";    // your network password (use for WPA, or use as key for WEP)

unsigned long t0 = millis();
int8_t isConnected = 0;

APPLEMIDI_CREATE_DEFAULTSESSION_INSTANCE();


int midiNote;
int btnState;
int lastBtnState;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void setup()
{
  // Set all pins as inputs with internal pullups
  for (byte i = 0; i < 9; i = i + 1) {
    pinMode(rowPins[i], INPUT_PULLUP);
  } 
  //Initialize button array
  for (byte i = 0; i < 34; i = i + 1) {
    buttonArray[i]=1; //reverse polarity cause we're grounding out the buttonis.
    buttonNoteStatus[i]=1; //1 is off, 0 is on. lol.
  }  
  
  DBG_SETUP(115200);
  DBG("Booting");

  WiFi.softAP(ssid, pass);

  DBG(F("Started soft access point:"), WiFi.softAPIP(), "Port", AppleMIDI.getPort());
  DBG(F("AppleMIDI device name:"), AppleMIDI.getName());
  // Set up mDNS responder:
  if (!MDNS.begin(AppleMIDI.getName()))
    DBG(F("Error setting up MDNS responder!"));
  char str[128] = "";
  strcat(str, AppleMIDI.getName());
  strcat(str,".local");
  DBG(F("mDNS responder started at:"), str);
  MDNS.addService("apple-midi", "udp", AppleMIDI.getPort());
  DBG(F("Open Wifi settings and connect to soft acess point using 'ssid'"));
  DBG(F("Start MIDI Network app on iPhone/iPad or rtpMIDI on Windows"));
  DBG(F("AppleMIDI-ESP8266 will show in the 'Directory' list (rtpMIDI) or"));
  DBG(F("under 'Found on the network' list (iOS). Select and click 'Connect'"));
  
  MIDI.begin();

  AppleMIDI.setHandleConnected([](const APPLEMIDI_NAMESPACE::ssrc_t & ssrc, const char* name) {
    isConnected++;
    DBG(F("Connected to session"), ssrc, name);
  });
  AppleMIDI.setHandleDisconnected([](const APPLEMIDI_NAMESPACE::ssrc_t & ssrc) {
    isConnected--;
    DBG(F("Disconnected"), ssrc);
  });
  
  MIDI.setHandleNoteOn([](byte channel, byte note, byte velocity) {
    DBG(F("NoteOn"), note);
  });
  MIDI.setHandleNoteOff([](byte channel, byte note, byte velocity) {
    DBG(F("NoteOff"), note);
  });

  DBG(F("Sending NoteOn/Off of note 45, every second"));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loop()
{
  //scan the calculator buttons
  scanThePins();
  
  //Scroll through button status array and act on it, yo!
  for (byte i = 0; i < 32; i = i + 1) {
    if((!buttonArray[i]) && (buttonNoteStatus[i])){
      //Send a note! If it's not already on though.
      MIDI.sendNoteOn(i+59, 127, 1);    //59 offset should have keycode 1 start at Middle C
      buttonNoteStatus[i]=0; //set note to on (ie. 0 in this context)
    }
    else if (buttonArray[i]){
      MIDI.sendNoteOff(i+59,127,1);
      buttonNoteStatus[i]=1; //set note to off (ie. 1 in this context)
    }
  } 
  
  //MDNS.update();
  
  // Listen to incoming notes
  MIDI.read();
}

void scanThePins(){
    
    //set rowZero to output
    pinMode(rowPins[0], OUTPUT);
    //pull it low
    digitalWrite(rowPins[0],LOW);
    delay(1);
    //now log status of all other pins
    buttonArray[8]=digitalRead(rowPins[1]);      //row Zero, row One is "2", keycode 8
    buttonArray[1]=digitalRead(rowPins[2]);     //row Zero, row Two is "0", keycode 1
    buttonArray[12]=digitalRead(rowPins[3]);     //row Zero, row Three is "4", keycode 12
    buttonArray[7]=digitalRead(rowPins[4]);     //row Zero, row Four is "1", keycode 7
    buttonArray[4]=digitalRead(rowPins[5]);     //row Zero, row Five is "+", keycode 4
    buttonArray[24]=digitalRead(rowPins[6]);     //row Zero, row Six is "TAX-", keycode 24
    buttonArray[9]=digitalRead(rowPins[7]);     //row Zero, row Seven is "3", keycode 9
    pinMode(rowPins[0], INPUT_PULLUP);
    //delay(5);
    
    //set rowOne to output
    pinMode(rowPins[1], OUTPUT);
    //pull it low
    digitalWrite(rowPins[1],LOW);
    delay(1);
    //now log status of all other pins
    buttonArray[14]=digitalRead(rowPins[2]);     //row One, row Two is "6", keycode 14
    buttonArray[20]=digitalRead(rowPins[4]);     //row One, row Four is "9", keycode 20
    buttonArray[16]=digitalRead(rowPins[5]);     //row One, row Five is "divide", keycode 16
    buttonArray[28]=digitalRead(rowPins[6]);     //row One, row Six is "MGN", keycode 28
    buttonArray[22]=digitalRead(rowPins[7]);     //row One, row Seven is "+/-", keycode 22
    buttonArray[29]=digitalRead(rowPins[8]);     //row One, row Eight is "MRC", keycode 29
    pinMode(rowPins[1], INPUT_PULLUP);
    //delay(5);
    
    //set rowTwo to output
    pinMode(rowPins[2], OUTPUT);
    //pull it low
    digitalWrite(rowPins[2],LOW);
    delay(1);
    //now log status of all other pins
    buttonArray[19]=digitalRead(rowPins[3]);     //row Two, row Three is "8", keycode 19
    buttonArray[13]=digitalRead(rowPins[4]);     //row Two, row Four is "5", keycode 13
    buttonArray[10]=digitalRead(rowPins[5]);     //row Two, row Five is "minus", keycode 16
    buttonArray[26]=digitalRead(rowPins[6]);     //row Two, row Six is "COST", keycode 26
    buttonArray[18]=digitalRead(rowPins[7]);     //row Two, row Seven is "7", keycode 18
    buttonArray[31]=digitalRead(rowPins[8]);     //row Two, row Eight is "M+", keycode 31
    pinMode(rowPins[2], INPUT_PULLUP);
    //delay(5);
    
    //set rowThree to output
    pinMode(rowPins[3], OUTPUT);
    //pull it low
    digitalWrite(rowPins[3],LOW);
    delay(1);
    //now log status of all other pins
    buttonArray[2]=digitalRead(rowPins[4]);     //row Three, row Four is "00", keycode 2
    buttonArray[5]=digitalRead(rowPins[5]);     //row Three, row Five is "=", keycode 5
    buttonArray[17]=digitalRead(rowPins[7]);     //row Three, row Seven is "->", keycode 17
    buttonArray[21]=digitalRead(rowPins[8]);     //row Three, row Eight is "%", keycode 21
    pinMode(rowPins[3], INPUT_PULLUP);
    //delay(5);
    
    //set rowFour to output
    pinMode(rowPins[4], OUTPUT);
    //pull it low
    digitalWrite(rowPins[4],LOW);
    delay(1);
    //now log status of all other pins
    buttonArray[15]=digitalRead(rowPins[5]);     //row Four, row Five is "X", keycode 15
    buttonArray[27]=digitalRead(rowPins[6]);     //row Four, row Six is "SELL", keycode 27
    buttonArray[3]=digitalRead(rowPins[7]);     //row Four, row Seven is ".", keycode 3
    buttonArray[30]=digitalRead(rowPins[8]);     //row Four, row Eight is "M-", keycode 29
    pinMode(rowPins[4], INPUT_PULLUP);
    //delay(5);
    
    //set rowFive to output
    pinMode(rowPins[5], OUTPUT);
    //pull it low
    digitalWrite(rowPins[5],LOW);
    delay(1);
    //now log status of all other pins
    buttonArray[23]=digitalRead(rowPins[6]);     //row Five, row Six is "SET", keycode 23
    buttonArray[11]=digitalRead(rowPins[8]);     //row Five, row Eight is "GT", keycode 11
    pinMode(rowPins[5], INPUT_PULLUP);
    //delay(5);
    
    //set rowSix to output
    pinMode(rowPins[6], OUTPUT);
    //pull it low
    digitalWrite(rowPins[6],LOW);
    delay(1);
    //now log status of all other pins
    buttonArray[6]=digitalRead(rowPins[7]);     //row Six, row Seven is "C/CE", keycode 6
    buttonArray[25]=digitalRead(rowPins[8]);     //row Six, row Eight is "TAX+", keycode 25
    pinMode(rowPins[6], INPUT_PULLUP);
    //delay(5); 
  
}
