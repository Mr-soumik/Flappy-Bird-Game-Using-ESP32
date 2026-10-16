#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Preferences prefs;

// Pin Definitions
const int BUTTON_PIN = 4;
const int BUZZER_PIN = 18;

// Game States
enum State { SPLASH, PLAYING, GAMEOVER };
State gameState = SPLASH;

// Bird Physics & Dimensions
const int BIRD_X = 22;
float birdY = 26.0;
float birdVelocity = 0.0;
const float gravity = 0.23;
const float jumpForce = -2.6;
bool flapFrame = false;

// Pipe Configuration
const int NUM_PIPES = 2;
const int PIPE_WIDTH = 14;
const int PIPE_GAP = 32;
const int PIPE_DISTANCE = 68;
float pipeX[NUM_PIPES];
int pipeGapY[NUM_PIPES];
bool pipeScored[NUM_PIPES];
float pipeSpeed = 1.35;

// Environment Details
float groundOffset = 0.0;
float cloudX = 100.0;

// Particle Explosion on Crash
struct Particle {
  float x, y, vx, vy;
  bool active;
};
Particle particles[12];

// Scoring
int currentScore = 0;
int highScore = 0;
int currentRound = 1;

// Push Switch Debounce
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 35;

// Sound Effects
void playTone(int freq, int duration) {
  tone(BUZZER_PIN, freq, duration);
}

// 8x8 Animated Bird Sprites (Wing Up & Wing Down)
const unsigned char bird_wing_down[] PROGMEM = {
  0b00111100,
  0b01111110,
  0b11011101,
  0b11111111,
  0b11111100,
  0b01111110,
  0b00110000,
  0b00000000
};

const unsigned char bird_wing_up[] PROGMEM = {
  0b00111100,
  0b01111110,
  0b11011101,
  0b11111111,
  0b11100000,
  0b01111000,
  0b00110000,
  0b00000000
};

// Render 3D Shaded Pipe with Collar Cap
void drawShadedPipe(int x, int y, int width, int height, bool isTop) {
  if (height <= 0) return;

  // Solid pipe body
  display.fillRect(x, y, width, height, SSD1306_WHITE);

  // Vertical highlight slit (gives 3D cylinder curvature)
  display.drawFastVLine(x + 2, y, height, SSD1306_BLACK);

  // Pipe collar dimensions
  int capHeight = 5;
  int capX = x - 2;
  int capWidth = width + 4;
  int capY = isTop ? (y + height - capHeight) : y;

  // Render Cap
  display.fillRect(capX, capY, capWidth, capHeight, SSD1306_WHITE);
  display.drawFastVLine(capX + 2, capY, capHeight, SSD1306_BLACK);
  display.drawRect(capX, capY, capWidth, capHeight, SSD1306_WHITE);
}

void showSplashScreen() {
  display.clearDisplay();

  // Decorative Clouds
  display.fillCircle(20, 16, 8, SSD1306_WHITE);
  display.fillCircle(28, 14, 10, SSD1306_WHITE);
  display.fillCircle(38, 16, 7, SSD1306_WHITE);

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(48, 12);
  display.print("FLAPPY");
  display.setCursor(48, 22);
  display.print("ARCADE 32");

  // Animated Loading Bar
  for (int w = 0; w <= 100; w += 4) {
    display.drawRoundRect(13, 38, 102, 9, 3, SSD1306_WHITE);
    display.fillRoundRect(14, 39, w, 7, 2, SSD1306_WHITE);
    display.display();
    playTone(180 + (w * 9), 12);
    delay(20);
  }

  display.setCursor(22, 53);
  display.print("PRESS TO FLY");
  display.display();

  while (digitalRead(BUTTON_PIN) == HIGH) {
    delay(10);
  }
  playTone(900, 80);
  delay(180);
}

void triggerGameOver() {
  playTone(180, 200);
  delay(100);
  playTone(110, 320);

  // Initialize Particle Burst at Crash Point
  for (int i = 0; i < 12; i++) {
    particles[i].x = BIRD_X + 4;
    particles[i].y = birdY + 4;
    float angle = i * (6.28 / 12);
    particles[i].vx = cos(angle) * (1.2 + (i % 3) * 0.5);
    particles[i].vy = sin(angle) * (1.2 + (i % 3) * 0.5);
    particles[i].active = true;
  }

  // Particle Animation Sequence
  for (int frame = 0; frame < 18; frame++) {
    display.clearDisplay();

    // Draw Static Pipes during crash
    for (int p = 0; p < NUM_PIPES; p++) {
      drawShadedPipe(pipeX[p], 0, PIPE_WIDTH, pipeGapY[p], true);
      int botY = pipeGapY[p] + PIPE_GAP;
      drawShadedPipe(pipeX[p], botY, PIPE_WIDTH, 56 - botY, false);
    }

    // Render Exploding Bits
    for (int i = 0; i < 12; i++) {
      if (particles[i].active) {
        particles[i].x += particles[i].vx;
        particles[i].y += particles[i].vy;
        display.drawPixel((int)particles[i].x, (int)particles[i].y, SSD1306_WHITE);
      }
    }
    display.display();
    delay(15);
  }

  if (currentScore > highScore) {
    highScore = currentScore;
    prefs.putInt("highscore", highScore);
  }
  gameState = GAMEOVER;
}

