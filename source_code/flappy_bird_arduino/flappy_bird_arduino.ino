#include <U8g2lib.h>

U8G2_SH1106_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0, -1, A5, A4);

// Screen properties
#define GRID_SIZE 4
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Button pins
#define BTN_ACTION 12

struct Bird {
  int x;
  int y;
  int Velocity;
  int jumpStrength;
};

struct Pipe {
  int x;
  int gapY;      
  int width;
  int gapSize;
  bool passed;
};

//Pipe settings
int pipeSpeed = 2;
Pipe pipes[3];
int lastPipeX = 0;

//Bird Bit map
const unsigned char bird_bits[] = {
  0x00, 0x00, 0x90, 0x03, 0x88, 0x04, 0x4E, 0x0C, 0x11, 0x0C, 0xA1, 0x08, 
  0x21, 0x1F, 0x92, 0x3F, 0xCC, 0x1F, 0x8C, 0x1F, 0xF8, 0x0F, 0x00, 0x00,
};

//Creating a bird
Bird bird;

// Game state
int score = 0;
bool game_over = false;

// Game Environement 
int game_speed = 600;
int gravity = 1;

void setup() {
  u8g2.begin();
  Serial.begin(9600);
  
  // Initialize button pins
  pinMode(BTN_ACTION, INPUT_PULLUP);
  
  // Initialize random seed
  randomSeed(analogRead(A0));
  
  // Initialize pipes with random spacing
  for(int i = 0; i < 3; i++) {
    int spacing = random(50, 80);  
    pipes[i].x = 128 + (i * 65) + random(-10, 10);  
    pipes[i].gapY = random(25, 48);  
    pipes[i].width = 10;
    pipes[i].gapSize = 25;
    pipes[i].passed = false;
  }
  
  //Track rightmost pipe
  lastPipeX = pipes[2].x;

  // initialize Bird
  bird.x=30;
  bird.y=32;
  bird.Velocity=0;
  bird.jumpStrength=-6;
}

void loop() {
  if(!game_over) {
    handle_user_input();
    update_Pipes();  
    draw_game();
    Update_Bird();
    delay(30); 
  }
}

void handle_user_input() {
  int action_state = digitalRead(BTN_ACTION);
  
  if(action_state == LOW) {
    Serial.println("Button Pressed");
    bird.Velocity = bird.jumpStrength;
  }

}

void Draw_Pipe(Pipe &p) {
  // Top pipe
  u8g2.drawBox(p.x, 8, p.width, p.gapY - p.gapSize/2 - 8);
  
  // Bottom pipe  
  u8g2.drawBox(p.x, p.gapY + p.gapSize/2, p.width, 64 - (p.gapY + p.gapSize/2));
}

void update_Pipes() {
  for(int i = 0; i < 3; i++) {
    pipes[i].x -= pipeSpeed;
    
    // Recycle pipe when it goes off-screen
    if(pipes[i].x < -pipes[i].width) {
      // Find the rightmost pipe
      int maxX = -100;
      for(int j = 0; j < 3; j++) {
        if(pipes[j].x > maxX) {
          maxX = pipes[j].x;
        }
      }
      
      // Place new pipe with random spacing after the rightmost pipe
      int spacing = random(35, 55);  
      pipes[i].x = maxX + spacing;
      
      // Random gap position (avoid edges)
      pipes[i].gapY = random(22, 50);
      
      pipes[i].passed = false;
      
    }
  }
}

void Draw_Bird() {
  u8g2.drawXBM(bird.x - 4, bird.y - 4, 14, 12, bird_bits);
}

void Update_Bird(){
  bool action_button_pressed = handle_user_input();
  Serial.println(action_button_pressed);
  if(bird.Velocity <= 8){
    bird.Velocity+=gravity;
  }

  if(bird.y > 64 - 6) {
    bird.y = 64 - 6;
    game_over = true;
  }

  if(bird.y <8 +6 ){
    bird.y = 8 + 6;
    game_over=true;
  }
}

void check_collision(){
  //Drawing a box around the bird for collision detection
  int birdLeft = bird.x - 6;
  int birdRight = bird.x + 6;
  int birdTop = bird.y - 6;
  int birdBottom = bird.y + 6;



}

void draw_game() {
  u8g2.firstPage();
  do { 
    // Draw score
    u8g2.setFont(u8g2_font_5x7_mf);
    u8g2.drawStr(0, 6, "Score:");
    u8g2.setCursor(30, 6);
    u8g2.print(score);
    
    // Draw game border
    u8g2.drawFrame(0, 8, SCREEN_WIDTH, SCREEN_HEIGHT - 8);
    
    // Draw all pipes
    for(int i = 0; i < 3; i++) {
      Draw_Pipe(pipes[i]);
    }
    
    // Draw bird (placeholder)
    Draw_Bird();
    
  } while(u8g2.nextPage());
}