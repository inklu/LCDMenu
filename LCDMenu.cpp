#include "LCDMenu.h"

//Добавление 0 для времени и даты
String decimate(byte b) { return ((b < 10) ? "0" : "") + String(b); }

//Установка значений списка через массив
void Menu::MenuLine::MenuLeaf::MenuLeaf_list::setValue(String _vals[],byte _size){
  size = _size;
  if (values) { delete[] values; }
  values = new String [size];
  for (int i = 0; i<size; i++){
	  values[i]=_vals[i];
  }
}

//Следующее значение одного из компонентов времени при редактировании
void Menu::MenuLine::MenuLeaf::MenuLeaf_time::nextVal(){
  byte h = time->hour;
  byte m = time->minute;
  byte s = time->second;
  switch(part) {
	  case 0: 
		  if(++h==24) h=0;
  		  break;
	  case 1:
		  if(++m==60) m=0;
	      break;
      case 2:
		  if(++s==60) s=0;
          break;
  }
  time->setTime(h,m,s);
}

//Предыдущее значение одного из компонентов времени при редактировании
void Menu::MenuLine::MenuLeaf::MenuLeaf_time::prevVal(){
  byte h = time->hour;
  byte m = time->minute;
  byte s = time->second;
  switch(part) {
	  case 0: 
		  if(h--==0) h=23;
  		  break;
	  case 1:
		  if(m--==0) m=59;
	      break;
      case 2:
		  if(s--==0) s=59;
          break;
  }
  time->setTime(h,m,s);
}

//Следующее значение одного из компонентов даты при редактировании
void Menu::MenuLine::MenuLeaf::MenuLeaf_date::nextVal(){
  uint16_t y = date->year;
  uint8_t m = date->month;
  uint8_t d = date->day;
  switch(part) {
	  case 0: 
		  if(++y==9999) y=0;
  		  break;
	  case 1:
		  if(++m==13) m=1;
	      break;
      case 2:
		  if(++d==32) d=1;
          break;
  }
  date->setDate(y,m,d);
}

//Предыдущее значение одного из компонентов даты при редактировании
void Menu::MenuLine::MenuLeaf::MenuLeaf_date::prevVal(){
  uint16_t y = date->year;
  uint8_t m = date->month;
  uint8_t d = date->day;
  switch(part) {
	  case 0: 
		  if(y--==0) y=9998;
  		  break;
	  case 1:
		  if(m--==1) m=12;
	      break;
      case 2:
		  if(d--==1) d=31;
          break;
  }
  date->setDate(y,m,d);
}

//Добавить строку в меню заданного типа
Menu::MenuLine* Menu::addMenuLine(menuLineType mlt){
  MenuLine *ml;
  switch(mlt){
    case mtNode:
      ml = new Menu::MenuLine::MenuNode;
      break;
    case mtInt:
      ml = new Menu::MenuLine::MenuLeaf::MenuLeaf_int;
      break;
    case mtList:
      ml = new Menu::MenuLine::MenuLeaf::MenuLeaf_list;
      break;
    case mtTime:
      ml = new Menu::MenuLine::MenuLeaf::MenuLeaf_time;
      break;
    case mtDate:
      ml = new Menu::MenuLine::MenuLeaf::MenuLeaf_date;
      break;
    /*case mtString:
      ml = new Menu::MenuLine::MenuLeaf::MenuLeaf_str;
      break;*/
  }
  return ml;
}

//вывести на дисплей многоточие
inline void Menu::lcdPrintNodeEllipsis(){
  lcd->print("...");
}

//вывести на дисплей знак >
inline void Menu::lcdPrintActiveLine(){
  lcd->print(">");
}

//отрисовать меню в режиме просмотра
void Menu::show(){
  Menu::MenuLine *pLine;

  editMode = false;
  lcd->noCursor();

  //подгон меню под размеры экрана
  if(!lcd_active_row && !pActiveLine->pNextLine && pActiveLine->pPreviousLine){
    lcd_active_row++;
  }
  else if(lcd_active_row && pActiveLine->pNextLine && !pActiveLine->pPreviousLine){
    lcd_active_row = 0;
  }
  
  byte row_up = lcd_active_row, row_down = lcd_active_row;

  //вывод активной строки меню
  lcd->clear();
  lcd->setCursor(0,lcd_active_row);
  lcdPrintActiveLine();
  lcd->print(pActiveLine->name);
  if(pActiveLine->isNode()){
    lcdPrintNodeEllipsis();
  }

  //вывод строк перед активной строкой (выше)
  pLine = pActiveLine;
  while(row_up-- && (pLine=pLine->pPreviousLine,pLine)){
    lcd->setCursor(1,row_up);
    lcd->print(pLine->name);
    if(pLine->isNode()){
      lcdPrintNodeEllipsis();
    }
  }

  //вывод строк после активной строкой (ниже)
  pLine = pActiveLine;
  while(row_down++ < lcd_last_row && (pLine=pLine->pNextLine,pLine)){
    lcd->setCursor(1,row_down);
    lcd->print(pLine->name);
    if(pLine->isNode()){
      lcdPrintNodeEllipsis();
    }
  }
}

