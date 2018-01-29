#include "Servo.h"
#include "EEPROM.h"
#include "ClickButton.h"

//   BOOT
uint8_t Status = 1;  
              // 0 - Just powered
              // 1 - Disarmed
              // 2 - Arming...
              // 3 - Armed
              // 4 - Cruise
              // 5 - Configuration
           
boolean Alert = 0;
boolean Danger = 0;
boolean AlreadyArmed = 0;
byte UserMode = EEPROM.read(0); //read the last Mode selected
byte DEVELOPER_mode = 0; // simulate troubles

int PreviousChoice = 0;
byte View = 1;    //for display

unsigned long previousSerialMillis;

boolean ledVal = false; 
int ledState = 0;


const int numReadingsBeginner = 30;   // Number of throttle readings for Beginner Mode
const int numReadingsNormal = 15;     // Number of throttle readings for Normal Mode
const int numReadingsAdvanced = 2;    // Number of throttle readings for Advanced Mode

int readingsBeginner[numReadingsBeginner];   
int readingsNormal[numReadingsNormal];        
int readingsAdvanced[numReadingsAdvanced];   
int readIndex = 0;                           

int ThumbTotalBeginner = 0;         
int ThumbTotalNormal = 0;           
int ThumbTotalAdvanced = 0;        

int Thumb = 0;
int ThumbRaw = 0;
int CruiseGas;
int ReqGas = 0;

const int ThumbMin = 1;                 // Minimum analog read of potentiometer
const int ThumbMax = 1010;              // Maximum analog read of potentiometer
const int ThumbSafe = 10;               // Maximum analog read to arm (will not arm if greater)
const int MicrosecondMin = 800;         // Minimum PWM to esc
const int MicrosecondIdle = 915;        // Idle PWM to esc
const int MicrosecondMax = 2500;        // Maximum PWM to esc
const int MicrosecondMinCruise = 950;   // Minimum PWM to activate Cruise Mode

int PowerMax = 2500;                    // Maximum power in microseconds

//Enabled sectors at startup
boolean Sector1 = 1;
boolean Sector2 = 1;
boolean Sector3 = 1;
boolean Sector4 = 1;

Servo Esc1;
Servo Esc2;
Servo Esc3;
Servo Esc4;


//   ALERTS AND LIMITS

//                                             *** Alert the pilot ***              *** Cut/limit power ***          
                                     const uint8_t BatIntResAlert = 10;   const uint8_t BatIntResDanger = 20;
                                     const uint8_t BatTempMinAlert = 10;  const uint8_t BatTempMinCut = 0;
const uint8_t BatTempNormal = 40;    const uint8_t BatTempAlert = 50;     const uint8_t BatTempDanger = 60;
const uint8_t EscTempNormal = 60;    const uint8_t EscTempAlert = 80;     const uint8_t EscTempDanger = 100;
const uint8_t MotTempNormal = 40;    const uint8_t MotTempAlert = 60;     const uint8_t MotTempDanger = 80;
const uint8_t AmpNormal = 80;        const uint8_t AmpAlert = 100;        const uint8_t AmpDanger = 120;
  const float CelNormal = 3.7;         const float CelAlert = 3.5;          const float CelDanger = 3.2;

//   CONNECTIONS
const uint8_t ThumbPin = A0;  //Potentiometer
const uint8_t buzzerPin = 8;  //Buzzer
const int buttonPin = 12;     //Button
const uint8_t ledPin = 13;    //Led

ClickButton button1(buttonPin, LOW, CLICKBTN_PULLUP);

void setup() {
  
pinMode(buttonPin, INPUT);
pinMode(ledPin, OUTPUT); // Set LED output pins
digitalWrite(buttonPin, HIGH );

PlayStartup();
Serial.begin(115200);
PrintBoot();

} //fine Setup


