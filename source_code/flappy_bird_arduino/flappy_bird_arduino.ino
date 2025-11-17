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
int pipeSpeed = 1;
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
  bird.jumpStrength=-3;
}

void loop() {
  if(!game_over) {
    handle_user_input();
    Update_Bird(); 
    update_Pipes();
    Update_Score();

    Update_Difficulty();
    draw_game();

    if(check_collision()){
      game_over=true;
    }
    delay(30); 
  }else {
    draw_gameover_page();
    if (digitalRead(BTN_ACTION)== LOW) {
      
      restart_game();

      delay(500);
    }
  }
}

void handle_user_input() {
  int action_state = digitalRead(BTN_ACTION);
  
  if(action_state == LOW) {
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
  // Apply gravity
  bird.Velocity += gravity;
  
  // Limit max fall speed to 2 (keep it slow like you want)
  if(bird.Velocity > 2){
    bird.Velocity = 2;
  }
  
  // ALWAYS update position (moved outside the if)
  bird.y += bird.Velocity;

  // Check ground collision
  if(bird.y > 64 - 6) {
    bird.y = 64 - 6;
    game_over = true;
  }

  // Check ceiling collision
  if(bird.y < 8 + 6){
    bird.y = 8 + 6;
    game_over=true;
  }
}

bool check_collision(){
  //Drawing a box around the bird for collision detection
  int bird_collision_box_Left_side = bird.x - 7;
  int bird_collision_box_Right_side = bird.x + 7;
  int bird_collision_box_Top_side = bird.y - 6;
  int bird_collision_box_Bottom_side = bird.y + 6;

  //Check the collision with pipes
  for (int I =0; I<3 ; I++) {
    
    if (bird_collision_box_Right_side>pipes[I].x && bird_collision_box_Left_side<pipes[I].x + pipes[I].width) {
        //Top pipe collision
        int top_pipe_bottom = pipes[I].gapY - pipes[I].gapSize/2;
        if (bird_collision_box_Top_side< top_pipe_bottom) {
          return true;
        }

        //Buttom pipe collision
        int bottom_pipe_top= pipes[I].gapY + pipes[I].gapSize/2;
        if (bird_collision_box_Bottom_side > bottom_pipe_top) {
          return true;
        }
      
    }
  }

  return false;

}


void restart_game(){
  //Reset game state
  game_over=false;
  score=0;
  pipeSpeed=1;

  // Reset bird
  bird.x = 30;
  bird.y = 32;
  bird.Velocity = 0;

  // Reset pipes
  for(int i = 0; i < 3; i++) {
    pipes[i].x = 128 + (i * 65) + random(-10, 10);
    pipes[i].gapY = random(25, 48);
    pipes[i].width = 10;
    pipes[i].gapSize = 25;
    pipes[i].passed = false;
  }
  
}

void Update_Score(){
  for (int I = 0 ; I<3 ; I++) {
    if(!pipes[I].passed && pipes[I].x + pipes[I].width<bird.x){
      pipes[I].passed=true;
      score++;
    }
  }
}

void Update_Difficulty(){
  // Every 10 points increase speed
  if(score >= 10 && score < 20) pipeSpeed = 2;
  else if(score >= 20 && score < 30) pipeSpeed = 3;
  else if(score >= 30 && score < 40) pipeSpeed = 4;
  else if(score >= 40) pipeSpeed = 5;
  else if(score < 10) pipeSpeed = 1;
}

void draw_game() {
  u8g2.firstPage();
  do { 
    // Draw score
    u8g2.setFont(u8g2_font_5x7_mf);
    u8g2.drawStr(0, 6, "Score: ");
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

void draw_gameover_page(){
  u8g2.firstPage();
  do {
    // Display "GAME OVER" text
    u8g2.setFont(u8g2_font_10x20_tf);
    u8g2.drawStr(20, 30 , "GAME OVER");

    // Display final score
    u8g2.setFont(u8g2_font_7x13_mf);
    u8g2.drawStr(35, 45, "Score: ");
    u8g2.setCursor(78, 45);
    u8g2.print(score);
    
    // Display restart instruction
    u8g2.setFont(u8g2_font_5x7_mf);
    u8g2.drawStr(20, 58, "Press action key");

  }while (u8g2.nextPage());
}