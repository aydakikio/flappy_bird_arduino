# Flappy Bird Game  :bird:

<p align="center">
  <img src="./Assets/gameplay.gif" alt="animated" width="90%" height="90%" />
</p>

<p align="center">
  <a href="#project-overview">Overview</a> •
  <a href="#installation">Installation</a> •
  <a href="#hardware-requirements">Hardware</a> •
  <a href="#software-architecture">Architecture</a> •
  <a href="#key-functions-documentation">Documentation</a> •
  <a href="#configuration--tuning">Configuration</a> •
  <a href="#troubleshooting">Troubleshooting</a>
</p>
<p align="center">
  <img src="https://img.shields.io/badge/Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white" alt="Arduino">
  <img src="https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++">
  <img src="https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge" alt="License">
  <img src="https://img.shields.io/badge/Version-1.2-blue.svg?style=for-the-badge" alt="Version">
  <img src="https://img.shields.io/badge/Display-SH1106_OLED-orange.svg?style=for-the-badge" alt="Display">
  <img src="https://img.shields.io/badge/RAM-~1KB-green.svg?style=for-the-badge" alt="RAM Usage">
</p>

---

## Project Overview

A complete Flappy Bird clone implementation for Arduino microcontrollers using the U8g2 graphics library and SH1106 OLED display (128x64 pixels). Features persistent high score tracking, dynamic difficulty scaling, and smooth gameplay.

---

## Installation

