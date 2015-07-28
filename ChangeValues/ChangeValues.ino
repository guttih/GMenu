#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <GMenu.h>

/* GMenu example sketch to show how to change values,
   add sub-menus and run a function.

   by: Gudjon Holm Sigurdsson,  http://www.guttih.com
                               https://github.com/guttih   

   The keypad characters used are 1, 2, 3 * and #
   Where: 
   '#' means next menu screen or enter and
   '*' means previous menu screen or cancel.
   '1' means select the first  visible menu item
   '2' means select the second visible menu item
   '3' means select the third  visible menu item
   '.' to place the comma (keypad-key 'D' )
*/

const int BACKLIGHT_PIN  = 3;
const unsigned int updateRate = 200;


//States
const byte STATE_MENU                       = 0; //menu active
const byte STATE_RETURN_TO_PARENT           = 1; //if a key is pressed the parent menu should be displayed 
const byte STATE_CHANGING_VALUE             = 2;
const byte STATE_RETURN_TO_PARENT_ANDUPDATE = 3;  /*even, if no key is pressed this menufunction will execute*/

/*Functions ENUMs used in runFunction*/
const byte FUNCTION_ABOUT            = 1;
const byte FUNCTION_CHANGE_DVALUE    = 2;
const byte FUNCTION_CHANGE_IVALUE    = 3;
const byte FUNCTION_CHANGE_BVALUE    = 4;
const byte FUNCTION_CHANGE_LVALUE    = 5;
const byte FUNCTION_CHANGE_BACKLIGHT = 6;
const byte FUNCTION_RUNNING          = 7;
const byte FUNCTION_DEFAULTVIEW      = 8;

//Change states
const byte CHANGE_VALUE_TYPE_BYTE    = 1;
const byte CHANGE_VALUE_TYPE_INT     = 2;
const byte CHANGE_VALUE_TYPE_LONG    = 3;
const byte CHANGE_VALUE_TYPE_DOUBLE  = 4;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','.'}  /*using the 'D' as a decimal point*/
};

byte rowPins[ROWS] = { 6,  7,  8,  9}; 
byte colPins[COLS] = {10, 11, 12, A0};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

GMenu menu("Main menu", runFunction, &lcd, &keypad);


//GLOBALS
unsigned long udateTimer = 0;
int displayState = STATE_MENU;
void *changeValuePtr;
int changeValueType;
char tempStr[21];
byte placeComma = FAIL; /*FAIL = 255, declared in MenuItem.h */
byte backLight = 127; //Brightness 50%
double dValue = 80.443;
long   lValue = 60000000;
int    iValue = 30000;
byte   bValue = 200;

void setup()
{
  pinMode(BACKLIGHT_PIN, OUTPUT);
  Serial.begin(9600);
  udateTimer = millis();
  lcd.begin(20, 4);               // initialize the lcd 
  lcd.home ();                   // go home
  analogWrite(BACKLIGHT_PIN, backLight);

  MenuItem *sub, *main;
  main = menu.getMainMenu();
  main->addMenuItem("Views");
  main->addMenuItem("Values");
  main->addMenuItem("About", FUNCTION_ABOUT);
 
  sub = main->getAt(0);//Views
  sub->addMenuItem("Default view", FUNCTION_DEFAULTVIEW);
  sub->addMenuItem("Not used view");
  sub->addMenuItem("Running status", FUNCTION_RUNNING);
  
  sub = main->getAt(1);//Values
  sub->addMenuItem("Change dValue",   FUNCTION_CHANGE_DVALUE);
  sub->addMenuItem("Change lValue",   FUNCTION_CHANGE_LVALUE);
  sub->addMenuItem("Change iValue",   FUNCTION_CHANGE_IVALUE);
  sub->addMenuItem("Change bValue",   FUNCTION_CHANGE_BVALUE);
  sub->addMenuItem("Change backLight",FUNCTION_CHANGE_BACKLIGHT);
  
  menu.printMain();
  //uncomment below to hard-code a selection of the menuItem defaultView when arduino powers on
  //menu.selectMenuItem(start, 0, '1'); //run the selected item in the specified menu
  
}