//отрисовать меню в режиме редактирования
void Menu::edit() {
  editMode = true;

  lcd->clear();

  //название элемента
  lcd->setCursor(0,0);
  lcd->print(pActiveLine->name);

  lcd->setCursor(0,1);
  lcdPrintActiveLine();

  //значение элемента с курсором на редактируемом элементе
  lcd->print(pActiveLine->getValue());
  lcd->setCursor(((Menu::MenuLine::MenuLeaf *) pActiveLine)->getShift() + 1,1);
  lcd->cursor();
}

//активировать меню и отобразить в режиме просмотра
void Menu::enable() {
	activeMenu = true;
	mlsInactionStart = mls;
	show();
}	

//деактивировать меню и очистить дисплей
void Menu::disable() {
	activeMenu = false;
	lcd->clear();
}	

//обработать события контроллера меню и передать их на обработку в меню
void Menu::run(unsigned long _mls=0) {
  if(!_mls) mls = millis();
  else mls = _mls;
  ctrl->run(mls);

  //здесь добавить вывод сообщения

  //изменение состояния меню активно\неактивно по долгому нажатию ok\back или таймауту
  if (activeMenu && ((mls - mlsInactionStart) > activityTime)){ disable(); }
  else if (activeMenu && ctrl->isHOLDBACK()) { disable(); ctrl->resetBACK(); }
  else if (!activeMenu && ctrl->isHOLDOK()) { enable(); ctrl->resetOK(); }
  
	
  //обрабатываем события кнопок только если меню активно
  if (activeMenu) {
    if (ctrl->isUP()){ 
      if(!editMode) { moveUP(); } //вверх при просмотре
      else { prevVal(); }         //предыдущее значение при редактировании
      ctrl->resetUP();
	  mlsInactionStart = mls;     
    }
    else if (ctrl->isDOWN()){ 
      if(!editMode) { moveDOWN(); }//вниз при просмотре
      else { nextVal(); }          //следующее значение при редактировании
      ctrl->resetDOWN();
	  mlsInactionStart = mls;
    }
    else if (ctrl->isOK()){ 
      if(!editMode) {
        if(pActiveLine->isNode()) { moveIN(); } //входим внутрь узла
        else { edit(); }						//вызываем на редактирование лист
      }
	  else {
		nextPart();					//переходим к следующему элементу значения при редактировании
	  }
      ctrl->resetOK();
	  mlsInactionStart = mls;
    }
    else if (ctrl->isBACK()){ 
      if(!editMode) { moveOUT(); }  //выход на вышестоящий уровень иерархии при просмотре
      else { show(); }				//переход в режим просмотра иерархии
      ctrl->resetBACK();
	  mlsInactionStart = mls;
    }
    else {}
  }
}

//Перемещаемся вверх по иерархии
void Menu::moveUP(){
  if(pActiveLine->pPreviousLine){
    pActiveLine = pActiveLine->pPreviousLine;
    if(lcd_active_row){
      lcd_active_row--;
    }  
    show();
  }
}
//Перемещаемся вниз по иерархии
void Menu::moveDOWN(){
  if(pActiveLine->pNextLine){
    pActiveLine = pActiveLine->pNextLine;
    if(lcd_active_row<lcd_last_row){
      lcd_active_row++;
    }  
    show();
  }
}
//Перемещаемся внутрь узла
void Menu::moveIN(){
  Menu::MenuLine::MenuNode *pNode;
  if(pActiveLine->isNode()){
    pNode = pActiveLine;
    pActiveLine = pNode->pFirstChild;
    lcd_active_row = 0;
    show();
  }
}
//Перемещаемся наружу из узла
void Menu::moveOUT(){
  if(pActiveLine->pParentLine){
    pActiveLine = pActiveLine->pParentLine;
    show();
  }
}
//Выбор предыдущего значения при редактировании
void Menu::prevVal(){
  MenuLine::MenuLeaf *ml = pActiveLine;
  ml->prevVal(); 
  edit();
}
//Выбор следующего значения при редактировании
void Menu::nextVal(){
  MenuLine::MenuLeaf *ml = pActiveLine;
  ml->nextVal(); 
  edit();
}
//Выбор следующего элемента значения при редактировании
void Menu::nextPart(){
  MenuLine::MenuLeaf *ml = pActiveLine;
  ml->nextPart(); 
  edit();
}
