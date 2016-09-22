#include <SerialDebug.h>

#include "FastLED.h"


/*
 *code is for the LED Streetlamp.
Written 6/24/2016 by Daniel Pogue (shadowfelldown@gamil.com)
 * rims are arranged as follows:
 *              | Left(1) |       |Right (2)|
 * Top(A)       ( 76 LED )        ( 76 LED )
 * Middle(B)   (  95 LED  )      (  95 LED  )
 * Bottom(C)  (  115 LED   )    (  115 LED   )
 */
#define LEFT_INDEX 1
#define RIGHT_INDEX 2

// Define the number of leds per rim and the number of rims. see above
/*
#define LEDS_RIGHT_A 76
#define LEDS_RIGHT_B 95
#define LEDS_RIGHT_C 115
#define LEDS_LEFT_A 76
#define LEDS_LEFT_B 95
#define LEDS_LEFT_C 115
#define LEDPIN_LEFT 10
#define LEDPIN_RIGHT 5
UNCOMMENT ABOVE WHEN DONE TESTING.*/

#define LEDS_RIGHT_A 12
#define LEDS_RIGHT_B 12
#define LEDS_RIGHT_C 12
#define LEDS_LEFT_A 12
#define LEDS_LEFT_B 12
#define LEDS_LEFT_C 12
#define LEDPIN_LEFT 5
#define LEDPIN_RIGHT 10

//define color order and the type of leds
#define LED_TYPE    WS2812
#define COLOR_ORDER RGB

//Define Brightness and framerate
#define BRIGHTNESS         80
#define FRAMES_PER_SECOND  120
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

//const int LEDS_RIGHT_A = 76;
//const int LEDS_RIGHT_B = 95;
//const int LEDS_RIGHT_C = 115;
//const int LEDS_LEFT_A = 76;
//const int LEDS_LEFT_B = 95;
//const int LEDS_LEFT_C = 115;
const int LEDS_RIGHT_ALL = LEDS_RIGHT_A + LEDS_RIGHT_B + LEDS_RIGHT_C;
const int LEDS_LEFT_ALL = (LEDS_LEFT_A + LEDS_LEFT_B + LEDS_LEFT_C);
//const int LEDPIN_LEFT = 6;
//const int LEDPIN_RIGHT = 5;

//define and allocate memory for all led matrices. commented matrices are for future implementation to control each individual rim.
//int LEDS_1 = LEDS_1a + LEDS_1b + LEDS_1c;
//int LEDS_2 = LEDS_2a + LEDS_2b + LEDS_2c;
CRGB left[LEDS_LEFT_ALL];
//CRGB right[LEDS_RIGHT_ALL];
CRGB right[0];
//CRGB rim_1a[LEDS_1a];
//CRGB rim_1b[LEDS_1b];
//CRGB rim_1c[LEDS_1c];
//CRGB rim_2a[LEDS_2a];
//CRGB rim_2b[LEDS_2b];
//CRGB rim_2c[LEDS_2c];

//more variables. possibly redundant.
  // Here's where we decide what two segments of the LED strip
  // each drawing routine will draw into.  You can set up any 
  // ranges of LEDs that you like here.  For this demo, the "left"
  // 1/4th of the LEDs will draw 'confetti', and the "right" 3/4ths
  // will draw a rainbow.  A few pixels are left blank between them.
 const int blankPixels = 0;
  
  // "Left" segment will be pixels 0..(NUM_LEDS/4)-1, i.e. the low-numbered quarter.
  // If you have 60 pixels, 1a_Start = 0, and 1a_Length = 15.
 const int rightA_Start = 0;
 const int leftA_Start = 0;
 
 const int rightA_Length = LEDS_RIGHT_A;
 const int leftA_Length = LEDS_LEFT_A;
  
  // "Right" segment will be pixels from end of left segment to NUM_PIXELS-1, 
  // i.e. the high-numbered three-quarters of the strip.
  // If you have 60 pixels, 1b_Start = 10, and 1b_Length = 45.
 const int rightB_Start = rightA_Start + rightA_Length + blankPixels;
 const int leftB_Start = leftA_Start + leftA_Length + blankPixels;
 
 const int rightB_Length = LEDS_RIGHT_B;
 const int leftB_Length = LEDS_LEFT_B;
  
  // "Right" segment will be pixels from end of left segment to NUM_PIXELS-1, 
  // i.e. the high-numbered three-quarters of the strip.
  // If you have 60 pixels, 1b_Start = 10, and 1b_Length = 45.
 const int rightC_Start = rightA_Start + rightA_Length + rightB_Start + rightB_Length + blankPixels;
 const int leftC_Start = leftA_Start + leftA_Length + leftB_Start + leftB_Length + blankPixels;
 
 const int rightC_Length = LEDS_RIGHT_C;
 const int leftC_Length = LEDS_LEFT_C;