void loop()
{
    //do not use delay in the loop because key-press events can be lost.
    dValue += 0.1; //show that this menu is non-blocking
    if (dValue > 100000)  
        dValue = 0;

    processKey(keypad.getKey());
}



void zeroPrint(int num, char *str){
  char buffer[10];
  itoa(num,buffer,10); 
  if (num > -1 && num < 10)
    strcat(str, "0");

  strcat(str, buffer);
}

void viewRunning(boolean clearScreen)
{
  if  (clearScreen)
  {  //calling to often will make screen flicker
    lcd.clear();
    menu.printTitle("Running status");
    menu.print(3,1, "running time:");
    menu.print(4,3, "Memory:");   
  }
     
  byte x = menu.centerX(strMillsToTimeStr(millis(), tempStr));
  menu.print(x,2, tempStr);
  menu.print(15,3,"  ");
  lcd.setCursor(12,3);
  lcd.print(menu.freeMemory());
  
  displayState = STATE_RETURN_TO_PARENT_ANDUPDATE;
}

//adds a space behind a number, also you can provide a unit which will be printed after the number
void lcdPrintAndAddSpace(byte x, byte y, unsigned long num, byte spaceForNumAndUnit, const char *strUnit){
    byte len = 0;
    lcd.setCursor(x, y);
    lcd.print(num);
    if (strUnit)
    {
        lcd.print(strUnit);
        len = strlen(strUnit);
    }

    int nDigits = (int(log10(num))) + 1;
    int spaces = ((int)spaceForNumAndUnit) - nDigits;
    spaces -= len;
    tempStr[0] = '\0';
    for (int i = 0; i < spaces; i++)
    {
        tempStr[i] = ' ';
        tempStr[i + 1] = '\0';
    }

    lcd.print(tempStr);
}

void viewDefault(boolean clearScreen)
{
 
    if  (clearScreen)
    { 
        lcd.clear();
        menu.print( 0, 1, "dValue:");
        menu.print( 0, 2, "lValue:");
        menu.print( 0, 3, "bVal:");
        menu.print(10, 3, "iVal:");
    }

    byte x = menu.centerX(strMillsToTimeStr(millis(), tempStr));
    menu.print(x,0, tempStr);
    menu.print(8, 1, "           ");lcd.setCursor(7, 1); printFloat(dValue, 2);
    lcdPrintAndAddSpace(7 , 2, lValue, 13,NULL);
    lcdPrintAndAddSpace(5 , 3, bValue, 3, NULL);
    lcdPrintAndAddSpace(15, 3, iValue, 5, NULL);

    displayState = STATE_RETURN_TO_PARENT_ANDUPDATE;
}

//returns a pointer to the buffer given and that buffer contains a time string
//largest possible ULONG is 4294967295, which is 49 days, 17:02:47
char *strMillsToTimeStr(unsigned long x, char *strResult)
{
  unsigned long d, y, s, m, h, mi;
  char buffer[10];         
   y = ((unsigned long )60*(unsigned long )60*(unsigned long )1000);
   h = x/y;
   m = (x-(h*y))/(y/60);
   s = (x-(h*y)-(m*(y/60)))/1000;
   mi = x-(h*y)-(m*(y/60))-(s*1000);

  if (h > 23 )
  {
    d = h / 24;
    h-=(d*24);
     
   itoa(d,buffer,10);   
   strcpy(strResult, buffer);
   strcat(strResult," day");
    if (d > 1)
      strcat(strResult,"s");
      
    strcat(strResult," + ");
  }
  else
    strcpy(strResult, "");
    
  zeroPrint(h, strResult);strcat(strResult,":");
  zeroPrint(m, strResult);strcat(strResult,":");
  zeroPrint(s, strResult);

}

