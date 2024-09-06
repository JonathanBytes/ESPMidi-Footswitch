#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <map>
#include <string>

// OLED display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// Initialize the OLED display with I2C address 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Animation frame data arrays
#include "angrycat.h"
#include "music.h"
#include "pullups.h"
#include "skateboarding.h"
#include "tune.h"

// Define a struct to hold animation details
struct Animation {
  const byte (*frames)[128];
  int frameCount;
  int frameWidth;
  int frameHeight;
  int frameDelay;
};

// Define a map for storing animations by name
std::map<std::string, Animation> animations;

// Function to initialize the animations
void setupAnimations() {
  animations["skateboarding"] = {frames, FRAME_COUNT, FRAME_WIDTH, FRAME_HEIGHT,
                                 FRAME_DELAY};
  animations["angrycat"] = {frames2, FRAME_COUNT2, FRAME_WIDTH2, FRAME_HEIGHT2,
                            FRAME_DELAY2};
  animations["tune"] = {frames3, FRAME_COUNT3, FRAME_WIDTH3, FRAME_HEIGHT3,
                        FRAME_DELAY3};
  animations["music"] = {frames4, FRAME_COUNT4, FRAME_WIDTH4, FRAME_HEIGHT4,
                         FRAME_DELAY4};
  animations["pullups"] = {frames5, FRAME_COUNT5, FRAME_WIDTH5, FRAME_HEIGHT5,
                           FRAME_DELAY5};
}

// Function to draw text on the display
void drawText(const std::string &text) {
  display.clearDisplay();
  display.setTextSize(2);      // Set text size to 1
  display.setTextColor(WHITE); // Set text color to white
  display.setCursor(14, 13);   // Set cursor to top-left corner
  display.print(text.c_str()); // Draw text
  display.display();           // Update display
}

// Function to display an animation by name with looping options
void displayAnimation(const std::string &animationName, int loops = 1,
                      bool infiniteLoop = false) {
  auto it = animations.find(animationName);
  if (it != animations.end()) {
    const Animation &anim = it->second;
    int currentLoop = 0;

    do {
      for (int i = 0; i < anim.frameCount; i++) {
        display.clearDisplay();
        display.drawBitmap(48, 0, anim.frames[i], anim.frameWidth,
                           anim.frameHeight, WHITE);
        display.display();
        delay(anim.frameDelay);
      }
      if (!infiniteLoop) {
        currentLoop++;
      }
    } while (infiniteLoop || currentLoop < loops);

  } else {
    Serial.println(("Animation not found: " + animationName).c_str());
  }
}

void bootAnimation() {
  displayAnimation("skateboarding", 1);
  displayAnimation("angrycat", 1);
  displayAnimation("tune", 1);
  displayAnimation("music", 1);
  displayAnimation("pullups", 1);
}
