#include <TFT_eSPI.h>

//#include "pet.h"    
//#include "eyepet.h"  
#include "StoneGolem.h"

#include "egg_hatch.h"
   
#include "effect.h" 
#include "background.h"  

TFT_eSPI tft = TFT_eSPI(); 

TFT_eSprite arrowSprite = TFT_eSprite(&tft); 
TFT_eSprite background = TFT_eSprite(&tft);

TFT_eSprite hatchingSprite = TFT_eSprite(&tft);

bool firstStart = true; // Ensure the animation only runs once

unsigned long lastUpdateTime = 0; // Track the last time logic was updated

// GPIO Button Pins
const int feedButton = 26;
const int playButton = 33;
const int healButton = 32;

// Tamagotchi stats
int hunger = 50;   // 0-100 scale
int happiness = 50; // 0-100 scale
int health = 50;   // 0-100 scale

// Timing variables
unsigned long hungerTimer = 0;
unsigned long happinessTimer = 0;
unsigned long healthTimer = 0;

// Animation frames
int currentFrame = 0; // Animation frame index
const int animationDelay = 200; // Delay between frames
unsigned long lastAnimationTime = 0;

bool hungerEffectActive = false; // Tracks if the hunger animation is active
unsigned long hungerEffectStartTime = 0; // Tracks when the effect starts
int hungerEffectFrame = 0; // Tracks the current frame of the effect


void setup() {
  tft.init();
  tft.setRotation(3);
  tft.setSwapBytes(true);

  background.setColorDepth(8);
  background.createSprite(320, 340);
  background.setSwapBytes(true);

  arrowSprite.createSprite(115, 110);

  // Initialize hatching sprite
  //hatchingSprite.setColorDepth(16);
  hatchingSprite.createSprite(115, 110);
  hatchingSprite.setSwapBytes(false);

  // Initialize the display
  background.fillSprite(TFT_BLACK);
  background.pushSprite(0, 0);



  // Set up buttons as inputs
  pinMode(feedButton, INPUT_PULLUP);
  pinMode(playButton, INPUT_PULLUP);
  pinMode(healButton, INPUT_PULLUP);
}

void loop() {
  unsigned long currentTime = millis();

  // Check button press (assuming hunger button is GPIO 26)
  if (digitalRead(26) == LOW) { // Button is pressed
    if (!hungerEffectActive) {
      hungerEffectActive = true;
      hungerEffectStartTime = millis(); // Record start time
      hungerEffectFrame = 0;            // Reset animation frame
    }
  }

  // Handle hunger effect animation
  if (hungerEffectActive) {
    unsigned long currentTime = millis();
    if (currentTime - hungerEffectStartTime > 100) { // Frame duration: 100ms
      hungerEffectStartTime = currentTime; // Reset frame time
      hungerEffectFrame++;                 // Advance to next frame

      if (hungerEffectFrame >= 4) { // End animation after 4 frames
        hungerEffectActive = false;
      }
    }
  }

  // Render the frame
  renderFrame();

  // Check button presses
  if (digitalRead(feedButton) == LOW) {
    feedPet();
    delay(200); // Debounce delay
  }
  if (digitalRead(playButton) == LOW) {
    playWithPet();
    delay(200); // Debounce delay
  }
  if (digitalRead(healButton) == LOW) {
    healPet();
    delay(200); // Debounce delay
  }

  // Update Tamagotchi stats every 5 seconds
  if (currentTime - hungerTimer >= 5000) {
    hunger = max(0, hunger - 5); // Decrease hunger
    hungerTimer = currentTime;
  }
  if (currentTime - happinessTimer >= 7000) {
    happiness = max(0, happiness - 3); // Decrease happiness
    happinessTimer = currentTime;
  }
  if (currentTime - healthTimer >= 10000) {
    health = max(0, health - 1); // Decrease health
    healthTimer = currentTime;
  }

  // Animation Logic
  if (currentTime - lastAnimationTime >= animationDelay) {
    currentFrame = (currentFrame + 1) % 4; // Loop through frames 0-3
    lastAnimationTime = currentTime;

    // Render the current frame
    renderFrame();
  }

  // Check if stats fall too low
  if (hunger <= 0 && happiness <= 0 && health <= 0) {
    displayGameOver();
    //return; // End the loop
  }

  if (firstStart) {
    playHatchingAnimationLoop(); // Run the hatching animation
    firstStart = false;          // Mark animation as done
    hatchingSprite.deleteSprite(); // Free memory after animation
  }
}

