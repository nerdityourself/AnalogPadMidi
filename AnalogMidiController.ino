/*
@author NerdItYourself
@version 1.0, 05/10/2016
*/


// MIDI channel from 0 to 15
#define  midichannel 1       
#define nPad 6
// Note: we use the same trasmitter pin for all the sensor in order to use less digital pin. 


// Note: change array size according to the number of resistive sensor that you want to manage.

byte Notes[nPad] = {67, 71 , 64, 66};                // MIDI notes associated with each sensor. Change these values to send other notes.
int Threshold[nPad] = {128, 128, 128, 128};      // Value that each sensor has to overcome to send a note                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
boolean VelocityFlag  = false;                    // Velocity ON (true) or OFF (false)

boolean activePad[nPad] = {0,0,0,0};                   // Array of flags indicating wich sensor is currently playing
int PinPlayTime[nPad] = {0,0,0,0};                     // Counter since a note started to play
int MaxTime[nPad] = {0,0,0,0};                      // Cycles before a 2nd hit is allowed

byte status1;                                     // byte that we will use to compose a part of the MIDI message

int pads[6] = {A0,A1,A2,A3,A4,A5};

int pin = 0;     
int velocity = 0;

bool debug = true; // Set this flag to true if you want to read cap sense values in Serila Monitor to check if everything is working properly


void setup() 
{
  // Initialize serial port (we will use serial to debug our code and then to send midi commands)
  Serial.begin(115200);  
}

void loop() 
{
  if(debug)
  {
    Serial.println("");
  }
    
  for(int pin=0; pin < nPad; pin++)                          
  {
     float resValue = analogRead(pads[pin]); // Read value of the resistive sensor
     if(debug)
     {
      Serial.print(" pin: "); 
      Serial.print(pin);
      Serial.print(" - "); 
      Serial.print(resValue); 
     }

    // Check if cap. sensor value is above the threshold
    if((resValue > Threshold[pin]))
    {
      if((activePad[pin] == false))
      {
        if(VelocityFlag == true)
        {
          velocity = (resValue / 8) -1 ;                                              
        }
        else
        {
          velocity = 127;     
        }
     
        MIDI_TX(144, Notes[pin], velocity); //note on (144 is signal for note on)
 
        PinPlayTime[pin] = 0;
        activePad[pin] = true;
      }
      else
      {
        PinPlayTime[pin] = PinPlayTime[pin] + 1;
      }
    }
    else if(activePad[pin] == true)
    {
      PinPlayTime[pin] = PinPlayTime[pin] + 1;
      if(PinPlayTime[pin] > MaxTime[pin])
      {
        activePad[pin] = false;
        MIDI_TX(128, Notes[pin],0); //note off (128 is signal for note on)
      }
    }
  } 
 }



//*******************************************************************************************************************
// Transmit MIDI Message
//*******************************************************************************************************************
void MIDI_TX(byte MESSAGE, byte PITCH, byte VELOCITY) 
{
  if(!debug)
  {
    status1 = MESSAGE + midichannel;    // The first byte we have to send contains information about the command (note on ,note off, etc..) and the channel were we want to send the message
    Serial.write(status1);
    Serial.write(PITCH);                // The second byte of the message contains the pitch
    Serial.write(VELOCITY);             // The last byte of the message contatins the velocity of the note
  }

}


