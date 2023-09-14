#include "MenuLCD.h"

// This is an usage example code 

MenuLCD *menu[4];

char cmd = "";
byte menu_pos = 0;
/* 
 * array for the variables associated to the menu: 
 * indexes[][0]: are the values - required
 * indexes[][1]: should be the bottom limits of the range - required
 * indexes[][2]: should be the top limits of the range - required
 * indexes[][3]: should be the incremental step in changing values - optional
 * if not specified it's set to 1/20 of range
 */
float values[][4] = {
  {0.0, 0.0, 5.0, 1.0},  
  {0.0, 0.0, 60.0, 2.0},
  {0.0, -1.0, 1.0, 0.1},
  {0.0, -2.0, 2.0, 1.0}, 
  {0.0, 0.0, 1.0, 0.1},
  {0.0, 0.0, 100.0, 5.0},
  {0.0, 0.0, 1.0, 1.0}  // associated to the confirmation on first item in menu[3]
};
// String vals_str[][2] = {};
byte val_ref = 0 ;

void setup() {
  Serial.begin(9600);
/* 
 * Constructors: 
 * the first value is the menu references number (byte). - needed
 * the second value is rhe reference snumber to the previous menu (byte). - needed
 */
  menu[0] = new MenuLCD(0, 0);
  menu[1] = new MenuLCD(1, 0);
  menu[2] = new MenuLCD(2, 0);
  menu[3] = new MenuLCD(3, 2);
// List of strings to display in each menu 
  menu[0] -> set_list(F("primo"), F("secondo"), F("terzo"));
  menu[1] -> set_list(F("alfa"), F("beta"), F("gamma"));
  menu[2] -> set_list(F("Qui"), F("Quo"), F("Qua"));
  menu[3] -> set_list(F("Tizio"), F("Caio"));
/*  
 * Reference to the next menu for each element of a menu. Insert 255 to set a variable setting 
 * or to a dead end. Insert a value of 254 to confirm an action (a variable in a 0-1 range is requested).
 */
  menu[0] -> set_next(1, 2, 255);
  menu[1] -> set_next(255, 255, 255);
  menu[2] -> set_next(255, 3, 255);
  menu[3] -> set_next(254, 255);  
/*  
 * Reference to the variables in the values array (for each element of a menu). 
 * Insert 255 to lead to a new menu or dead end.
 */
  menu[0] -> set_val_ref(255, 255, 0);
  menu[1] -> set_val_ref(1, 2, 3);
  menu[2] -> set_val_ref(4, 255, 255);
  menu[3] -> set_val_ref(6, 5);
/*
 * set user defined strings displayed during the confirm and change value mode
 */
  menu[0] -> set_conf_str(F("Confermi ?"), F("Sì"), F("No"), F("SAME"));
  menu[1] -> set_conf_str(F("Confermi ?"), F("Sì"), F("No"), F("SAME"));
  menu[2] -> set_conf_str(F("Confermi ?"), F("Sì"), F("No"), F("SAME"));
  menu[3] -> set_conf_str(F("Confermi ?"), F("Sì"), F("No"), F("SAME"));

/*  
 * print the output strings on serial terminal (for debugging)   
 */  
  menu[0] -> serial_on();
  menu[1] -> serial_on();
  menu[2] -> serial_on();
  menu[3] -> serial_on();

  Serial.println("OK");
  Serial.println("Press 'u' + enter to go up the menu");
  Serial.println("Press 'j' + enter to go down the menu");
  Serial.println("Press the space bar + enter to enter a menu or confirm"); 
  Serial.println("Press the space 'q' +  enter to go esc"); 
}

void loop() {
  if (Serial.available()) {
    cmd = Serial.read();
    Serial.println(cmd);
  }

  switch (cmd) {
  case 'u':
    menu[menu_pos] -> up();
    menu[menu_pos] -> get_line(0);
    menu[menu_pos] -> get_line(1);
    cmd = "";
    break;
  case 'j':
    menu[menu_pos] -> down();
    menu[menu_pos] -> get_line(0);
    menu[menu_pos] -> get_line(1);
    cmd = "";
    break;
  case ' ':
    val_ref = menu[menu_pos] -> value_ref();
    menu[menu_pos] -> enter(&menu_pos, &values[val_ref][0], values[val_ref][1], values[val_ref][2], values[val_ref][3]);
    menu[menu_pos] -> get_line(0);
    menu[menu_pos] -> get_line(1);
    cmd = "";
    break;
  case 'q':
    menu[menu_pos] -> esc(&menu_pos);
    menu[menu_pos] -> get_line(0);
    menu[menu_pos] -> get_line(1);
    cmd = "";
    break;
  }
}
