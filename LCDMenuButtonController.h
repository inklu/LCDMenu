#ifndef LCDMENUBUTTONCONTROLLER_H
#define LCDMENUBUTTONCONTROLLER_H

#if ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

#include "LCDMenuController.h"
#include "DigitalButton.h"

//Объявление класса Кнопочный Контроллер для управления Меню
class ButtonController:public Controller {
    //enum buttonRole {brUP,brDOWN,brOK,brBACK,brNONE=999};
    typedef actionType buttonRole;
    struct bc {
      DigitalButton *bt; //адрес на объект Кнопка
      buttonRole  br;  //роль кнопки
    } btns[4];         //таких 4 штуки
    byte btns_cnt=0;
    void add(DigitalButton &bt, buttonRole br){ //добавить кнопку в кнопочный контроллер
      if(btns_cnt<4){
        btns[btns_cnt].bt = &bt;
        btns[btns_cnt++].br = br;
      }
    }
  public:
    ButtonController() {}; 
    void addUP(DigitalButton &bt){ //добавить кнопку ВВЕРХ
      add(bt,aUP);
    }
    void addDOWN(DigitalButton &bt){ //добавить кнопку ВНИЗ
      add(bt,aDOWN);
    }
    void addOK(DigitalButton &bt){ //добавить кнопку ОК
      add(bt,aOK);
    }
    void addBACK(DigitalButton &bt){ //добавить кнопку НАЗАД
      add(bt,aBACK);
    }
    void run(unsigned long mls=0){ //обравботка событий от всех добавленных кнопок
      if (!mls) mls = millis();
      for(int i=0;i<btns_cnt;i++){
        btns[i].bt->run(mls);
        if(btns[i].bt->checkEvent(beOffClick)) { //событие нажатия
          setAction(btns[i].br);
          setAction(aCLICK);
          btns[i].bt->flushEvents();
        }
		else if(btns[i].bt->checkEvent(beOffHold)) { //событие долгого нажатия
          setAction(btns[i].br);
          setAction(aHOLD);
          btns[i].bt->flushEvents();
		}
      }
    }
};

#endif
