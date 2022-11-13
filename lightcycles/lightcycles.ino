#define ACTIVE LOW
#define NEGATED HIGH
#define MAX_OUT_CHARS 16
#define CLOCK A4
#define DATA A5
#define P2_UP 7
#define P2_DOWN 10
#define P2_RIGHT 9
#define P2_LEFT 8
#define P1_UP 2
#define P1_DOWN 3
#define P1_RIGHT 4
#define P1_LEFT 5
#define DEFAULT_FONT u8g_font_4x6
#define MAXWIDTH 128
#define MAXHEIGHT 64
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

#include <U8glib.h>
#include <stdio.h>

typedef struct Point {
  int x;
  int y;
} Point;

typedef struct Player {
  int x = 10;
  int y = 6;
  int dir = RIGHT;
} Player;

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);

Player player_one; // = {10,  32, RIGHT};
Player player_two; // = {118, 32, LEFT};
uint64_t grid[MAXWIDTH] = {};

bool start_flag = true;
bool game_over = false;
bool player_one_dead = false;
bool player_two_dead = false;
char buffer[MAX_OUT_CHARS + 1];

void setup() {
  pinMode(P1_LEFT, INPUT_PULLUP);
  pinMode(P1_RIGHT, INPUT_PULLUP);
  pinMode(P1_UP, INPUT_PULLUP);
  pinMode(P1_DOWN, INPUT_PULLUP);
  
  pinMode(P2_LEFT, INPUT_PULLUP);
  pinMode(P2_RIGHT, INPUT_PULLUP);
  pinMode(P2_UP, INPUT_PULLUP);
  pinMode(P2_DOWN, INPUT_PULLUP);
  
  //Serial.begin(57600);
  ///randomSeed(analogRead(0));

  player_one.x = 10;
  player_one.y = 32;
  player_one.dir = RIGHT;
  
  player_two.x = 118;
  player_two.y = 32;
  player_two.dir = LEFT;

  grid[0] =           0b1111111111111111111111111111111111111111111111111111111111111111;
  for (int i = 1; i < MAXWIDTH-1; i++) 
    grid[i] =         0b1000000000000000000000000000000000000000000000000000000000000001;
  grid[MAXWIDTH-1] =  0b1111111111111111111111111111111111111111111111111111111111111111;
}

void loop() {
  checkKeys(&player_one, P1_LEFT, P1_RIGHT, P1_UP, P1_DOWN);
  checkKeys(&player_two, P2_LEFT, P2_RIGHT, P2_UP, P2_DOWN);
  movePlayer(&player_one);
  movePlayer(&player_two);
  
  checkCollision();

  if ( game_over ) {
    gameOver();
  }

  // draw the screen
  u8g.firstPage();  
  do {
    drawPlayer(player_one);
    drawPlayer(player_two);
    drawGrid();
  } while( u8g.nextPage() );

  //delay(5000);
}

void checkCollision()
{
  if (grid[player_one.x] & 1ull << player_one.y)
    player_one_dead = true;

  if (grid[player_two.x] & 1ull << player_two.y)
    player_two_dead = true;

  if (player_one_dead || player_two_dead)
    game_over = true;
}

void movePlayer(Player* player) {
  // 1ull unsigned long long = 64bits
  grid[player->x] = grid[player->x] | 1ull << player->y;
 
  switch(player->dir) {
    case UP:
      player->y--;
      break;
    case DOWN:
      player->y++;
      break;
    case LEFT:
      player->x--;
      break;
    case RIGHT:
      player->x++;
      break; 
  }
}

void drawPlayer(Player player) {
  u8g.setDefaultForegroundColor();
  u8g.drawPixel(player.x, player.y);
}

void drawGrid() {
  bool drawPixel;
  u8g.setDefaultForegroundColor();

  for (int y = 0; y < MAXHEIGHT; y++)
    for (int x = 0; x < MAXWIDTH; x++)
      if ((grid[x] >> y) & 1)
        u8g.drawPixel(x, y);
}

void checkKeys(Player* player, int left, int right, int up, int down) {
  boolean left_button_pressed = digitalRead(left) == ACTIVE;
  boolean right_button_pressed = digitalRead(right) == ACTIVE;
  boolean up_button_pressed = digitalRead(up) == ACTIVE;
  boolean down_button_pressed = digitalRead(down) == ACTIVE;
  
  if (left_button_pressed) {
    if (!(player->dir == RIGHT))
      player->dir = LEFT;
  }
  else if (right_button_pressed) {
    if (!(player->dir == LEFT))
        player->dir = RIGHT;
  }
  else if (up_button_pressed) {
    if (!(player->dir == DOWN))
      player->dir = UP;
  }
  else if (down_button_pressed) {
    if (!(player->dir == UP))
      player->dir = DOWN;
  }

}

void gameOver() {
    while(1) {
      u8g.firstPage();  
      do {
        drawGameOver();
      } while( u8g.nextPage() );
    }
}

void drawGameOver() {
  u8g.setFont(DEFAULT_FONT);
  u8g.setFontPosTop();
  u8g.setDefaultForegroundColor();

  if ( player_one_dead && player_two_dead )
    sprintf(buffer, "Mutual Destruction!");
  else if (!player_one_dead)
    sprintf(buffer, "Player One Wins");
  else
    sprintf(buffer, "Player Two Wins");

  u8g.drawStr(MAXWIDTH/2 - u8g.getStrWidth("Player One Wins")/2,
              MAXHEIGHT/2 - u8g.getFontAscent(), buffer);
}
