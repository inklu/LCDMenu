#include "LCDMenuController2.h"

//Добавление 0 для времени и даты
//String decimate(byte b) { return ((b < 10) ? "0" : "") + String(b); }

////Menu controller
void MenuController::run(unsigned long _mls=0){ 
  if(!_mls) mls = millis();
  else mls = _mls;

  //Serial.println(mls);
  runEachMls(mls);

  if(mls >= cycle){
    runEachCycle(cycle);
    cycle += MC_CYCLE_MLS;
  }

  if(menu!=nullptr) menu->run(mls);
}

void MenuController::menuAction(const mcPos _mcp, const mcEvent _mce){
#ifdef DEBUG
Serial.print(_mcp);
Serial.print(" ");
Serial.println(_mce);
#endif
  switch(menu->isActive()){
    case false: 
      if(_mcp == mcpOK && _mce == mceHold) {
        cycle = mls + MC_CYCLE_MLS * 3; //пауза перед отправкой событий контроллера в меню
        menu->enable(); //при длительном нажатии OK активируем меню
      }
      break;
    case true:
      menu->onAction(_mcp,_mce,*this); //отправляем действие на обработку только в случае активности меню
      break;
  }
}

////Joystick controller
MenuJoystickController::MenuJoystickController(const uint8_t &_x_pin, 
                       const uint8_t &_y_pin, 
                       const uint8_t &_bt_pin, 
                       const uint16_t &_sig_high=JSTK_HIGH_SIG,
                       const uint16_t &_sig_low=JSTK_LOW_SIG,
                       const uint16_t &_sig_high_ts=JSTK_HIGH_TRESHOLD,
                       const uint16_t &_sig_low_ts=JSTK_LOW_TRESHOLD): Joystick(_x_pin,_y_pin,_bt_pin,_sig_high,_sig_low,_sig_high_ts,_sig_low_ts){
  mcType = mccJoystick;
}

void MenuJoystickController::runEachMls(unsigned long _mls){ 
  Joystick::run(_mls); 
}

void MenuJoystickController::runEachCycle(unsigned long _mls){ 
  if(isHolding) sendEvent(jspHolding,mceHold);
}

void MenuJoystickController::offClick(const jsPos jsp){
  sendEvent(jsp, mceClick);
}

void MenuJoystickController::onHold(const jsPos jsp){
  isHolding=true; 
  jspHolding=jsp; 
  sendEvent(jsp, mceHold);
}

void MenuJoystickController::sendEvent(const jsPos jsp, const mcEvent mce){
  switch(jsp){
    case jpUP:
      menuAction(mcpUP,mce);
      break;
    case jpDOWN:
      menuAction(mcpDOWN,mce);
      break;
    case jpLEFT:
      menuAction(mcpBACK,mce);
      break;
    case jpBUTTON:
      menuAction(mcpOK,mce);
      break;
  }
}

////Button controller
void MenuButtonController::runEachMls(unsigned long _mls){ 
  if(btUP!=nullptr) {
    if(btUP->btt == MenuButton::btDigital) ((MenuDigitalButton*)btUP)->run(_mls); 
    else ((MenuAnalogButton*)btUP)->run(_mls); 
  }
  if(btDOWN!=nullptr) {
    if(btDOWN->btt == MenuButton::btDigital) ((MenuDigitalButton*)btDOWN)->run(_mls); 
    else ((MenuAnalogButton*)btDOWN)->run(_mls); 
  }
  if(btOK!=nullptr) {
    if(btOK->btt == MenuButton::btDigital) ((MenuDigitalButton*)btOK)->run(_mls); 
    else ((MenuAnalogButton*)btOK)->run(_mls); 
  }
  if(btBACK!=nullptr) {
    if(btBACK->btt == MenuButton::btDigital) ((MenuDigitalButton*)btBACK)->run(_mls); 
    else ((MenuAnalogButton*)btBACK)->run(_mls); 
  }
}