void playHatchingAnimationLoop() {
  int delayTime = 300; // Delay between frames

    // Frame 1
    background.pushImage(0, 0, 340, 240, backgroundImage2);
    hatchingSprite.pushImage(0, 0, 115, 110, egg_hatch_1);
    hatchingSprite.pushToSprite(&background, 100, 90, TFT_WHITE); // Adjust position
    background.pushSprite(0, 0);
    delay(delayTime);

    // Frame 2
    background.pushImage(0, 0, 340, 240, backgroundImage2);
    hatchingSprite.pushImage(0, 0, 115, 110, egg_hatch_2);
    hatchingSprite.pushToSprite(&background, 100, 90, TFT_WHITE);
    background.pushSprite(0, 0);
    delay(delayTime);

    // Frame 3
    background.pushImage(0, 0, 340, 240, backgroundImage2);
    hatchingSprite.pushImage(0, 0, 115, 110, egg_hatch_3);
    hatchingSprite.pushToSprite(&background, 100, 90, TFT_WHITE);
    background.pushSprite(0, 0);
    delay(delayTime);

    // Frame 4
    background.pushImage(0, 0, 340, 240, backgroundImage2);
    hatchingSprite.pushImage(0, 0, 115, 110, egg_hatch_4);
    hatchingSprite.pushToSprite(&background, 100, 90, TFT_WHITE);
    background.pushSprite(0, 0);
    delay(delayTime);

    // Frame 5
    background.pushImage(0, 0, 340, 240, backgroundImage2);
    hatchingSprite.pushImage(0, 0, 115, 110, egg_hatch_5);
    hatchingSprite.pushToSprite(&background, 100, 90, TFT_WHITE);
    background.pushSprite(0, 0);
    delay(delayTime); 
       
  while (digitalRead(26) == HIGH) { // Wait until button is pressed
    // Cracking Frame
    background.pushImage(0, 0, 340, 240, backgroundImage2);
    hatchingSprite.pushImage(0, 0, 115, 110, idle_1);
    hatchingSprite.pushToSprite(&background, 100, 90, TFT_WHITE);
    background.pushSprite(0, 0);
    delay(delayTime);
  }
}


void renderFrame() {
  background.pushImage(0, 0, 340, 240, backgroundImage); // Draw the background image
  arrowSprite.setSwapBytes(false); 
  
  // Draw the regular pet animation
  switch (currentFrame) {
    case 0:
      arrowSprite.pushImage(0, 0, 115, 110, idle_1);
      break;
    case 1:
      arrowSprite.pushImage(0, 0, 115, 110, idle_2);
      break;
    case 2:
      arrowSprite.pushImage(0, 0, 115, 110, idle_3);
      break;
    case 3:
      arrowSprite.pushImage(0, 0, 115, 110, idle_4);
      break;
  }

  arrowSprite.pushToSprite(&background, 180, 80, TFT_WHITE); // Place the pet on the background

  // If the hunger effect is active, overlay the effect animation on top of the pet
  if (hungerEffectActive) {
    TFT_eSprite effectSprite = TFT_eSprite(&tft);  // Temporary sprite for the effect
    effectSprite.createSprite(100, 95);            // Match the pet sprite size
    effectSprite.setSwapBytes(false);              // Enable 8-bit color depth if needed

    switch (hungerEffectFrame) {
      case 0:
        effectSprite.pushImage(0, 0, 100, 96, hunger1);
        break;
      case 1:
        effectSprite.pushImage(0, 0, 100, 95, hunger2);
        break;
      case 2:
        effectSprite.pushImage(0, 0, 100, 95, hunger3);
        break;
      case 3:
        effectSprite.pushImage(0, 0, 100, 95, hunger4);
        break;
    }

    effectSprite.pushToSprite(&background, 180, 80, TFT_WHITE); // Overlay effect on top of the pet
    effectSprite.deleteSprite(); // Clean up temporary sprite
  }

  drawStats(); // Render stat bars
  background.pushSprite(0, 0); // Push everything to the display
}

void drawStats() {
  // Define bar properties
  int barWidth = 80;     // Width of stat bars
  int barHeight = 8;     // Height of stat bars
  int barX = 50;         // X-coordinate for all bars
  int hungerY = 73;      // Y-coordinate for hunger bar
  int happinessY = 105;  // Y-coordinate for happiness bar
  int healthY = 137;     // Y-coordinate for health bar

  // Draw Hunger bar
  background.fillRect(barX, hungerY, barWidth, barHeight, TFT_DARKGREY); // Background bar
  background.fillRect(barX, hungerY, hunger, barHeight, TFT_RED);        // Filled portion

  // Draw Happiness bar
  background.fillRect(barX, happinessY, barWidth, barHeight, TFT_DARKGREY); // Background bar
  background.fillRect(barX, happinessY, happiness, barHeight, TFT_YELLOW);  // Filled portion

  // Draw Health bar
  background.fillRect(barX, healthY, barWidth, barHeight, TFT_DARKGREY); // Background bar
  background.fillRect(barX, healthY, health, barHeight, TFT_GREEN);      // Filled portion
}

// Actions triggered by buttons
void feedPet() {
  hunger = min(80, hunger + 20);
}

void playWithPet() {
  happiness = min(80, happiness + 20);
}

void healPet() {
  health = min(80, health + 20);
}

void displayGameOver() {

        // Display the frames of the dying animation
        background.pushImage(0, 0, 340, 240, backgroundImage);
        arrowSprite.pushImage(0, 0, 115, 110, dead_1);
        arrowSprite.pushToSprite(&background, 180, 80, TFT_WHITE);
        background.pushSprite(0, 0);
        delay(300);

        background.pushImage(0, 0, 340, 240, backgroundImage);
        arrowSprite.pushImage(0, 0, 115, 110, dead_2);
        arrowSprite.pushToSprite(&background, 180, 80, TFT_WHITE);
        background.pushSprite(0, 0);
        delay(300);

        while (digitalRead(26) == HIGH) {

        background.pushImage(0, 0, 340, 240, backgroundImage);
        arrowSprite.pushImage(0, 0, 115, 110, dead_3);
        arrowSprite.pushToSprite(&background, 180, 80, TFT_WHITE);
        background.pushSprite(0, 0);
        delay(300);
        
  }
}
