#ifndef LCDMENUCONTROLLER2_H
#define LCDMENUCONTROLLER2_H

#ifndef DEBUG
#define DEBUG
#endif

#if ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

#ifndef MC_CYCLE_MLS
#define MC_CYCLE_MLS (1000)
#endif

#ifndef MC_INACT_TIMEOUT
#define MC_INACT_TIMEOUT (30000)
#endif

#ifndef LCD_ELLIPSIS 
#define LCD_ELLIPSIS "..."
#endif

#ifndef LCD_ACTIVE_LINE
#define LCD_ACTIVE_LINE ">"
#endif

#include "Joystick.h"
#include "DigitalButton.h"
#include "AnalogButton.h"
#include "RotEnc.h"

#include <DTime.h>
#include <LiquidCrystal.h>

//Добавление 0 для времени и даты
String decimate(byte b); //{ return ((b < 10) ? "0" : "") + String(b); }

////Abstract controller class for menu
class MenuController{
  public:
    //// controller definition
    enum mcPos {mcpNONE,mcpUP,mcpDOWN,mcpOK,mcpBACK};
    enum mcEvent {mceNONE,mceClick,mceHold,mceLongHold};
/*Contoller Type: 
* mccButtons = 4 positions: UP,DOWN,OK,BACK
* mccJoystick = 5 positions: UP,DOWN,OK(BUTTON),BACK(LEFT),(RIGHT)
* mccEncoder = 3 positions: UP(COUNTERCLOCKWISE),DOWN(CLOCKWISE),OK(BUTTON)
*/
    enum mcCtrlType {mccNONE,mccButtons,mccJoystick,mccRotaryEncoder} mcType=mccNONE;
    class Menu;
    MenuController(){ mls = cycle = millis(); }
    MenuController(const MenuController::Menu &_m){ mls = cycle = millis(); bindMenu(_m); };
    void bindMenu(const MenuController::Menu &_m){ menu = &_m; }
    void run(unsigned long _mls=0);
  protected:
    void menuAction(const mcPos _mcp, const mcEvent _mce);//menu->UP(_mce,this); }
    virtual void runEachMls(unsigned long _mls){}
    virtual void runEachCycle(unsigned long _mls){}
  private:
    unsigned long mls;
    unsigned long cycle;
    MenuController::Menu *menu;
};

////Joystick controller class for menu
class MenuJoystickController:public MenuController,public Joystick{
  public:
    MenuJoystickController(const uint8_t &_x_pin, 
                       const uint8_t &_y_pin, 
                       const uint8_t &_bt_pin, 
                       const uint16_t &_sig_high=JSTK_HIGH_SIG,
                       const uint16_t &_sig_low=JSTK_LOW_SIG,
                       const uint16_t &_sig_high_ts=JSTK_HIGH_TRESHOLD,
                       const uint16_t &_sig_low_ts=JSTK_LOW_TRESHOLD);
    void run(unsigned long _mls=0){ MenuController::run(_mls); }
  protected:
    void offClick(const jsPos jsp);
    void onHold(const jsPos jsp);
    void offHold(const jsPos jsp){ isHolding=false; };
    void offLongHold(const jsPos jsp){ isHolding=false; };
    void offIdle(const jsPos jsp){ isHolding=false; };
  private:
    bool isHolding=false;
    jsPos jspHolding;
    void sendEvent(const jsPos jsp, const mcEvent mce);
    void runEachMls(unsigned long _mls);
    void runEachCycle(unsigned long _mls);
};

