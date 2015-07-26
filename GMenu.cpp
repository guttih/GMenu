#include "Arduino.h"
#include "GMenu.h"


GMenu::~GMenu(){
    m_mainMenu->destroy();
}
GMenu::GMenu(const char *text, void (* runFunction)(MenuItem *item, boolean firstRunAfterKeyPressed), LiquidCrystal_I2C *lcd, Keypad *keypad){
    m_lcd = lcd;
    m_keypad = keypad;
    m_mainMenu = MenuItem::newMenu(text);
    runFunctionPtr = runFunction;
}

//freeMemory is from : http://playground.arduino.cc/Code/AvailableMemory     //
extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;

#ifdef GMENU_DEBUGGING
int GMenu::freeMemory() {
  int free_memory;

  if((int)__brkval == 0)
     free_memory = ((int)&free_memory) - ((int)&__bss_end);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);

  return free_memory;
}
#endif

void GMenu:: printMain(){

  printMenu(m_mainMenu, 0);
}

void GMenu:: printPage(){
  printMenu(nav.getMenu(), nav.getPage());
}
void GMenu:: printNextPage(){
    printMenu(nav.getMenu(), nav.nextPage());
}
void GMenu:: printPrevPage(){
   printMenu(nav.getMenu(), nav.prevPage());
}

void GMenu:: printMenu(MenuItem *printMe, byte page){
    MenuItem *pItem;
    nav.setMenu(printMe, page);
    int start = (page * 3),
    offset = 0;
    
    m_lcd->clear();
    if (printMe == NULL)
        return;
  
    printTitle(printMe->text);

    //pItem = getAt(printMe, start);
    pItem = printMe->getAt(start);
    while(pItem && (offset < 3))
    {
        m_lcd->setCursor ( 0, offset+1 );
        m_lcd->print(offset + 1);
        m_lcd->print(":");
        m_lcd->setCursor ( 3, offset+1 );
        m_lcd->print(pItem->text);
        pItem = (MenuItem*)pItem->pNextSibling;
        offset++;
    }
}

//does not check if lenght is more thant 20
int GMenu::centerX(const char *text){
    return (20-strlen(text))/2;
}
int GMenu::printTitle(const char *text){
    int offset, 
        center,
        len = strlen( text);
    center = (20-len)/2;
    print(center, 0, text);
    if (len < 17)
    {
        if (len < 13)
            offset = 2;
        else
            offset = 1;
        print(center-(offset+1), 0,"-");
        print(center+len+(offset ), 0,"-");
  }
}

 void GMenu::print(byte x, byte y, const char *text){
      m_lcd->setCursor ( x, y );
      m_lcd->print(text);
 }
 void GMenu::print(byte x, byte y, int num){
      m_lcd->setCursor ( x, y );
      m_lcd->print(num);
 }
 
void GMenu::selectMenuItem(MenuItem *thisMenu, byte thisPage, const char keyPressed){
    printMenu(thisMenu, thisPage);
    processKey(keyPressed);
}
 void GMenu::processKey(const char key){

   switch(key)
   {
     case '#' : printMenu(nav.getMenu(), nav.nextPage());
               break;
     case '*' :
              if (nav.getPage() == 0)
              {
                m_lastSelected = nav.getMenu()->getParent(getMainMenu());
                if (m_lastSelected)
                  printMenu(m_lastSelected, 0);
                
              }
               else 
                 printMenu(nav.getMenu(), nav.prevPage());
               break;
     case '1' :
     case '2' :
     case '3' : 
               byte itemIndex = nav.getPageItemIndex(key - 49);
               m_lastSelected = nav.getMenu()->getAt(itemIndex);            
               
               if (m_lastSelected)
               { //this item exists
                 if (m_lastSelected->m_functionIndex)
                   runFunctionPtr(m_lastSelected, true);//item is attached to a functionIndex
                 else
                   printMenu(m_lastSelected, 0);
               }
               break;
   }
}

 
  