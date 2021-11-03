#include <Arduino.h>
#include <M5StickCPlus.h>

////////// Declaration //////////

/*In this section, variables, and functions assigned to them, are declared.
  Functions in this section are only for simple modification (like deleting and NULLifying),
  more complex functions (like rotation or restart) are declared later in "Game functions" sector.*/

const int backgroundCollor = BLACK;
const int filledColor = WHITE;
const int otherColor = WHITE; //for things like score, lines, game over screen, etc.
const int placedColor = GREEN;

const int startSpeed = 750;
int score = 0;
bool isGameOver = false;
const int pointsForFilledLine = 10;

//-------- game mode --------//
enum gameMode { Classic, Resized };
gameMode selectedGameMode = Resized;

//-------- block and tetromin --------//
const int tetrominGridSize = 4; //used for 4*4 grid
const int tetrominColor = BLUE;

// tetromin type //
enum tetrominType {Brick, L, ReverseL, S, ReverseS, I, T};
tetrominType tetrominNext; //contains value of next tetromin
void TetrominDeclareNext(){ //declares next tetromin
    int tetId = random(1, 7); //1 = Brick, 2 = L, 3 = ReverseL, 4 = S, 5 = ReverseS, 6 = I
    switch (tetId)
    {
    case 1:
        tetrominNext = Brick;
        break;
    case 2:
        tetrominNext = L;
        break;
    case 3:
        tetrominNext = ReverseL;
        break;
    case 4:
        tetrominNext = S;
        break;
    case 5:
        tetrominNext = ReverseS;
        break;
    case 6:
        tetrominNext = I;
        break;
    case 7:
        tetrominNext = T;
        break;
    }
    
}

// block //
/*object contained in maps and masks (like tetrominArea in tetromin).
  It's used for easier understanding of how the game works.
  It contains only color var., variables like position are managed in block grids*/
class block{
public:
  int color;
  block(int Color){
    color = Color;
  }
};

// tetromin //
class tetromin{
public:
  block* tetrominArea[tetrominGridSize][tetrominGridSize]; //area used to store blocks
  /*tetrominArea can be described as some sort of mask, witch will overlap game map and help us find exact position of tetromin block.
    We need to change just var. of tetromin (axisY, axisY) to move whole grid.
    It also makes turning easier (we can make copy and put blocks on diferent position).*/
  int axisX = 0;
  int axisY = 0;
  tetromin(){ //NULLifyes whole grid, NULL is uded to identify empty cols
    for (int line = 0; line <= tetrominGridSize - 1; line++){
        for (int col = 0; col <= tetrominGridSize - 1; col++){
            tetrominArea[line][col] = NULL;
        }
    }
  }
  ~tetromin(){ //deletes tetrominArea
    for (int line = 0; line <= tetrominGridSize - 1; line++){
        for (int col = 0; col <= tetrominGridSize - 1; col++){
            delete tetrominArea[line][col];
        }
    }
  }
};

tetromin* tetrominPice; //pice o tetromin
void TetrominDelete(){ //deletes tetrominPice
    delete tetrominPice;
}
tetromin* tetrominCopy; //used for manipulation of tetrominPice like turning
void TetrominCopyMakeLeftRotated(){
    tetrominCopy = new tetromin();
    //copy position
    tetrominCopy->axisX = tetrominPice->axisX;
    tetrominCopy->axisY = tetrominPice->axisY;
    //make rotated grid copy
    tetrominCopy->tetrominArea[0][0] = tetrominPice->tetrominArea[0][3]; tetrominCopy->tetrominArea[0][1] = tetrominPice->tetrominArea[1][3]; tetrominCopy->tetrominArea[0][2] = tetrominPice->tetrominArea[2][3]; tetrominCopy->tetrominArea[0][3] = tetrominPice->tetrominArea[3][3];
    tetrominCopy->tetrominArea[1][0] = tetrominPice->tetrominArea[0][2]; tetrominCopy->tetrominArea[1][1] = tetrominPice->tetrominArea[1][2]; tetrominCopy->tetrominArea[1][2] = tetrominPice->tetrominArea[2][2]; tetrominCopy->tetrominArea[1][3] = tetrominPice->tetrominArea[3][2];
    tetrominCopy->tetrominArea[2][0] = tetrominPice->tetrominArea[0][1]; tetrominCopy->tetrominArea[2][1] = tetrominPice->tetrominArea[1][1]; tetrominCopy->tetrominArea[2][2] = tetrominPice->tetrominArea[2][1]; tetrominCopy->tetrominArea[2][3] = tetrominPice->tetrominArea[3][1];
    tetrominCopy->tetrominArea[3][0] = tetrominPice->tetrominArea[0][0]; tetrominCopy->tetrominArea[3][1] = tetrominPice->tetrominArea[1][0]; tetrominCopy->tetrominArea[3][2] = tetrominPice->tetrominArea[2][0]; tetrominCopy->tetrominArea[3][3] = tetrominPice->tetrominArea[3][0];
}
void tetrominCopyMakeRightRotated(){
        tetrominCopy = new tetromin();
    //copy position
    tetrominCopy->axisX = tetrominPice->axisX;
    tetrominCopy->axisY = tetrominPice->axisY;
    //make rotated grid copy
    tetrominCopy->tetrominArea[0][0] = tetrominPice->tetrominArea[3][0]; tetrominCopy->tetrominArea[0][1] = tetrominPice->tetrominArea[2][0]; tetrominCopy->tetrominArea[0][2] = tetrominPice->tetrominArea[1][0]; tetrominCopy->tetrominArea[0][3] = tetrominPice->tetrominArea[0][0];
    tetrominCopy->tetrominArea[1][0] = tetrominPice->tetrominArea[3][1]; tetrominCopy->tetrominArea[1][1] = tetrominPice->tetrominArea[2][1]; tetrominCopy->tetrominArea[1][2] = tetrominPice->tetrominArea[1][1]; tetrominCopy->tetrominArea[1][3] = tetrominPice->tetrominArea[0][1];
    tetrominCopy->tetrominArea[2][0] = tetrominPice->tetrominArea[3][2]; tetrominCopy->tetrominArea[2][1] = tetrominPice->tetrominArea[2][2]; tetrominCopy->tetrominArea[2][2] = tetrominPice->tetrominArea[1][2]; tetrominCopy->tetrominArea[2][3] = tetrominPice->tetrominArea[0][2];
    tetrominCopy->tetrominArea[3][0] = tetrominPice->tetrominArea[3][3]; tetrominCopy->tetrominArea[3][1] = tetrominPice->tetrominArea[2][3]; tetrominCopy->tetrominArea[3][2] = tetrominPice->tetrominArea[1][3]; tetrominCopy->tetrominArea[3][3] = tetrominPice->tetrominArea[0][3];
}
void TetrominCopyDelete(){ //deletes tetrominCopy
    delete tetrominCopy;
}