### Prerequisites
- **Arduino IDE 1.8.x or higher** ([Download here](https://www.arduino.cc/en/software))
- USB cable (USB-A to USB-B for Uno, or appropriate for your board)
- Git (optional, for cloning method)

---

### Method 1: Download from GitHub Release (Easiest)

**Perfect for beginners who just want to use the game**

#### Step 1: Download the Code
1. Visit the [Releases page](https://github.com/aydakikio/flappy_bird_arduino/releases)
2. Click on the latest release
3. Download the **flappy_bird_arduino.ino** file

#### Step 2: Install U8g2 Library
1. Open **Arduino IDE**
2. Go to **Sketch → Include Library → Manage Libraries...**
3. In the search box, type: `U8g2`
4. Find **U8g2 by olikraus**
5. Click **Install**
6. Wait for "Installed" confirmation

#### Step 3: Open the Project
1. In Arduino IDE, go to **File → Open**
2. Navigate to the project folder
3. Select `flappy_bird.ino`
4. The code will open in the IDE

#### Step 4: Configure Arduino
1. Connect your Arduino to computer via USB
2. Select Board: **Tools → Board → Arduino AVR Boards → Arduino Uno**
3. Select Port: **Tools → Port → COM3** (Windows) or **/dev/ttyUSB0** (Linux) or **/dev/cu.usbserial** (Mac)
   - The port with "(Arduino Uno)" next to it

#### Step 5: Upload
1. Click the **Upload** button (→ icon) or press `Ctrl+U`
2. Wait for compilation to complete
3. Look for "Done uploading" message at the bottom
4. The game should start immediately!

---

### Method 2: Clone from GitHub

**Perfect for developers who want to modify or contribute**

#### Step 1: Clone the Repository
Open terminal/command prompt and run:

```bash
# Clone using HTTPS
git clone https://github.com/aydakikio/flappy_bird_arduino.git

# Navigate into project folder
cd flappy_bird_arduino
```

#### Step 2: Install U8g2 Library
1. Open **Arduino IDE**
2. **Sketch → Include Library → Manage Libraries...**
3. Search for `U8g2`
4. Install **U8g2 by olikraus**

#### Step 3: Open the Project
```bash
# Option A: Open from terminal (if arduino-cli installed)
arduino source_code/flappy_bird.ino

# Option B: Open Arduino IDE manually
# File → Open → Navigate to cloned folder→ source_code folder → flappy_bird.ino
```

#### Step 4: Configure and Upload
1. **Tools → Board → Arduino AVR Boards → Arduino Uno**
2. **Tools → Port** → Select your Arduino port
3. Click **Upload** (→) or press `Ctrl+U`

---

### Verify Installation

After successful upload, you should see:

✅ **Display powers on** - OLED screen lights up  
✅ **Game border appears** - Rectangle frame around play area  
✅ **Score shows "Score: 0"** - At top left of screen  
✅ **High score shows "Best: 0"** - At top right of screen  
✅ **Bird is visible** - Small sprite at left side  
✅ **Pipes scroll** - Moving from right to left  
✅ **Action button works** - Pressing button makes bird jump  
✅ **Reset button works** - Holding reset button clears high score  

**If anything doesn't work**, check the [Troubleshooting](#troubleshooting) section below.

---
## Hardware Requirements

### Components
- **Microcontroller**: Arduino Uno/Nano or compatible
- **Display**: SH1106 128x64 OLED (I2C interface)
- **Input**: 2 push buttons (pull-up configuration)
- **Connections**:
  - I2C SDA → A4
  - I2C SCL → A5
  - Action Button → Pin 12 (INPUT_PULLUP)
  - Reset Button → Pin 4 (INPUT_PULLUP)

### Wiring Diagram
<p align="center">
  <img src="./Assets/wiring_diagram.png" alt="animated" width="80%" height="80%" />
</p>

---

## Software Architecture

### System Design Pattern
The codebase follows a **Game Loop Architecture** with clear separation of concerns:

```
┌─────────────────────────────────────┐
│         Main Game Loop              │
│  ┌───────────────────────────────┐  │
│  │  1. Input Handling            │  │
│  │  2. State Update              │  │
│  │  3. Collision Detection       │  │
│  │  4. Rendering                 │  │
│  │  5. High Score Management     │  │
│  └───────────────────────────────┘  │
└─────────────────────────────────────┘
```

### Core Components

#### 1. **State Management**
- Game operates in two states: `PLAYING` and `GAME_OVER`
- State transitions handled via `game_over` boolean flag
- Score and difficulty tracked globally
- High score persisted to EEPROM with magic byte validation

#### 2. **Physics Engine**
```cpp
Velocity Update:  V(t+1) = V(t) + gravity
Position Update:  Y(t+1) = Y(t) + V(t+1)
Jump Impulse:     V = jumpStrength (negative)
```

#### 3. **Rendering Pipeline**
Uses U8g2's full buffer system:
- `clearBuffer()` - Clear display buffer
- Draw game objects
- `sendBuffer()` - Send buffer to display
- Eliminates screen flicker

#### 4. **Persistent Storage**
- EEPROM addresses:
  - `0x00-0x01`: High score (2 bytes, int)
  - `0x02`: Magic byte (validation)
- Magic byte (42) ensures valid initialization
- Automatic save on new high score

---

## Code Structure

### File Organisation

```
├── Hardware Configuration
│   ├── Display driver initialisation
│   └── Pin definitions (action + reset buttons)
│
├── Game Constants
│   ├── Display dimensions
│   ├── Physics parameters
│   ├── Game object sizes
│   └── EEPROM addresses
│
├── Data Structures
│   ├── Bird (position, velocity, jump)
│   └── Pipe (position, gap, state)
│
├── Game Logic
│   ├── Initialise/Restart
│   ├── Input Handling
│   ├── Physics Update
│   ├── Collision Detection
│   ├── Scoring System
│   ├── Difficulty Scaling
│   └── High Score Management
│
├── EEPROM Functions
│   ├── Load High Score
│   ├── Save High Score
│   └── Reset High Score
│
└── Rendering
    ├── Game Scene (with dual scores)
    └── Game Over Screen (with celebration)
```

---

## Key Functions Documentation

### Core Game Loop

#### `loop()`
**Purpose**: Main execution loop  
**Frequency**: ~33 FPS (30ms delay)  
**Flow**:
```
Check reset button (anytime)

if (game_active):
    process_input()
    update_physics()
    detect_collisions()
    render_frame()
else:
    show_game_over()
    wait_for_restart()
```

---

### Initialization

#### `initialize_game()`
**Purpose**: Reset all game objects to the starting state  
**Called**: On startup and restart  
**Operations**:
- Bird position: (30, 32)
- Bird velocity: 0
- Pipes: 3 instances with random gaps
- Pipe spacing: 65px ± 10px variation

#### `setup()`
**Purpose**: Hardware initialization and game setup  
**Operations**:
- Initialize U8g2 display
- Configure button pins (INPUT_PULLUP)
- Load high score from EEPROM
- Seed random number generator
- Call `initialize_game()`

---

### EEPROM Management

#### `load_high_score()`
**Purpose**: Load high score from EEPROM with validation  
**Algorithm**:
```
Read magic byte from address 0x02
if (magic != 42):
    Initialize EEPROM (high_score = 0, magic = 42)
else:
    Load high_score from address 0x00
```

**Safety**: Magic byte prevents reading uninitialized EEPROM

#### `save_high_score()`
**Purpose**: Write current high score to EEPROM  
**Trigger**: Called immediately when new high score achieved  
**Address**: 0x00 (2 bytes for int)

#### `reset_high_score()`
**Purpose**: Clear high score and reset EEPROM  
**Trigger**: Reset button (Pin 4) pressed  
**Operations**:
- Set `high_score = 0`
- Clear `new_high_score` flag
- Write to EEPROM

---

### Physics System

#### `Update_Bird()`
**Purpose**: Apply gravity and update bird position  
**Physics Model**:
```
Gravity Constant:     1 pixel/frame²
Terminal Velocity:    2 pixels/frame
Jump Strength:        -3 pixels/frame
```

**Boundary Conditions**:
- Top boundary: Y = 14 (GAME_AREA_TOP + BIRD_SIZE)
- Bottom boundary: Y = 58 (SCREEN_HEIGHT - BIRD_SIZE)
- Collision with boundary → Game Over

---

### Collision Detection

#### `check_collision()`
**Algorithm**: Axis-Aligned Bounding Box (AABB)

**Bird Hitbox**:
```
Left:   bird.x - 7
Right:  bird.x + 7
Top:    bird.y - 6
Bottom: bird.y + 6
```

**Collision Logic**:
1. Check horizontal overlap with each pipe
2. If overlapping:
   - Check if the bird is above the top pipe
   - Check if the bird is below the bottom pipe
3. Return `true` on any collision

**Time Complexity**: O(n) where n = NUM_PIPES

---

### Pipe Management

#### `update_Pipes()`
**Purpose**: Move pipes and recycle off-screen pipes

**Recycling Algorithm**:
```python
for each pipe:
    move left by pipeSpeed
    
    if pipe off-screen:
        Find the rightmost pipe position
        place new pipe with random spacing (35-55px)
        randomise gap position (22-50px)
        reset passed flag
```

**Design Pattern**: Object Pool Pattern (reuses 3 pipe objects)

---

### Scoring System

#### `Update_Score()`
**Trigger**: Bird's X position passes pipe's right edge  
**Implementation**:
```cpp
if (!pipe.passed && pipe.right < bird.x):
    pipe.passed = true
    score++
    
    if (score > high_score):
        high_score = score
        new_high_score = true
        save_high_score()  // Immediate save
```

**State Management**: 
- `passed` flag prevents double-counting
- `new_high_score` flag tracks if current session beat record

---

### Difficulty Progression

#### `Update_Difficulty()`
**Strategy**: Speed scaling based on score thresholds

| Score Range | Pipe Speed | Difficulty |
|-------------|------------|------------|
| 0-9         | 1 px/frame | :star:       |
| 10-19       | 2 px/frame | :star::star:     |
| 20-29       | 3 px/frame | :star::star::star:       |
| 30-39       | 4 px/frame | :star::star::star::star:  |
| 40+         | 5 px/frame | :star::star::star::star::star:     |

**Design Note**: Linear progression every 10 points maintains balanced difficulty curve

---

## Graphics & Rendering

### Bird Sprite
- **Format**: XBM (X Bitmap)
- **Dimensions**: 14x12 pixels
- **Size**: 24 bytes
- **Storage**: PROGMEM (flash memory)
- **Transparency**: Supported (0x00 = transparent)

### Display Layout
```
┌────────────────────────────────────┐
│ Score: XX        Best: XX  (0-7)   │
├────────────────────────────────────┤
│                                    │
│    🐦                 ║            │
│                       ║            │
│  Game Area            ║            │
│   (8-63)              ║            │
│                       ║            │
└────────────────────────────────────┘
```

### Game Over Screen
```
┌────────────────────────────────────┐
│                                    │
│         GAME OVER                  │
│                                    │
│    New high score!  (conditional)  │
│                                    │
│         Score: XX                  │
│                                    │
│    Press any button...             │
└────────────────────────────────────┘
```

### Rendering Performance
- **Frame Rate**: ~33 FPS
- **Buffer Mode**: Full buffer (F) for flicker-free display
- **Draw Calls per Frame**: ~10-15

---

## Memory Usage

### RAM Breakdown
```
Global Variables:
├── Bird struct        : 8 bytes
├── Pipes[3]          : 36 bytes (12 bytes × 3)
├── Game state vars   : 16 bytes (added high_score flags)
├── Bird sprite       : 0 bytes (moved to PROGMEM)
└── U8g2 buffer       : ~1KB

Total Static RAM: ~1.06 KB
Stack Usage:      ~200 bytes
Available (Uno):  ~950 bytes free
```

### Flash Memory
- **Program Size**: ~11KB
- **U8g2 Library**: ~15KB
- **Total**: ~26KB (fits comfortably on 32KB devices)

### EEPROM Usage
```
Address Map:
0x00-0x01: High score (int, 2 bytes)
0x02:      Magic byte (0x2A = 42)
0x03-0xFF: Available for future features
```

---

## Configuration & Tuning

### Adjustable Parameters

#### Game Feel
```cpp
// Make game easier
#define PIPE_GAP_SIZE 30      // Larger gap
int jumpStrength = -4;        // Stronger jump
const uint8_t gravity = 1;    // Keep same

// Make game harder  
#define PIPE_GAP_SIZE 20      // Smaller gap
int jumpStrength = -2;        // Weaker jump
const uint8_t gravity = 2;    // Faster fall
```

#### Difficulty Progression
```cpp
// Slower progression
if (score >= 15) pipeSpeed = 2;
if (score >= 30) pipeSpeed = 3;

// Faster progression
if (score >= 5) pipeSpeed = 2;
if (score >= 10) pipeSpeed = 3;
```

#### Visual Tweaks
```cpp
#define PIPE_WIDTH 12         // Wider pipes
int pipeSpacing = random(40, 70);  // Varied spacing
```

#### EEPROM Settings
```cpp
#define EEPROM_HIGH_SCORE_ADDR 0    // Change storage location
#define EEPROM_MAGIC_VALUE 99       // Custom magic byte
```

---

## Troubleshooting

### Common Issues

#### 1. **Display Not Working**
- **Check**: I2C address (default 0x3C)
- **Fix**: Scan I2C bus, adjust U8G2 constructor
- **Test**: Run U8g2 GraphicsTest example

#### 2. **Bird Moves Too Fast**
- **Cause**: Missing `delay(30)` in loop
- **Fix**: Ensure frame rate limiting is active

#### 3. **Score Not Updating**
- **Cause**: `passed` flag not resetting
- **Fix**: Verify `pipes[i].passed = false` in recycle logic

#### 4. **Collision Detection Off**
- **Cause**: Hitbox size mismatch
- **Fix**: Adjust collision constants to match sprite size

#### 5. **Memory Issues (Uno)**
- **Symptom**: Random crashes, corrupted display
- **Fix**: PROGMEM already used for sprites; consider reducing buffer

#### 6. **High Score Not Saving**
- **Symptom**: Score resets to 0 on power cycle
- **Possible Causes**:
  - EEPROM write failure
  - Magic byte mismatch
- **Debug**: Add Serial prints in `load_high_score()` and `save_high_score()`
- **Fix**: Verify EEPROM library is included

#### 7. **High Score Shows Random Value**
- **Cause**: Uninitialized EEPROM (first upload)
- **Fix**: Magic byte check automatically handles this; if persists, manually reset via reset button

#### 8. **Reset Button Not Working**
- **Check**: Pin 4 wiring and INPUT_PULLUP configuration
- **Test**: Add Serial debug in reset button handler
- **Fix**: Ensure button connected to GND when pressed

---

## Performance Optimization

### Current Optimizations
1. **Object Pooling**: Reuses 3 pipe objects instead of dynamic allocation
2. **Full Buffer Mode**: Switched from page buffer for smoother rendering
3. **PROGMEM Usage**: Bird sprite stored in flash, saving 24 bytes RAM
4. **Integer Math**: Avoids slow floating-point operations
5. **Minimal Branching**: Simple linear difficulty scaling
6. **Efficient EEPROM**: Only writes on actual high score change

### Potential Future Optimizations
1. **Compression**: Implement simple sprite compression
2. **Fixed-Point Math**: For more complex physics
3. **Look-Up Tables**: For common calculations
4. **Lazy EEPROM Writes**: Batch multiple writes (if needed)

---

## Code Style Guide

### Naming Conventions
```cpp
// Constants: UPPER_SNAKE_CASE
#define SCREEN_WIDTH 128
#define EEPROM_HIGH_SCORE_ADDR 0

// Global variables: snake_case or camelCase
int high_score = 0;
bool new_high_score = false;
uint8_t pipeSpeed = 1;

// Functions: PascalCase or snake_case (consistent within project)
void Update_Bird() { }
void draw_game() { }
void load_high_score() { }

// Struct members: camelCase
struct Bird {
    int jumpStrength;
};
```

### Comments
- Use section headers for organisation (===== ... =====)
- Comment "why" not "what"
- Document complex algorithms
- Keep inline comments brief

---

## Contributing Guidelines

### Code Contributions
1. Maintain existing code style
2. Test thoroughly on hardware
3. Document new features in README
4. Keep functions under 50 lines
5. Avoid external dependencies beyond U8g2 and EEPROM
6. Test EEPROM features with power cycles

### Feature Requests
Consider:
- Memory constraints (2KB RAM on Uno)
- Performance impact (maintain 30+ FPS)
- EEPROM wear leveling (100,000 write cycles typical)
- Hardware compatibility

### Bug Reports
Include:
- Arduino model
- Display type
- Steps to reproduce
- Expected vs actual behavior
- High score value when bug occurred
- Serial output if available

---

## Version History

### v1.2 (Current)
**New Features:**
- 🏆 Persistent high score saved to EEPROM
- 🎊 New high score celebration message
- ⭐ Live high score display during gameplay
- 🔄 High score reset button (Pin 4)
- 📊 Dual score tracking (current + best)

**Code Improvements:**
- 💾 PROGMEM optimization for bird sprite
- 🔧 EEPROM magic byte validation
- 📝 Enhanced code documentation
- 🐛 Improved button handling

### v1.0
- Initial release
- Basic Flappy Bird gameplay
- Dynamic difficulty scaling
- Collision detection
- Score tracking (session only)

---

## License & Credits

### Author
Developed for Arduino learning and embedded systems education

### License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

### Libraries Used
- **U8g2**: Monochrome graphics library by olikraus
- **EEPROM**: Arduino built-in EEPROM library

### Inspired By
Original Flappy Bird by Dong Nguyen (2013)

---

## Appendix

### A. Pin Assignment Table

| Pin | Function      | Direction | Notes |
|-----|---------------|-----------|-------|
| D12 | Action Button | INPUT     | Pull-up enabled, Jump/Flap |
| D4  | Reset Button  | INPUT     | Pull-up enabled, Reset high score |
| A4  | I2C SDA       | I/O       | Display data |
| A5  | I2C SCL       | Output    | Display clock |
| A0  | Random        | Input     | Seed for randomSeed() |

### B. Memory Map

```
Flash (Program Memory):
0x0000 - 0x6800: Application code (~26KB)
0x6800 - 0x8000: Available space

RAM (SRAM):
0x0100 - 0x0500: Global variables (~1KB)
0x0500 - 0x08FF: Stack + heap (~950 bytes free)

EEPROM:
0x00-0x01: High score (2 bytes)
0x02:      Magic byte (1 byte)
0x03-0xFF: Available (253 bytes)
```

### C. U8g2 Configuration

```cpp
// Constructor breakdown
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(
    U8G2_R0,    // No rotation
    -1,         // Reset pin (unused)
    A5,         // Clock pin (SCL)
    A4          // Data pin (SDA)
);

// F variant: Full buffer mode (1KB RAM, flicker-free)
// Alternative: _1_ or _2_ for page buffer (less RAM)
```

### D. EEPROM Lifecycle

```
Power On
   ↓
Load High Score
   ├─ Magic byte == 42? → YES → Load high_score
   └─ Magic byte != 42? → NO  → Initialize (0, 42)
   ↓
Game Loop
   ├─ New high score? → Save immediately
   └─ Reset pressed?  → Clear and save
   ↓
Power Off (score persists in EEPROM)
```

### E. Build Instructions

```bash
# Arduino IDE
1. Install U8g2 library (Library Manager)
2. Install EEPROM library (built-in, no action needed)
3. Select board: Arduino Uno
4. Select port: COM3 (or appropriate)
5. Compile and upload

# First Run
- EEPROM automatically initializes
- High score starts at 0
- Play to set your first record!

# Reset High Score
- Hold reset button (Pin 4)
- Score resets to 0 immediately
```

---
## 📬 Contact & Support

- **Issues:** Found a bug? [Open an issue](https://github.com/aydakikio/arduino_snake/issues)
- **Discussions:** Have questions? [Start a discussion](https://github.com/aydakikio/arduino_snake/discussions)
---

**Document Version**: 1.3  

---
<p align="center">
  <a href="#arduino-snake-game-">Back to top ↑</a>
</p>
