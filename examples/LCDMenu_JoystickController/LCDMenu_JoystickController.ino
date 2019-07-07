#include <LCDMenuController.h>
#include <DTime.h>

//derive class to overload create method 
class MyMenu:public MenuController::Menu {
  public:
    MyMenu(){ create(); }
    void create();
};

//global variables binded to menu
int8_t in=10;  //integer
float fl=20.32; //float
String st="Hello!"; //string
bool bl=false; //bool value
byte ls=0; //list index

//declare calling function from menu
void f1();

//global date&time for clock
DTime dtime(2019,05,27,10,20,30);
uint64_t cycle;

//create menu
void MyMenu::create() {
  MenuLine::MenuLeaf::MenuLeaf_num<int8_t> *mli;   //integer value
  MenuLine::MenuLeaf::MenuLeaf_num<float> *mlfl;   //float
  MenuLine::MenuLeaf::MenuLeaf_str *mls; //string
  MenuLine::MenuLeaf::MenuLeaf_list *mlst; //list
  MenuLine::MenuLeaf::MenuLeaf_time *mlt;  //time
  MenuLine::MenuLeaf::MenuLeaf_date *mld;  //date
  MenuLine::MenuLeaf::MenuLeaf_func *mlf;  //function
  MenuLine::MenuNode *mn; //node
  
  pFirstLine = mli = new MenuLine::MenuLeaf::MenuLeaf_num<int8_t>; //first line is an integer value leaf 
  mli->name = "Integer1"; //line name
  mli->setValue(0,2,-20,50,&in);  //set value: decimal point=0, step=2, min val=-20, max val=50, pointer to "in" variable

  mn = new MenuLine::MenuNode;  //create node
  mn->name = "Node1";            //node name
  pFirstLine->pNextLine = mn;      //node follows first line
  mn->pPreviousLine = pFirstLine;    //predecessor is first line

  mlst = new MenuLine::MenuLeaf::MenuLeaf_list;   //create leaf of list value
  mlst->name = "List2";     //leaf name
  String lst[]={"Hello","World","I love you!"}; //values of leaf
  mlst->setValue(lst,3,&ls);   //set values: list of values, size=3, pointer to "byte ls" variable which represent the index of the selected value
  mn->pNextLine = mlst;             //follows node
  mlst->pPreviousLine = mn;           //predecessor is node
  
  mlfl = new MenuLine::MenuLeaf::MenuLeaf_num<float>;       //create float value leaf
  mlfl->name = "Float3";       //leaf name
  mlfl->setValue(2,0.25,-30.5,20.75,&fl);  //set value: decimal point=2, step=0.25, min val=-30.5, max val=20.75, pointer to "fl" variable
  mn->pNextLine->pNextLine = mlfl;   //follows the list
  mlfl->pPreviousLine = mn->pNextLine; //predecessor is list

  mlt = new MenuLine::MenuLeaf::MenuLeaf_time;  //create leaf of time
  mlt->name = "Time";     //leaf name
  mlt->setValue(&dtime);    //set value as a pointer to global variable
  mn->pFirstChild = mlt;    //the first line of the node
  mlt->pParentLine = mn;    //parent is node
  
  mld = new MenuLine::MenuLeaf::MenuLeaf_date;      //create leaf of date
  mld->name = "Date";         //leaf name
  mld->setValue(&dtime);        //set value as a pointer to global variable
  mld->pParentLine = mn;        //parent is node
  mn->pFirstChild->pNextLine = mld;   //follows the time line
  mld->pPreviousLine = mn->pFirstChild; //predecessor - time line

  mlf = new MenuLine::MenuLeaf::MenuLeaf_func;      //create leaf with function
  mlf->name = "Func1";         //leaf name
  mlf->setValue(f1);        //value as a ponter to function "void f1()"
  mlf->pParentLine = mn;        //parent is node
  mld->pNextLine = mlf;   //follows the date leaf
  mlf->pPreviousLine = mld; //predecessor is the date leaf

  mls = new MenuLine::MenuLeaf::MenuLeaf_str;  //create leaf with string
  mls->name = "String5";
  mls->setValue(&st); //reference to global string "st"
  mlfl->pNextLine = mls;   //str follows the float
  mls->pPreviousLine = mlfl; //str predecessor is float
  
  pActiveLine = pFirstLine; //active line is the first line
}

MyMenu m; //define menu object
byte xPin=A1,yPin=A0,swPin=8; //Joystick parms
MenuJoystickController mjc(xPin,yPin,swPin); //define joystick object

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

  mjc.bindMenu(m);// bind menu to controller
  mjc.setTopPos(xPin,LOW); //set Joystick Top position

  cycle = (uint64_t)millis() + 1000;

}

void loop() {
  // put your main code here, to run repeatedly:
  mjc.run(); //holding OK enables menu, holding Back disables
  
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
