#ifndef MenuItem_h
#define MenuItem_h

#include "Arduino.h"

#define GMENU_DEBUGGING  /*Comment this out to save memory, these functions are only used for debugging*/


const byte FAIL = 255;  //Function was unsuccessful

class MenuItem
{
    public:
        char *text;
        boolean initialize(const char *text); //call only once, before using this object
        void destroy();//call only once, after using this object
        static MenuItem *newMenu(const char *text);
        MenuItem * pFirstChild;
        MenuItem * pNextSibling;
        MenuItem * findLastChild();
        MenuItem * findLastSibling();
        byte       m_functionIndex;
        MenuItem * addMenuItem(const char *text);
        MenuItem * addMenuItem(const char *text, byte functionIndex);
         MenuItem * getParent(MenuItem * rootMenu);
        MenuItem * getAt(byte index);
#ifdef GMENU_DEBUGGING
        void serialPrintTree(MenuItem * rootMenu, byte level);
#endif
        byte getItemIndex(MenuItem *item);
        byte getItemIndexHelper(MenuItem *sibling, MenuItem *findMe, byte index);
        byte itemCount();
        byte pageCount();
    private:
        byte itemCount(MenuItem *item);
        MenuItem * findLastSibling(MenuItem *item);
        MenuItem * addMenuItem(MenuItem *addMe);
        MenuItem * getAtHelper(MenuItem *item, byte index);
        //void (* functionPointer)();
        void freeMenu(MenuItem *item);     
};
#endif