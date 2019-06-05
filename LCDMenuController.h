#ifndef LCDMENUCONTROLLER_H
#define LCDMENUCONTROLLER_H

#if ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

//Объявление класса Контроллер, базовый для классов Кнопочный контроллер, Енкодер, Джойстик
class Controller{
    byte act;
  protected:
	//типы действий в контроллере: ВВЕРХ, ВНИЗ, ОК, НАЗАД, НАЖАТИЕ, УДЕРЖАНИЕ
    enum actionType {aUP,aDOWN,aOK,aBACK,aCLICK,aHOLD,aNONE=999};
    void setAction(const actionType at) { bitSet(act,at); } //установить действие
    void resetAction(const actionType at) { bitClear(act,at); } //сбросить действие
	void resetActions() { resetAction(aHOLD); resetAction(aCLICK);} //сбросить нажатие и удержание
  public:
    virtual void run(unsigned long mls=0) {} //фиксирование и преобразование событий контроллера в действия
    //bool isUP() { return act & bit(aUP); }
    //bool isDOWN() { return act & bit(aDOWN); }
    //bool isOK() { return act & bit(aOK); }
    //bool isBACK() { return act & bit(aBACK); }
    bool isUP() { return act & bit(aUP) && act & bit(aCLICK); }			//нажато ВВЕРХ
    bool isDOWN() { return act & bit(aDOWN) && act & bit(aCLICK); }		//нажато ВНИЗ
    bool isOK() { return act & bit(aOK) && act & bit(aCLICK); }			//нажато ОК
    bool isBACK() { return act & bit(aBACK) && act & bit(aCLICK); }		//нажато НАЗАД
    bool isHOLDUP() { return act & bit(aUP) && act & bit(aHOLD); }		//долго нажато ВВЕРХ
    bool isHOLDDOWN() { return act & bit(aDOWN) && act & bit(aHOLD); }  //долго нажато ВНИЗ
    bool isHOLDOK() { return act & bit(aOK) && act & bit(aHOLD); }		//долго нажато ОК
    bool isHOLDBACK() { return act & bit(aBACK) && act & bit(aHOLD); }  //долго нажато НАЗАД
    void resetUP() { resetAction(aUP); resetActions();}					//сбросить нажатие ВВЕРХ
    void resetDOWN() { resetAction(aDOWN); resetActions();}				//сбросить нажатие ВНИЗ
    void resetOK() { resetAction(aOK); resetActions();}				    //сбросить нажатие ОК
    void resetBACK() { resetAction(aBACK); resetActions();}				//сбросить нажатие НАЗАД
};



#endif