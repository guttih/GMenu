/*
  GMenu.h - Library for displaying a Menu on a 20x4 LCD and
  using a Keypad characters used ('0' - '9') and '*' and '#'
  Where '#' means next menu screen or enter and
        '*' means previous screen or cancel.

  Created by : Gudjon Holm Sigurdsson
  My web-page: http://www.guttih.com
  
*/
#ifndef GMenu_h
#define GMenu_h

#include "Arduino.h"
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#include <MenuItem.h>
#include <MenuNavigation.h>

class GMenu{

    public:        
                    GMenu(const char *text, void (* runFunction)(MenuItem *item, boolean firstRunAfterKeyPressed), LiquidCrystal_I2C *lcd, Keypad *keypad);
                    ~GMenu();
        MenuItem *  getMainMenu()   { return m_mainMenu; };
        MenuItem *  geLastSelected()   { return m_lastSelected; };
        MenuItem *  addMenuItem(MenuItem *menu, MenuItem *addMe);
       
        void        printPage();
        void        printNextPage();
        void        printPrevPage();
        void        printMain();
        MenuItem *  getCurrentMenu(){return nav.getMenu();}
        MenuItem *  findLastSibling(MenuItem *item);
       
        int         printTitle(const char *text);
        void        print(byte x, byte y, const char *text);
        void        print(byte x, byte y, int num);
        int         centerX(const char*text); 
        void        processKey(const char key);
        void        selectMenuItem(MenuItem *thisMenu, byte thisPage, const char keyPressed);
                        
        void (* runFunctionPtr)(MenuItem *item, boolean firstRunAfterKeyPressed);

    private:
        LiquidCrystal_I2C   *m_lcd;
        Keypad              *m_keypad;
        MenuItem            *m_mainMenu;
        MenuItem            *m_lastSelected;
void        printMenu(MenuItem *printMe, byte page);         
         MenuNavigation  nav;
         
#ifdef GMENU_DEBUGGING
    public:
        void        serialPrintTree(){m_mainMenu->serialPrintTree(m_mainMenu, 0);}
        int         freeMemory();
#endif
      
};


#endif