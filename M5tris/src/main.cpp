#include <Arduino.h>
#include <M5StickCPlus.h>

enum gameMode { Resized, Classic}; //game mode stuff
gameMode mode = Resized;

//declar global
enum tetrominType {Brick, L, ReverseL, S, ReverseS, I};
const int tetronimSize = 4;

class block
{
public:
  int color;
  block(int Color){
    color = Color;
  }
};

class tetrominPre{ //tetromin is a one 4 block pice falling from top of grid
  public:
  block* tetrominArea[tetronimSize][tetronimSize]; //this area will be able to handle cube and even long stick
  int axisX;
  int axisY;

  tetrominPre(int Color, tetrominType type, int XStart, int YStart){
    axisX = XStart;
    axisY = YStart;

    switch (type) //creates tetronim shape
    {
    case Brick:
      tetrominArea[0][0] = NULL; tetrominArea[1][0] = NULL; tetrominArea[2][0] = NULL; tetrominArea[3][0] = NULL;
      tetrominArea[0][1] = NULL; tetrominArea[1][1] = new block(Color); tetrominArea[2][1] = new block(Color); tetrominArea[3][1] = NULL;
      tetrominArea[0][2] = NULL; tetrominArea[1][2] = new block(Color); tetrominArea[2][2] = new block(Color); tetrominArea[3][2] = NULL;
      tetrominArea[0][0] = NULL; tetrominArea[1][0] = NULL; tetrominArea[2][0] = NULL; tetrominArea[3][0] = NULL;      break;

    case L:
      tetrominArea[0][0] = NULL; tetrominArea[1][0] = NULL; tetrominArea[2][0] = NULL; tetrominArea[3][0] = NULL;
      tetrominArea[0][1] = NULL; tetrominArea[1][1] = new block(Color); tetrominArea[2][1] = NULL; tetrominArea[3][1] = NULL;
      tetrominArea[0][2] = NULL; tetrominArea[1][2] = new block(Color); tetrominArea[2][2] = NULL; tetrominArea[3][2] = NULL;
      tetrominArea[0][3] = NULL; tetrominArea[1][3] = new block(Color); tetrominArea[2][3] = new block(Color); tetrominArea[3][3] = NULL;
      break;

    case ReverseL:
      tetrominArea[0][0] = NULL; tetrominArea[1][0] = NULL; tetrominArea[2][0] = NULL; tetrominArea[3][0] = NULL;
      tetrominArea[0][1] = NULL; tetrominArea[1][1] = NULL; tetrominArea[2][1] = new block(Color); tetrominArea[3][1] = NULL;
      tetrominArea[0][2] = NULL; tetrominArea[1][2] = NULL; tetrominArea[2][2] = new block(Color); tetrominArea[3][2] = NULL;
      tetrominArea[0][3] = NULL; tetrominArea[1][3] = new block(Color); tetrominArea[2][3] = new block(Color); tetrominArea[3][3] = NULL;
      break;

    case S:
      tetrominArea[0][0] = NULL; tetrominArea[1][0] = NULL; tetrominArea[2][0] = new block(Color); tetrominArea[3][0] = new block(Color);
      tetrominArea[0][1] = NULL; tetrominArea[1][1] = NULL; tetrominArea[2][1] = new block(Color); tetrominArea[3][1] = NULL;
      tetrominArea[0][2] = NULL; tetrominArea[1][2] = NULL; tetrominArea[2][2] = new block(Color); tetrominArea[3][2] = NULL;
      tetrominArea[0][3] = NULL; tetrominArea[1][3] = new block(Color); tetrominArea[2][3] = new block(Color); tetrominArea[3][3] = NULL;
      break;

    case ReverseS:
      tetrominArea[0][0] = new block(Color); tetrominArea[1][0] = new block(Color); tetrominArea[2][0] = NULL; tetrominArea[3][0] = NULL;
      tetrominArea[0][1] = NULL; tetrominArea[1][1] = new block(Color); tetrominArea[2][1] = NULL; tetrominArea[3][1] = NULL;
      tetrominArea[0][2] = NULL; tetrominArea[1][2] = new block(Color); tetrominArea[2][2] = NULL; tetrominArea[3][2] = NULL;
      tetrominArea[0][3] = NULL; tetrominArea[1][3] = new block(Color); tetrominArea[2][3] = new block(Color); tetrominArea[3][3] = NULL;
      break;

    case I:
      tetrominArea[0][0] = NULL; tetrominArea[1][0] = NULL; tetrominArea[2][0] = new block(Color); tetrominArea[3][0] = NULL;
      tetrominArea[0][1] = NULL; tetrominArea[1][1] = NULL; tetrominArea[2][1] = new block(Color); tetrominArea[3][1] = NULL;
      tetrominArea[0][2] = NULL; tetrominArea[1][2] = NULL; tetrominArea[2][2] = new block(Color); tetrominArea[3][2] = NULL;
      tetrominArea[0][3] = NULL; tetrominArea[1][3] = NULL; tetrominArea[2][3] = new block(Color); tetrominArea[3][3] = NULL;
      break;
    }
  }
};