//-------- resized --------//
const int resizedGridWidht = 9; //9 widht
const int resizedGridHight = 15; //15 hight
const int resizedBlockSize = 15;
const int resizedTetronimStartLine = 0;
const int resizedTetronimStartCol = 2;
const int resizedNextBlockSize = 2; //var. for size of block of next tetromin bar

// resized map grid //
block* resizedMap[resizedGridHight][resizedGridWidht];
void ResizedClearMap(){  //deletes every item resizedMap
    for(int line = 0; line <= resizedGridHight - 1; line++){
        for (int col = 0; col <= resizedGridWidht - 1; col++)
        {
            delete resizedMap[line][col];
        }
    }
}
void ResizedNULLMap(){ //set every col in resizedMap to NULL. NULL is used to identify empty cols
    for(int line = 0; line <= resizedGridHight - 1; line++){
        for (int col = 0; col <= resizedGridWidht - 1; col++)
        {
            resizedMap[line][col] = NULL;
        }
    }
}

//-------- classic --------//
const int classicGridWidht = 10; //10 widht
const int classicGridHight = 24; // 24 hight
const int classicBlockSize = 10;
const int classicTetronimStartLine = 0;
const int classicTetronimStartCol = 3;
const int classicNextBlockSize = 2; //var. for size of block of next tetromin bar

// classic map grid //
block* classicMap[classicGridHight][classicGridWidht];
void ClassicClearMap(){ //deletes every item classicMap
    for(int line = 0; line <= classicGridHight - 1; line++){
        for (int col = 0; col <= classicGridWidht - 1; col++)
        {
            delete classicMap[line][col];
        }
    }
}
void ClassicNULLMap(){ //set every col in classicMap to NULL. NULL is used to identify empty cols
    for(int line = 0; line <= classicGridHight - 1; line++){
        for (int col = 0; col <= classicGridWidht - 1; col++)
        {
            classicMap[line][col] = NULL;
        }
    }
}



////////// Game functions /////////
/*Here are declared main game functions, variables are declared in "Declaration" section*/

//-------- display functions --------//
/*M5tris has two main position properties: 
  - First are map grids (classicMap, resizedMap).
    They are used as a map for the game, column are used as a position on map where blocks can be contained
  - Second is tetromin.
    It contains position of top-left corner column of it's tetrominArea, and used as a "mask"
    
  Display order is like this:
    1. display score
    2. display map
    3. display tetromin
  Things lower will overlap things higher.
  Display functions are depanding on selected game mode.
  
  Classic display - Map will be displayed on right side of screen (area 60x240) from position x = 76
                  - Score and next tetromin will be displayed on left side of screen (area 75x240)
                  - Block size will be 6x6
                  - Next tetromin will be displayed under score in score area
                  - Next tetromin block size is 2x2 

  Resized display - Score and next tetromin will be placed on top of screen (area 135x15)
                  - Map will be displayed under score (area 135x225) from position y = 16
                  - Block size will be 15x15
                  - Next tetromin will be placed on right side of score area
                  - Next tetromin block size is 2x2 
                  
  In this section displayed tetromin removal is also defined.*/