void processKey(const char key){
  if (!key && (
                (displayState != STATE_RETURN_TO_PARENT_ANDUPDATE)
               )
      )
     return; //only act when a key is pressed
  analogWrite(BACKLIGHT_PIN, backLight);
  switch(displayState){
  
  case STATE_RETURN_TO_PARENT_ANDUPDATE: 
                             if (key){
                               displayState = STATE_RETURN_TO_PARENT;
                               processKey(key);
                               return;
                            } 
                            if (millis() > udateTimer + updateRate)
                            {
                              udateTimer = millis();
                              runFunction(menu.geLastSelected(), false);
                            }
                            break;
  case STATE_RETURN_TO_PARENT:
                            
                            menu.printPage();
                             displayState = STATE_MENU;
                             break;
   case STATE_CHANGING_VALUE:
                             renderChangeValue(key);
                             break;
                             
    default:
      menu.processKey(key);
  }
}

void runFunction(MenuItem *item, boolean firstRunAfterKeyPressed){
  
  byte index = item->m_functionIndex;
  switch(index){
    case FUNCTION_ABOUT:
                        display("GMenu - created by",
                                "Gudjon Holm Sigurds.",
                                "gudjonholm@gmail.com",
                                "web: www.guttih.com",
                                 false);
                        
                        break;
    case FUNCTION_CHANGE_DVALUE:
                        changeValue(&dValue);
                        break;
    case FUNCTION_CHANGE_IVALUE:
                        changeValue(&iValue);
                        break;
        case FUNCTION_CHANGE_BVALUE:
                        changeValue(&bValue);
                        break;
    case FUNCTION_CHANGE_LVALUE:
                        changeValue(&lValue);
                        break;
    case FUNCTION_CHANGE_BACKLIGHT:
                        changeValue(&backLight);
                        break;
                        
    case FUNCTION_RUNNING:                            
                           viewRunning(firstRunAfterKeyPressed);
                           break;
    case FUNCTION_DEFAULTVIEW:                            
                           viewDefault(firstRunAfterKeyPressed);
                           break;                      
  }
}


const char *getValueName(void *value)
{
       if (value == &dValue   ) return "dValue";
  else if (value == &iValue   ) return "iValue";
  else if (value == &bValue   ) return "bValue";
  else if (value == &lValue   ) return "lValue"; 
  else if (value == &backLight) return "backlight";
  else                         return "Unknown";
}
void displayValue(void *value, byte valueType){
  
  char valueName[21];
  strcpy(valueName, getValueName(value));

  lcd.clear();
  menu.printTitle("Showing value");
  menu.print(menu.centerX(valueName),1, valueName);
  lcd.setCursor(0,3);lcd.print("Value:");
  long tmp;
  lcd.setCursor(7,3);
 if (changeValueType == CHANGE_VALUE_TYPE_BYTE)
   {
      lcd.print(*((byte*)value));
   }
 else if (changeValueType == CHANGE_VALUE_TYPE_DOUBLE)
   {
      printFloat(*((double*)value), 5);
     
   }
 else if (changeValueType == CHANGE_VALUE_TYPE_LONG)
 {
     lcd.print(*((long*)value));

 }
 else
  lcd.print(*((int*)value));
 
  
 displayState = STATE_RETURN_TO_PARENT;
  
}

void display(const char *line1,
             const char *line2,
             const char *line3,
             const char *line4, 
             boolean firstIsTitle){
  lcd.clear();
  if (firstIsTitle)
    menu.printTitle(line1);
  else
    menu.print(0,0, line1);
  
  menu.print(0,1, line2);  
  menu.print(0,2, line3);
  menu.print(0,3, line4);

 displayState = STATE_RETURN_TO_PARENT;
  
}

void changeValue(byte *value){
  changeValueHelper((void*)value, CHANGE_VALUE_TYPE_BYTE);
}

void changeValue(int *value){
  changeValueHelper((void*)value, CHANGE_VALUE_TYPE_INT);
}
void changeValue(long *value){
    changeValueHelper((void*)value, CHANGE_VALUE_TYPE_LONG);
}