////Button controller class for menu
class MenuButtonController:public MenuController{
  public:
    class MenuButton;
    class MenuDigitalButton;
    class MenuAnalogButton;
    MenuButtonController(){ mcType = mccButtons; }
    MenuButtonController(const MenuController::Menu &_m):MenuController(_m){ mcType = mccButtons;  };
    const MenuDigitalButton& newButtonUp(const byte pin){ return newButton(mcpUP,pin); }
    const MenuAnalogButton& newButtonUp(const byte pin,const word sigVal){ return newButton(mcpUP,pin,sigVal); }
    const MenuDigitalButton& newButtonDown(const byte pin){ return newButton(mcpDOWN,pin); }
    const MenuAnalogButton& newButtonDown(const byte pin,const word sigVal){ return newButton(mcpDOWN,pin,sigVal); }
    const MenuDigitalButton& newButtonOk(const byte pin){ return newButton(mcpOK,pin); }
    const MenuAnalogButton& newButtonOk(const byte pin,const word sigVal){ return newButton(mcpOK,pin,sigVal); }
    const MenuDigitalButton& newButtonBack(const byte pin){ return newButton(mcpBACK,pin); }
    const MenuAnalogButton& newButtonBack(const byte pin,const word sigVal){ return newButton(mcpBACK,pin,sigVal); }
  protected:
    void offClick(const MenuButton& bt);
    void onHold(const MenuButton& bt);
    void resetHolding(){ isHolding = false; }
  private:
    MenuButton *btUP=nullptr,*btDOWN=nullptr,*btOK=nullptr,*btBACK=nullptr;
    bool isHolding=false;
    MenuButton *btHolding;
    void runEachMls(unsigned long _mls);
    void runEachCycle(unsigned long _mls);
    const MenuDigitalButton& newButton(const mcPos mcp, const byte pin);
    const MenuAnalogButton& newButton(const mcPos mcp, const byte pin,const word sigVal);
};

class MenuButtonController::MenuButton{
  public:
    enum btType {btDigital=1,btAnalog} btt;
};

class MenuButtonController::MenuDigitalButton:public DigitalButton,public MenuButton{
  public:
    MenuDigitalButton(const byte _pin, const MenuButtonController& _mbc);
  private:
    const MenuButtonController &mbc;
    void offClick(){ mbc.offClick(*this); };
    void onHold(){ mbc.onHold(*this); };
    void offHold(){ mbc.resetHolding(); };
    void offLongHold(){ mbc.resetHolding(); };
    void offIdle(){ mbc.resetHolding(); };
};

class MenuButtonController::MenuAnalogButton:public AnalogButton,public MenuButton{
  public:
    MenuAnalogButton(const byte _pin, const word _sigVal, const MenuButtonController& _mbc);
  private:
    const MenuButtonController &mbc;
    void offClick(){ mbc.offClick(*this); };
    void onHold(){ mbc.onHold(*this); };
    void offHold(){ mbc.resetHolding(); };
    void offLongHold(){ mbc.resetHolding(); };
    void offIdle(){ mbc.resetHolding(); };
};

////Rotary encoder controller class for menu
class MenuEncoderController:public MenuController,public RotEnc{
  public:
    MenuEncoderController(const byte _clkPin, const byte _dtPin, const byte _swPin);
    void run(unsigned long _mls=0){ MenuController::run(_mls); }
  protected:
    void onClockwise();
    void onCounterclockwise();
    void offClick();
    void onHold();
    void offHold(){ isHolding = false; };
    void offLongHold(){ isHolding = false; };
    void offIdle(){ isHolding = false; };
  private:
    bool isHolding=false;
    void runEachMls(unsigned long _mls);
    void runEachCycle(unsigned long _mls);
};