void DisplayScoreAndNextBlock(){ //displays curent score
    switch (selectedGameMode)
    {
    case Resized:
        // show score //
        M5.Lcd.fillRect(0,0,135,15,backgroundCollor); //clear area where score and next block is
        M5.Lcd.drawLine(0, 15, 135, 15, otherColor); //draw split line
        M5.Lcd.setCursor(4, 4); //score position on top-left side of the screen
        M5.Lcd.print(score); //write score

        // show next tetromin //
        //Brick, L, ReverseL, S, ReverseS, I, T
        //next tetromin bar will be 3-4px away from top, bottom and right side
        //function calling is writen in shape of the next tetromin for better understanding of the code
        switch (tetrominNext) 
        {
        case Brick: 
            M5.Lcd.fillRect(127, 6, resizedNextBlockSize, resizedNextBlockSize, otherColor); M5.Lcd.fillRect(129, 6, resizedNextBlockSize, resizedNextBlockSize, otherColor);
            M5.Lcd.fillRect(127, 8, resizedNextBlockSize, resizedNextBlockSize, otherColor); M5.Lcd.fillRect(129, 8, resizedNextBlockSize, resizedNextBlockSize, otherColor);
            break;
        case L: 
            M5.Lcd.fillRect(127, 6, resizedNextBlockSize, resizedNextBlockSize, otherColor); M5.Lcd.fillRect(129, 6, resizedNextBlockSize, resizedNextBlockSize, otherColor); M5.Lcd.fillRect(131, 6, resizedNextBlockSize, resizedNextBlockSize, otherColor);
            M5.Lcd.fillRect(127, 8, resizedNextBlockSize, resizedNextBlockSize, otherColor);
            break;
        case ReverseL: 
            M5.Lcd.fillRect(127, 6, resizedNextBlockSize, resizedNextBlockSize, otherColor); M5.Lcd.fillRect(129, 6, resizedNextBlockSize, resizedNextBlockSize, otherColor); M5.Lcd.fillRect(131, 6, resizedNextBlockSize, resizedNextBlockSize, otherColor);
                                                                                                                                                                              M5.Lcd.fillRect(131, 8, resizedNextBlockSize, resizedNextBlockSize, otherColor);
            break;
        case S:
                                                                                             M5.Lcd.fillRect(129, 6, resizedNextBlockSize, resizedNextBlockSize, otherColor); M5.Lcd.fillRect(131, 6, resizedNextBlockSize, resizedNextBlockSize, otherColor);
            M5.Lcd.fillRect(127, 8, resizedNextBlockSize, resizedNextBlockSize, otherColor); M5.Lcd.fillRect(129, 8, resizedNextBlockSize, resizedNextBlockSize, otherColor);
            break;
        case ReverseS:
            M5.Lcd.fillRect(127, 6, resizedNextBlockSize, resizedNextBlockSize, otherColor); M5.Lcd.fillRect(129, 6, resizedNextBlockSize, resizedNextBlockSize, otherColor);
                                                                                             M5.Lcd.fillRect(129, 8, resizedNextBlockSize, resizedNextBlockSize, otherColor); M5.Lcd.fillRect(131, 8, resizedNextBlockSize, resizedNextBlockSize, otherColor);
            break;
        case I:
            M5.Lcd.fillRect(125, 6, resizedNextBlockSize, resizedNextBlockSize, otherColor); M5.Lcd.fillRect(127, 6, resizedNextBlockSize, resizedNextBlockSize, otherColor); M5.Lcd.fillRect(129, 6, resizedNextBlockSize, resizedNextBlockSize, otherColor); M5.Lcd.fillRect(131, 6, resizedNextBlockSize, resizedNextBlockSize, otherColor);
            break;
        case T:
                                                                                             M5.Lcd.fillRect(129, 6, resizedNextBlockSize, resizedNextBlockSize, otherColor);
            M5.Lcd.fillRect(127, 8, resizedNextBlockSize, resizedNextBlockSize, otherColor); M5.Lcd.fillRect(129, 8, resizedNextBlockSize, resizedNextBlockSize, otherColor); M5.Lcd.fillRect(131, 8, resizedNextBlockSize, resizedNextBlockSize, otherColor);
            break;
        }

        break;
    case Classic:
        // show score //
        M5.Lcd.fillRect(0,0,35,240,backgroundCollor); //clear area where score and next block is
        M5.Lcd.drawLine(35, 0, 35, 240, otherColor); //draw split line
        M5.Lcd.setCursor(4, 4); //score position on top-left side of the screen
        M5.Lcd.print(score); //write score
        
        // show next tetromin //
        //Brick, L, ReverseL, S, ReverseS, I, T
        //next tetromin bar will be 3-4px away from left side and from bottom of score
        //function calling is writen in shape of the next tetromin for better understanding of the code
        switch (tetrominNext) 
        {
        case Brick: 
            M5.Lcd.fillRect(6, 13, classicNextBlockSize, classicNextBlockSize, otherColor); M5.Lcd.fillRect(8, 13, classicNextBlockSize, classicNextBlockSize, otherColor);
            M5.Lcd.fillRect(6, 15, classicNextBlockSize, classicNextBlockSize, otherColor); M5.Lcd.fillRect(8, 15, classicNextBlockSize, classicNextBlockSize, otherColor);
            break;
        case L: 
            M5.Lcd.fillRect(6, 13, classicNextBlockSize, classicNextBlockSize, otherColor); M5.Lcd.fillRect(8, 13, classicNextBlockSize, classicNextBlockSize, otherColor); M5.Lcd.fillRect(10, 13, classicNextBlockSize, classicNextBlockSize, otherColor);
            M5.Lcd.fillRect(6, 15, classicNextBlockSize, classicNextBlockSize, otherColor);
            break;
        case ReverseL: 
            M5.Lcd.fillRect(6, 13, classicNextBlockSize, classicNextBlockSize, otherColor); M5.Lcd.fillRect(8, 13, classicNextBlockSize, classicNextBlockSize, otherColor); M5.Lcd.fillRect(10, 13, classicNextBlockSize, classicNextBlockSize, otherColor);
                                                                                                                                                                            M5.Lcd.fillRect(10, 15, classicNextBlockSize, classicNextBlockSize, otherColor);
            break;
        case S:
                                                                                            M5.Lcd.fillRect(8, 13, classicNextBlockSize, classicNextBlockSize, otherColor); M5.Lcd.fillRect(10, 13, classicNextBlockSize, classicNextBlockSize, otherColor);
            M5.Lcd.fillRect(6, 15, classicNextBlockSize, classicNextBlockSize, otherColor); M5.Lcd.fillRect(8, 15, classicNextBlockSize, classicNextBlockSize, otherColor);
            break;
        case ReverseS:
            M5.Lcd.fillRect(6, 13, classicNextBlockSize, classicNextBlockSize, otherColor); M5.Lcd.fillRect(8, 13, classicNextBlockSize, classicNextBlockSize, otherColor);
                                                                                            M5.Lcd.fillRect(8, 15, classicNextBlockSize, classicNextBlockSize, otherColor); M5.Lcd.fillRect(10, 15, classicNextBlockSize, classicNextBlockSize, otherColor);
            break;
        case I:
            M5.Lcd.fillRect(4, 13, classicNextBlockSize, classicNextBlockSize, otherColor); M5.Lcd.fillRect(6, 13, classicNextBlockSize, classicNextBlockSize, otherColor); M5.Lcd.fillRect(8, 13, classicNextBlockSize, classicNextBlockSize, otherColor); M5.Lcd.fillRect(10, 13, classicNextBlockSize, classicNextBlockSize, otherColor);
            break;
        case T:
                                                                                            M5.Lcd.fillRect(8, 13, resizedNextBlockSize, resizedNextBlockSize, otherColor);
            M5.Lcd.fillRect(6, 15, resizedNextBlockSize, resizedNextBlockSize, otherColor); M5.Lcd.fillRect(8, 15, resizedNextBlockSize, resizedNextBlockSize, otherColor); M5.Lcd.fillRect(10, 15, resizedNextBlockSize, resizedNextBlockSize, otherColor);
            break;
        }
        break;
    }
}
void DisplayScoreAndNextBlockClear(){
    switch (selectedGameMode)
    {
    case Classic:
        M5.Lcd.fillRect(0,0,35,240,backgroundCollor);
        break;
    
    case Resized:
        M5.Lcd.fillRect(0,0,135,15,backgroundCollor);
        break;
    }
}
void DisplayMap(){ //displays maps
    switch (selectedGameMode)
    {
    case Resized:
        for(int line = 0; line <= resizedGridHight - 1; line++){
            for(int col = 0; col <= resizedGridWidht - 1; col++){
                if(resizedMap[line][col] != NULL){
                    M5.Lcd.fillRect(col*resizedBlockSize, 16+(line*resizedBlockSize), resizedBlockSize, resizedBlockSize, placedColor);
                }
            }
        }
        break;
    case Classic:
        for(int line = 0; line <= classicGridHight - 1; line++){
            for(int col = 0; col <= classicGridWidht - 1; col++){
                if(classicMap[line][col] != NULL){
                    M5.Lcd.fillRect(36+(col*classicBlockSize), line*classicBlockSize, classicBlockSize, classicBlockSize, placedColor);
                }
            }
        }
        break;
    }
}
void DisplayMapClear(){
    switch (selectedGameMode)
    {
    case Resized:
        M5.Lcd.fillRect(0,16,135,225,backgroundCollor);
        break;
    
    case Classic:
        M5.Lcd.fillRect(36,0,100,240,backgroundCollor);
        break;
    }
}
void DisplayTetromin(){ //displays tetrominPice
    switch (selectedGameMode)
    {
    case Resized:
        for(int line = 0; line <= tetrominGridSize - 1; line++){
            for(int col = 0; col <= tetrominGridSize - 1; col++){
                if (tetrominPice->tetrominArea[line][col] != NULL)
                {
                    M5.Lcd.fillRect(resizedBlockSize*(tetrominPice->axisX+col), 16 + (resizedBlockSize*(tetrominPice->axisY+line)), resizedBlockSize, resizedBlockSize, tetrominPice->tetrominArea[line][col]->color);
                }
            }
        }
        break;
    case Classic:
        for(int line = 0; line <= tetrominGridSize - 1; line++){
            for(int col = 0; col <= tetrominGridSize - 1; col++){
                if (tetrominPice->tetrominArea[line][col] != NULL)
                {
                    M5.Lcd.fillRect(36+(classicBlockSize*(tetrominPice->axisX+col)), classicBlockSize*(tetrominPice->axisY+line), classicBlockSize, classicBlockSize, tetrominPice->tetrominArea[line][col]->color);
                }
            }
        }
        break;
    }
}

