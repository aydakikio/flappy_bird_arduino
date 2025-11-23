#include <U8g2lib.h>
#include <EEPROM.h>

// ===== HARDWARE CONFIGURATION =====
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, -1, A5, A4);
#define BTN_ACTION 12
#define BTN_RESET 4

// ===== EEPROM CONFIGURATION =====
#define EEPROM_HIGH_SCORE_ADDR 0
#define EEPROM_MAGIC_ADDR 2
#define EEPROM_MAGIC_VALUE 42

// ===== DISPLAY CONSTANTS =====
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define GAME_AREA_TOP 8
#define GAME_AREA_HEIGHT (SCREEN_HEIGHT - GAME_AREA_TOP)

// ===== GAME CONSTANTS =====
#define NUM_PIPES 3
#define BIRD_SIZE 6
#define PIPE_WIDTH 10
#define PIPE_GAP_SIZE 25

// ===== STRUCTURES =====
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

// ===== GAME OBJECTS =====
Bird bird;
Pipe pipes[NUM_PIPES];

// ===== GAME STATE =====
bool new_high_score=0;
int high_score = 0;
int score = 0;
bool game_over = false;
uint8_t pipeSpeed = 1;
const uint8_t gravity = 1;

// ===== BIRD SPRITE (14x12) =====
const unsigned char bird_bits[] PROGMEM= {
  0x00, 0x00, 0x90, 0x03, 0x88, 0x04, 0x4E, 0x0C, 0x11, 0x0C, 0xA1, 0x08,
  0x21, 0x1F, 0x92, 0x3F, 0xCC, 0x1F, 0x8C, 0x1F, 0xF8, 0x0F, 0x00, 0x00,
};

// ===== FORWARD DECLARATIONS =====
void initialize_game();
void handle_user_input();
void update_game_state();
void draw_game();
void draw_gameover_page();
void restart_game();

void setup() {
  u8g2.begin();

  //pin configuration 
  pinMode(BTN_ACTION, INPUT_PULLUP);
  pinMode(BTN_RESET , INPUT_PULLUP);

  load_high_score();

  randomSeed(analogRead(A0));
  
  initialize_game();
}

// ===== MAIN GAME LOOP =====
void loop() {
  if(digitalRead(4)==LOW){
    reset_high_score();
    delay(30);
  }

  if (!game_over) {
    handle_user_input();
    update_game_state();
    draw_game();
    delay(30);
  } else {
    draw_gameover_page();
    
    if (digitalRead(BTN_ACTION) == LOW) {
      restart_game();
      delay(500);
    }
  }
}

// ===== INITIALIZATION =====
void initialize_game() {
  // Initialize bird
  bird.x = 30;
  bird.y = 32;
  bird.Velocity = 0;
  bird.jumpStrength = -3;
  
  // Initialize pipes
  for (int I = 0; I < NUM_PIPES; I++) {
    pipes[I].x = SCREEN_WIDTH + (I * 65) + random(-10, 10);
    pipes[I].gapY = random(25, 48);
    pipes[I].width = PIPE_WIDTH;
    pipes[I].gapSize = PIPE_GAP_SIZE;
    pipes[I].passed = false;
  }
}

// ===== INPUT HANDLING =====
void handle_user_input() {
  if (digitalRead(BTN_ACTION) == LOW) {
    bird.Velocity = bird.jumpStrength;
  }
}

// ===== GAME STATE UPDATE =====
void update_game_state() {
  Update_Bird();
  update_Pipes();
  Update_Score();
  Update_Difficulty();
  
  if (check_collision()) {
    game_over = true;
  }
}

// ===== BIRD PHYSICS =====
void Update_Bird() {
  // Apply gravity
  bird.Velocity += gravity;
  
  // Limit max fall speed
  if (bird.Velocity > 2) {
    bird.Velocity = 2;
  }
  
  // Update position
  bird.y += bird.Velocity;
  
  // Check boundaries
  if (bird.y > SCREEN_HEIGHT - BIRD_SIZE) {
    bird.y = SCREEN_HEIGHT - BIRD_SIZE;
    game_over = true;
  }
  
  if (bird.y < GAME_AREA_TOP + BIRD_SIZE) {
    bird.y = GAME_AREA_TOP + BIRD_SIZE;
    game_over = true;
  }
}

// ===== PIPE MANAGEMENT =====
void update_Pipes() {
  for (int I = 0; I < NUM_PIPES; I++) {
    pipes[I].x -= pipeSpeed;
    
    // Recycle pipe when off-screen
    if (pipes[I].x < -pipes[I].width) {
      // Find rightmost pipe
      int maxX = -100;
      for (int J = 0; J < NUM_PIPES; J++) {
        if (pipes[J].x > maxX) {
          maxX = pipes[J].x;
        }
      }
      
      // Reset pipe with random spacing
      int spacing = random(35, 55);
      pipes[I].x = maxX + spacing;
      pipes[I].gapY = random(22, 50);
      pipes[I].passed = false;
    }
  }
}