const int backgroundCollor = BLACK;
const int filledColor = WHITE;
const int placedColor = GREEN;
const int tetrominColor = BLUE;

const int startSpeed = 200;
int score = 0;
int speed = 0; //move down every 200 miliseconds, value will be lower over time
bool isGameOver = false;
const int pointsForFilledLine = 10;

tetrominPre* tetromin = NULL;

//declar resized
const int resizedGridWidht = 9;
const int resizedGridHight = 15;
const int resizedBlockSize = 15;
const int resizedTetronimStartLine = 0;
const int resizedTetronimStartCol = 2;


block* mapArrayResized[resizedGridWidht][resizedGridHight];

//declar classic
const int classicGridWidht = 10;
const int classicGridHight = 40;
const int classicBlockSize = 6;

block* mapArrayClassic[classicGridWidht][classicGridHight];



void DisplayGameModeOprion(){
  M5.Lcd.fillScreen(BLACK);

  //write text and stuff

}
////////////////////////////////////// SETUP //////////////////////////////////////
void setup() {
  M5.begin();
  M5.Lcd.begin();
  M5.Lcd.setTextSize(3);
  Serial.begin(38400);
  delay(100);
  Serial.println("setup done");
/*                          //game v1.0 will be only playable on Resized game mode, Classic mode will be done in v2.0
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
  }*/
}

/*
Board spec.

Display           135x240
--------------------------------
Game spec.

Used grid for Resized 9x15 (block size 15 | top will be used for score) 
Grid used for Classic 10x40 (block size 6)
Speed will increse over time (by increesing score)
*/








//functions

void Start(){
  //score restart
  score = 0;
  //speed restart
  speed = startSpeed;
  //isGameOver
  isGameOver = false;

  //tetromin
  Serial.println("Start - Tetromin");
  if(tetromin != NULL){
    delete(tetromin);
  }
  int randomTetrominType = random(0, 5); //0 - Brick, 1 - L, 2 - ReverseL, 3 - S, 4 - ReverseS, 5 - I
  switch (randomTetrominType)
  {
  case 0:
    tetromin = new tetrominPre(tetrominColor, Brick, resizedTetronimStartCol, resizedTetronimStartLine - 1);
    Serial.println("Start - Tetromin - brick made");
    break;

  case 1:
    tetromin = new tetrominPre(tetrominColor, Brick, resizedTetronimStartCol, resizedTetronimStartLine - 1);
    Serial.println("Start - Tetromin - L made");
    break;

  case 2:
    tetromin = new tetrominPre(tetrominColor, Brick, resizedTetronimStartCol, resizedTetronimStartLine - 1);
    Serial.println("Start - Tetromin - ReverseL made");
    break;

  case 3:
    tetromin = new tetrominPre(tetrominColor, Brick, resizedTetronimStartCol, resizedTetronimStartLine);
    Serial.println("Start - Tetromin - S made");
    break;

  case 4:
    tetromin = new tetrominPre(tetrominColor, Brick, resizedTetronimStartCol, resizedTetronimStartLine);
    Serial.println("Start - Tetromin - ReverseS made");
    break;

  case 5:
    tetromin = new tetrominPre(tetrominColor, Brick, resizedTetronimStartCol, resizedTetronimStartLine);
    Serial.println("Start - Tetromin - I made");
    break;
  }

  switch (mode) //by mode
  {
  case Classic:
    for (int row = classicGridWidht - 1; row >= 0; row--)
    {
      for (int col = classicGridHight - 1; col >= 0; row--)
      {
        if (mapArrayClassic[row][col] != NULL){
          delete(mapArrayClassic[row][col]);
          mapArrayClassic[row][col] = NULL;
        }
      }
    }
    break;

  case Resized:
    Serial.println("Start - resized - map");
    for (int line = 0; line <= resizedGridHight - 1; line++)
    {
      for (int col = 0; col < resizedGridWidht - 1; col++)
      {
        Serial.println("row col");
        Serial.println(line);
        Serial.println(col);
        if (mapArrayResized[col][line] != NULL){
          delete(mapArrayResized[col][line]);
        }
        mapArrayResized[col][line] = NULL;
      }
    }
    Serial.println("Start - resized - done");
    break;
  }
}