// tetromin removal //
void DisplayTetrominRemove(){
    switch (selectedGameMode)
    {
    case Resized:
        for(int line = 0; line <= tetrominGridSize - 1; line++){
            for(int col = 0; col <= tetrominGridSize - 1; col++){
                if (tetrominPice->tetrominArea[line][col] != NULL)
                {
                    M5.Lcd.fillRect(resizedBlockSize*(tetrominPice->axisX+col), 16 + (resizedBlockSize*(tetrominPice->axisY+line)), resizedBlockSize, resizedBlockSize, backgroundCollor);
                }
            }
        }
        break;
    case Classic:
        for(int line = 0; line <= tetrominGridSize - 1; line++){
            for(int col = 0; col <= tetrominGridSize - 1; col++){
                if (tetrominPice->tetrominArea[line][col] != NULL)
                {
                    M5.Lcd.fillRect(36+(classicBlockSize*(tetrominPice->axisX+col)), classicBlockSize*(tetrominPice->axisY+line), classicBlockSize, classicBlockSize, backgroundCollor);
                }
            }
        }
        break;
    }
}

//-------- create tetromin --------//
/*here tetromins are created. 
  Each chape has it's own set function and one finction manages complete creation of tetromin.
  one or more tetromin blocks will be createt on top of tetrominArea grid.
  Block creating calls are placed in shape of created tetromin*/

