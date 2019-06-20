#include "LCDMenuController2.h"
#include <DTime.h>

//наследуем класс Menu, чтобы переопределить метод create
class MyMenu:public MenuController::Menu {
  public:
    MyMenu(){ create(); }
    void create();
};

//выполняемую функцию добавляем в меню
void f1();

//переменная времени и даты для часов
DTime dtime(2019,05,27,10,20,30);
uint64_t cycle;

//создаём меню
void MyMenu::create() {
  MenuLine::MenuLeaf::MenuLeaf_int *mli;   //целое значение
  MenuLine::MenuLeaf::MenuLeaf_list *mlst; //список
  MenuLine::MenuLeaf::MenuLeaf_time *mlt;  //время
  MenuLine::MenuLeaf::MenuLeaf_date *mld;  //дата
  MenuLine::MenuLeaf::MenuLeaf_func *mlf;  //функция
  MenuLine::MenuNode *mn;
  
  pFirstLine = mli = newMenuLine(mtInt); //создаём первую строку меню с целым значением
  mli->name = "Integer1";                //имя строки
  //mli->value = 10;            
  *mli = 10;               //значение, шаг=1

  mn = newMenuLine(mtNode);              //создаём узел
  mn->name = "Node1";            //имя узла
  pFirstLine->pNextLine = mn;      //идёт следом за первой строкой меню
  mn->pPreviousLine = pFirstLine;    //предшественник - первая строка

  mlst = newMenuLine(mtList);         //создаём строку список
  mlst->name = "List2";             //имя строки
  String lst[]={"Hello","World","I love you!"}; //значения строки
  mlst->setValue(lst,3);            //установка значений
  mn->pNextLine = mlst;             //следует за узлом
  mlst->pPreviousLine = mn;           //предшественник - узел
  
  mli = newMenuLine(mtInt);       //создаём строку меню с целым значением
  mli->name = "Integer3";       //имя строки
  //mli->value = 30;
  *mli = 30;              //значение
  //mli->step = 3;
  //mli->setStep(3);            //шаг
  mn->pNextLine->pNextLine = mli;   //следует за списком
  mli->pPreviousLine = mn->pNextLine; //предшественник - список

  mlt = newMenuLine(mtTime);  //создаём первую строку меню со временем
  mlt->name = "Time";     //имя строки
  mlt->setValue(&dtime);    //значение - ссылка на часы (глобальная переменная) 
  mn->pFirstChild = mlt;    //первая строка узла Node1
  mlt->pParentLine = mn;    //родитель - узел Node1
  
  mld = newMenuLine(mtDate);      //создаём строку меню с датой
  mld->name = "Date";         //имя строки
  mld->setValue(&dtime);        //значение - ссылка на дату (глобальная переменная)
  mld->pParentLine = mn;        //родитель - узел Node1
  mn->pFirstChild->pNextLine = mld;   //следует за строкой Time
  mld->pPreviousLine = mn->pFirstChild; //предшественник - строка Time

  mlf = newMenuLine(mtFunc);      //создаём строку меню с функцией
  mlf->name = "Func1";         //имя строки
  mlf->setValue(f1);        //значение - ссылка на дату (глобальная переменная)
  mlf->pParentLine = mn;        //родитель - узел Node1
  mld->pNextLine = mlf;   //следует за строкой Date
  mlf->pPreviousLine = mld; //предшественник - строка Date

  pActiveLine = pFirstLine;  //активная строка - первая строка
}

MyMenu m;
byte xPin=A1,yPin=A0,swPin=8;
MenuJoystickController mjc(xPin,yPin,swPin);

//LCD parms
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const LiquidCrystal &lcd = m.newDisplay(rs,en,d4,d5,d6,d7,16,2);

void f1(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Function f1()");
  lcd.setCursor(0,1);
  lcd.print("executed!");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Start");

  mjc.bindMenu(m);
  //mjc.setTopPos(xPin,LOW);

  cycle = (uint64_t)millis() + 1000;

}

void loop() {
  // put your main code here, to run repeatedly:
  mjc.run(); //длительное нажатие на OK автивирует меню, длительное нажатие на BACK деактивирует меню
  
  //if (!m.isActive()) { lcd... } //если меню не активно, то можно использовать lcd для других целей

  if ((uint64_t)millis() >= cycle) {
    cycle += 1000;
    dtime.tick(); //+1сек к часам
  //вывод времени и даты, когда меню не активно
  if (!m.isActive()){
      lcd.clear();
      lcd.noCursor();
      lcd.setCursor(0,0);
      lcd.print(decimate(dtime.hour)+":"+decimate(dtime.minute)+":"+decimate(dtime.second));
      lcd.setCursor(0,1);
      lcd.print((String) dtime.year+"-"+decimate(dtime.month)+"-"+decimate(dtime.day));
    }
  }  
}