void loop() {
  
int b = checkButton();
if (b == 1) clickEvent();
if (b == 2) doubleClickEvent();
if (b == 3) holdEvent();
if (b == 4) longHoldEvent();

 
  
// -------------READINGS THUMB--------------
     ThumbRaw = analogRead(A0);            // read from the sensor
switch (UserMode) {
    case 1:// Beginner
          ThumbTotalBeginner = ThumbTotalBeginner - readingsBeginner[readIndex];
          readingsBeginner[readIndex] = ThumbRaw;
          ThumbTotalBeginner = ThumbTotalBeginner + readingsBeginner[readIndex];
          readIndex = readIndex + 1;
          if (readIndex >= numReadingsBeginner)
            readIndex = 0;
          Thumb = ThumbTotalBeginner / numReadingsBeginner;    
          break;

    case 2:// Normal
          ThumbTotalNormal = ThumbTotalNormal - readingsNormal[readIndex];
          readingsNormal[readIndex] = ThumbRaw;
          ThumbTotalNormal = ThumbTotalNormal + readingsNormal[readIndex];
          readIndex = readIndex + 1;
          if (readIndex >= numReadingsNormal)
            readIndex = 0;
          Thumb = ThumbTotalNormal / numReadingsNormal;  
          break;
          
     case 3:// Advanced
          ThumbTotalAdvanced = ThumbTotalAdvanced - readingsAdvanced[readIndex];
          readingsAdvanced[readIndex] = ThumbRaw;
          ThumbTotalAdvanced = ThumbTotalAdvanced + readingsAdvanced[readIndex];
          readIndex = readIndex + 1;
          if (readIndex >= numReadingsAdvanced)
            readIndex = 0;
          Thumb = ThumbTotalAdvanced / numReadingsAdvanced;   
          break;
      default: 
          UserMode = 1;
      break;
  }

  
        Thumb = constrain(Thumb, ThumbMin, ThumbMax);      
    
        ReqGas = map(Thumb, ThumbMin, ThumbMax, MicrosecondIdle, PowerMax); 

     // ReqGas = map(Thumb, ThumbMin, ThumbMax, MicrosecondMin, PowerMax); 

// STATUS
           // 0 - Just turned on
           // 1 - Disarmed
           // 2 - Arming...
           // 3 - Armed
           // 4 - Cruise
           // 5 - Configuration
  
    switch (Status) { case 0: //---------------------- 0 - Just turned on 
                      break;
                      case 1: //---------------------- 1 - Disarmed
                           ledVal = false;
                           Esc1.writeMicroseconds(MicrosecondMin);
                           Esc2.writeMicroseconds(MicrosecondMin); 
                           Esc3.writeMicroseconds(MicrosecondMin); 
                           Esc4.writeMicroseconds(MicrosecondMin);  
                      break;
                      case 2: //---------------------- 2 - Arming...
                           ledVal = true;
                           ReqGas = MicrosecondMin; 
                           tone(buzzerPin, 2093, 50);
                           Esc1.attach(3);                 // initialize ESC 1
                           delay(650); 
                           tone(buzzerPin, 2093, 50);
                           Esc2.attach(4);                 // initialize ESC 2
                           delay(650); 
                           tone(buzzerPin, 2093, 50);
                           Esc3.attach(5);                 // initialize ESC 3
                           delay(650); 
                           tone(buzzerPin, 2093, 50);
                           Esc4.attach(6);                 // initialize ESC 4
                           delay(1500); 
                           
                           PlaySomeFuturisticSound();
                           
                           delay(1000);
                           Status = 3;
                           AlreadyArmed = 1;
                        break;
                      case 3: //---------------------- 3 - Armed
                           ledVal = true;
                          if (Sector1=1){Esc1.writeMicroseconds(ReqGas);}else{Esc1.writeMicroseconds(MicrosecondMin);}
                          if (Sector2=1){Esc2.writeMicroseconds(ReqGas);}else{Esc2.writeMicroseconds(MicrosecondMin);}
                          if (Sector3=1){Esc3.writeMicroseconds(ReqGas);}else{Esc3.writeMicroseconds(MicrosecondMin);}
                          if (Sector4=1){Esc4.writeMicroseconds(ReqGas);}else{Esc4.writeMicroseconds(MicrosecondMin);}
                        break;
                      case 4: //---------------------- 4 - Cruise
                           ledVal = true;
                           Esc1.writeMicroseconds(CruiseGas);
                           Esc2.writeMicroseconds(CruiseGas); 
                           Esc3.writeMicroseconds(CruiseGas); 
                           Esc4.writeMicroseconds(CruiseGas);  
                           if ((CruiseGas-ReqGas) < -40){  // How much push the throttle to exit from cruise (negative number) 
                                Status = 3;
                            PlayExit();     
                            }                           
                      break;
                      case 5: //---------------------- 5 - Configuration
                       break;
                      default:
                      break;
                     }
                   
    digitalWrite(ledPin, ledVal);
  
  
    switch (DEVELOPER_mode) { case 0:
                              Sector1=1; Sector2=1; Sector3=1; Sector4=1;
                              break;
                              case 1: 
                              Sector1=1; Sector2=1; Sector3=0; Sector4=1;
                              break;
                              case 2:
                              Sector1=1; Sector2=1; Sector3=1; Sector4=0;
                              break;
                              case 3:
                              Sector1=1; Sector2=0; Sector3=0; Sector4=1;
                              break;
                              case 4:
                              Sector1=0; Sector2=1; Sector3=1; Sector4=0;
                              break;
                              case 5:
                              Sector1=1; Sector2=0; Sector3=0; Sector4=0;
                              break;
    }
  
    
    PrintLoop();

} //fine Loop
          