void changeValue(double *value){
  changeValueHelper((void*)value, CHANGE_VALUE_TYPE_DOUBLE);
}
void changeValueHelper(void *value, byte ValueType){

  changeValueType = ValueType;
  changeValuePtr = value;
  tempStr[0] = '\0';
  
  lcd.clear();
  menu.print(1,0,"- Changing value -");
  menu.print(menu.centerX(getValueName(value)),1, getValueName(value));
  menu.print(0,2,"Old value: ");

  switch (changeValueType)
  {
      case CHANGE_VALUE_TYPE_BYTE:
                                    lcd.print(*((byte*)(void*)value));
                                    break;
      case CHANGE_VALUE_TYPE_INT:
                                  lcd.print(*((int*)(void*)value));
                                  break;
      case CHANGE_VALUE_TYPE_LONG:
                                  lcd.print(*((long*)(void*)value));
                                  break;
      case CHANGE_VALUE_TYPE_DOUBLE:
                                  printFloat(*((double*)(void*)value), 5);
                                  break;
  }
  
  menu.print(0,3,"New value: ");
  lcd.blink();
  displayState = STATE_CHANGING_VALUE;
  
}

void printFloat(float value, int places) {
  int digit;
  float tens = 0.1;
  int tenscount = 0;
  int i;
  float tempfloat = value;
  float d = 0.5;
  if (value < 0)
    d *= -1.0;

  for (i = 0; i < places; i++)
    d/= 10.0;    
  tempfloat +=  d;

  if (value < 0)
    tempfloat *= -1.0;
  while ((tens * 10.0) <= tempfloat) {
    tens *= 10.0;
    tenscount += 1;
  }
  if (value < 0)
    lcd.print('-');

  if (tenscount == 0)
    lcd.print(0, DEC);

  for (i=0; i< tenscount; i++) {
    digit = (int) (tempfloat/tens);
    lcd.print(digit, DEC);
    tempfloat = tempfloat - ((float)digit * tens);
    tens /= 10.0;
  }

  if (places <= 0)
    return;

 lcd.print('.');  

  for (i = 0; i < places; i++) {
    tempfloat *= 10.0; 
    digit = (int) tempfloat;
    lcd.print(digit,DEC);  
    tempfloat = tempfloat - (float) digit; 
  }
}

void renderChangeValue(const char key){
  double dTemp;
  int len = strlen(tempStr);
  int offset = 11;
  if (key == '*')
 { //backspace or cancel
   if (len == 0){
     lcd.noBlink();
     menu.printPage();
     displayState = STATE_MENU;
     return;
   }
 
   tempStr[len-1]='\0';
   lcd.setCursor(offset+len-1,3);
   lcd.print(" ");
   lcd.setCursor(offset+len-1,3);
   return;
     
 }
 else if (key == '#')
 {
    int i=len-1,  multi = 1;
    dTemp = strtod(tempStr, NULL);
   
    switch (changeValueType)
    {
        case CHANGE_VALUE_TYPE_BYTE : 
                                *((byte*)(void*)changeValuePtr) = (byte)dTemp; 
                                break;
        case CHANGE_VALUE_TYPE_INT:
                                *((int*)(void*)changeValuePtr) = (int)dTemp; 
                                break;
        case CHANGE_VALUE_TYPE_LONG:
                                *((long*)(void*)changeValuePtr) = (long)dTemp;
                                break;
        case CHANGE_VALUE_TYPE_DOUBLE:
                                *((double*)(void*)changeValuePtr) = dTemp;
                                break;
    }
   
    lcd.noBlink();
    displayValue(changeValuePtr, changeValueType);
    return;
 }

//process keys
  int num = key -48;
  if (num > -1 && num < 10 || 
      (key == '.' && changeValueType == CHANGE_VALUE_TYPE_DOUBLE))
  {
      if (key == '.')
      {
          for (int i = 0; i < len; i++)
              if (tempStr[i] == '.')
                  return; //only one '.' in every number
      }
     tempStr[len]=key;
     tempStr[len+1]='\0';
     dTemp = strtod(tempStr, NULL);
    if (
        (changeValueType == CHANGE_VALUE_TYPE_LONG  && dTemp > 2147483647) ||
          (changeValueType == CHANGE_VALUE_TYPE_INT  && dTemp > 32767) ||
          (changeValueType == CHANGE_VALUE_TYPE_BYTE && dTemp > 255) 
       )
    {
      tempStr[len]='\0';
      return;//over MAXINT limit
    }
   lcd.setCursor(offset,3);
   lcd.print(tempStr);
  }
 
}