// set shape //
//Brick, L, ReverseL, S, ReverseS, I, T
//these functions will create blocks to allready existing tetrominPeace
void TetrominSetBrick(){
    tetrominPice->tetrominArea[0][1] = new block(tetrominColor); tetrominPice->tetrominArea[0][2] = new block(tetrominColor);
    tetrominPice->tetrominArea[1][1] = new block(tetrominColor); tetrominPice->tetrominArea[1][2] = new block(tetrominColor);
}
void TetrominSetL(){
    tetrominPice->tetrominArea[0][1] = new block(tetrominColor);
    tetrominPice->tetrominArea[1][1] = new block(tetrominColor);
    tetrominPice->tetrominArea[2][1] = new block(tetrominColor); tetrominPice->tetrominArea[2][2] = new block(tetrominColor);
}
void TetrominSetReverseL(){
                                                                 tetrominPice->tetrominArea[0][2] = new block(tetrominColor);
                                                                 tetrominPice->tetrominArea[1][2] = new block(tetrominColor);
    tetrominPice->tetrominArea[2][1] = new block(tetrominColor); tetrominPice->tetrominArea[2][2] = new block(tetrominColor);
}
void TetrominSetS(){
    tetrominPice->tetrominArea[0][1] = new block(tetrominColor);
    tetrominPice->tetrominArea[1][1] = new block(tetrominColor); tetrominPice->tetrominArea[1][2] = new block(tetrominColor);
                                                                 tetrominPice->tetrominArea[2][2] = new block(tetrominColor);
}
void TetrominSetReverseS(){
                                                                 tetrominPice->tetrominArea[0][2] = new block(tetrominColor);
    tetrominPice->tetrominArea[1][1] = new block(tetrominColor); tetrominPice->tetrominArea[1][2] = new block(tetrominColor);
    tetrominPice->tetrominArea[2][1] = new block(tetrominColor);
}
void TetrominSetI(){
    tetrominPice->tetrominArea[0][2] = new block(tetrominColor);
    tetrominPice->tetrominArea[1][2] = new block(tetrominColor);
    tetrominPice->tetrominArea[2][2] = new block(tetrominColor);
    tetrominPice->tetrominArea[3][2] = new block(tetrominColor);
}
void TetrominSetT(){
                                                                 tetrominPice->tetrominArea[0][2] = new block(tetrominColor);
    tetrominPice->tetrominArea[1][1] = new block(tetrominColor); tetrominPice->tetrominArea[1][2] = new block(tetrominColor); tetrominPice->tetrominArea[1][3] = new block(tetrominColor);
}
// tetromin creation //
void TetrominCreate(){
    TetrominDelete(); //deletes old tetromin
    tetrominPice = new tetromin(); //creates new tetromin instance

    switch (selectedGameMode) //set position
    {
    case Resized:
        tetrominPice->axisX = resizedTetronimStartCol; //assign position
        tetrominPice->axisY = resizedTetronimStartLine;
        break;
    case Classic:
        tetrominPice->axisX = classicTetronimStartCol; //assign position
        tetrominPice->axisY = classicTetronimStartLine;
        break;
    }
    //Brick, L, ReverseL, S, ReverseS, I, T
    switch (tetrominNext) //set shape
    {
    case Brick:
        TetrominSetBrick();
        break;
    case L:
        TetrominSetL();
        break;
    case ReverseL:
        TetrominSetReverseL();
        break;
    case S:
        TetrominSetS();
        break;
    case ReverseS:
        TetrominSetReverseS();
        break;
    case I:
        TetrominSetI();
        break;
    case T:
        TetrominSetT();
        break;
    }
    TetrominDeclareNext();
}

//-------- tetromin falling --------//
/*Function will increse tetrominPice axisY value to simulate fallingm.
  For stoping falling and placing tetromin there will be different function*/
