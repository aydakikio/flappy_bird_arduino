#include <U8g2lib.h>

U8G2_SH1106_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0, -1, A5, A4);

// Snake properties
#define GRID_SIZE 4
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Button pins
#define BTN_ACTION 12

// Game state
int score = 0;
bool game_over = false;
bool is_bird_passed = false;

// Timing
//played_time=0
int game_speed = 600;  // milliseconds - much slower starting speed


struct Point {
  int x, y;
};

struct barrier{
  int x , y , width,hight;
};


void setup() {
  // put your setup code here, to run once:
  u8g2.begin();
  Serial.begin(9500);
  // Initialize button pins
  pinMode(BTN_ACTION, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!game_over){

  }
}

void hanle_user_input(){
  int action_state= digitalRead(BTN_ACTION)

  if(action_state== LOW){
    Serial.println("LL");
  }
}
