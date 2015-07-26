#include "Arduino.h"
#include "MenuItem.h"

 boolean MenuItem::initialize(const char *text){
      
      //allocate memory for text of variable length.
      this->text = (char *)malloc(strlen( (char *)text ) + 1);
      if (this->text)
          strcpy((char *)this->text,(char *)text);

    this->pFirstChild = NULL;
    this->pNextSibling = NULL;
    this->m_functionIndex = 0;
    
    return this->text !=NULL;  
}
void MenuItem::destroy(){
      freeMenu(this);
      free(this);
  }

MenuItem *MenuItem::newMenu(const char *text){
    MenuItem *p;
    p = (MenuItem*)malloc( sizeof(MenuItem) );

    if (p == NULL)
        return NULL;  //Error, no more memory for this MenuItem
    if (!p->initialize(text)){
        free(p); //there was no memory left for the text so let's free the object, it's useless without the text
        return NULL; //Error no memory left for the text in MenuItem");
    }
    
    return p;
}

//remove given menuitem from memory
void MenuItem::freeMenu(MenuItem *item){
    
    if (item == NULL) 
        return;
    free(item->text);
    item->text = NULL;
    
    freeMenu(item->pFirstChild);
    item->pFirstChild = NULL;
    
    freeMenu(item->pNextSibling);
    item->pNextSibling = NULL;
    
    free(item);
    
}

byte MenuItem::pageCount(){
     int count = itemCount();
     if (count < 4) return 1;
     return ((count -1) / 3) + 1;
 }
 
byte MenuItem::itemCount(){
     if (!pFirstChild)
         return 0;
     return itemCount(pFirstChild) + 1;
 }
 
 byte MenuItem::itemCount(MenuItem *item){
     if (!item->pNextSibling)
         return 0;
     return itemCount(item->pNextSibling) + 1;
 }
 
//returns the first item that has no sibling
 MenuItem * MenuItem::findLastChild(){
        return findLastSibling((MenuItem*)this->pFirstChild);
 }
 
  MenuItem * MenuItem::findLastSibling(){
        return findLastSibling(this);
 }

 MenuItem * MenuItem::findLastSibling(MenuItem *item){
    if (item->pNextSibling == NULL)
    {
        return item;
    }
    else
        return findLastSibling((MenuItem *)item->pNextSibling);
 }
 
  MenuItem * MenuItem::addMenuItem(const char *addMe, byte functionIndex){
      MenuItem * ret = addMenuItem(newMenu(addMe));
      ret->m_functionIndex = functionIndex;
  }
  
 MenuItem * MenuItem::addMenuItem(const char *addMe){
      return addMenuItem(newMenu(addMe));
  }
  MenuItem * MenuItem::addMenuItem(MenuItem *addMe){
      MenuItem *menu = this;
       MenuItem *p;
      p = (MenuItem *)menu->pFirstChild;
      if (p == NULL)
      {//this is the first child in this menu
        menu->pFirstChild = addMe;
      }
      else
      { //there are 1 or more children already in the menu
        p = findLastSibling(p);
        p->pNextSibling = addMe;
      }
      return addMe;
  }
  
 //todo: move getAt from GMenu to MenuItem
 MenuItem * MenuItem::getAt(byte index){
     MenuItem *item = this;
     if(item == NULL) 
         return NULL;
     
     if(index == 0)
        return item->pFirstChild;
    
     return getAtHelper(item->pFirstChild, index);
 }
 
  MenuItem * MenuItem::getAtHelper(MenuItem *item, byte index){
    if(index == 0 || item == NULL)
        return item;
    
    return getAtHelper(item->pNextSibling, index - 1);
 }
 
MenuItem * MenuItem::getParent(MenuItem * rootMenu /*usually main menu*/)
{
    if (rootMenu == NULL || rootMenu->pFirstChild == NULL) 
        return NULL;
    byte index = rootMenu->getItemIndex(this);
    if (index != FAIL)
    {
        return rootMenu;
    }
    //nothing found in level 1, let's take this to the next level :)
    MenuItem * ret;
    ret = getParent(rootMenu->pFirstChild);
    if (ret)
    {
        return ret;
    }
    
    if (!rootMenu->pFirstChild->pNextSibling)
        return NULL;
    
    return getParent(rootMenu->pFirstChild->pNextSibling);
}

//returns FAIL if nothing found
//searches for item in 1. level items
byte MenuItem::getItemIndex(MenuItem *item){
    if (item == NULL)
        return FAIL;
    if (item == this->pFirstChild)
        return 0;
    
    return getItemIndexHelper(this->pFirstChild->pNextSibling, item, 1);
}

byte MenuItem::getItemIndexHelper(MenuItem *sibling, MenuItem *findMe, byte index)
{ 
    if (sibling == NULL)
        return FAIL;
    
    if(sibling == findMe)
        return index;
    
    return getItemIndexHelper(sibling->pNextSibling, findMe, index + 1 );
}

#ifdef GMENU_DEBUGGING
void MenuItem::serialPrintTree(MenuItem * item, byte level)
{
    if (item == NULL) 
        return;
    
    MenuItem *parent = item->getParent(this);
    
    for(byte b = 0; b < level; b++)
        Serial.print("     ");
    
    if (parent)
    {   byte index =  parent->getItemIndex(item);
        if (index < 10) 
            Serial.print(" ");
        Serial.print(index);
        Serial.print(".");
    }

    Serial.print(item->text);
    Serial.println("");
    
    serialPrintTree(item->pFirstChild ,level + 1);
    serialPrintTree(item->pNextSibling, level);    
}
#endif
 
 
