#include <AnalogButton.h>
#include <DigitalButton.h>

#include <LCDMenuController.h>
#include <LCDMenuButtonController.h>
#include <LCDMenu.h>

#include <DTime.h>

//наследуем класс Menu, чтобы переопределить метод create
class MyMenu:public Menu {
  public:
    void create();
};

//переменная времени и даты для часов
DTime dtime(2019,05,27,10,20,30);
uint64_t cycle;

//создаём меню
void MyMenu::create() {
  Menu::MenuLine::MenuLeaf::MenuLeaf_int *mli;   //целое значение
  Menu::MenuLine::MenuLeaf::MenuLeaf_list *mlst; //список
  Menu::MenuLine::MenuLeaf::MenuLeaf_time *mlt;  //время
  Menu::MenuLine::MenuLeaf::MenuLeaf_date *mld;  //дата
  Menu::MenuLine::MenuNode *mn;
  
  pFirstLine = mli = addMenuLine(mtInt); //создаём первую строку меню с целым значением
  mli->name = "Integer1";                //имя строки
  //mli->value = 10;				    
  *mli = 10;							 //значение, шаг=1

  mn = addMenuLine(mtNode);              //создаём узел
  mn->name = "Node1";				     //имя узла
  pFirstLine->pNextLine = mn;			 //идёт следом за первой строкой меню
  mn->pPreviousLine = pFirstLine;		 //предшественник - первая строка

  mlst = addMenuLine(mtList);					//создаём строку список
  mlst->name = "List2";							//имя строки
  String lst[]={"Hello","World","I love you!"}; //значения строки
  mlst->setValue(lst,3);						//установка значений
  mn->pNextLine = mlst;							//следует за узлом
  mlst->pPreviousLine = mn;						//предшественник - узел
  
  mli = addMenuLine(mtInt);			  //создаём строку меню с целым значением
  mli->name = "Integer3";			  //имя строки
  //mli->value = 30;
  *mli = 30;						  //значение
  //mli->step = 3;
  mli->setStep(3);					  //шаг
  mn->pNextLine->pNextLine = mli;	  //следует за списком
  mli->pPreviousLine = mn->pNextLine; //предшественник - список

  mlt = addMenuLine(mtTime);  //создаём первую строку меню со временем
  mlt->name = "Time";		  //имя строки
  mlt->setValue(&dtime);	  //значение - ссылка на часы (глобальная переменная) 
  mn->pFirstChild = mlt;	  //первая строка узла Node1
  mlt->pParentLine = mn;	  //родитель - узел Node1
  
  mld = addMenuLine(mtDate);			//создаём строку меню с датой
  mld->name = "Date";					//имя строки
  mld->setValue(&dtime);				//значение - ссылка на дату (глобальная переменная)
  mld->pParentLine = mn;				//родитель - узел Node1
  mn->pFirstChild->pNextLine = mld;		//следует за строкой Time
  mld->pPreviousLine = mn->pFirstChild; //предшественник - строка Time

  pActiveLine = pFirstLine;  //активная строка - первая строка
}

const int btnPin=A5;           //кнопки на пине A5
AnalogButton btUP(btnPin,160), //кнопка вверх, уровень сигнала 160 (R=2K)
             btDOWN(btnPin,68),//кнопка вниз, уровень сигнала 68 (R=2K)
             btOK(btnPin,116), //кнопка ок, уровень сигнала 116 (R=2K)
             btBACK(btnPin,14);//кнопка назад, уровень сигнала 14 (R=2K)

ButtonController ctrl; //кнопочный контроллер

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); //дисплей

MyMenu menu; //меню

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  ctrl.addUP(btUP);     //добавляем в контроллер кнопку вверх 
  ctrl.addDOWN(btDOWN); //добавляем в контроллер кнопку вниз 
  ctrl.addOK(btOK);     //добавляем в контроллер кнопку ок
  ctrl.addBACK(btBACK); //добавляем в контроллер кнопку назад

  lcd.begin(16,2);      //инициализация дисплея

  menu.create();             //создание меню
  menu.addController(&ctrl); //добавление контроллера в меню
  menu.addDisplay(&lcd);     //добавление дисплея в меню
  //menu.show();

  cycle = (uint64_t)millis() + 1000;
}

void loop() {
  // put your main code here, to run repeatedly:

  menu.run(); //длительное нажатие на OK автивирует меню, длительное нажатие на BACK деактивирует меню
  //if (!menu.isActive()) { lcd... } //если меню не активно, то можно использовать lcd для других целей

  if ((uint64_t)millis() >=  cycle) {
    cycle += 1000;
    dtime.tick(); //+1сек к часам
	//вывод времени и даты, когда меню не активно
	if (!menu.isActive()){
      lcd.clear();
      lcd.noCursor();
      lcd.setCursor(0,0);
      lcd.print(decimate(dtime.hour)+":"+decimate(dtime.minute)+":"+decimate(dtime.second));
      lcd.setCursor(0,1);
      lcd.print((String) dtime.year+"-"+decimate(dtime.month)+"-"+decimate(dtime.day));
    }
  }

}
