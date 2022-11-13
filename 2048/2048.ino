#include <U8glib.h>
#include <stdio.h>

#define DEBUG 0

// PINS
#define CLOCK A4
#define DATA A5
#define LEFT_BUTTON 2
#define RIGHT_BUTTON 3
#define SELECT_BUTTON 4
#define BACK_BUTTON 5
// DRAWING
#define MAX_OUT_CHARS 16
#define BOX_HEIGHT 14
#define BOX_WIDTH 20
#define BOX_SPACING 2
#define DEFAULT_FONT u8g_font_4x6
//#define DEFAULT_FONT u8g_font_unifont

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);
char outputBuffer[MAX_OUT_CHARS + 1];
char debugBuffer[MAX_OUT_CHARS + 1];
int grid[16] = {0, 0, 0, 2,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0};

void setup() {
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(SELECT_BUTTON, INPUT_PULLUP);
  pinMode(BACK_BUTTON, INPUT_PULLUP);
  
  randomSeed(analogRead(0));
  
  if (DEBUG) {
    Serial.begin(9600);
    Serial.println("Setup finished");
  }
}

void loop(void) {
  bool moved = false;
  draw();

  moved = checkUpdate();
  if (moved == true) {
    draw();
    delay(70);
    addSquare();
  }
}

void draw(void) {
    u8g.firstPage();  
    do {
      drawPage();
    } while( u8g.nextPage() );
}

void drawPage(void) {
  int i, j;

  u8g.setFont(DEFAULT_FONT);
  u8g.setFontPosTop();
  
  for(int i=0; i<4; i++)
    for(int j=0; j<4; j++ )
      if(grid[i+j*4] != 0) {
        u8g.setDefaultForegroundColor();
        u8g.drawBox(i*(BOX_WIDTH+BOX_SPACING),
                  j*(BOX_HEIGHT+BOX_SPACING),
                  BOX_WIDTH,
                  BOX_HEIGHT);

        u8g.setDefaultBackgroundColor();      
        sprintf(outputBuffer, "%d", grid[i+j*4]);
        u8g.drawStr(i*(BOX_WIDTH+BOX_SPACING)+1,
                    j*(BOX_HEIGHT+BOX_SPACING)+1,
                    outputBuffer);    
      }
}

int nextEmpty(int x, int y, char dir) {
  int away = 0;
  int y_offset = y*4;
  
  switch(dir) {
    case 'r': // right shift
      if (x==3)
        return away;
      else
        while(grid[x+1+away+y_offset] == 0 && (x+1+away < 4))
          away++;
      break;
      
    case 'l': // left shift
      if (x==0)
        return away;
      else
        while (grid[x-1-away+y_offset] == 0 && (x-1-away >= 0))
          away++;
      break;
    
    case 'u':
      if (y==0)
        return away;
      else
        while (grid[x+4*(y-1-away)] == 0 && (y-1-away) >= 0)
          away++;
      break;

    case 'd':
      if (y==3)
        return away;
      else
        while (grid[x+4*(y+1+away)] == 0 && (y+1+away) < 4)
          away++;
      break;
  }
  return away;
}

bool shiftRight(void) {
  int away;
  bool matched;
  bool moved = false;
  for (int y=0; y<4; y++) {
    matched = false;
    for (int x=2; x>=0; x--) {

      if ( grid[x+y*4] == 0)
        continue;

      away = nextEmpty(x, y, 'r');
      
      if(away > 0){
        if(grid[x+away+1+y*4] == grid[x+y*4] && matched == false && x+away+1 < 4){
          grid[x+away+1 + y*4] *= 2;
          grid[x+y*4] = 0;
          matched = true;
        }else{
          grid[x+away+y*4] = grid[x+y*4];
          grid[x+y*4] = 0;
          matched = false;
        }
        moved = true;
      } else {
        if(grid[x+1+y*4] == grid[x+y*4] && matched == false){ 
          grid[x+1+y*4] *= 2;
          grid[x+y*4] = 0;
          matched = true;
          moved = true;
        } else {
          matched = false;
        }
      }
    }
  }
  return moved;
}