void MenuButtonController::runEachCycle(unsigned long _mls){ 
  if(isHolding) onHold(*btHolding);
}

void MenuButtonController::offClick(const MenuButton& bt){
  mcPos m = mcpNONE;
  if(&bt==btUP) m = mcpUP;
  else if(&bt==btDOWN) m = mcpDOWN;
  else if(&bt==btOK) m = mcpOK;
  else if(&bt==btBACK) m = mcpBACK;
  if(m) menuAction(m,mceClick);
}

void MenuButtonController::onHold(const MenuButton& bt){
  mcPos m = mcpNONE;
  if(&bt==btUP) m = mcpUP;
  else if(&bt==btDOWN) m = mcpDOWN;
  else if(&bt==btOK) m = mcpOK;
  else if(&bt==btBACK) m = mcpBACK;
  if(m){
    isHolding = true;
    btHolding = &bt;
    menuAction(m,mceHold);
  }
}

const MenuButtonController::MenuDigitalButton& MenuButtonController::newButton(const mcPos mcp, const byte pin){
  switch(mcp){
    case mcpUP:
      if(btUP!=nullptr) delete btUP;
      btUP = new MenuDigitalButton(pin,*this);
      return (MenuDigitalButton&)*btUP;
    case mcpDOWN:
      if(btDOWN!=nullptr) delete btDOWN;
      btDOWN = new MenuDigitalButton(pin,*this);
      return (MenuDigitalButton&)*btDOWN;
    case mcpOK:
      if(btOK!=nullptr) delete btOK;
      btOK = new MenuDigitalButton(pin,*this);
      return (MenuDigitalButton&)*btOK;
    case mcpBACK:
      if(btBACK!=nullptr) delete btBACK;
      btBACK = new MenuDigitalButton(pin,*this);
      return (MenuDigitalButton&)*btBACK;
  }
}
const MenuButtonController::MenuAnalogButton& MenuButtonController::newButton(const mcPos mcp, const byte pin,const word sigVal){
  switch(mcp){
    case mcpUP:
      if(btUP!=nullptr) delete btUP;
      btUP = new MenuAnalogButton(pin,sigVal,*this);
      return (MenuAnalogButton&)*btUP;
    case mcpDOWN:
      if(btDOWN!=nullptr) delete btDOWN;
      btDOWN = new MenuAnalogButton(pin,sigVal,*this);
      return (MenuAnalogButton&)*btDOWN;
    case mcpOK:
      if(btOK!=nullptr) delete btOK;
      btOK = new MenuAnalogButton(pin,sigVal,*this);
      return (MenuAnalogButton&)*btOK;
    case mcpBACK:
      if(btBACK!=nullptr) delete btBACK;
      btBACK = new MenuAnalogButton(pin,sigVal,*this);
      return (MenuAnalogButton&)*btBACK;
  }
}


MenuButtonController::MenuDigitalButton::MenuDigitalButton(const byte _pin, const MenuButtonController& _mbc):DigitalButton(_pin),
                                                                                                              mbc(_mbc){
  btt = btDigital;
}

MenuButtonController::MenuAnalogButton::MenuAnalogButton(const byte _pin, const word _sigVal, const MenuButtonController& _mbc):AnalogButton(_pin,_sigVal),
                                                                                                                                mbc(_mbc){
  btt = btAnalog;
}

////Rotary Encoder controller
MenuEncoderController::MenuEncoderController(const byte _clkPin, const byte _dtPin, const byte _swPin):RotEnc(_clkPin,_dtPin,_swPin){
  mcType = mccRotaryEncoder;
}

void MenuEncoderController::runEachMls(unsigned long _mls){ 
  RotEnc::run(_mls); 
}

void MenuEncoderController::runEachCycle(unsigned long _mls){ 
  if(isHolding) menuAction(mcpOK,mceHold);
}

void MenuEncoderController::onClockwise(){
  menuAction(mcpDOWN, mceClick);
}