void DisplayPlaced(){ //displays allready placed blocks
  M5.Lcd.fillScreen(backgroundCollor);
  switch (mode)
  {
  case Resized:
    for(int line = resizedGridHight - 1; line >= 0; line--){
      for (int col = resizedGridWidht - 1; col >= 0; col--){
        if(mapArrayResized[col][line] != NULL ){
          M5.Lcd.fillRect(line * resizedBlockSize, col * resizedBlockSize, resizedBlockSize, resizedBlockSize, mapArrayResized[col][line]->color);
        }
      }
    }
    break;
  }
}

void CheckAndProcessFilled(){ //check filled lines and clears them
  bool isFilled = false;
  int consecutivelyFilled = 0;

  switch (mode)
  {
  case Resized:
    for(int line = 0; line <= resizedGridHight - 1; line++){
      for (int col = 0; col <= resizedGridWidht - 1; col++){
        isFilled = true;
        if(mapArrayResized[col][line] == NULL ){
          isFilled = false;
          break;
        }
      }
      if (isFilled){ //clears line and moves blocks down
        consecutivelyFilled++;

        //deletes filled and moves everithing what is higher down, displays blocks whit filledColor on screen
        for (int col = 0; col <= resizedGridWidht - 1; col++){ //delete and fill  whitfilledCollor
          M5.Lcd.fillRect(col, line, resizedBlockSize, resizedBlockSize, filledColor);
          delete(mapArrayResized[col][line]);
        }

        for (int opline = resizedGridHight - (resizedGridHight - 1 - line); opline <= resizedGridHight - 1; opline++) //moves higher blocks down
        {
          for (int opcol = 0; opcol <= resizedGridWidht - 1; opcol++){
            mapArrayResized[opcol][opline - 1] = mapArrayResized[opcol][opline]; //moves blocks down

            if(mapArrayResized[opcol][opline] != NULL){
              mapArrayResized[opcol][opline] = NULL;
            }
          }
        }
        
      }
      else{
        score += consecutivelyFilled * pointsForFilledLine;
        consecutivelyFilled = 0;
      }
    }
    break;
  }
  delay(100);
}

void CheckGameOver(){ //checs if first line contains placed block
  switch (mode)
  {
  case Resized:
    for(int col = 0; col <= resizedGridWidht - 1; col++){
      if (mapArrayResized[col][0] != NULL)
      {
        isGameOver = true;
        break;
      }
    }
    break;
  }
}

void PlaceTetromin(){
  bool canPlace = false;
  switch (mode)
  {
  case Resized:
    for (int line = 0; line <= tetronimSize - 1; line++) //check if blocks are under tetromin
    {
      for (int col = 0; col <= tetronimSize; col++)
      {
        if (tetromin->tetrominArea[line][col] != NULL){
          if(tetromin->axisY + 1 == resizedGridHight  || mapArrayResized[tetromin->axisX + col][tetromin->axisY + line + 1] != NULL){
            canPlace = true;
          } 
        }
      }
    }

    if(canPlace){
      for (int line = 0; line <= tetronimSize - 1; line++) //place tetromin
      {
        for (int col = 0; col <= tetronimSize; col++)
        {
          if (tetromin->tetrominArea[line][col] != NULL){
            mapArrayResized[tetromin->axisX + col][tetromin->axisY + line] = new block(placedColor);
          }
        }
      }
    }

    delete(tetromin); //make new tetromin
    int randomTetrominType = random(0, 5); //0 - Brick, 1 - L, 2 - ReverseL, 3 - S, 4 - ReverseS, 5 - I
    switch (randomTetrominType)
    {
    case 0:
      tetromin = new tetrominPre(tetrominColor, Brick, resizedTetronimStartCol, resizedTetronimStartLine - 1);
      break;

    case 1:
      tetromin = new tetrominPre(tetrominColor, Brick, resizedTetronimStartCol, resizedTetronimStartLine - 1);
      break;

    case 2:
      tetromin = new tetrominPre(tetrominColor, Brick, resizedTetronimStartCol, resizedTetronimStartLine - 1);
      break;

    case 3:
      tetromin = new tetrominPre(tetrominColor, Brick, resizedTetronimStartCol, resizedTetronimStartLine);
      break;

    case 4:
      tetromin = new tetrominPre(tetrominColor, Brick, resizedTetronimStartCol, resizedTetronimStartLine);
      break;

    case 5:
      tetromin = new tetrominPre(tetrominColor, Brick, resizedTetronimStartCol, resizedTetronimStartLine);
      break;
    }

    break;
  }
}