void TetrominFall(){
    DisplayTetrominRemove(); //tetromin is redisplayed
    tetrominPice->axisY++;
    DisplayTetromin();
}
//-------- game update --------//
/*Originally this functions purpose was to just check if something is under the blocks of tetromin, but it was enhanced with function calls like DisplayMap for better optimalization.
  If tetromin block has a ground or a block under it, this function will place blocks of tetromin on map grid and call createion of new tetromin, also will call check of filled lines.
  If tetromin cannot be placed, tetromin will fall*/
void GameUpdateService(){
    bool canPlace = false; //it true, tetromin will be placed, else tetrominFall will be called
    /*This variable is nesesary for this function.
      If false, tetromin will fust fall one block and show goes on.
      If true, function will place tetromin, call creation of new tetromin, displays map, etc.
      Function is divided to few parts.*/

    //function uses different var. for each mode, way how the code works is same.
    switch (selectedGameMode)
    {
    case Resized:

        //here code checks if tetromin has ground or block under it, canPlace is changed to true based on result
        for (int line = 0; line <= tetrominGridSize - 1; line++)
        {
            for(int col = 0; col <= tetrominGridSize - 1; col++){
                if (tetrominPice->tetrominArea[line][col] != NULL){
                    if (resizedMap[tetrominPice->axisY+line + 1][col + tetrominPice->axisX] != NULL){ //check if block is under it 
                        canPlace = true;
                    }
                    else if(tetrominPice->axisY + line + 1 == resizedGridHight) //check if ground is under it 
                    {
                        canPlace = true;
                    }
                }
            }
        }

        //here code works based on state of canPlace boolean
        if(canPlace){ //Tetromin can be placed. In this part tetromin is placed and several functions are called and executed.

            //place tetromin
            for(int line = 0; line <= tetrominGridSize - 1; line++){ 
                for (int col = 0; col <= tetrominGridSize - 1; col++)
                {
                    if (tetrominPice->tetrominArea[line][col] != NULL){
                        resizedMap[line + tetrominPice->axisY][col + tetrominPice->axisX] = new block(placedColor);
                    }
                }
            }

            DisplayMap(); //To this point, blocks are just added to map grid, so clearing the display is unnecessary.

            //create new tetromin
            TetrominCreate(); 
            
            //Checks if lines are filled. Description can be found in the function.
            //CheckFilled();

            //check if game over
            //CheckGameOver();

            //score and next block bar update
            DisplayScoreAndNextBlockClear();
            DisplayScoreAndNextBlock(); //next block is genarated in TetrominCreate, this function is for this reason called after it

            //DisplayTetromin is called every time in main loop begining, so there is no need to use it here
        }
        else{ //tetromin cannot be placed, so tetromin just falls
            TetrominFall();
        }

        break;
    case Classic:
        //here code checks if tetromin has ground or block under it, canPlace is changed to true based on result
        for (int line = 0; line <= tetrominGridSize - 1; line++)
        {
            for(int col = 0; col <= tetrominGridSize - 1; col++){
                if (tetrominPice->tetrominArea[line][col] != NULL){
                    if (tetrominPice->axisY + line + 1 == classicGridHight){ //check if ground is under it
                        canPlace = true;
                    }
                    else if (classicMap[tetrominPice->axisY+line+1][col + tetrominPice->axisX] != NULL) //check if block is under it
                    {
                        canPlace = true;
                    }
                }
            }
        }

        //here code works based on state of canPlace boolean
        if(canPlace){ //Tetromin can be placed. In this part tetromin is placed and several functions are called and executed.

            //place tetromin
            for(int line = 0; line <= tetrominGridSize - 1; line++){
                for (int col = 0; col <= tetrominGridSize - 1; col++)
                {
                    if (tetrominPice->tetrominArea[line][col] != NULL){
                        classicMap[line + tetrominPice->axisY][col + tetrominPice->axisX] = new block(placedColor);
                    }
                }
            }

            DisplayMap(); //To this point, blocks are just added to map grid, so clearing the display is unnecessary.
            //find filled lines //Checks if lines are filled. Description can be found in the function.

            //create new tetromin
            TetrominCreate(); 

            //score and next block bar update
            DisplayScoreAndNextBlockClear();
            DisplayScoreAndNextBlock(); //next block is genarated in TetrominCreate, this function is for this reason called after it

            //DisplayTetromin is called every time in main loop begining, so there is no need to use it here
        }
        else{ //tetromin cannot be placed, so tetromin just falls
            TetrominFall();
        }


        break;
    }
}

//-------- Movement --------//
/*Here you can find functions for moving and turning of tetromin.
  Input events are handled by MovementService().*/

