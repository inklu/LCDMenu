#include "LCDMenuController2.h"
#include <DTime.h>

//derive class to overload create method 
class MyMenu:public MenuController::Menu {
  public:
    MyMenu(){ create(); }
    void create();
};

//declare calling function from menu
void f1();

//global date&time for clock
DTime dtime(2019,05,27,10,20,30);
uint64_t cycle;

//create menu
void MyMenu::create() {
  MenuLine::MenuLeaf::MenuLeaf_int *mli;   //integer value
  MenuLine::MenuLeaf::MenuLeaf_list *mlst; //list
  MenuLine::MenuLeaf::MenuLeaf_time *mlt;  //time
  MenuLine::MenuLeaf::MenuLeaf_date *mld;  //date
  MenuLine::MenuLeaf::MenuLeaf_func *mlf;  //function
  MenuLine::MenuNode *mn;
  
  pFirstLine = mli = newMenuLine(mtInt); //first line is an integer value leaf 
  mli->name = "Integer1";                //line name
  //mli->value = 10;            
  *mli = 10;               //set value, step=1

  mn = newMenuLine(mtNode);              //create node
  mn->name = "Node1";            //node name
  pFirstLine->pNextLine = mn;      //node follows first line
  mn->pPreviousLine = pFirstLine;    //predecessor is first line

  mlst = newMenuLine(mtList);         //create leaf of list value
  mlst->name = "List2";             //leaf name
  String lst[]={"Hello","World","I love you!"}; //values of leaf
  mlst->setValue(lst,3);            //set values
  mn->pNextLine = mlst;             //follows node
  mlst->pPreviousLine = mn;           //predecessor is node
  
  mli = newMenuLine(mtInt);       //create another integer value leaf
  mli->name = "Integer3";       //leaf name
  //mli->value = 30;
  *mli = 30;              //value
  //mli->step = 3;
  //mli->setStep(3);            //step
  mn->pNextLine->pNextLine = mli;   //follows the list
  mli->pPreviousLine = mn->pNextLine; //predecessor is list

  mlt = newMenuLine(mtTime);  //create leaf of time
  mlt->name = "Time";     //leaf name
  mlt->setValue(&dtime);    //set value as a pointer to global variable
  mn->pFirstChild = mlt;    //the first line of the node
  mlt->pParentLine = mn;    //parent is node
  
  mld = newMenuLine(mtDate);      //create leaf of date
  mld->name = "Date";         //leaf name
  mld->setValue(&dtime);        //set value as a pointer to global variable
  mld->pParentLine = mn;        //parent is node
  mn->pFirstChild->pNextLine = mld;   //follows the time line
  mld->pPreviousLine = mn->pFirstChild; //predecessor - time line

  mlf = newMenuLine(mtFunc);      //create leaf with function
  mlf->name = "Func1";         //leaf name
  mlf->setValue(f1);        //value as a ponter to function
  mlf->pParentLine = mn;        //parent is node
  mld->pNextLine = mlf;   //follows the date leaf
  mlf->pPreviousLine = mld; //predecessor is the date leaf

  pActiveLine = pFirstLine;  //active line is the first line
}

MyMenu m; //declares menu object
MenuButtonController mbc(m); //declares button controller & bind menu to controller
//MenuButtonController mbc;

//LCD parms
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const LiquidCrystal &lcd = m.newDisplay(rs,en,d4,d5,d6,d7,16,2); //create lcd for menu

//define void function 
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
  Serial.println((unsigned long)&mbc.newButtonOk(A4,139),HEX); //create button OK in controller 
  Serial.println((unsigned long)&mbc.newButtonUp(A4,42),HEX); //create button Up in controller
  Serial.println((unsigned long)&mbc.newButtonDown(A4,93),HEX); //create button Down in controller
  Serial.println((unsigned long)&mbc.newButtonBack(A4,14),HEX); //create button Back in controller
  //m.create();
  //mbc.bindMenu(m);

  cycle = (uint64_t)millis() + 1000;

}

void loop() {
  // put your main code here, to run repeatedly:
  mbc.run(); //holding OK enables menu, holding Back disables
  
  //if (!m.isActive()) { lcd... } //if menu is disabled the lcd can be used for other purposes

  if ((uint64_t)millis() >= cycle) {
    cycle += 1000;
    dtime.tick(); //+1sec to clock
  //display date&time when menu is disabled 
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
