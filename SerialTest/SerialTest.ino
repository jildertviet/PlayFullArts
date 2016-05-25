#include <Messenger.h>
#include <Laser.h>

#define kNumLasers  3
#define kNumTiles   5
Laser* lasers[kNumLasers]; 
int ids[] =         {60,  63, 67};
int laserPins[] =   {4,   8,  9};
int LDRPins[] =     {0,   1,  2};
int tilesPins[] =   {1,   2,   3,   4,    5};
unsigned long startTime = 0;
unsigned long endTime = 0; 
unsigned long highScore = 0;
bool gameRunning = false;

Messenger message = Messenger();
//------------------------------------------------------------------------------------------------------------------------------------||
void setup() {
  Serial.begin(9600);
  message.attach(messageCompleted);
  for(int i=0; i<kNumLasers; i++)
    lasers[i] = new Laser(ids[i], LDRPins[i], laserPins[i]);
}
//------------------------------------------------------------------------------------------------------------------------------------||
void loop() {
  if(!gameRunning){
//    checkTiles();
  } else{
    checkLDRs();
  }
  receiveSerialData();
  delay(100);
}
//------------------------------------------------------------------------------------------------------------------------------------||
void receiveSerialData(){ // send like "idstate" / "001" id & state, id = two chars
  while ( Serial.available() ) message.process( Serial.read() );
}
//------------------------------------------------------------------------------------------------------------------------------------||
void checkLDRs(){
    for(int i=0; i<kNumLasers; i++){
      if(lasers[i]->checkIfObstructed()){
        stopGame();
      }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------||
void messageCompleted() {
  // This loop will echo each element of the message separately
    char stringTemp[30];
    message.copyString(stringTemp,30);
    
    char idChar[2];
    char stateChar;
    String stateString;
    String idString;
    
    idChar[0] = stringTemp[0];
    idChar[1] = stringTemp[1];
    stateChar = stringTemp[2];
    idString = idChar; // to string
    stateString = stateChar;

    int idInt = idString.toInt();
    int stateInt = stateString.toInt();
//    int state = (int)stringTemp[2];


    if(idInt==99){
      startGame(); return;
    }
    for(int i=0; i<kNumLasers; i++){
      if(idInt==lasers[i]->id)
        lasers[i]->setState(stateInt);    
    }
}
//------------------------------------------------------------------------------------------------------------------------------------||
void startGame(){
  gameRunning = true;
  sendMsg(100,6);
  startTime = millis();
}
//------------------------------------------------------------------------------------------------------------------------------------||
void stopGame(){
  endTime = millis() - startTime;
  if(endTime > highScore){
    highScore = endTime;
    highScoreFunction(); 
  }
  gameRunning = true;
}
//------------------------------------------------------------------------------------------------------------------------------------||
void highScoreFunction(){
  sendMsg(100,7);
}
//------------------------------------------------------------------------------------------------------------------------------------||
void checkTiles(){
  for(int i=0; i<kNumTiles; i++){
    if(digitalRead(tilesPins[0])==LOW){
      return;
    }
  }
  startGame();
}
//------------------------------------------------------------------------------------------------------------------------------------||

void sendValue(unsigned int value){
    Serial.print(value);
    Serial.write(32);
}
//------------------------------------------------------------------------------------------------------------------------------------||
void sendMsg(unsigned int id, unsigned int mode){
    sendValue(id);
    sendValue(mode);
    Serial.println();


    // Moet dit?
    Serial.flush();
}