void clickEvent() { //  SHORT BUTTON PRESS
    switch (Status) { case 0: PlayError(); break;
                      case 1: PlayPush();        // Disarmed
                              if(View < 4)
                              {View = View +1;
                              }else{View = 1; }
                              break;                      
                      case 2: Status = 1; break; // Arming...
                      case 3: PlayPush();        // Armed
                              if(View < 4)
                              {View = View +1;
                              }else{View = 1; }
                              break; 
                      case 4: PlayPush();        // Cruise
                              if(PreviousChoice < 5)
                              {PreviousChoice = PreviousChoice +1;
                              }else{PreviousChoice = 0; }
                              break;
                      case 5: PlayPush();
                      UserMode = UserMode +1;  // Configuration
                      break;
                      default: break;
                     }
                   }


void holdEvent() { //  NORMAL BUTTON PRESS
    switch (Status) { case 0: PlayError(); break;
                      case 1:  break;                      
                      case 2: DEVELOPER_mode = 0; Status = 1; break;
                      case 3:  break;
                      case 4: PlayPush();
                              if(PreviousChoice < 5)
                                {PreviousChoice = PreviousChoice +1;
                                }else{PreviousChoice = 0; }
                              break;
                      case 5: PlayPush(); UserMode = UserMode +1; break;
                      default: break;
                     }
                   }


void longHoldEvent() { // LONG BUTTON PRESS
    switch (Status) { case 0:                 // If boot is not complete
                              PlayError();    // Error sound
                              break;          // Exit
                              
                      case 1:                             // If boot is complete
                              if (Thumb < ThumbSafe){     // And the throttle is at minimum
                                
                                  if(AlreadyArmed == 1){      // If is NOT the first time you're arming
                                      Status = 3;             // arm.
                                  }else{                      // If is the first time you're arming 
                                      Status = 2;             // go in the arming process.
                                  }    
                                  PlayArming();               // Arming sound
                              }else{
                                    PlayError();              // Error sound
                                }
                              break;                          // Exit
                      case 2: DEVELOPER_mode = 0; Status = 1; break;
                      case 3: PlayExit(); DEVELOPER_mode = 0; Status = 1; break;
                      case 4: DEVELOPER_mode = 0; Status = 1; break; 
                      case 5: PlayError(); break; 
                      default: break;
                     }
                   }
                   
void doubleClickEvent() { //  DOUBLE BUTTON PRESS
    switch (Status) { case 0: PlayError(); break;
                      case 1: PlayPush();
                              PlayConfiguration();
                              PreviousChoice = UserMode;
                              Status = 5;
                              break;
                      case 2: Status = 1; break;
                      case 3: PreviousChoice = 0;
                              if(ReqGas > (MicrosecondIdle+100) && ReqGas < (MicrosecondMax-100)){   //Activate cruise only if in range
                                Status = 4;
                                CruiseGas = ReqGas;  
                                PlayCruise();
                              }else{PlayError();}
                              break;
                      case 4: PlayExit();
                              DEVELOPER_mode = PreviousChoice;
                              Status = 3;
                              break;
                      case 5:
                      if(PreviousChoice == UserMode){
                      PlayExit();
                      }else{
                      EEPROM.write(0, UserMode);
                      PlayConfirm();
                      delay(200);
                      PlayExit();
                      }
                      Status = 1;
                      break;
                      default: break;
                     }
                   }
                   