// move left //
void MoveLeft(){
    bool canMove = true; //if false -> cannot move | if true -> can move
    switch (selectedGameMode)
    {
    case Resized:
        //check if tetromn can be moved left
        for(int line = 0; line <= tetrominGridSize - 1; line++){
            for (int col = 0; col <= tetrominGridSize - 1; col++){
                if (tetrominPice->tetrominArea[line][col] != NULL){ //check if tetrominArea[x][x] isn't empty
                    if (tetrominPice->axisX+col-1 >= 0){ //check if left side from tetromin block isn't void
                        if (resizedMap[line+tetrominPice->axisY][tetrominPice->axisX+col-1] != NULL) //check if left side from tetromin block is empty
                        {
                            canMove = false;
                            break; //left side isn't empety, tetromin cannot be moved
                        }
                    }
                    else{
                        canMove = false;
                        break; //left side is void, tetromin cannot be moved
                    }
                }
            }
            if (!canMove)
            {
                break; //tetromin cannot be moved left, this loop is no longer needed
            }
        }

        if(canMove){ //if tetromin can be moved left, move tetromin left
            DisplayTetrominRemove(); //tetromnin is redisplayed
            tetrominPice->axisX--;
            DisplayTetromin();
        }

        break;
    case Classic:
        //check if tetromn can be moved left
        for(int line = 0; line <= tetrominGridSize - 1; line++){
            for (int col = 0; col <= tetrominGridSize - 1; col++){
                if (tetrominPice->tetrominArea[line][col] != NULL){ //check if tetrominArea[x][x] isn't empty
                    if (tetrominPice->axisX+col-1 >= 0){ //check if left side from tetromin block isn't void
                        if (classicMap[line+tetrominPice->axisY][tetrominPice->axisX+col-1] != NULL) //check if left side from tetromin block is empty
                        {
                            canMove = false;
                            break; //left side isn't empety, tetromin cannot be moved
                        }
                    }
                    else{
                        canMove = false;
                        break; //left side is void, tetromin cannot be moved
                    }
                }
            }
            if (!canMove)
            {
                break; //tetromin cannot be moved left, this loop is no longer needed
            }
        }

        if(canMove){ //if tetromin can be moved left, move tetromin left
            DisplayTetrominRemove(); //tetromnin is redisplayed
            tetrominPice->axisX--;
            DisplayTetromin();
        }
        break;
    }
}

// move right //
void MoveRight(){
    bool canMove = true; //if false -> cannot move | if true -> can move
    switch (selectedGameMode)
    {
    case Resized:
        //check if tetromn can be moved right
        for(int line = 0; line <= tetrominGridSize - 1; line++){
            for (int col = 0; col <= tetrominGridSize - 1; col++){
                if (tetrominPice->tetrominArea[line][col] != NULL){ //check if tetrominArea[x][x] isn't empty
                    if (tetrominPice->axisX+col+1 <= resizedGridWidht - 1){ //check if left side from tetromin block isn't void
                        if (resizedMap[line+tetrominPice->axisY][tetrominPice->axisX+col+1] != NULL) //check if right side from tetromin block is empty
                        {
                            canMove = false;
                            break; //right side isn't empety, tetromin cannot be moved
                        }
                    }
                    else{
                        canMove = false;
                        break; //right side is void, tetromin cannot be moved
                    }
                }
            }
            if (!canMove)
            {

                
                break; //tetromin cannot be moved right, this loop is no longer needed
            }
        }

        if(canMove){ //if tetromin can be moved right, move tetromin right
            DisplayTetrominRemove(); //tetromnin is redisplayed
            tetrominPice->axisX++;
            DisplayTetromin();
        }
        break;
    case Classic:
        //check if tetromn can be moved right
        for(int line = 0; line <= tetrominGridSize - 1; line++){
            for (int col = 0; col <= tetrominGridSize - 1; col++){
                if (tetrominPice->tetrominArea[line][col] != NULL){ //check if tetrominArea[x][x] isn't empty
                    if (tetrominPice->axisX+col+1 <= classicGridWidht - 1){ //check if left side from tetromin block isn't void
                        if (classicMap[line+tetrominPice->axisY][tetrominPice->axisX+col+1] != NULL) //check if right side from tetromin block is empty
                        {
                            canMove = false;
                            break; //right side isn't empety, tetromin cannot be moved
                        }
                    }
                    else{
                        canMove = false;
                        break; //right side is void, tetromin cannot be moved
                    }
                }
            }
            if (!canMove)
            {

                
                break; //tetromin cannot be moved right, this loop is no longer needed
            }
        }

        if(canMove){ //if tetromin can be moved right, move tetromin right
            DisplayTetrominRemove(); //tetromnin is redisplayed
            tetrominPice->axisX++;
            DisplayTetromin();
        }
        break;
    }
}