bool shiftLeft(void) {
  int away;
  bool matched;
  bool moved = false;
  for(int y=0; y<4; y++){
    matched = false;
    for(int x=1; x<4; x++){
      if( grid[x+y*4] == 0){
        continue;
      }
      away = nextEmpty(x, y, 'l');
      if(away > 0){
        if(grid[x-away-1+y*4] == grid[x+y*4] && matched == false && x-away-1 >= 0){
          grid[x-away-1+y*4] *= 2;
          grid[x+y*4] = 0;
          matched = true;
        } else {
          grid[x-away+y*4] = grid[x+y*4];
          grid[x+y*4] = 0;
          matched = false;
        }
        moved = true;
      } else {
        if(grid[x-1+y*4] == grid[x+y*4]) { 
          grid[x-1+y*4] *= 2;
          grid[x+y*4] = 0;
          matched = true;
          moved = true;
        } else {
          matched = false;
        }
      }
    } 
  }
  return moved;
}

bool shiftUp(void) {
  int away;
  bool matched;
  bool moved = false;

  for(int x=0; x<4; x++){
    matched = false;
    for(int y=1; y<4; y++) {

      if( grid[x+y*4] == 0)
        continue;

      away = nextEmpty(x, y, 'u');
      if(away > 0){
        if(grid[x+(y-away-1)*4] == grid[x+y*4] && matched == false && y-away-1 >= 0){
          grid[x+(y-away-1)*4] *= 2;
          grid[x+y*4] = 0;
          matched = true;
        } else {
          grid[x+(y-away)*4] = grid[x+y*4];
          grid[x+y*4] = 0;
          matched = false;
        }
        moved = true;
      } else {
        if(grid[x+(y-1)*4] == grid[x+y*4])
        {  
          grid[x+(y-1)*4] *= 2;
          grid[x+y*4] = 0;
          matched = true;
          moved = true;
        } else {
          matched = false;
        }
      }
    }
  }
  return moved;
}

bool shiftDown(void) {
  int away;
  bool matched;
  bool moved = false;
  for(int x=0; x<4; x++) {
    matched = false;
    for(int y=2; y>=0; y--) {
      if( grid[x+y*4] == 0)
        continue;

      away = nextEmpty(x, y, 'd');
      if(away > 0) {
        if(grid[x+(y+away+1)*4] == grid[x+y*4] && matched == false && y+away+1 < 4) {
         grid[x+(y+away)*4] *= 2;
         grid[x+y*4] = 0;
         matched = true; 
        }
        else {
          grid[x+(y+away)*4] = grid[x+y*4];
          grid[x+y*4] = 0;
          matched = false;
        }
        moved = true;
      }
      else {
        if(grid[x+(y+1)*4] == grid[x+y*4]) {
          grid[x+(y+1)*4] *= 2;
          grid[x+y*4] = 0;
          matched = true;
          moved = true;
        }
        else {
          matched = false;
        }
      }
    }
  }
  return moved;
}

void addSquare(void) {
  int count = 0;
  int pick = 0;
  int j = 0;
  
  for (int i=0; i<16; i++)
    if (grid[i] == 0)
      count++;

  pick = random(0,count);
  for (int i=0; i<16; i++) {
    if (grid[i] == 0) {
      if (j==pick) {
        grid[i] = 2;
        break;
      }
      j++;
    }
  }
}

bool checkUpdate(void){
  bool moved = false;
  
  if(digitalRead(SELECT_BUTTON) == LOW){
    moved = shiftUp();
  }

  if(digitalRead(BACK_BUTTON) == LOW){
    if (moved == true)
      addSquare();
    moved = shiftDown();
  }

  if(digitalRead(LEFT_BUTTON) == LOW){
    if (moved == true)
      addSquare();
    moved = shiftLeft();
  }

  if(digitalRead(RIGHT_BUTTON) == LOW){
    if (moved == true)
      addSquare();
    moved = shiftRight();
  }

  return moved;
}
