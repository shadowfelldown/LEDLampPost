#include <SerialDebug.h>

#include "FastLED.h"
#include <time.h>

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

#define LEDS_RIGHT_A 76
#define LEDS_RIGHT_B 95
#define LEDS_RIGHT_C 115
#define LEDS_LEFT_A 76
#define LEDS_LEFT_B 95
#define LEDS_LEFT_C 115
#define LEDPIN_LEFT 22
#define LEDPIN_RIGHT 21
//UNCOMMENT ABOVE WHEN DONE TESTING.*/
/*
#define LEDS_RIGHT_A 10
#define LEDS_RIGHT_B 10
#define LEDS_RIGHT_C 10
#define LEDS_LEFT_A 12
#define LEDS_LEFT_B 12
#define LEDS_LEFT_C 12
#define LEDPIN_LEFT 22
#define LEDPIN_RIGHT 21
//Testing Code*/
//define color order and the type of leds
#define LED_TYPE    WS2812
#define COLOR_ORDER RGB

//Define Brightness and framerate
#define BRIGHTNESS         255
#define FRAMES_PER_SECOND  200
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
bool randomSeeded = false;
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
 const int rightC_Start = rightB_Start + rightB_Length + blankPixels;
 const int leftC_Start = leftB_Start + leftB_Length + blankPixels;
 
 const int rightC_Length = LEDS_RIGHT_C;
 const int leftC_Length = LEDS_LEFT_C;
 
//const int LEDS_RIGHT_A = 76;
//const int LEDS_RIGHT_B = 95;
//const int LEDS_RIGHT_C = 115;
//const int LEDS_LEFT_A = 76;
//const int LEDS_LEFT_B = 95;
//const int LEDS_LEFT_C = 115;
const int LEDS_RIGHT_ALL = LEDS_RIGHT_A + LEDS_RIGHT_B + LEDS_RIGHT_C;
const int LEDS_LEFT_ALL = LEDS_LEFT_A + LEDS_LEFT_B + LEDS_LEFT_C;
//const int LEDPIN_LEFT = 6;
//const int LEDPIN_RIGHT = 5;

//define and allocate memory for all led matrices. commented matrices are for future implementation to control each individual rim.
//int LEDS_1 = LEDS_1a + LEDS_1b + LEDS_1c;
//int LEDS_2 = LEDS_2a + LEDS_2b + LEDS_2c;
CRGB leftRaw[LEDS_LEFT_ALL];
CRGB rightRaw[LEDS_RIGHT_ALL];
//Populate Right CRGBSets//
CRGBSet right(rightRaw, LEDS_RIGHT_ALL);
CRGBSet righta(right(rightA_Start, (rightA_Start+rightA_Length)));
CRGBSet rightb(right(rightB_Start, (rightB_Start+rightB_Length)));
CRGBSet rightc(right(rightC_Start, (rightC_Start+rightC_Length)));
CRGBSet rightAll(right(0,LEDS_RIGHT_ALL));

//Populate Left CRGBSets.//
CRGBSet left(leftRaw, LEDS_LEFT_ALL);
CRGBSet lefta(left(leftA_Start, (leftA_Start+leftA_Length)));
CRGBSet leftb(left(leftB_Start, (leftB_Start+leftB_Length)));
CRGBSet leftc(left(leftC_Start, (leftC_Start+leftC_Length)));
CRGBSet leftAll(left(0,LEDS_LEFT_ALL));

