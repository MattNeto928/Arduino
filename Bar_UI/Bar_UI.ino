/*
  This example draws fonts (as used by the Adafruit_GFX library) onto the
  screen. These fonts are called the GFX Free Fonts (GFXFF) in this library.

  Other True Type fonts could be converted using the utility within the
  "fontconvert" folder inside the library. This converted has also been
  copied from the Adafruit_GFX library.

  Since these fonts are a recent addition Adafruit do not have a tutorial
  available yet on how to use the utility.   Linux users will no doubt
  figure it out!  In the meantime there are 48 font files to use in sizes
  from 9 point to 24 point, and in normal, bold, and italic or oblique
  styles.

  This example sketch uses both the print class and drawString() functions
  to plot text to the screen.

  Make sure LOAD_GFXFF is defined in the User_Setup.h file within the
  TFT_HX8357 library folder.

  --------------------------- NOTE ----------------------------------------
  The free font encoding format does not lend itself easily to plotting
  the background without flicker. For values that changes on screen it is
  better to use Fonts 1- 8 which are encoded specifically for rapid
  drawing with background.
  -------------------------------------------------------------------------
  
  >>>>>>>>>>>>>>>>>>>>>>>>>>> WARNING <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

  This sketch  is at the limit of how many fonts can be
  loaded in the Mega's memory.

  As supplied with the default settings the sketch has 11 fonts loaded,
  i.e. GLCD (Font 1), Font 2, Font 4, Font 6, Font 7, Font 8 and five Free Fonts,
  even though they are not all used in the sketch.
  
  Disable fonts you do not need in User_Setup.h in the library folder.
 
  If too many fonts are loaded they may occupy more than 64K bytes and the
  Due will crash or corrupted characters will be drawn, even though the
  sketch compiles and uploads correctly.

  This is a limit imposed by the Arduino environment.

  #########################################################################
  ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
  #########################################################################
*/

#include <TFT_HX8357.h> // Hardware-specific library

#include "Free_Fonts.h" // Include the header file attached to this sketch

TFT_HX8357 tft = TFT_HX8357();       // Invoke custom library

unsigned long drawTime = 0;

bool hasInput = false;

int lightBlue = 0x8cfaf1;

int VRx = A0;
int VRy = A1;
int SW = 3;

int xPosition = 0;
int yPosition = 0;
int SW_state = 0;
int mapX = 0;
int mapY = 0;

int xpos =  0;
int ypos = 40;

int page = 0;
int drinkSelecting = 0;
bool selectionTab = false;
bool canCloseDrink = false;
bool canOpenDrink = true;


char *drinks[20];
int amountOfDrinks; 

int selectionMenu = 0;


void setup(void) {

  tft.begin();
  tft.setRotation(2);

  Serial.begin(9600); 
  
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT_PULLUP);

  digitalWrite(SW, HIGH);
  
  tft.fillScreen(TFT_WHITE);

  drawHeader();
  drawWords();
  drinkPresets();
}

void loop() {
  
  checkForInput();

}

void checkForInput()
{
  
  tft.fillRect(0, 0, tft.width() / 2, 50, TFT_BLACK);
  tft.fillRect(tft.width() / 2, 0, tft.width() / 2, 50, TFT_DARKGREY);

  drawHeader();
  
  if(page == 0)
  drawWords();
  
  for(int i = 0; i < 100000; i++)
  {
    
  xPosition = analogRead(VRx);
  yPosition = analogRead(VRy);
  SW_state = digitalRead(SW);
  mapX = map(xPosition, 0, 1023, -512, 512);
  mapY = map(yPosition, 0, 1023, -512, 512);

  if(isRight() && selectionTab == false)
  {
    page = 1;
    pageManager();
  }
    else
    if(isLeft() && selectionTab == false)
    {
      page = 0;
      pageManager();
    }

if(page == 0 && selectionTab == false)
{
    int x = 12;
    int y = drinkSelecting * 40 + 102;
    
    tft.fillCircle(x, y, 5, TFT_RED);
    
    if(isDown())
    {
      if(drinkSelecting != amountOfDrinks - 1)
      {
      tft.fillCircle(x, y, 5, TFT_WHITE);
      drinkSelecting++;
      y = drinkSelecting * 40 + 102;
      tft.fillCircle(x, y, 5, TFT_RED);
      delay(100);
      }
    }
    else
    if(isUp())
    {
      if(drinkSelecting != 0)
      {
      tft.fillCircle(x, y, 5, TFT_WHITE);
      drinkSelecting--;
      y = drinkSelecting * 40 + 102;
      tft.fillCircle(x, y, 5, TFT_RED);
      delay(100);
      }
      
    }
}

  if(selectionTab)
  {
    if(isUp() && selectionMenu == 1)
    {
        selectionMenu = 0;
        drinkSelected();
    }
      else
      if(isDown() && selectionMenu == 0 && canOpenDrink)
      {
      selectionMenu = 1;
      drinkSelected();
      }

      if(SW_state == 1)
      {
        canCloseDrink = true;
      }

      if(SW_state == 0 && selectionMenu == 0 && canCloseDrink)
  {
    selectionTab = false;
    resetBackground();
    drawWords();
    canCloseDrink = false;
    canOpenDrink = false;
  }
  }
  
  Serial.print("X: ");
  Serial.print(mapX);
  Serial.print(" | Y: ");
  Serial.print(mapY);
  Serial.print(" | Button: ");
  Serial.println(SW_state);

    delay(50);

  if(!canOpenDrink && SW_state == 1)
    canOpenDrink = true;

    if(SW_state == 0 && page == 0 && canOpenDrink)
    drinkSelected();
  }

}

