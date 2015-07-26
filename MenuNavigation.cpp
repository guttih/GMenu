#include "Arduino.h"
#include "MenuNavigation.h"

void MenuNavigation::setMenu(MenuItem * menu, byte page){
    m_page = page;
    m_itemCount = m_pageCount = 0;
    m_menu = menu;
    if (m_menu)
    {
        m_pageCount = m_menu->pageCount();
        m_itemCount = m_menu->itemCount();
    }
}
MenuNavigation::MenuNavigation(){
    m_menu = NULL;
    m_page = 0;
    m_pageCount=0;
}
byte MenuNavigation::nextPage(){
   
    if (m_page + 1 < m_pageCount)
        m_page++;
    return m_page;
}
byte MenuNavigation::prevPage(){
    if (m_page > 0)
        m_page--;

    return m_page;
}