void TurnLeft(){
  tetrominPre* tetrominCpy = tetromin;
  bool canPlace = true;
  tetrominCpy->tetrominArea[0][0] = tetromin->tetrominArea[3][0];    tetrominCpy->tetrominArea[1][0] = tetromin->tetrominArea[3][1];    tetrominCpy->tetrominArea[2][0] = tetromin->tetrominArea[3][2];    tetrominCpy->tetrominArea[3][0] = tetromin->tetrominArea[3][3];
  tetrominCpy->tetrominArea[0][1] = tetromin->tetrominArea[2][0];    tetrominCpy->tetrominArea[1][1] = tetromin->tetrominArea[2][1];    tetrominCpy->tetrominArea[2][1] = tetromin->tetrominArea[2][2];    tetrominCpy->tetrominArea[3][1] = tetromin->tetrominArea[2][3];
  tetrominCpy->tetrominArea[0][2] = tetromin->tetrominArea[1][0];    tetrominCpy->tetrominArea[1][2] = tetromin->tetrominArea[1][1];    tetrominCpy->tetrominArea[2][2] = tetromin->tetrominArea[1][2];    tetrominCpy->tetrominArea[3][2] = tetromin->tetrominArea[1][3];
  tetrominCpy->tetrominArea[0][3] = tetromin->tetrominArea[0][0];    tetrominCpy->tetrominArea[1][3] = tetromin->tetrominArea[0][1];    tetrominCpy->tetrominArea[2][3] = tetromin->tetrominArea[0][2];    tetrominCpy->tetrominArea[3][3] = tetromin->tetrominArea[0][3];

  for (int col = 0; col >= tetronimSize - 1; col++){
    for (int line = 0; line >= tetronimSize - 1; line++){
      if(tetrominCpy->tetrominArea[col][line] != NULL){
        if(tetrominCpy->axisX + col < 0 || tetrominCpy->axisX + col >= resizedGridWidht || mapArrayResized[tetrominCpy->axisX+col][tetrominCpy->axisY+line] != NULL){
          canPlace = false;
          break;
        }
      }
    }
  }
  if (canPlace) {
    tetromin = tetrominCpy;
    delete(tetrominCpy);
  }
  else{
    delete(tetrominCpy);
  }
}

void MoveLeft(){
  bool canPlace = true;
  for (int col = 0; col >= tetronimSize - 1; col++){
    for (int line = 0; line >= tetronimSize - 1; line++){
      if(tetromin->tetrominArea[col][line] != NULL){
        if(tetromin->axisX + col < 0  || mapArrayResized[tetromin->axisX+col-1][tetromin->axisY+line] != NULL){
          canPlace = false;
          break;
        }
      }
    }
  }
  
  if (canPlace)
  {
    tetromin->axisX--;
  }
}

void TurnRight(){
  tetrominPre* tetrominCpy = tetromin;
  bool canPlace = true;
  tetrominCpy->tetrominArea[0][0] = tetromin->tetrominArea[0][3];    tetrominCpy->tetrominArea[1][0] = tetromin->tetrominArea[0][2];    tetrominCpy->tetrominArea[2][0] = tetromin->tetrominArea[0][1];    tetrominCpy->tetrominArea[3][0] = tetromin->tetrominArea[0][0];
  tetrominCpy->tetrominArea[0][1] = tetromin->tetrominArea[1][3];    tetrominCpy->tetrominArea[1][1] = tetromin->tetrominArea[1][2];    tetrominCpy->tetrominArea[2][1] = tetromin->tetrominArea[1][1];    tetrominCpy->tetrominArea[3][1] = tetromin->tetrominArea[1][0];
  tetrominCpy->tetrominArea[0][2] = tetromin->tetrominArea[2][3];    tetrominCpy->tetrominArea[1][2] = tetromin->tetrominArea[2][2];    tetrominCpy->tetrominArea[2][2] = tetromin->tetrominArea[2][1];    tetrominCpy->tetrominArea[3][2] = tetromin->tetrominArea[2][0];
  tetrominCpy->tetrominArea[0][3] = tetromin->tetrominArea[3][3];    tetrominCpy->tetrominArea[1][3] = tetromin->tetrominArea[3][2];    tetrominCpy->tetrominArea[2][3] = tetromin->tetrominArea[3][1];    tetrominCpy->tetrominArea[3][3] = tetromin->tetrominArea[3][0];

  for (int col = 0; col >= tetronimSize - 1; col++){
    for (int line = 0; line >= tetronimSize - 1; line++){
      if(tetrominCpy->tetrominArea[col][line] != NULL){
        if(tetrominCpy->axisX + col < 0 || tetrominCpy->axisX + col >= resizedGridWidht || mapArrayResized[tetrominCpy->axisX+col][tetrominCpy->axisY+line] != NULL){
          canPlace = false;
          break;
        }
      }
    }
  }
  if (canPlace) {
    tetromin = tetrominCpy;
    delete(tetrominCpy);
  }
  else{
    delete(tetrominCpy);
  }
}

