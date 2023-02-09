#include <IRremote.h> // include the IRremote library\

int redR = 12;
int redG = 3;
int redB = 4;
int yellowR = 5;
int yellowG = 6;
int yellowB = 7;
int greenR = 8;
int greenG = 11;
int greenB = 13;

//Sound detection
bool detectSound = false;
const int soundPin = A5;
const int threshold = 500;
unsigned int currentTime;
unsigned int interval = 50;
unsigned int nextTime;
unsigned int soundSpike;

#define IR_RECEIVE_PIN 23 // define the IR receiver pin
unsigned long key_value = 0; // variable to store the pressed key value


int maxBrightness = 255;

int redRBrightness = 170;    // how bright the LED is
int redGBrightness = 0;
int redBBrightness = 170;
int yellowRBrightness = 170;    // how bright the LED is
int yellowGBrightness = 0;
int yellowBBrightness = 170;
int greenRBrightness = 170;    // how bright the LED is
int greenGBrightness = 0;
int greenBBrightness = 170;

int rFadeAmount = -10;    // how many points to fade the LED by
int gFadeAmount = 10;    // how many points to fade the LED by
int bFadeAmount = 10;    // how many points to fade the LED by

double delayInterval = 0;
float nextDelay = 0;
int deltaB;
double timeOfFade = 1500;
bool fading = false;

bool powerOn = true;
bool firstClick = false;
int firstPower;
int colorPickMode = 0;

bool strobe = false;
bool smooth = false;
bool playingSmooth = false;


constexpr long button[6][4] = {
  { 4127850240, 3793616640, 3760193280, 4061003520},
  { 3860463360, 3827040000, 3994156800, 3927310080},
  { 3893886720, 3977445120, 3910598400, 2991456000},
  { 3208707840, 3008167680, 4211408640, 4278255360},
  { 4111138560, 3776904960, 4044291840, 3843751680},
  { 3810328320, 3944021760, 4027580160, 4077715200}
};



void setup() {
  pinMode(redR, OUTPUT);
  pinMode(redG, OUTPUT);
  pinMode(redB, OUTPUT);
  pinMode(yellowR, OUTPUT);
  pinMode(yellowG, OUTPUT);
  pinMode(yellowB, OUTPUT);
  pinMode(greenR, OUTPUT);
  pinMode(greenG, OUTPUT);
  pinMode(greenB, OUTPUT);
  pinMode(soundPin, INPUT);
  nextTime = millis() + interval;

  Serial.begin(115200); // begin serial communication with a baud rate of 9600
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Start the receiver
  IrReceiver.blink13(false); // enable blinking of the built-in LED when an IR signal is received


}

void loop() {

 /* Serial.print("RedR: ");
  Serial.print(yellowRBrightness);
  Serial.print(" YellowR: ");
  Serial.print(yellowGBrightness);
  Serial.print(" GreenR: ");
  Serial.print(yellowBBrightness);
  Serial.print(" ColorPickMode: ");
  Serial.print(colorPickMode);
  Serial.print(" Brightness: ");
  Serial.println(maxBrightness);
*/

  //Serial.println(maxBrightness);

  sensorController();
  modeController();
  if(nextDelay < millis())
  {
    brightnessController();
    if(detectSound)
      soundDetection();
  }
  

  //delay(delayInterval);


}

void modeController()
{
  if (colorPickMode > 0)
  {
    whiteBlink(colorPickMode);
  }
}

