#include <Arduino.h>
#include <M5StickCPlus.h>

enum gameMode { Resized, Classic}; //game mode stuff
gameMode mode = Resized;

void DisplayGameModeOprion(){
  M5.Lcd.fillScreen(BLACK);

  //write text and stuff

}

void setup() {
  M5.begin();
  M5.Lcd.begin();
  M5.Lcd.textsize(3);
  Serial.begin(38400);

  while (true)
  {
    if(M5.BtnB.read() == 1){
      switch (mode)
      {
      case Resized:
        mode = Classic;
        break;
      
      case Classic:
        mode = Resized;
        break;
      }
      delay(100);
    }
    if (M5.BtnA.read() == 1){
      break;
    }
  }
}

/*
Board spec.

Display           135x240
Avaiable grid     9x16
--------------------------------
Game spec.

Used grid 9x15 (top will be used for score)   in original game it was 10x40  6x6
Speed will increse over time (by increesing score)
*/

//game settings
//classic
const int classicGridWidht = 10;
const int classicGridHight = 40;
const int classicBlockSize = 6;
//resized
const int resizedGridWidht = 10;
const int resizedGridHight = 40;
const int resizedBlockSize = 6;

class block
{
public:
  int x;
  int y;
  int collor;

  block(int X, int Y, int Collor){
    x = X;
    y = Y;
    collor = Collor;
  }
};

block* mapArrayResized[9][15];
block* mapArrayClassic[10][40];


void Start(){
  switch (mode)
  {
  case Classic:
    //mapArrayClassic

    for

    break;
  
  case Resized:
    //mapArrayResized



    break;
  }
}


void loop() {
  Start();
}