//TESTING//
int cycleCount = 0;
//END - TESTING//
void setup() {

//Testing

//Testing

  delay(3000); // 3 second delay for recovery
  random16_set_seed(analogRead(A0));
  SERIAL_DEBUG_SETUP(38400);

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
//SimplePatternList script_Patterns = {allBlend, allEase, rainbowConfetti_Mirror, rainbowConfetti_Mirror2, allRainbow, allConfetti, allJuggle, allSinelon, mixedBag, Random};
//SimplePatternList script_Patterns = {Random, Random, Random, Random, Random, Random, Random, Random};
SimplePatternList script_Patterns = {testPattern, testPattern, testPattern, testPattern, testPattern, testPattern, testPattern, testPattern};
uint8_t script_CurrentPatternNumber = 0; // Index number of which pattern is current

typedef void (*IndividualPatterns[])(CRGB* leds, int pixelcount);
IndividualPatterns indep_Patterns = { drawRainbow, drawConfetti, drawJuggle, drawSinelon, drawBlend, drawEase};

uint8_t indep_CurrentPatternNumber = 0; // Index number of which pattern is current
void loop()
{
   DEBUG("Looped", millis());
  // Call the current pattern function once, updating the 'leds' array

  script_Patterns[script_CurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
const int randomSize = ARRAY_SIZE(indep_Patterns);
int randomArray[randomSize];
void nextPattern()
{
  DEBUG("Called Next Pattern", millis());
  // add one to the current pattern number, and wrap around at the end
  script_CurrentPatternNumber = random16(ARRAY_SIZE(script_Patterns));
  //script_CurrentPatternNumber = (script_CurrentPatternNumber + 1) % ARRAY_SIZE( script_Patterns);
  indep_CurrentPatternNumber = (indep_CurrentPatternNumber + 1) % ARRAY_SIZE( indep_Patterns);
  // Turn off random
    randomSeeded = false;
}
void indep_nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  indep_CurrentPatternNumber = (indep_CurrentPatternNumber + 1) % ARRAY_SIZE( indep_Patterns);
}
//Conductor patterns. tell each side which led patterns to use.

void testPattern() {
 
  fill_solid(righta,rightA_Length, CHSV(0,255,255));
  fill_solid(lefta,leftA_Length, CHSV(0,255,255));
  
  fill_solid(rightb, rightB_Length, CHSV(124,255,255));
  fill_solid(leftb, leftB_Length, CHSV(124,255,255));

  fill_solid(rightc, rightC_Length,CHSV(255,255,255));
  fill_solid(leftc, leftC_Length,CHSV(255,255,255));
}
void rainbowConfetti_Mirror() {
DEBUG("rainbowConfetti_Mirror", millis());
  drawRainbow(righta, rightA_Length);
  drawConfetti(rightb, rightB_Length);
  drawRainbow(rightc, rightC_Length);
  drawRainbow(lefta, leftA_Length);
  drawConfetti(leftb, leftB_Length);
  drawRainbow(leftc, leftC_Length);

  
}

void rainbowConfetti_Mirror2() {
DEBUG("rainbowConfetti_Mirror2", millis());
  drawConfetti(righta,rightA_Length);
  drawConfetti(lefta,leftA_Length);
  
  drawRainbow(rightb, rightB_Length);
  drawRainbow(leftb,leftB_Length);

  drawConfetti(rightc,rightC_Length);
  drawConfetti(leftc,leftC_Length);
}

void allRainbow() {

  drawRainbow(righta,rightA_Length);
  drawRainbow(lefta,leftA_Length);
  
  drawRainbow(rightb, rightB_Length);
  drawRainbow(leftb, leftB_Length);

  drawRainbow(rightc, rightC_Length);
  drawRainbow(leftc, leftC_Length);
}

void allConfetti() {

  drawConfetti(righta, rightA_Length);
  drawConfetti(lefta, leftA_Length);
  
  drawConfetti(rightb, rightB_Length);
  drawConfetti(leftb, leftB_Length);

  drawConfetti(rightc, rightC_Length);
  drawConfetti(leftc, leftC_Length);
}

void allJuggle() {

  drawJuggle(rightAll, rightA_Length);
  drawJuggle(leftAll, leftA_Length);
  
  drawJuggle(rightAll, rightB_Length);
  drawJuggle(leftAll, leftB_Length);

  drawJuggle(rightAll, rightC_Length);
  drawJuggle(leftAll, leftC_Length);
}

void allSinelon() {

  drawSinelon(rightAll, rightA_Length);
  drawSinelon(leftAll, leftA_Length);
  
  drawSinelon(rightAll, rightB_Length);
  drawSinelon(leftAll, leftB_Length);

  drawSinelon(rightAll, rightC_Length);
  drawSinelon(leftAll, leftC_Length);
}

void mixedBag() {

  drawSinelon(righta, rightA_Length);
  drawJuggle(lefta, leftA_Length);
  
  drawRainbow(rightb, rightB_Length);
  drawRainbow(leftb, leftB_Length);

  drawJuggle(rightc, rightC_Length);
  drawSinelon(leftc, leftC_Length);
}

void Random() {
  DEBUG("random Seeded", randomSeeded);
if (randomSeeded == false){
for (int randID=0; randID < randomSize; randID++){
  randomArray[randID] = random16(randomSize);
  DEBUG("array contents", randomArray[randID]);
}
randomSeeded = true;
DEBUG("random Seeded", randomSeeded);
}
else{
  indep_Patterns[randomArray[0]](righta, rightA_Length);
  indep_Patterns[randomArray[1]](lefta, leftA_Length);
  
  indep_Patterns[randomArray[2]](rightb, rightB_Length);
  indep_Patterns[randomArray[3]](leftb, leftB_Length);

  indep_Patterns[randomArray[4]](rightc, rightC_Length);
  indep_Patterns[randomArray[5]](leftc, leftC_Length);
  cycleCount = (cycleCount+1);
  DEBUG("Printed LEDS", cycleCount);
}
}
void allEase() {

  drawEase(righta, rightA_Length);
  drawEase(lefta, leftA_Length);
  
  drawEase(rightb, rightB_Length);
  drawEase(leftb, leftB_Length);

  drawEase(rightc, rightC_Length);
  drawEase(leftc, leftC_Length);
}
void allBlend() {

  drawBlend(righta, rightA_Length);
  drawBlend(lefta, leftA_Length);
  
  drawBlend(rightb, rightB_Length);
  drawBlend(leftb, leftB_Length);

  drawBlend(rightc, rightC_Length);
  drawBlend(leftc, leftC_Length);
}
// Animation 'draw' functions
// Each animation draw function takes a starting pixel number
// and a count of how many pixels to draw.
// These functions do NOT call FastLED.show() or delay().
//
// For purposes of illustration, the 'classic' way of writing the code
// has been included, commented out, and the new parameterized version
// is included below.

void Blend() {

  drawBlend(righta, rightA_Length);
  drawBlend(lefta, leftA_Length);
  
  drawBlend(rightb, rightB_Length);
  drawBlend(leftb, leftB_Length);

  drawBlend(rightc, rightC_Length);
  drawBlend(leftc, leftC_Length);
}
// Animation 'draw' functions
// Each animation draw function takes a starting pixel number
// and a count of how many pixels to draw.
// These functions do NOT call FastLED.show() or delay().
//
// For purposes of illustration, the 'classic' way of writing the code
// has been included, commented out, and the new parameterized version
// is included below.

void drawRainbow(CRGB* leds, int pixelcount) {

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
    fill_rainbow(leds , pixelcount, gHue, 17);
      
}

void drawSolid(CRGB* leds, int pixelcount) {
fill_solid( leds, pixelcount, CHSV(random16(255),100,100)); 
}
void drawConfetti(CRGB* leds, int pixelcount) 
{
  // random colored speckles that blink in and fade smoothly
  
  //fadeToBlackBy( leds             , NUM_LEDS  , 30);
    fadeToBlackBy( leds, pixelcount, 30);
    
  //int pos = random16( NUM_LEDS);
    int pos = random16( pixelcount);
    
  //leds[pos             ] += CHSV( gHue -32 + random8(64), 200, 255);
    leds[pos] += CHSV( gHue -32 + random8(64), 200, 255);
}
void drawJuggle(CRGB* leds, int pixelcount) {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, pixelcount, 30);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,pixelcount)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void drawSinelon(CRGB* leds, int pixelcount)
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, pixelcount, 20);
  int pos = beatsin16(13,0,pixelcount);
  leds[pos] += CHSV( gHue, 255, 192);
}
void drawBlend(CRGB* leds, int pixelcount) {
  uint8_t starthue = gHue;
  uint8_t endhue = gHue % random16(255);
  if (starthue < endhue) {
    fill_gradient(leds, pixelcount, CHSV(starthue,255,255), CHSV(endhue,255,255), FORWARD_HUES);    // If we don't have this, the colour fill will flip around
  } else {
    fill_gradient(leds, pixelcount, CHSV(starthue,255,255), CHSV(endhue,255,255), BACKWARD_HUES);
  }
} // blendme()

void drawEase(CRGB* leds, int pixelcount) {
  
  static uint8_t easeOutVal = 0;
  static uint8_t easeInVal  = 0;
  static uint8_t lerpVal    = 0;
  uint8_t ColorHue = gHue;

  easeOutVal = ease8InOutQuad(easeInVal);
  easeInVal++;

  lerpVal = lerp8by8(0, pixelcount, easeOutVal);

  leds[lerpVal] = CHSV(ColorHue,255,255);
  fadeToBlackBy(leds, pixelcount, 16);                     // 8 bit, 1 = slow, 255 = fast
} // loop()