void sensorController()
{
  if (IrReceiver.decode()) {
    Serial.println(IrReceiver.decodedIRData.decodedRawData); // Prints decimal value of IR recieved
    switch (IrReceiver.decodedIRData.decodedRawData)
    {
      case (button[0][0]):          // Brightness up
        if (maxBrightness < 255)
        {
          if (maxBrightness == 0 || powerOn == false)
          {
            maxBrightness = 15;
            powerOn = true;
          }
          else
          {
            maxBrightness += 30;
          }

          if (fading)
            resetRGB();
        }
        else
          maxBrightness = 255;
        break;

      case (button[0][1]):           // Brightness down
        if (maxBrightness > 15)
        {
          maxBrightness -= 30;
        }
        else
        {
          maxBrightness = 0;
          powerOn = false;
        }
        if (fading)
          resetRGB();
        break;

      case (button[0][2]):              // Power off
        if (powerOn && colorPickMode > 0)
        {
          colorPickMode = 0;
          break;
        }
        powerOn = false;
        break;

      case (button[0][3]):              // Power on 

        if (powerOn)
        {
          switch (colorPickMode)
          {
            case 0:
              colorPickMode = 1;
              break;

            case 1:
              colorPickMode = 2;
              break;

            case 2:
              colorPickMode = 3;
              break;

            case 3:
              colorPickMode = 1;
              break;
          }
        }

        powerOn = true;
        if (maxBrightness == 0)
          maxBrightness = 255;

        break;

      case (button[1][0]):              //Red
        if (colorPickMode == 0)
        {
          changeAllColors(255, 0, 0);
        }
        else
          changeColor(colorPickMode, 255, 0, 0);
        fading = false;
        colorPickMode = 0;
        break;

      case (button[1][1]):             //Green
        if (colorPickMode == 0)
        {
          changeAllColors(0, 255, 0);
        }
        else
          changeColor(colorPickMode, 0, 255, 0);
        fading = false;
        colorPickMode = 0;
        break;

      case (button[1][2]):             //Blue
        if (colorPickMode == 0)
        {
          changeAllColors(0, 0, 255);
        }
        else
          changeColor(colorPickMode, 0, 0, 255);
        fading = false;
        colorPickMode = 0;
        break;

      case (button[1][3]):             //White
        if (colorPickMode == 0)
        {
          changeColor(1, 255, 0, 0);
          changeColor(2, 255, 75, 0);
          changeColor(3, 0, 255, 0);
        }
        else
          changeColor(colorPickMode, 255, 255, 255);
        fading = false;
        colorPickMode = 0;
        break;

      case (button[2][0]):             //Dark red/brown
        if (colorPickMode == 0)
        {
          changeAllColors(255, 10, 0);
        }
        else
          changeColor(colorPickMode, 255, 10, 0);
        fading = false;
        colorPickMode = 0;
        break;

      case (button[2][1]):             //Light Green
        if (colorPickMode == 0)
        {
          changeAllColors(106, 255, 0);
        }
        else
          changeColor(colorPickMode, 106, 255, 0);
        fading = false;
        colorPickMode = 0;
        break;

      case (button[2][2]):             //Royal Blue
        if (colorPickMode == 0)
        {
          changeAllColors(30, 0, 255);
        }
        else
          changeColor(colorPickMode, 30, 0, 255);
        fading = false;
        colorPickMode = 0;
        break;

      case (button[3][0]):             //Orange
        if (colorPickMode == 0)
        {
          changeAllColors(255, 21, 0);
        }
        else
          changeColor(colorPickMode, 255, 21, 0);
        fading = false;
        colorPickMode = 0;
        break;

      case (button[3][1]):             //Cyan
        if (colorPickMode == 0)
        {
          changeAllColors(0, 183, 255);
        }
        else
          changeColor(colorPickMode, 0, 183, 255);
        fading = false;
        colorPickMode = 0;
        break;

      case (button[3][2]):             //Violet
        if (colorPickMode == 0)
        {
          changeAllColors(64, 0, 255);
        }
        else
          changeColor(colorPickMode, 64, 0, 255);
        fading = false;
        colorPickMode = 0;
        break;

      case (button[4][0]):             //Light Orange
        if (colorPickMode == 0)
        {
          changeAllColors(255, 30, 0);
        }
        else
          changeColor(colorPickMode, 255, 45, 0);
        fading = false;
        colorPickMode = 0;
        break;

      case (button[4][1]):             //Turquoise
        if (colorPickMode == 0)
        {
          changeAllColors(0, 89, 255);
        }
        else
          changeColor(colorPickMode, 70, 0, 199);
        fading = false;
        colorPickMode = 0;
        break;

      case (button[4][2]):             //Indigo
        if (colorPickMode == 0)
        {
          changeAllColors(157, 0, 255);
        }
        else
          changeColor(colorPickMode, 157, 0, 255);
        fading = false;
        colorPickMode = 0;
        break;


      case (button[5][0]):             //Yellow
        if (colorPickMode == 0)
        {
          changeAllColors(255, 60, 0);
        }
        else
          changeColor(colorPickMode, 255, 60, 0);
        fading = false;
        colorPickMode = 0;
        break;

      case (button[5][1]):             //Dark Turquoise
        if (colorPickMode == 0)
        {
          changeAllColors(0, 32, 255);
        }
        else
          changeColor(colorPickMode, 0, 32, 255);
        fading = false;
        colorPickMode = 0;
        break;

      case (button[5][2]):             //Pink
        if (colorPickMode == 0)
        {
          changeAllColors(255, 0, 200);
        }
        else
          changeColor(colorPickMode, 255, 0, 200);
        fading = false;
        colorPickMode = 0;
        break;

      case (button[4][3]):             //Rainbow Fade
        fading = true;
        resetRGB();
        break;

      case (button[3][3]):             // Strobe
        strobe = !strobe;
        break;

      case (button [5][3]):           // Smooth
        detectSound = !detectSound;
        break;

    }


    IrReceiver.resume();
  }
}

