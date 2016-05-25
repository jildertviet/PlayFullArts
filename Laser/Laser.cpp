/*
 Laser.cpp -
 Created by Jildert Viet, 13-05-2015.
 Released into the public domain.
 */

#include "Arduino.h"
#include "Laser.h"

Laser::Laser(){
    
}

Laser::Laser(int id, int ldrPin, int laserPin){
    this->id = id;
    this->ldrPin = ldrPin;
    this->laserPin = laserPin;
    pinMode(ldrPin, INPUT);
    pinMode(laserPin, OUTPUT);
}

bool Laser::checkIfObstructed(){
    if(state){ // If state == 1 (On)
        int value = analogRead(ldrPin);

        if(millis() > timeToWait){ // na x sec wordt de laser een gewone KILLING laser
            justTurnedOn = false;
        }
        
        if(justTurnedOn){
            if(value < threshold){
                // SET BOOL !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! die als ie changed de sendMSg doet
                if(!obstructedDuringTimerPhase){
                    obstructedDuringTimerPhase = true;
                    makeWarningNoise();
                }
            } else{
                if(obstructedDuringTimerPhase){
                    obstructedDuringTimerPhase = false;
                    muteWarningNoise();
                }
                
            }
        } else{ // If laser is running normally
            if(value < threshold){
                // JE BENT AF!
                gameOver();
                return true;
            }
        }
    // Read pin, return true is obstructed, false if not
    } else{
        // laser is off, dus geen check nodig
    }
    return false;
}

void Laser::turnOn(){
    state = 1;
    digitalWrite(laserPin, state);
    justTurnedOn = true;
    timeToWait = millis() + warningTime;
    sendMsg(1);
}

void Laser::turnOff(){
    state = 0;
    digitalWrite(laserPin, state);
    justTurnedOn = false;
    sendMsg(2);
}

int Laser::getLDRval(){
    return analogRead(ldrPin);
}

void Laser::setState(int stateTemp){
    if(stateTemp==1){
        turnOn();
        return;
    }
    if(stateTemp==0){
        turnOff();
        return;
    }
}

void Laser::gameOver(){
    turnOff();
    sendMsg(3);
    sendMsg(millis());
}

void Laser::makeWarningNoise(){
    sendMsg(4);
    sendMsg(millis());
    sendMsg(timeToWait);
}

void Laser::muteWarningNoise(){
    sendMsg(5);
}

void Laser::process(){
    if(state){
        // Shine
        digitalWrite(laserPin, state);
    }
}

void Laser::sendValue(unsigned int value){
    Serial.print(value);
    Serial.write(32);
}

void Laser::sendMsg(unsigned int mode){
    sendValue(id);
    sendValue(mode);
    Serial.println();
    Serial.flush();
}