void PrintLoop(){

    //SERIAL
     if (millis() - previousSerialMillis >= 1000) {
            previousSerialMillis = millis(); 

      Serial.println();

      Serial.print(F("View: ")); Serial.print(View);
      Serial.print(F("   PreviousChoice: ")); Serial.print(PreviousChoice);
      Serial.print(F("   DEVELOPER_mode: ")); Serial.print(DEVELOPER_mode);
      Serial.print(F("   Status: ")); Serial.print(Status);
      Serial.print(F("   Alert: ")); Serial.print(Alert);
      Serial.print(F("   Danger: ")); Serial.print(Danger);
      Serial.print(F("   UserMode: ")); Serial.print(UserMode);
      Serial.println();

      Serial.print(F("   Thumb: ")); Serial.print(Thumb);
      Serial.print(F("   ThumbRaw: ")); Serial.print(ThumbRaw);
      Serial.print(F("   ReqGas: ")); Serial.print(ReqGas);
      Serial.print(F("   CruiseGas: ")); Serial.print(CruiseGas);
      Serial.println();
      Serial.print(F("   Sector1: ")); Serial.print(Sector1);
      Serial.print(F("   Sector2: ")); Serial.print(Sector2);
      Serial.print(F("   Sector3: ")); Serial.print(Sector3);
      Serial.print(F("   Sector4: ")); Serial.print(Sector4);

      Serial.println();
      Serial.println();      Serial.println();
      Serial.println();      Serial.println();
      Serial.println();      Serial.println();
      Serial.println();      Serial.println();
      Serial.println();      Serial.println();
      Serial.println();      Serial.println();
      Serial.println();      Serial.println();
      Serial.println();      Serial.println();
      Serial.println();      Serial.println();
      Serial.println();      Serial.println();
      Serial.println();      Serial.println();
      Serial.println();      Serial.println();
      Serial.println();      Serial.println();
      Serial.println();      Serial.println();
      Serial.println();      Serial.println();
      Serial.println();      Serial.println();
      Serial.println();      Serial.println();
      Serial.println();      Serial.println();
      Serial.println();      Serial.println();
      Serial.println();      Serial.println();
      Serial.println();      Serial.println();
      Serial.println();

       }  // SerialInterval end
}

void PrintBoot(){
      Serial.print(F("BatIntResAlert: ")); Serial.print(BatIntResAlert);
      Serial.print(F("   BatIntResDanger: ")); Serial.print(BatIntResDanger);
      Serial.println();
      Serial.print(F("BatTempMinCut: ")); Serial.print(BatTempMinCut);
      Serial.print(F("   BatTempMinAlert: ")); Serial.print(BatTempMinAlert);  
      Serial.println();
      Serial.print(F("BatTempNormal: ")); Serial.print(BatTempNormal);
      Serial.print(F("   BatTempAlert: ")); Serial.print(BatTempAlert);  
      Serial.print(F("   BatTempDanger: ")); Serial.print(BatTempDanger);  
      Serial.println();
      Serial.print(F("EscTempNormal: ")); Serial.print(EscTempNormal);
      Serial.print(F("   EscTempAlert: ")); Serial.print(EscTempAlert);  
      Serial.print(F("   EscTempDanger: ")); Serial.print(EscTempDanger);  
      Serial.println();
      Serial.print(F("MotTempNormal: ")); Serial.print(MotTempNormal);
      Serial.print(F("   MotTempAlert: ")); Serial.print(MotTempAlert);  
      Serial.print(F("   MotTempDanger: ")); Serial.print(MotTempDanger);  
      Serial.println();
      Serial.print(F("MotTempNormal: ")); Serial.print(MotTempNormal);
      Serial.print(F("   MotTempAlert: ")); Serial.print(MotTempAlert);  
      Serial.print(F("   MotTempDanger: ")); Serial.print(MotTempDanger);  
      Serial.println();
      Serial.print(F("    AmpNormal: ")); Serial.print(AmpNormal);
      Serial.print(F("       AmpAlert: ")); Serial.print(AmpAlert);  
      Serial.print(F("      AmpDanger: ")); Serial.print(AmpDanger);  
      Serial.println();
      Serial.print(F("    CelNormal: ")); Serial.print(CelNormal);
      Serial.print(F("     CelAlert: ")); Serial.print(CelAlert);  
      Serial.print(F("     CelDanger: ")); Serial.print(CelDanger);  
      Serial.println();  
}