void brightnessController()
{
  
  if (powerOn)
  {

    if (strobe == true)
    {
      strobeFlash();
      return;
    }

    if(smooth && !playingSmooth)
    {
      playSmooth();
      return;
    }
    
    if (fading)
    {
      if(!detectSound)
      {
      if(maxBrightness < 105 && powerOn)
        maxBrightness = 105;
        
      setFadeAmount(1);
      delayInterval = (double)(timeOfFade / ((double)(maxBrightness / abs(rFadeAmount)))) ; // Schoefflin Equation
      nextDelay = delayInterval + millis();
      //Serial.println(delayInterval);

      if (redRBrightness > maxBrightness)     // Red RGB brightness check
        redRBrightness = maxBrightness;
      if (redRBrightness < 0)
        redRBrightness = 0;

      if (redGBrightness > maxBrightness)
        redGBrightness = maxBrightness;
      if (redGBrightness < 0)
        redGBrightness = 0;

      if (redBBrightness > maxBrightness)
        redBBrightness = maxBrightness;
      if (redBBrightness < 0)
        redBBrightness = 0;

      if (yellowRBrightness > maxBrightness)   // Yellow RGB brightness check
        yellowRBrightness = maxBrightness;
      if (yellowRBrightness < 0)
        yellowRBrightness = 0;

      if (yellowGBrightness > maxBrightness)
        yellowGBrightness = maxBrightness;
      if (yellowGBrightness < 0)
        yellowGBrightness = 0;

      if (yellowBBrightness > maxBrightness)
        yellowBBrightness = maxBrightness;
      if (yellowBBrightness < 0)
        yellowBBrightness = 0;

      if (greenRBrightness > maxBrightness)     // Green RGB brightness check
        greenRBrightness = maxBrightness;
      if (greenRBrightness < 0)
        greenRBrightness = 0;

      if (greenGBrightness > maxBrightness)
        greenGBrightness = maxBrightness;
      if (greenGBrightness < 0)
        greenGBrightness = 0;

      if (greenBBrightness > maxBrightness)
        greenBBrightness = maxBrightness;
      if (greenBBrightness < 0)
        greenBBrightness = 0;



      
      
      analogWrite(redR, map(redRBrightness * .4, 0, 255, 0, maxBrightness));
      analogWrite(redG, map(redGBrightness, 0, 255, 0, maxBrightness));
      analogWrite(redB, map(redBBrightness, 0, 255, 0, maxBrightness));
      analogWrite(yellowR, map(yellowRBrightness * .4, 0, 255, 0, maxBrightness));
      analogWrite(yellowG, map(yellowGBrightness, 0, 255, 0, maxBrightness));
      analogWrite(yellowB, map(yellowBBrightness, 0, 255, 0, maxBrightness));
      analogWrite(greenR, map(greenRBrightness * .4, 0, 255, 0, maxBrightness));
      analogWrite(greenG, map(greenGBrightness, 0, 255, 0, maxBrightness));
      analogWrite(greenB, map(greenBBrightness, 0, 255, 0, maxBrightness));
      }
    }   


    else
    {
      if(!detectSound)
      {
      analogWrite(redR, map(redRBrightness * .4, 0, 255, 0, maxBrightness));
      analogWrite(redG, map(redGBrightness, 0, 255, 0, maxBrightness));
      analogWrite(redB, map(redBBrightness, 0, 255, 0, maxBrightness));
      analogWrite(yellowR, map(yellowRBrightness * .4, 0, 255, 0, maxBrightness));
      analogWrite(yellowG, map(yellowGBrightness, 0, 255, 0, maxBrightness));
      analogWrite(yellowB, map(yellowBBrightness, 0, 255, 0, maxBrightness));
      analogWrite(greenR, map(greenRBrightness * .4, 0, 255, 0, maxBrightness));
      analogWrite(greenG, map(greenGBrightness, 0, 255, 0, maxBrightness));
      analogWrite(greenB, map(greenBBrightness, 0, 255, 0, maxBrightness));
      }
    }

  }
  else
  {
    analogWrite(redR, 0);
    analogWrite(redG, 0);
    analogWrite(redB, 0);
    analogWrite(yellowR, 0);
    analogWrite(yellowG, 0);
    analogWrite(yellowB, 0);
    analogWrite(greenR, 0);
    analogWrite(greenG, 0);
    analogWrite(greenB, 0);
  }


  if (fading && !detectSound)
  {
    // change the brightness for next time through the loop:
    redRBrightness = redRBrightness + rFadeAmount;
    redGBrightness = redGBrightness + gFadeAmount;
    redBBrightness = redBBrightness + bFadeAmount;
    yellowRBrightness = yellowRBrightness + rFadeAmount;
    yellowGBrightness = yellowGBrightness + gFadeAmount;
    yellowBBrightness = yellowBBrightness + bFadeAmount;
    greenRBrightness = greenRBrightness + rFadeAmount;
    greenGBrightness = greenGBrightness + gFadeAmount;
    greenBBrightness = greenBBrightness + bFadeAmount;

    // reverse the direction of the fading at the ends of the fade:
    if (redRBrightness <= 0 || redRBrightness >= maxBrightness) {
      rFadeAmount = -rFadeAmount;
    }

    // reverse the direction of the fading at the ends of the fade:
    if (redGBrightness <= 0 || redGBrightness >= maxBrightness) {
      gFadeAmount = -gFadeAmount;
    }

    // reverse the direction of the fading at the ends of the fade:
    if (redBBrightness <= 0 || redBBrightness >= maxBrightness) {
      bFadeAmount = -bFadeAmount;
    }
  }
}