void resetGame() {
  birdY = 26.0;
  birdVelocity = 0.0;
  currentScore = 0;
  currentRound = 1;
  pipeSpeed = 1.35;

  for (int i = 0; i < NUM_PIPES; i++) {
    pipeX[i] = SCREEN_WIDTH + (i * PIPE_DISTANCE);
    pipeGapY[i] = random(10, 56 - PIPE_GAP - 10);
    pipeScored[i] = false;
  }
  gameState = PLAYING;
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }

  prefs.begin("flappy", false);
  highScore = prefs.getInt("highscore", 0);

  randomSeed(analogRead(0));
  showSplashScreen();
  resetGame();
}

void loop() {
  if (gameState == PLAYING) {
    // Button Input
    int reading = digitalRead(BUTTON_PIN);
    if (reading == LOW && lastButtonState == HIGH && (millis() - lastDebounceTime) > debounceDelay) {
      birdVelocity = jumpForce;
      playTone(650, 25);
      lastDebounceTime = millis();
    }
    lastButtonState = reading;

    // Bird Motion
    birdVelocity += gravity;
    birdY += birdVelocity;

    // Wing Animation Frame Swap
    flapFrame = (birdVelocity < 0);

    // Ceiling & Ground Hitboxes
    if (birdY <= 0 || birdY >= 56 - 6) {
      triggerGameOver();
      return;
    }

    // Parallax Updates
    cloudX -= 0.3;
    if (cloudX < -25) cloudX = 135;

    groundOffset += pipeSpeed;
    if (groundOffset >= 8) groundOffset = 0;

    // Update Pipes
    for (int i = 0; i < NUM_PIPES; i++) {
      pipeX[i] -= pipeSpeed;

      if (pipeX[i] < -PIPE_WIDTH) {
        int prev = (i == 0) ? (NUM_PIPES - 1) : (i - 1);
        pipeX[i] = pipeX[prev] + PIPE_DISTANCE;
        pipeGapY[i] = random(10, 56 - PIPE_GAP - 10);
        pipeScored[i] = false;
      }

      // Point Scoring
      if (pipeX[i] + PIPE_WIDTH < BIRD_X && !pipeScored[i]) {
        currentScore++;
        pipeScored[i] = true;
        playTone(950, 40);

        if (currentScore % 5 == 0) {
          currentRound++;
          pipeSpeed += 0.12;
          playTone(1300, 90);
        }
      }

      // Hitbox Check
      if (BIRD_X + 7 > pipeX[i] && BIRD_X < pipeX[i] + PIPE_WIDTH) {
        if (birdY < pipeGapY[i] || birdY + 7 > pipeGapY[i] + PIPE_GAP) {
          triggerGameOver();
          return;
        }
      }
    }

    // Render Scene
    display.clearDisplay();

    // 1. Background Cloud
    display.fillCircle((int)cloudX, 10, 5, SSD1306_WHITE);
    display.fillCircle((int)cloudX + 5, 8, 6, SSD1306_WHITE);
    display.fillCircle((int)cloudX + 11, 10, 4, SSD1306_WHITE);

    // 2. Solid Shaded Pipes
    for (int i = 0; i < NUM_PIPES; i++) {
      drawShadedPipe(pipeX[i], 0, PIPE_WIDTH, pipeGapY[i], true);
      int botY = pipeGapY[i] + PIPE_GAP;
      drawShadedPipe(pipeX[i], botY, PIPE_WIDTH, 56 - botY, false);
    }

    // 3. Scrolling Striped Ground Line
    display.drawFastHLine(0, 56, 128, SSD1306_WHITE);
    for (int g = -8; g < 128; g += 8) {
      display.drawLine(g + (int)groundOffset, 63, g + 4 + (int)groundOffset, 57, SSD1306_WHITE);
    }

    // 4. Animated Bird
    display.drawBitmap(BIRD_X, (int)birdY, flapFrame ? bird_wing_up : bird_wing_down, 8, 8, SSD1306_WHITE);

    // 5. Clean Arcade HUD
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(2, 2);
    display.print("R");
    display.print(currentRound);

    // Right-aligned Current Score
    display.setCursor(currentScore < 10 ? 116 : (currentScore < 100 ? 110 : 104), 2);
    display.print(currentScore);

    display.display();
    delay(18);
  } 
  else if (gameState == GAMEOVER) {
    display.clearDisplay();

    display.drawRoundRect(8, 4, 112, 56, 4, SSD1306_WHITE);
    display.fillRect(26, 2, 76, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(33, 4);
    display.print("GAME OVER");

    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20, 20);
    display.print("STAGE :");
    display.setCursor(80, 20);
    display.print(currentRound);

    display.setCursor(20, 32);
    display.print("SCORE :");
    display.setCursor(80, 32);
    display.print(currentScore);

    display.setCursor(20, 44);
    display.print("BEST  :");
    display.setCursor(80, 44);
    display.print(highScore);

    display.display();

    delay(700);
    while (digitalRead(BUTTON_PIN) == HIGH) {
      delay(10);
    }
    resetGame();
  }
}
