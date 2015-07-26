#ifndef MenuNavigation_h
#define MenuNavigation_h

#include "Arduino.h"
#include "MenuItem.h"   

class MenuNavigation{
    public:
        MenuNavigation();
        void setMenu(MenuItem * menu, byte page);
        byte nextPage();
        byte prevPage();
        byte getPageItemIndex(byte index){ return (m_page * 3) + index;}
        byte getPage(){return m_page;}
        MenuItem * getMenu(){return m_menu;}
    private:
        byte m_page;
        byte m_itemCount;
        byte m_pageCount;
        MenuItem * m_menu;
};

#endif