void setFadeAmount(int fadeAmount)
{
  if (fadeAmount < 1)
    fadeAmount = 1;

  if (rFadeAmount < 0)
    rFadeAmount = -fadeAmount;
  else
    rFadeAmount = fadeAmount;

  if (gFadeAmount < 0)
    gFadeAmount = -fadeAmount;
  else
    gFadeAmount = fadeAmount;

  if (bFadeAmount < 0)
    bFadeAmount = -fadeAmount;
  else
    bFadeAmount = fadeAmount;

}

void resetRGB()
{
  redRBrightness = 2 * maxBrightness / 3;
  redGBrightness = 0;
  redBBrightness = 2 * maxBrightness / 3;
  yellowRBrightness = 2 * maxBrightness / 3;
  yellowGBrightness = 0;
  yellowBBrightness = 2 * maxBrightness / 3;
  greenRBrightness = 2 * maxBrightness / 3;
  greenGBrightness = 0;
  greenBBrightness = 2 * maxBrightness / 3;
  rFadeAmount = -abs(rFadeAmount);
  gFadeAmount = abs(gFadeAmount);
  bFadeAmount = abs(gFadeAmount);




}

void whiteBlink(int lightID)
{
  if (lightID == 1)
  {
    analogWrite(redR, map(redRBrightness * .4, 0, 255, 0, maxBrightness));
    analogWrite(redG, map(redGBrightness, 0, 255, 0, maxBrightness));
    analogWrite(redB, map(redBBrightness, 0, 255, 0, maxBrightness));
    delay(500);
    analogWrite(redR, map(redRBrightness * .4, 0, maxBrightness, 0, 20));
    analogWrite(redG, map(redGBrightness, 0, maxBrightness, 0, 20));
    analogWrite(redB, map(redBBrightness, 0, maxBrightness, 0, 20));
    delay(500);
  }

  else if (lightID == 2)
  {
    analogWrite(yellowR, map(yellowRBrightness * .4, 0, 255, 0, maxBrightness));
    analogWrite(yellowG, map(yellowGBrightness, 0, 255, 0, maxBrightness));
    analogWrite(yellowB, map(yellowBBrightness, 0, 255, 0, maxBrightness));
    delay(500);
    analogWrite(yellowR, map(yellowRBrightness * .4, 0, maxBrightness, 0, 20));
    analogWrite(yellowG, map(yellowGBrightness, 0, maxBrightness, 0, 20));
    analogWrite(yellowB, map(yellowBBrightness, 0, maxBrightness, 0, 20));
    delay(500);
  }
  else if (lightID == 3)
  {
    analogWrite(greenR, map(greenRBrightness * .4, 0, 255, 0, maxBrightness));
    analogWrite(greenG, map(greenGBrightness, 0, 255, 0, maxBrightness));
    analogWrite(greenB, map(greenBBrightness, 0, 255, 0, maxBrightness));
    delay(500);
    analogWrite(greenR, map(greenRBrightness * .4, 0, maxBrightness, 0, 20));
    analogWrite(greenG, map(greenGBrightness, 0, maxBrightness, 0, 20));
    analogWrite(greenB, map(greenBBrightness, 0, maxBrightness, 0, 20));
    delay(500);
  }
}