void MenuEncoderController::onCounterclockwise(){
  menuAction(mcpUP, mceClick);
}

void MenuEncoderController::offClick(){
  menuAction(mcpOK, mceClick);
}

void MenuEncoderController::onHold(){
  isHolding=true; 
  menuAction(mcpOK, mceHold);
}

////Menu
void MenuController::Menu::onAction(const mcPos _mcp, const mcEvent _mce, const MenuController &_mc){
  mlsInactionStart = mls;//сбрасываем счётчик таймаута
  switch(_mc.mcType){
    case mccButtons:
    case mccJoystick:
      //Buttons&Joystick similar
      switch(_mcp){
        case mcpUP:
          if(!editMode) moveUP();
          else nextVal();
        break;
        case mcpDOWN:
          if(!editMode) moveDOWN();
          else prevVal();
        break;
        case mcpOK:
          if(_mce==mceClick){ //короткое нажатие
            if(!editMode) {
              if(pActiveLine->isNode()) moveIN(); //входим внутрь узла
              else if(pActiveLine->isFunc()) execFunc();  //выполняем функцию
              else {
                ((MenuLine::MenuLeaf *)pActiveLine)->editVal(); //копирование значения для редактирования
                edit(); //вызываем на редактирование лист
              }
            }
	    else nextPart(); //переходим к следующему элементу значения при редактировании
          }
          else if(_mce==mceHold){ //длительное нажатие
            if(!editMode) moveOUT(); //выход на вышестоящий уровень меню при просмотре
            else show(); //переход в режим просмотра меню
          }
        break;
        case mcpBACK:
          if(!editMode) moveOUT(); //выход на вышестоящий уровень иерархии при просмотре
          else {
            ((MenuLine::MenuLeaf *)pActiveLine)->saveVal(); //сохранение значения после редактирования
            show(); //переход в режим просмотра иерархии
          }
        break;
      }
      break;
    case mccRotaryEncoder:
      //do peculiar action, there is no BACK command
      switch(_mcp){
        case mcpUP:
          if(!editMode) moveUP();
          else nextVal();
        break;
        case mcpDOWN:
          if(!editMode) moveDOWN();
          else prevVal();
        break;
        case mcpOK:
          if(_mce==mceClick){ //короткое нажатие
            if(!editMode) {
              if(pActiveLine->isNode()) moveIN(); //входим внутрь узла
              else if(pActiveLine->isFunc()) execFunc();  //выполняем функцию
              else {
                ((MenuLine::MenuLeaf *)pActiveLine)->editVal(); //копирование значения для редактирования
                edit(); //вызываем на редактирование лист
              }
            }
	    else nextPart(); //переходим к следующему элементу значения при редактировании
          }
          else if(_mce==mceHold){ //длительное нажатие
            if(!editMode) moveOUT(); //выход на вышестоящий уровень меню при просмотре
            else {
              ((MenuLine::MenuLeaf *)pActiveLine)->saveVal(); //сохранение значения после редактирования
              show(); //переход в режим просмотра меню
            }
          }
        break;
      }
      break;
  }
}

//Установка значений списка через массив
void MenuController::Menu::MenuLine::MenuLeaf::MenuLeaf_list::setValue(String _vals[],byte _size){
  size = _size;
  if (values) { delete[] values; }
  values = new String [size];
  for (int i = 0; i<size; i++){
	  values[i]=_vals[i];
  }
}

//Следующее значение одного из компонентов времени при редактировании
void MenuController::Menu::MenuLine::MenuLeaf::MenuLeaf_time::nextVal(DTime &_tm){
  byte h = _tm.hour;
  byte m = _tm.minute;
  byte s = _tm.second;
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
  _tm.setTime(h,m,s);
}

//Предыдущее значение одного из компонентов времени при редактировании
void MenuController::Menu::MenuLine::MenuLeaf::MenuLeaf_time::prevVal(DTime &_tm){
  byte h = _tm.hour;
  byte m = _tm.minute;
  byte s = _tm.second;
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
  _tm.setTime(h,m,s);
}

