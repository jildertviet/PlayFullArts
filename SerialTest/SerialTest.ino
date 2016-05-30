#include <Messenger.h>
#include <Laser.h>
#include <Adafruit_NeoPixel.h>
#include <Oscillator.h>

#define kNumLasers  1
#define kNumTiles   1

Laser* lasers[kNumLasers]; 
int ids[] =         {60,  63, 67};

int laserPins[] =   {3,   4,  5};
int LDRPins[] =     {0,   1,  2};
int tilesPins[] =   {6,   7,   8,   9,    10};

int pixelPin = 2;
int numPixels = 60-kNumTiles; // Min 5, for tiles
uint32_t highScoreColor, gameTimeColor, gameOverColor;

unsigned long startTime = 0;
unsigned long endTime = 0; 
unsigned long highScore = 50000;
unsigned long gameTime = 120000; // 2 minuten (120 sec)
bool gameRunning = false;

bool bDisplayHighScore = true; 
bool bDisplayGameTime = false;
bool bDisplayGameOver = false;
unsigned long displayGameOverEndTime = 0;
int displayGameOverTime = 16000; // 16 sec

Messenger message = Messenger();
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixels+kNumTiles, pixelPin, NEO_GRB + NEO_KHZ800);
Oscillator osc = Oscillator(128, 1, 0); // Sqaurewave Oscillator for pulsing light when gameOver();
//------------------------------------------------------------------------------------------------------------------------------------||
void setup() {
  Serial.begin(9600);
  message.attach(messageCompleted);
  for(int i=0; i<kNumLasers; i++)
    lasers[i] = new Laser(ids[i], LDRPins[i], laserPins[i]);

  osc.pulseWidth = 0.8;
  osc.setupWaveTable(1);
  // Moet ook nog met de tiles... 
  pinMode(pixelPin, OUTPUT);
  
  highScoreColor = strip.Color(255, 255, 255);
  gameTimeColor = strip.Color(50, 255, 50);
  gameOverColor = strip.Color(255, 0, 0);
  
  strip.show();


//  stopGame(); // moet weg
}

//------------------------------------------------------------------------------------------------------------------------------------||
void loop() {
  receiveSerialData();
  if(!gameRunning){
    checkTiles();
  } else{
    checkLDRs();
  }

  displayGameOver();
  displayGameTime();
  displayHighScore();

//  strip.setPixelColor(1,gameTimeColor);
//  strip.show();
  
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
  resetAllPixels();
  gameRunning = true;
  sendMsg(100,6);
  startTime = millis();
  bDisplayGameTime = true; 
  bDisplayHighScore = true;
}
//------------------------------------------------------------------------------------------------------------------------------------||
void stopGame(){
  endTime = millis() - startTime;
  if(endTime > highScore){
    highScore = endTime;
    highScoreFunction(); 
  }
  bDisplayGameOver = true; 
  osc.retrigger();
  displayGameOverEndTime = millis()+displayGameOverTime;
  
  gameRunning = false;
  bDisplayGameTime = false; 
  bDisplayHighScore = false;
}
//------------------------------------------------------------------------------------------------------------------------------------||
void highScoreFunction(){ // Send if new high score is achieved
  sendMsg(100,7);
}
//------------------------------------------------------------------------------------------------------------------------------------||
void checkTiles(){
  bool stopFunction = false;
  for(int i=0; i<kNumTiles; i++){
    if(digitalRead(tilesPins[0])==LOW){
      strip.setPixelColor(numPixels+i,255,0,0);
      if(!stopFunction)
        stopFunction = true;
    } else{
      strip.setPixelColor(numPixels+i,0,255,0);
    }
  }
  strip.show();
  
  if(stopFunction)
    return;
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
}
//------------------------------------------------------------------------------------------------------------------------------------||
void displayHighScore(){
  if(!bDisplayHighScore)
    return;
  // High score
  float ratio = highScore / (float)gameTime;
  int highScorePixel = ratio * numPixels;
  strip.setPixelColor(highScorePixel, highScoreColor);

  strip.show();
}
//------------------------------------------------------------------------------------------------------------------------------------||
void displayGameTime(){
  for(int i=0; i<numPixels; i++)
    strip.setPixelColor(i,0,0,0);
    
  if(!bDisplayGameTime)
    return;
  // Current time
  unsigned long timePlayed = (millis() - startTime);
  float ratio = timePlayed / (float)gameTime; 
  int gameTimePixel = ratio * numPixels;
  for(int i=0; i<gameTimePixel; i++)
    strip.setPixelColor(i, gameTimeColor);
  
  strip.show();
}
//------------------------------------------------------------------------------------------------------------------------------------||
void displayGameOver(){
  if(!bDisplayGameOver)
    return;
  if(millis()>displayGameOverEndTime){
    bDisplayGameOver = false;
    bDisplayHighScore = true;
  }

  if(osc.process()==0){
    for(int i=0; i<numPixels; i++)
      strip.setPixelColor(i, gameOverColor);
  } else{
    for(int i=0; i<numPixels; i++)
      strip.setPixelColor(i, 0,0,0);
  }
  
  strip.show();
}
//------------------------------------------------------------------------------------------------------------------------------------||
void resetAllPixels(){
  for(int i=0; i<numPixels+kNumTiles; i++)
    strip.setPixelColor(i,0,0,0);
}