void setup() {
  
  delay(3000); // 3 second delay for recovery
  SERIAL_DEBUG_SETUP(9600);

  //Configure the left strips.
  FastLED.addLeds<LED_TYPE, LEDPIN_LEFT, COLOR_ORDER>(left, LEDS_LEFT_ALL);

  //Configure the right strip.
  FastLED.addLeds<LED_TYPE, LEDPIN_RIGHT, COLOR_ORDER>(right, LEDS_RIGHT_ALL);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();  
  FastLED.show();  
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbowConfetti_Mirror, rainbowConfetti_Mirror2, allRainbow, allConfetti, allJuggle, allSinelon, mixedBag};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
  
void loop()
{
  
  // Call the current pattern function once, updating the 'leds' array

  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

//Conductor patterns. tell each side which led patterns to use.

void rainbowConfetti_Mirror() {

  drawRainbow(right, rightA_Start, rightA_Length);
  drawRainbow(left, leftA_Start, leftA_Length);
  
  drawConfetti(right, rightB_Start, rightB_Length);
  drawConfetti(left, leftB_Start, leftB_Length);

  drawRainbow(right, rightC_Start, rightC_Length);
  drawRainbow(left, leftC_Start, leftC_Length);
}

void rainbowConfetti_Mirror2() {

  drawConfetti(right, rightA_Start, rightA_Length);
  drawConfetti(left, leftA_Start, leftA_Length);
  
  drawRainbow(right, rightB_Start, rightB_Length);
  drawRainbow(left, leftB_Start, leftB_Length);

  drawConfetti(right, rightC_Start, rightC_Length);
  drawConfetti(left, leftC_Start, leftC_Length);
}

void allRainbow() {

  drawRainbow(right, rightA_Start, rightA_Length);
  drawRainbow(left, leftA_Start, leftA_Length);
  
  drawRainbow(right, rightB_Start, rightB_Length);
  drawRainbow(left, leftB_Start, leftB_Length);

  drawRainbow(right, rightC_Start, rightC_Length);
  drawRainbow(left, leftC_Start, leftC_Length);
}

void allConfetti() {

  drawConfetti(right, rightA_Start, rightA_Length);
  drawConfetti(left, leftA_Start, leftA_Length);
  
  drawConfetti(right, rightB_Start, rightB_Length);
  drawConfetti(left, leftB_Start, leftB_Length);

  drawConfetti(right, rightC_Start, rightC_Length);
  drawConfetti(left, leftC_Start, leftC_Length);
}

void allJuggle() {

  drawJuggle(right, rightA_Start, rightA_Length);
  drawJuggle(left, leftA_Start, leftA_Length);
  
  drawJuggle(right, rightB_Start, rightB_Length);
  drawJuggle(left, leftB_Start, leftB_Length);

  drawJuggle(right, rightC_Start, rightC_Length);
  drawJuggle(left, leftC_Start, leftC_Length);
}

void allSinelon() {

  drawSinelon(right, rightA_Start, rightA_Length);
  drawSinelon(left, leftA_Start, leftA_Length);
  
  drawSinelon(right, rightB_Start, rightB_Length);
  drawSinelon(left, leftB_Start, leftB_Length);

  drawSinelon(right, rightC_Start, rightC_Length);
  drawSinelon(left, leftC_Start, leftC_Length);
}

void mixedBag() {

  drawSinelon(right, rightA_Start, rightA_Length);
  drawJuggle(left, leftA_Start, leftA_Length);
  
  drawRainbow(right, rightB_Start, rightB_Length);
  drawRainbow(left, leftB_Start, leftB_Length);

  drawJuggle(right, rightC_Start, rightC_Length);
  drawSinelon(left, leftC_Start, leftC_Length);
}
// Animation 'draw' functions
// Each animation draw function takes a starting pixel number
// and a count of how many pixels to draw.
// These functions do NOT call FastLED.show() or delay().
//
// For purposes of illustration, the 'classic' way of writing the code
// has been included, commented out, and the new parameterized version
// is included below.

void drawRainbow(CRGB* leds, int startpixel, int pixelcount) {

/*{
  if (LedSide == LEFT_INDEX){
   int leds[] = left[];
  }
  else if (LedSide == RIGHT_INDEX){
   int leds[] = right[];
  }
  */
  // FastLED's built-in rainbow generator
  
  //fill_rainbow( leds             , NUM_LEDS,   gHue, 17);
    fill_rainbow(&(leds[startpixel]),pixelcount, gHue, 17);
      
}

void drawConfetti(CRGB* leds, int startpixel, int pixelcount) 
{
  // random colored speckles that blink in and fade smoothly
  
  //fadeToBlackBy( leds             , NUM_LEDS  , 30);
    fadeToBlackBy( leds, pixelcount, 30);
    
  //int pos = random16( NUM_LEDS);
    int pos = random16( pixelcount);
    
  //left[pos             ] += CHSV( gHue -32 + random8(64), 200, 255);
    left[pos + startpixel] += CHSV( gHue -32 + random8(64), 200, 255);
}
void drawJuggle(CRGB* leds, int startpixel, int pixelcount) {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, pixelcount, 30);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    left[beatsin16(i+7,0,pixelcount)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void drawSinelon(CRGB* leds, int startpixel, int pixelcount)
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds + startpixel, pixelcount, 20);
  int pos = beatsin16(13,0,pixelcount);
  left[pos + startpixel] += CHSV( gHue, 255, 192);
}