String MenuController::Menu::MenuLine::MenuLeaf::MenuLeaf_time::getValue(DTime &_tm){
  String stm = format;
  stm.replace("HH",decimate(_tm.hour));
  stm.replace("MM",decimate(_tm.minute));
  stm.replace("SS",decimate(_tm.second));
  return stm;
}

//Следующее значение одного из компонентов даты при редактировании
void MenuController::Menu::MenuLine::MenuLeaf::MenuLeaf_date::nextVal(DTime &_dt){
  uint16_t y = _dt.year;
  uint8_t m = _dt.month;
  uint8_t d = _dt.day;
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
  _dt.setDate(y,m,d);
}

//Предыдущее значение одного из компонентов даты при редактировании
void MenuController::Menu::MenuLine::MenuLeaf::MenuLeaf_date::prevVal(DTime &_dt){
  uint16_t y = _dt.year;
  uint8_t m = _dt.month;
  uint8_t d = _dt.day;
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
  _dt.setDate(y,m,d);
}

String MenuController::Menu::MenuLine::MenuLeaf::MenuLeaf_date::getValue(DTime &_dt){
  String sdt = format;
  sdt.replace("YYYY",(String)_dt.year);
  sdt.replace("YY",((String)_dt.year).substring(2));
  sdt.replace("MM",decimate(_dt.month));
  sdt.replace("DD",decimate(_dt.day));
  return sdt;
}

//Добавить строку в меню заданного типа
MenuController::Menu::MenuLine* MenuController::Menu::newMenuLine(menuLineType mlt){
  MenuLine *ml;
  switch(mlt){
    case mtNode:
      ml = new MenuController::Menu::MenuLine::MenuNode;
      break;
    case mtInt:
      ml = new MenuController::Menu::MenuLine::MenuLeaf::MenuLeaf_int;
      break;
    case mtList:
      ml = new MenuController::Menu::MenuLine::MenuLeaf::MenuLeaf_list;
      break;
    case mtTime:
      ml = new MenuController::Menu::MenuLine::MenuLeaf::MenuLeaf_time;
      break;
    case mtDate:
      ml = new MenuController::Menu::MenuLine::MenuLeaf::MenuLeaf_date;
      break;
    case mtFunc:
      ml = new MenuController::Menu::MenuLine::MenuLeaf::MenuLeaf_func;
      break;
  }
  return ml;
}

////создать дисплей
const LiquidCrystal& MenuController::Menu::newDisplay(const byte rsPin, const byte enPin, const byte d4Pin, const byte d5Pin, const byte d6Pin, const byte d7Pin, const byte _cols=16,const byte _rows=2){

  lcd_last_col = _cols-1;
  lcd_last_row = _rows-1;

  if(lcd) delete lcd;
  lcd = new LiquidCrystal(rsPin,enPin,d4Pin,d5Pin,d6Pin,d7Pin);
  lcd->begin(_cols,_rows);
  return *lcd;//можно будет использовать для других целей
}

////отрисовать меню в режиме просмотра
void MenuController::Menu::show(){
  MenuController::Menu::MenuLine *pLine;

  editMode = false;
  lcd->noCursor();

  //подгон меню под размеры экрана
  if(!lcd_active_row && pActiveLine->pNextLine==nullptr && pActiveLine->pPreviousLine!=nullptr){
    lcd_active_row++;
  }
  else if(lcd_active_row && pActiveLine->pNextLine!=nullptr && pActiveLine->pPreviousLine==nullptr){
    lcd_active_row = 0;
  }
  
  byte row_up = lcd_active_row, row_down = lcd_active_row;

  //вывод активной строки меню
  lcd->clear();
  lcd->setCursor(0,lcd_active_row);
  lcd->print(LCD_ACTIVE_LINE);
  lcd->print(pActiveLine->name);
  if(pActiveLine->isNode()){
    lcd->print(LCD_ELLIPSIS);
  }

  //вывод строк перед активной строкой (выше)
  pLine = pActiveLine;
  while(row_up-- && (pLine=pLine->pPreviousLine,pLine!=nullptr)){
    lcd->setCursor(1,row_up);
    lcd->print(pLine->name);
    if(pLine->isNode()){
      lcd->print(LCD_ELLIPSIS);
    }
  }

  //вывод строк после активной строкой (ниже)
  pLine = pActiveLine;
  while(row_down++ < lcd_last_row && (pLine=pLine->pNextLine,pLine!=nullptr)){
    lcd->setCursor(1,row_down);
    lcd->print(pLine->name);
    if(pLine->isNode()){
      lcd->print(LCD_ELLIPSIS);
    }
  }
}