// rotate left //
/*rotation is made like this:
    1. make a rotated copy of tetrominPice (tetrominCopy)
    2. check if blocks of copy of tetromin aren't placed on placed blocks, or in void
    3a. if there is not any issue with copy, owerwrite tetrominPice with tetrominCopy
    3b1. if there is a issue try moving the copy one position on oposite side from rotation
    3b2. if issue still ocurs, do nothing
    4. delete copy
*/
void RotateLeft(){
    bool canPlace = true; //true -> can be placed | false -> cannot be placed
    //make copy
    TetrominCopyMakeLeftRotated();
    Serial.println("copy made");

    //check if cannot be placed
    for (int line = 0; line <= tetrominGridSize - 1; line++)
    {
        for(int col = 0; col <= tetrominGridSize - 1; col++){
            if(tetrominCopy->axisX+col>=0){ //check if void
                switch (selectedGameMode)
                {
                case Resized:
                    if(resizedMap[tetrominCopy->axisY+line][tetrominCopy->axisX+col] != NULL){
                        canPlace = false;
                        break; //tetromin cannot be placed, we don't need this loop anymore
                    }
                    break;
                case Classic:
                    if(classicMap[tetrominCopy->axisY+line][tetrominCopy->axisX+col] != NULL){
                        canPlace = false;
                        break; //tetromin cannot be placed, we don't need this loop anymore
                    }
                    break;
                }
            }
            else{
                canPlace = false;
                break; //tetromin cannot be placed, we don't need this loop anymore
            }
        }
        if(!canPlace){
            break; //tetromin cannot be placed, we don't need this loop anymore
        }
    }

    if(canPlace){ //tetromin can be placed
        DisplayTetrominRemove();

        //overwrite tetromin
        tetrominPice->axisX = tetrominCopy->axisX;
        tetrominPice->axisY = tetrominCopy->axisY;
        for (int line = 0; line <= tetrominGridSize - 1; line++){
            for(int col = 0; col <= tetrominGridSize - 1; col++){
                tetrominPice->tetrominArea[line][col] = tetrominCopy->tetrominArea[line][col];
            }
        }

        DisplayTetromin();

        Serial.println("tetromin rotate 1 succesful");

        //delete copy
        delete tetrominCopy;
        Serial.println("delete 1 succesful");
        return; //rotation is done, we dont need this function anymore
    }
    
    Serial.println("cannot place tetromin, trying again");
    //try moving tetromin one block right
    tetrominCopy->axisX++;
    canPlace = true;
    //check again if cannot be placed
    for (int line = 0; line <= tetrominGridSize - 1; line++)
    {
        for(int col = 0; col <= tetrominGridSize - 1; col++){
            if(tetrominCopy->axisX+col>=0){ //check if void
                switch (selectedGameMode)
                {
                case Resized:
                    if(resizedMap[tetrominCopy->axisY+line][tetrominCopy->axisX+col] != NULL){
                        canPlace = false;
                        break; //tetromin cannot be placed, we don't need this loop anymore
                    }
                    break;
                case Classic:
                    if(classicMap[tetrominCopy->axisY+line][tetrominCopy->axisX+col] != NULL){
                        canPlace = false;
                        break; //tetromin cannot be placed, we don't need this loop anymore
                    }
                    break;
                }
            }
            else{
                canPlace = false;
                break; //tetromin cannot be placed, we don't need this loop anymore
            }
        }
        if(!canPlace){
            break; //tetromin cannot be placed, we don't need this loop anymore
        }
    }

    if(canPlace){ //tetromin can be placed
        DisplayTetrominRemove();

        //overwrite tetromin
        tetrominPice->axisX = tetrominCopy->axisX;
        tetrominPice->axisY = tetrominCopy->axisY;
        for (int line = 0; line <= tetrominGridSize - 1; line++){
            for(int col = 0; col <= tetrominGridSize - 1; col++){
                tetrominPice->tetrominArea[line][col] = tetrominCopy->tetrominArea[line][col];
            }
        }

        DisplayTetromin();
        Serial.println("tetromin rotate 2 succesful");
        //delete copy
        delete tetrominCopy;
        Serial.println("delete 2 succesful");
        return; //rotation is done, we dont need this function anymore
    }

    //tetomin cannot be placed, just delete copy
    delete tetrominCopy;
    Serial.println("delete 3 succesful");
}

// rotate right //
/*rotation is made like this:
    1. make a rotated copy of tetrominPice (tetrominCopy)
    2. check if blocks of copy of tetromin aren't placed on placed blocks, or in void
    3a. if there is not any issue with copy, owerwrite tetrominPice with tetrominCopy
    3b1. if there is a issue try moving the copy one position on oposite side from rotation
    3b2. if issue still ocurs, do nothing
    4. delete copy
*/
void RotateRight(){
    //make copy
    tetrominCopyMakeRightRotated();
}

// service //
void MovementService(){
    
}



////////// setup //////////
/*setup is used to setup and prepare the game to work properly*/
void setup() {
  M5.begin();
  M5.Lcd.begin();
  Serial.begin(38400);
  M5.Lcd.setTextSize(0.5);
  delay(100);

  // game preparation //
  ResizedNULLMap();
  ClassicNULLMap();
  TetrominDeclareNext(); //choses first tetromin
  tetrominPice = new tetromin(); //creates instance of tetrominPice to run TetrominCreate smoothly
  TetrominCreate(); //creates tetromin
}


////////// loop //////////
void loop(){
    DisplayScoreAndNextBlock();
    while (!isGameOver)
    {
        DisplayTetromin();

        //movement and turning
        MovementService();

        GameUpdateService(); //this function provides falling, clearing lines, placing of tetromin, display map, game over checking
        /*GameUpdateService provides almost every feature of this game for these reasons:
        - optimalization - some functions like DisplayMap, CheckGameOver or CheckFilledLines won't be called for no reason, it will only slow down the game
        
        Function is described with comments, so if you want to find about how this game works, look there*/
        //-------- Debug --------//
        delay(800);
        RotateLeft();
    }
    //game over and restart stuff
}