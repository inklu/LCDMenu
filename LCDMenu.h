#ifndef LCDMENU_H
#define LCDMENU_H

#if ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

#include "LCDMenuController.h"
#include <LiquidCrystal.h>
#include <DTime.h>

//Добавление 0 для времени и даты
String decimate(byte b); //{ return ((b < 10) ? "0" : "") + String(b); }

//Объявление класса Меню, Строки меню, Узлы и Листы 
class Menu {
  public:
	//Типы строк: Узел, Целочисленный, с плавающей запятой, список, время, дата, строка, IP, ...
    enum menuLineType {mtNONE, mtNode, mtInt, mtFloat, mtList, mtTime, mtDate, mtString, mtIP, mtURL};
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
		int *parts=nullptr,part=0;  //смещения на элементы значения листа
      public:
        class MenuLeaf_int;			//Объявление листа типа целочисленный
        class MenuLeaf_list;		//Объявление листа типа список
        class MenuLeaf_time;		//Объявление листа типа время
        class MenuLeaf_date;		//Объявление листа типа дата
        class MenuLeaf_str;			//Объявление листа типа строка
        virtual void nextVal(){};   //Следующее значение текущего элемента листа
        virtual void prevVal(){};   //Предыдущее значение текущего элемента листа
		virtual void nextPart(){};  //Установка следующего элемента значеня
		virtual int getShift(){ return 0; } //Получить смещение текущего элемента значения
        //void edit();
      //private:
        
    };
	//Объявление класса "Лист" целочисленного значения
    class MenuLine::MenuLeaf::MenuLeaf_int: public MenuLine::MenuLeaf {
        int value;     //значение
        int defValue;
        int step;	   //шаг изменения
        //void edit();
      public:
        MenuLeaf_int(int _val=0, int _step=1) { mlType = mtInt; value = defValue = _val; step = _step; };
        String getValue() { //String s; s = value; return s; 
			return (String) value; }
        MenuLeaf_int operator=(int _val){ value = defValue = _val; return *this; } //задать значение
        void setStep(int _st){ step = _st; } //задать шаг
        void nextVal(){value+=step;}
        void prevVal(){value-=step;}
    };
	//Объявление класса "Лист" типа список
    class MenuLine::MenuLeaf::MenuLeaf_list: public MenuLine::MenuLeaf {
		String *values=nullptr; //массив значений
		byte idx=0;             //текущее значение
		byte size;				//размер массива значений
	  public:
		MenuLeaf_list(){};
		String getValue() { return values[idx];}
		void setValue(String _vals[], byte _size); //задать массив значений
        void nextVal(){++idx==size?idx=0:idx;}
        void prevVal(){idx==0?idx=size-1:idx--;}
	};
	//Объявление класса "Лист" типа время (DTime)
	class MenuLine::MenuLeaf::MenuLeaf_time: public MenuLine::MenuLeaf {
		DTime *time; //значение
	  public:
		MenuLeaf_time(){ parts = new int [3]; parts[0]=0; parts[1]=3; parts[2]=6;}
		String getValue() { 
			return decimate(time->hour) + ":" + 
				   decimate(time->minute) + ":" + 
				   decimate(time->second);
		}
		void setValue(DTime *_tm){ time = _tm;} //задать значение через ссылку на глобальную переменную
		void setValue(byte _h,byte _m,byte _s){ time = new DTime; time->setTime(_h,_m,_s);} //задать новое значение
		void nextVal();
		void prevVal();
		void nextPart(){ if(++part==3) part=0;} 
		int getShift() { return parts[part]; }
	};
	//Объявление класса "Лист" типа дата (DTime)
	class MenuLine::MenuLeaf::MenuLeaf_date: public MenuLine::MenuLeaf {
		DTime *date; //значение
	  public:
		MenuLeaf_date(){ parts = new int [3]; parts[0]=0; parts[1]=5; parts[2]=8;}
		String getValue() { 
			return (String) date->year + "-" + 
				   decimate(date->month) + "-" + 
				   decimate(date->day);
		}
		void setValue(DTime *_dt){ date = _dt;} //задать значение через ссылку на глобальную переменную
		void setValue(uint16_t _y,uint8_t _m,uint8_t _d){ date = new DTime; date->setDate(_y,_m,_d);} //задать новое значение
		void nextVal();
		void prevVal();
		void nextPart(){ if(++part==3) part=0;} 
		int getShift() { return parts[part]; }  
	};
    //Menu(){};
    //~Menu(){};
    MenuLine* addMenuLine(menuLineType mlt); //создание строки меню указанного типа
    void addController(Controller *_ctrl){ctrl = _ctrl;} //добавить контроллер меню
	//добавить дисплей для меню, по умолчанию используется 16 столбцов и 2 строки
    void addDisplay(LiquidCrystal *_lcd,const byte _cols=16,const byte _rows=2){lcd = _lcd;lcd_last_col = _cols-1;lcd_last_row = _rows-1;} 
    virtual void create(){}; //создать меню, переопределяется в дочернем классе, смотри пример
    void run(unsigned long _mls=0); //обработка событий от контроллера
  //private:
    void show(); //вывести на дисплей меню в режиме навигации по меню
    void edit(); //вывести на дисплей элемент в режиме редактирования
    void enable(); //активировать меню и вывести на дисплей
    void disable();//деактивировать меню и очистить дисплей
	bool isActive(){ return activeMenu;}; //меню активно?
  //protected:
    MenuLine *pFirstLine; //адрес первой строки меню
    MenuLine *pActiveLine;//адрес активной строки меню
  private:
	bool activeMenu=false; //индикатор активности меню
    unsigned long mls;     //время millis()
    unsigned long mlsMessageStart;
	unsigned long mlsInactionStart;
    bool showMessage=false;
    char *message;
    const int messageTime=1000;
	const int activityTime=30000; //таймаут для деактивации
    
    bool editMode=false; //режим редактирования\навигации
    byte lcd_last_row;   //последняя строка дисплея
    byte lcd_last_col;   //последняя колонка дисплея
    byte lcd_active_row=0;//активная строка дисплея
    Controller *ctrl=nullptr; //адрес контроллера
    LiquidCrystal *lcd=nullptr;//адрес дисплея
    inline void lcdPrintNodeEllipsis();//напечатать многоточие
    inline void lcdPrintActiveLine();//напечатать >

    void moveUP();   //переместиться вверх по меню
    void moveDOWN(); //переместиться вниз по меню
    void moveIN();   //войти внутрь узла
    void moveOUT();  //выйти из узла
    void prevVal();  //следующее значение элемента редактируемой строки
    void nextVal();  //предыдущее значение элемента редактируемой строки
	void nextPart(); //переключиться на следующий элемент редактируемой строки
};


#endif