////Menu class
class MenuController::Menu {
  public:
    //Тип указатель на функцию void
    typedef void (*pFunc)();
    //Типы строк: Узел, Целочисленный, с плавающей запятой, список, время, дата, строка, IP, ...
    enum menuLineType {mtNONE, mtNode, mtInt, mtFloat, mtList, mtTime, mtDate, mtString, mtIP, mtURL, mtFunc};
    //конструктор по умолчанию
    Menu(){ }
    //Объявление класса "строка меню"
    class MenuLine {
        menuLineType mlType=mtNONE;
      public:
        class MenuNode; //узел
        class MenuLeaf; //лист
        String name;    //отображаемое имя узла
        MenuLine *pPreviousLine=nullptr; //предыдущая строка меню
        MenuLine *pNextLine=nullptr;     //следующая строка меню
        MenuLine *pParentLine=nullptr;	 //родительская строка меню
        inline virtual bool isNode(){ return mlType == mtNode; }; //это узел?
        inline virtual bool isFunc(){ return mlType == mtFunc; }; //это функция?
        //virtual void edit();
        virtual String getValue(){};     //возвращает строковое значение узла
    };
    //Объявление класса "Узел"
    class MenuLine::MenuNode: public MenuLine {
      public:
        MenuLine *pFirstChild=nullptr; //первый дочерний узел
        MenuNode() { mlType = mtNode; };
    };
    //Объявление класса "Лист"
    class MenuLine::MenuLeaf: public MenuLine {
      protected:
	byte *parts, partsCnt=0, part=0;  //смещения на элементы значения листа
      public:
        class MenuLeaf_int;			//Объявление листа типа целочисленный
        class MenuLeaf_list;		//Объявление листа типа список
        class MenuLeaf_time;		//Объявление листа типа время
        class MenuLeaf_date;		//Объявление листа типа дата
        class MenuLeaf_func;			//Объявление листа типа функция
        class MenuLeaf_str;			//Объявление листа типа строка
        virtual String getValue(){}
        //virtual setValue(String _s) { }
        virtual void editVal(){} //редактировать значение
        virtual void saveVal(){} //сохранить отредактированное значение
        virtual void nextVal(){}   //Следующее значение текущего элемента листа
        virtual void prevVal(){}   //Предыдущее значение текущего элемента листа
	virtual void nextPart(){ if(partsCnt) if(++part==partsCnt) part=0; }  //Установка следующего элемента значеня
	virtual int getShift(){ return (partsCnt ? parts[part] : 0); } //Получить смещение текущего элемента значения
    };
    //Объявление класса "Лист" целочисленного значения
    class MenuLine::MenuLeaf::MenuLeaf_int: public MenuLine::MenuLeaf {
        int value,     //текущее значение
            editValue, //редактируемое значение
            minVal,    //минимальное значение
            maxVal,    //максимальное значение
            step;      //шаг изменения
        //void edit();
      public:
        MenuLeaf_int(int _val=0, int _minVal=-32768, int _maxVal=32767, int _step=1) { mlType = mtInt; value = _val; minVal = _minVal; maxVal = _maxVal; step = _step; };
        String getValue() { //String s; s = value; return s; 
			return (String) editValue; }
        MenuLeaf_int operator=(int _val){ value = _val; return *this; } //задать значение
        void setValue(int _val=0, int _minVal=-32768, int _maxVal=32767, int _step=1){ value = _val; minVal = _minVal; maxVal = _maxVal; step = _step; } //задать значение
        void nextVal(){ if((editValue+step)<=maxVal) editValue+=step; }
        void prevVal(){ if((editValue-step)>=minVal) editValue-=step; }
        void editVal(){ editValue = value; }
        void saveVal(){ value = editValue; }
    };
    //Объявление класса "Лист" типа список
    class MenuLine::MenuLeaf::MenuLeaf_list: public MenuLine::MenuLeaf {
        String *values=nullptr; //массив значений
        byte idx=0,             //текущее значение
             editIdx=0;         //редактируемое значение
        byte size;				//размер массива значений
      public:
        MenuLeaf_list(){};
        String getValue() { return values[editIdx];}
        void setValue(String _vals[], byte _size); //задать массив значений
        void nextVal(){++editIdx==size?editIdx=0:editIdx;}
        void prevVal(){editIdx==0?editIdx=size-1:editIdx--;}
        void editVal(){ editIdx = idx; }
        void saveVal(){ idx = editIdx; }
    };
    //Объявление класса "Лист" типа время (DTime)
    class MenuLine::MenuLeaf::MenuLeaf_time: public MenuLine::MenuLeaf {
        DTime *time,    //текущее значение
              editTime; //редактируемое значение
        String format = "HH:MM";
        void nextVal(DTime &_tm);
        void prevVal(DTime &_tm);
        String getValue(DTime &_tm);
      public:
        MenuLeaf_time(){ partsCnt = 2; parts = new byte [partsCnt]; parts[0]=0; parts[1]=3;}// parts[2]=6;}
        String getValue(){ return getValue(editTime); }// { return decimate(time->hour) + ":" + decimate(time->minute) + ":" + decimate(time->second); }
        void setValue(DTime *_tm){ time = _tm;} //задать значение через ссылку на глобальную переменную
        void setValue(byte _h,byte _m,byte _s){ if(time) delete time; time = new DTime; time->setTime(_h,_m,_s);} //задать новое значение
        void nextVal(){ nextVal(editTime); }
        void prevVal(){ prevVal(editTime); }
        void editVal(){ editTime.setTime(time->hour,time->minute,0); }
        void saveVal(){ time->setTime(editTime.hour,editTime.minute,0); }
    };
	//Объявление класса "Лист" типа дата (DTime)
    class MenuLine::MenuLeaf::MenuLeaf_date: public MenuLine::MenuLeaf {
        DTime *date, //текущее значение
              editDate; //редактируемое значение
        String format = "YYYY-MM-DD";
        void nextVal(DTime &_dt);
        void prevVal(DTime &_dt);
        String getValue(DTime &_dt);
      public:
        MenuLeaf_date(){ partsCnt = 3; parts = new byte [partsCnt]; parts[0]=0; parts[1]=5; parts[2]=8;}
        String getValue(){ return getValue(editDate); }// { return (String) date->year + "-" + decimate(date->month) + "-" + decimate(date->day); }
        void setValue(DTime *_dt){ date = _dt;} //задать значение через ссылку на глобальную переменную
        void setValue(uint16_t _y,uint8_t _m,uint8_t _d){ if(date) delete date; date = new DTime; date->setDate(_y,_m,_d);} //задать новое значение
        void nextVal(){ nextVal(editDate); }
        void prevVal(){ prevVal(editDate); }
        void editVal(){ editDate.setDate(date->year,date->month,date->day); }
        void saveVal(){ date->setDate(editDate.year,editDate.month,editDate.day); }
    };
    class MenuLine::MenuLeaf::MenuLeaf_func: public MenuLine::MenuLeaf {
      public:
        pFunc func;
        MenuLeaf_func(){ mlType = mtFunc; }
        MenuLeaf_func(const pFunc &_f):func(_f){ mlType = mtFunc; }
        setValue(const pFunc &_f){ func = _f; }
    };