void strobeFlash()
{
  if (strobe == true)
  {
      analogWrite(redR, map(redRBrightness * .4, 0, 255, 0, maxBrightness));
      analogWrite(redG, map(redGBrightness, 0, 255, 0, maxBrightness));
      analogWrite(redB, map(redBBrightness, 0, 255, 0, maxBrightness));
      analogWrite(yellowR, map(yellowRBrightness * .4, 0, 255, 0, maxBrightness));
      analogWrite(yellowG, map(yellowGBrightness, 0, 255, 0, maxBrightness));
      analogWrite(yellowB, map(yellowBBrightness, 0, 255, 0, maxBrightness));
      analogWrite(greenR, map(greenRBrightness * .4, 0, 255, 0, maxBrightness));
      analogWrite(greenG, map(greenGBrightness, 0, 255, 0, maxBrightness));
      analogWrite(greenB, map(greenBBrightness, 0, 255, 0, maxBrightness));
    delay(30);
    analogWrite(redR, 0);
    analogWrite(redG, 0);
    analogWrite(redB, 0);
    analogWrite(yellowR, 0);
    analogWrite(yellowG, 0);
    analogWrite(yellowB, 0);
    analogWrite(greenR, 0);
    analogWrite(greenG, 0);
    analogWrite(greenB, 0);
    delay(30);
  }
}