void pageManager()
{
  if(page == 0)
    {
      drawHeader();
      drawWords();
    
    }
    else
    if(page == 1)
    {
      resetBackground();
      drawHeader();
    }
}

void drawHeader()
{
   if(page == 0)
    {
      tft.drawRect(0, 0, tft.width() / 2, 50, TFT_WHITE);
      tft.drawRect(tft.width() / 2, 0, tft.width() / 2, 50, TFT_DARKGREY);
    }
      else
      {
         tft.drawRect(tft.width() / 2, 0, tft.width() / 2, 50, TFT_WHITE);
         tft.drawRect(0, 0, tft.width() / 2, 50, TFT_BLACK);
      }

  tft.setTextColor(TFT_WHITE);
  tft.setFreeFont(FSB9);
  tft.drawString("Make A Drink", 20, 19, GFXFF);
  tft.drawString("Edit Drinks", 195, 19, GFXFF);
}

void drinkPresets()
{
  for(int i = 0; i < 20; i++)
    drinks[i] = "";
    
  drinks[0] = "L.I.I.T.";
  drinks[1] = "Vodka";
  drinks[2] = "Whiskey";
  drinks[3] = "Gin";

  for(int i = 0; i < 20; i++)
  {
    if(drinks[i] == "")
    {
    amountOfDrinks = i;
    break;
    }
  }


 
      tft.setFreeFont(FSB9);
      tft.setTextColor(TFT_BLUE);
      tft.drawString("Total Drinks In Database: ", 5, 60, GFXFF);
      tft.drawNumber(amountOfDrinks, 225, 60, GFXFF);


}

void drawWords()
{
  

  drinkPresets();

  int x = 25;
  int y = 90;
  
  tft.setTextColor(TFT_BLACK);
  tft.setFreeFont(FSB18);
  for(int i = 0; i < amountOfDrinks; i++)
  {
    char *string = drinks[i];
    tft.drawString(string, x, y, GFXFF);
    y += 40;
  }
  
  xpos = tft.width() / 2; // Half the screen width
  ypos = 70;

  
  //tft.setFreeFont(FSB12);                               // Select the font
  ypos += tft.fontHeight(GFXFF);                        // Get the font height and move ypos down
}


void drinkSelected()
{
    
  selectionTab = true;
  tft.fillRect(140, 55, 175, 420, lightBlue);
  tft.drawRect(140, 55, 175, 420, TFT_BLACK);
  tft.setFreeFont(FS18);
  tft.drawString("x", 290, 60, GFXFF);
  tft.fillRect(177.5, tft.height() - 40, 100, 30, TFT_GREEN);
  tft.drawRect(177.5, tft.height() - 40, 100, 30, TFT_BLACK);
  tft.setFreeFont(FS12);
  tft.drawString("Begin", 200, tft.height() - 35, GFXFF);

  if(selectionMenu == 0)
  {
    tft.fillCircle(280, 75, 5, TFT_BLACK);
    tft.fillCircle(160, tft.height() - 25, 5, lightBlue);
  }
  else
  {
    tft.fillCircle(160, tft.height() - 25, 5, TFT_BLACK);
    tft.fillCircle(280, 75, 5, lightBlue);
  }
  
}

bool isUp()
{
  if(mapY == -512)
  return true;
  else
  return false;
}

bool isDown()
{
  if(mapY == 512)
  return true;
  else
  return false;
}

bool isRight()
{
  if(mapX == 512)
  return true;
  else
  return false;
}

bool isLeft()
{
  if(mapX == -512)
  return true;
  else
  return false;
}

// Print the header for a display screen
void header(char *string)
{
  tft.setTextSize(1);
  tft.setTextColor(TFT_MAGENTA);
  tft.fillRect(0, 0, 480, 30, TFT_BLUE);
  tft.setTextDatum(TC_DATUM);
  tft.drawString(string, 239, 2, 4); // Font 4 for fast drawing with background
}

void resetBackground()
{
  tft.fillRect(0, 50, tft.width(), tft.height() - 50, TFT_WHITE);
}

// There follows a crude way of flagging that this example sketch needs fonts which
// have not been enbabled in the User_Setup.h file inside the TFT_HX8357 library.
//
// These lines produce errors during compile time if settings in User_Setup are not correct
//
// The error will be "does not name a type" but ignore this and read the text between ''
// it will indicate which font or feature needs to be enabled
//
// Either delete all the following lines if you do not want warnings, or change the lines
// to suit your sketch modifications.

#ifndef LOAD_GLCD
//ERROR_Please_enable_LOAD_GLCD_in_User_Setup
#endif

#ifndef LOAD_FONT2
//ERROR_Please_enable_LOAD_FONT2_in_User_Setup!
#endif

#ifndef LOAD_FONT4
//ERROR_Please_enable_LOAD_FONT4_in_User_Setup!
#endif

#ifndef LOAD_FONT6
//ERROR_Please_enable_LOAD_FONT6_in_User_Setup!
#endif

#ifndef LOAD_FONT7
//ERROR_Please_enable_LOAD_FONT7_in_User_Setup!
#endif

#ifndef LOAD_FONT8
//ERROR_Please_enable_LOAD_FONT8_in_User_Setup!
#endif

#ifndef LOAD_GFXFF
ERROR_Please_enable_LOAD_GFXFF_in_User_Setup!
#endif