void MoveRight(){
    bool canPlace = true;
  for (int col = 0; col >= tetronimSize - 1; col++){
    for (int line = 0; line >= tetronimSize - 1; line++){
      if(tetromin->tetrominArea[col][line] != NULL){
        if(tetromin->axisX + col >= resizedGridWidht || mapArrayResized[tetromin->axisX+col][tetromin->axisY+line] != NULL){
          canPlace = false;
          break;
        }
      }
    }
  }
  if (canPlace) {
    tetromin->axisX++;
  }
}

void MoveDown(){
  bool canMove = true;

  switch (mode)
  {
  case Resized:
    for (int line = 0; line <= tetronimSize - 1; line++)
    {
      for(int col = 0; col <= tetronimSize - 1; col++){
        if(tetromin->axisY + line + 1 < resizedGridHight || tetromin->tetrominArea[col][line] != NULL && mapArrayResized[col][line+1] != NULL){
          canMove = false;
        }
      }
    }
    break;
  }
  
  if(canMove){
    tetromin->axisY++;
  }
}

void DisplayTetromin(){
  for (int line = 0; line <= tetronimSize - 1; line++)
  {
    for(int col = 0; col <= tetronimSize - 1; col++){
      if(tetromin->tetrominArea[col][line] != NULL){
        M5.Lcd.fillRect(tetromin->axisX + col, tetromin->axisY + line , resizedBlockSize, resizedBlockSize, tetrominColor);
      }
    }
  }
}

void ClearTetromin(){
  for (int line = 0; line <= tetronimSize - 1; line++)
  {
    for(int col = 0; col <= tetronimSize - 1; col++){
      if(tetromin->tetrominArea[col][line] != NULL){
        M5.Lcd.fillRect(tetromin->axisX + col, tetromin->axisY + line, resizedBlockSize, resizedBlockSize, backgroundCollor);
      }
    }
  }
}

void loop() {
  Start();
  Serial.println("loop - Start done");
  while (!isGameOver)
  {
    MoveDown();
    Serial.println("loop - move down done");

    PlaceTetromin();
    Serial.println("loop - place done");

    CheckAndProcessFilled();
    Serial.println("loop - check filled");

    DisplayPlaced();
    Serial.println("loop - display done");

    DisplayTetromin();
    Serial.println("loop - display tetromin done");

    for (int cicle = speed; cicle >= 1; cicle--)
    {
      M5.BtnA.read();
      M5.BtnB.read();
      while (M5.BtnA.read() == 1 && M5.BtnA.pressedFor(100) == 1 || M5.BtnB.read() == 1 && M5.BtnB.pressedFor(100) != 1){}

      if(M5.BtnA.pressedFor(100)){
        ClearTetromin();
        TurnLeft();
        DisplayTetromin();
      }
      else if (M5.BtnA.read() == 1)
      {
        ClearTetromin();
        MoveLeft();
        DisplayTetromin();
      }
      
      if(M5.BtnB.pressedFor(100)){
        ClearTetromin();
        TurnRight();
        DisplayTetromin();
      }
      else if(M5.BtnB.read() == 1){
        ClearTetromin();
        MoveRight();
        DisplayTetromin();
      }
      
      delay(1);
    }

    CheckGameOver();
    Serial.println("loop - CheckGameOver done");
  }
  //game over
  Serial.println("loop - GameOver");
}