void playSmooth()
{

  int randomR, randomY, randomG = random(5, 15);

  

}


void changeColor(int lightID, int r, int g, int b)
{
  if (lightID == 1)
  {
    redRBrightness = r;
    redGBrightness = g;
    redBBrightness = b;

  }
  else if (lightID == 2)
  {
    yellowRBrightness = r;
    yellowGBrightness = g;
    yellowBBrightness = b;
  }
  else if (lightID == 3)
  {
    greenRBrightness = r;
    greenGBrightness = g;
    greenBBrightness = b;

  }
}
void changeAllColors(int r, int g, int b)
{
  redRBrightness = r;
  redGBrightness = g;
  redBBrightness = b;
  yellowRBrightness = r;
  yellowGBrightness = g;
  yellowBBrightness = b;
  greenRBrightness = r;
  greenGBrightness = g;
  greenBBrightness = b;

}

void fadeTransition(String direction, int light)
{
  if(direction == "in")
  {
    
  }
  else
  if(direction == "out")
  {
    
  }
  else
    return;
}

void soundDetection()
{
  if(powerOn && detectSound)
  {
    currentTime = millis();
    if(currentTime > nextTime)
    {
      if(soundSpike > 30000)
        soundSpike = 30000;
      double soundMultiplier = (double)map(soundSpike, 500, 20000, 5, 100)/100.0;
      Serial.println(soundMultiplier);
      nextTime =  currentTime + interval;

      if(soundMultiplier > .15 && fading)
      {
        /*redRBrightness = random(50, 255);
        redGBrightness = random(255 - redRBrightness, 255);
        redBBrightness = (255 * 3) - redRBrightness - redBBrightness;
        yellowRBrightness = random(50, 255);
        yellowGBrightness = random(255 - yellowRBrightness, 255);
        yellowBBrightness = (255 * 3) - yellowRBrightness - yellowBBrightness;
        greenRBrightness = random(50, 255);
        greenGBrightness = random(255 - greenRBrightness, 255);
        greenBBrightness = (255 * 3) - greenRBrightness - greenBBrightness;*/
        
        redRBrightness = random(0, 255);
        redGBrightness = random(0, 255);
        redBBrightness = random(0, 255);
        yellowRBrightness = random(0, 255);
        yellowGBrightness = random(0, 255);
        yellowBBrightness = random(0, 255);
        greenRBrightness = random(0, 255);
        greenGBrightness = random(0, 255);
        greenBBrightness = random(0, 255);
      }
      Serial.println(soundMultiplier);
      
      analogWrite(redR, map(redRBrightness * .4 * soundMultiplier, 0, 255, 0, maxBrightness));
      analogWrite(redG, map(redGBrightness * soundMultiplier, 0, 255, 0, maxBrightness));
      analogWrite(redB, map(redBBrightness * soundMultiplier, 0, 255, 0, maxBrightness));
      analogWrite(yellowR, map(yellowRBrightness * .4 * soundMultiplier, 0, 255, 0, maxBrightness));
      analogWrite(yellowG, map(yellowGBrightness * soundMultiplier, 0, 255, 0, maxBrightness));
      analogWrite(yellowB, map(yellowBBrightness * soundMultiplier, 0, 255, 0, maxBrightness));
      analogWrite(greenR, map(greenRBrightness * .4 * soundMultiplier, 0, 255, 0, maxBrightness));
      analogWrite(greenG, map(greenGBrightness * soundMultiplier, 0, 255, 0, maxBrightness));
      analogWrite(greenB, map(greenBBrightness * soundMultiplier, 0, 255, 0, maxBrightness));
      
      soundSpike = 0;
      return;
    }

  soundSpike+=analogRead(soundPin);
  }
}