////отрисовать меню в режиме редактирования
void MenuController::Menu::edit() {
  editMode = true;

  lcd->clear();

  //название элемента
  lcd->setCursor(0,0);
  lcd->print(pActiveLine->name);

  lcd->setCursor(0,1);
  lcd->print(LCD_ACTIVE_LINE);

  //значение элемента с курсором на редактируемом элементе
  lcd->print(pActiveLine->getValue());
  lcd->setCursor(((MenuController::Menu::MenuLine::MenuLeaf *) pActiveLine)->getShift() + 1,1);
  lcd->cursor();
}

//активировать меню и отобразить в режиме просмотра
void MenuController::Menu::enable() {
  activeMenu = true;
  mlsInactionStart = mls;
  show();
}	

//деактивировать меню и очистить дисплей
void MenuController::Menu::disable() {
  activeMenu = false;
  lcd->clear();
}	

//Перемещаемся вверх по иерархии
void MenuController::Menu::moveUP(){
  if(pActiveLine->pPreviousLine!=nullptr){
    pActiveLine = pActiveLine->pPreviousLine;
    if(lcd_active_row){
      lcd_active_row--;
    }  
    show();
  }
}
//Перемещаемся вниз по иерархии
void MenuController::Menu::moveDOWN(){
  if(pActiveLine->pNextLine!=nullptr){
    pActiveLine = pActiveLine->pNextLine;
    if(lcd_active_row<lcd_last_row){
      lcd_active_row++;
    }  
    show();
  }
}
//Перемещаемся внутрь узла
void MenuController::Menu::moveIN(){
  MenuLine::MenuNode *pNode;
  if(pActiveLine->isNode()){
    pNode = pActiveLine;
    pActiveLine = pNode->pFirstChild;
    lcd_active_row = 0;
    show();
  }
}
//Перемещаемся наружу из узла
void MenuController::Menu::moveOUT(){
  if(pActiveLine->pParentLine!=nullptr){
    pActiveLine = pActiveLine->pParentLine;
    show();
  }
  else disable(); //деактивация меню при нахождении в основном меню
}

//выполнить функцию void
void MenuController::Menu::execFunc(){
  ((MenuLine::MenuLeaf::MenuLeaf_func*)pActiveLine)->func();
}

//Выбор предыдущего значения при редактировании
void MenuController::Menu::prevVal(){
  ((MenuLine::MenuLeaf *) pActiveLine)->prevVal();
  edit();
}

//Выбор следующего значения при редактировании
void MenuController::Menu::nextVal(){
  ((MenuLine::MenuLeaf *) pActiveLine)->nextVal(); 
  edit();
}

//Выбор следующего элемента значения при редактировании
void MenuController::Menu::nextPart(){
  ((MenuLine::MenuLeaf *) pActiveLine)->nextPart(); 
  edit();
}

void MenuController::Menu::run(unsigned long _mls=0) {
  if(!_mls) mls = millis();
  else mls = _mls;

  //здесь добавить вывод сообщения

  //изменение состояния меню активно\неактивно 
  if (isActive() && ((mls - mlsInactionStart) > MC_INACT_TIMEOUT)) disable();
}