    ////events receiver from controller
    void onAction(const mcPos _mcp,const mcEvent _mce,const MenuController &_mc);

    ////вывод меню на дисплей
    virtual void show(); //вывести на дисплей меню в режиме навигации по меню
    virtual void edit(); //вывести на дисплей элемент в режиме редактирования
    virtual void enable(); //активировать меню и вывести на дисплей
    virtual void disable();//деактивировать меню и очистить дисплей
    bool isActive(){ return activeMenu;}; //меню активно?

    const LiquidCrystal& newDisplay(const byte rsPin, const byte enPin, const byte d4Pin, const byte d5Pin, const byte d6Pin, const byte d7Pin, const byte _cols=16,const byte _rows=2);

    void run(unsigned long _mls=0);//независимая от контроллера активность: вывод сообщений, выход по таймауту

  protected:
    MenuLine *pFirstLine; //адрес первой строки меню
    MenuLine *pActiveLine;//адрес активной строки меню

    MenuLine* newMenuLine(menuLineType mlt); //создание строки меню указанного типа

    ////создание структуры меню
    virtual void create(){}; //создать меню, переопределяется в дочернем классе, смотри пример

  private:
    unsigned long mls;     //время millis()
    unsigned long mlsInactionStart;
    bool activeMenu=false; //индикатор активности меню
    bool editMode=false; //режим редактирования\навигации

    byte lcd_last_row;   //последняя строка дисплея
    byte lcd_last_col;   //последняя колонка дисплея
    byte lcd_active_row=0;//активная строка дисплея
    LiquidCrystal *lcd=nullptr;//адрес дисплея

    ////для внутренних манипуляций
    void moveUP();   //переместиться вверх по меню
    void moveDOWN(); //переместиться вниз по меню
    void moveIN();   //войти внутрь узла
    void moveOUT();  //выйти из узла
    void execFunc(); //выполнить функцию

    void prevVal();  //следующее значение элемента редактируемой строки
    void nextVal();  //предыдущее значение элемента редактируемой строки
    void nextPart(); //переключиться на следующий элемент редактируемой строки
};


#endif