// ===== SCORING =====
void Update_Score() {
  for (int I = 0; I < NUM_PIPES; I++) {
    if (!pipes[I].passed && pipes[I].x + pipes[I].width < bird.x) {
      pipes[I].passed = true;
      score++;
      if(score>high_score){
        high_score= score;
        new_high_score = true;

        save_high_score();
      }
    }
  }
}

// ===== DIFFICULTY SCALING =====
void Update_Difficulty() {
  if (score < 10) pipeSpeed = 1;
  else if (score >= 10 && score < 20) pipeSpeed = 2;
  else if (score >= 20 && score < 30) pipeSpeed = 3;
  else if (score >= 30 && score < 40) pipeSpeed = 4;
  else if (score >= 40) pipeSpeed = 5;
}

// ===== COLLISION DETECTION =====
bool check_collision() {
  int birdLeft = bird.x - 7;
  int birdRight = bird.x + 7;
  int birdTop = bird.y - BIRD_SIZE;
  int birdBottom = bird.y + BIRD_SIZE;
  
  for (int I = 0; I < NUM_PIPES; I++) {
    // Check horizontal overlap
    if (birdRight > pipes[I].x && birdLeft < pipes[I].x + pipes[I].width) {
      
      // Check top pipe collision
      int topPipeBottom = pipes[I].gapY - pipes[I].gapSize / 2;
      if (birdTop < topPipeBottom) {
        return true;
      }
      
      // Check bottom pipe collision
      int bottomPipeTop = pipes[I].gapY + pipes[I].gapSize / 2;
      if (birdBottom > bottomPipeTop) {
        return true;
      }
    }
  }
  
  return false;
}

// ===== EEPROM FUNCTIONS =====
void load_high_score() {
  byte magic = EEPROM.read(EEPROM_MAGIC_ADDR);
  
  if (magic != EEPROM_MAGIC_VALUE) {
    high_score = 0;
    EEPROM.write(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VALUE);
    EEPROM.put(EEPROM_HIGH_SCORE_ADDR, high_score);
  } else {
    EEPROM.get(EEPROM_HIGH_SCORE_ADDR, high_score);
  }
}

void save_high_score() {
  EEPROM.put(EEPROM_HIGH_SCORE_ADDR, high_score);
}

void reset_high_score() {
  high_score = 0;
  new_high_score = false;
  EEPROM.put(EEPROM_HIGH_SCORE_ADDR, high_score);
}

// ===== RESTART =====
void restart_game() {
  new_high_score=false;
  game_over = false;
  score = 0;
  pipeSpeed = 1;
  
  initialize_game();
}

// ===== RENDERING GAME OBJECTS =====
void Draw_Pipe(Pipe &p) {
  // Top pipe
  int topPipeHeight = p.gapY - p.gapSize / 2 - GAME_AREA_TOP;
  u8g2.drawBox(p.x, GAME_AREA_TOP, p.width, topPipeHeight);
  
  // Bottom pipe
  int bottomPipeTop = p.gapY + p.gapSize / 2;
  int bottomPipeHeight = SCREEN_HEIGHT - bottomPipeTop;
  u8g2.drawBox(p.x, bottomPipeTop, p.width, bottomPipeHeight);
}

void Draw_Bird() {
  u8g2.drawXBMP(bird.x - 7, bird.y - 6, 14, 12, bird_bits);
}

// ===== RENDERING GAME SCENES =====
void draw_game() {
  u8g2.clearBuffer();

  // Draw score
  u8g2.setFont(u8g2_font_5x7_mf);
  u8g2.drawStr(0, 6, "Score: ");
  u8g2.setCursor(35, 6);
  u8g2.print(score);
  
  //Draw high score
  u8g2.drawStr(70, 6, "Best: ");
  u8g2.setCursor(95, 6);
  u8g2.print(high_score);
  
  // Draw game border
  u8g2.drawFrame(0, GAME_AREA_TOP, SCREEN_WIDTH, GAME_AREA_HEIGHT);
    
  // Draw pipes
  for (int i = 0; i < NUM_PIPES; i++) {
    Draw_Pipe(pipes[i]);
  }
    
  // Draw bird
  Draw_Bird();
    
  u8g2.sendBuffer();
}

void draw_gameover_page() {
  u8g2.clearBuffer();
  
  // Game Over title
  u8g2.setFont(u8g2_font_7x13_mf);
  u8g2.drawStr(31, 20, "GAME OVER");
    
  u8g2.setFont(u8g2_font_5x7_mf);
  
  if (new_high_score) {
    u8g2.drawStr(24, 40, "New high score!");
  }
  
  // Final score
  u8g2.drawStr(40, 52, "Score: ");
  u8g2.setCursor(75, 52);
  u8g2.print(score);
    
  // Restart instruction
  u8g2.drawStr(20, 62, "Press any button...");

  u8g2.sendBuffer();
}
