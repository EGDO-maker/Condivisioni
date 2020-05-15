#ifndef menu_lcd_h
#define menu_lcd_h
#include <stdlib.h>

/* 
 * This class create a menu object to manage the string output for a LCD screen of 2 rows.
 * It should make it easier to define menu lists to manage the cursor position in order to select a specific task 
 * trough a switch-case or if statement in the main code. The use of the menu can be done using 4 buttons 
 * (up, down, esc and enter).
 * Each menu object in the sketch should have an unique menu number to set in the instaces of the class.
 * To manage menu variables you should define an array of floats [N][4] where a value, an lower and upper limit 
 * is set, you can also specify a quantity to increase or decrease the values trough menu.
 * In this version each menu can contain at max 10 arguments.
 * It is possible to assign e modify values trough a sub menu or confirm an action.
 * 
 * Egidio Fabris - 26/03/2020 
*/
class MenuLCD {
  private :
    byte menu_id, prev_id;
    int next_id[10] = {};
    int val_ref[10] = {};
    String list[10] = {};
    String confirm_txt = "Confirm ?";
    String accept_txt = "Yes";
    String refuse_txt = "No";
    String value_txt = "Set value:";
    bool same_as_list = false;
    byte rows = 0, cur_pos = 0, line0 = 0, line1 = 1;
    float value, inf, sup, stp;
    bool confirm  = false;
    bool ser_active = false;
    byte task = 0;
    
  public:
    // Constructor: define the menu number (or index) and the reference numeber of the previous menu.
    MenuLCD(byte, byte);
    // Next 3 methods have to be defined carefully in the main code!!!
    // set_list: import the strings to display in the menu
    void set_list(String, String, String, String, String, String, String, String, String, String);
    // set_next: import the index to the next menu or action. If the value is 255 it leads to a dead end.
    void set_next(byte, byte, byte, byte, byte, byte, byte, byte, byte, byte);
    // set_val_ref: set the index of array variable in the main code
    void set_val_ref(byte, byte, byte, byte, byte, byte, byte, byte, byte, byte);
    // set_conf_str: set the string displayed during the confirm and change value mode 
    void set_conf_str(String, String, String, String);
    //
    void up();
    //
    void down();
    //
    void enter(byte*, float*, float, float, float);
    //
    void esc(byte*);
    

    void move_cursor(int); // change the selected value
    String get_line(byte); // return the output string
    float ret_value(); // return the selected value in confirm mode
    
    void serial_on(); // activate serial printing (for debugging)
    void serial_off(); // disable serial printing

    byte current(); // return the current menu index
    byte previous(); // return the index of previous menu
    byte next(); // return the index of the next menu or action
    byte value_ref(); // return the index of the associate variable

    byte mode(); // return the current task or mode status (browsing = 0, confirm = 1, change values = 2)
    void browse_mode(); // set task to browsing mode (task = 0)
    void confirm_mode(); // set task to confirm mode (task = 1)
    bool confirmed(); // return true when a modify have been accpeted
    void change_value_mode(float, float, float, float); // set task to confirm mode (task = 2) need actual value, 
    // upper and lower limit and the amplitude of the step

    
};

MenuLCD::MenuLCD(byte id=0, byte prev=0) {
  menu_id = id;
  prev_id = prev;
  rows = 0;
  ser_active = false;
}
/*
 * Import the strings to display for this menu.
 */
void MenuLCD::set_list(String I0="", String I1="", String I2="", String I3="", String I4="", String I5="", String I6="", String I7="", String I8="", String I9="") {
  String input_list[10] = {I0, I1, I2, I3, I4, I5, I6, I7, I8, I9};
  for (byte i = 0; i < 10; i++) {
    if (input_list[i] != "") {
      list[i] = input_list[i];
      rows++;
    }
  }
}
/*
 * Import values of indexes to the next menu
 */
void MenuLCD::set_next(byte I0=255, byte I1=255, byte I2=255, byte I3=255, byte I4=255, byte I5=255, byte I6=255, byte I7=255, byte I8=255, byte I9=255) {
  byte index_list[] = {I0, I1, I2, I3, I4, I5, I6, I7, I8, I9};
  for (byte i = 0; i < 10; i++) {
    next_id[i] = index_list[i];
  }
}
/*
 * Import the reference indexes to the variables in the main code
 */
void MenuLCD::set_val_ref(byte I0=255, byte I1=255, byte I2=255, byte I3=255, byte I4=255, byte I5=255, byte I6=255, byte I7=255, byte I8=255, byte I9=255) {
  byte ref_list[] = {I0, I1, I2, I3, I4, I5, I6, I7, I8, I9};
  for (byte i = 0; i < 10; i++) {
    val_ref[i] = ref_list[i];
  }
}
/*
 * Set the output strings to user's defined values
 */
void MenuLCD::set_conf_str(String conf="Confirm ?", String acc="Yes", String ref="No", String set_val="Set value:") {
    confirm_txt = conf;
    accept_txt = acc;
    refuse_txt = ref;
    if (set_val == "SAME") {
      same_as_list = true;
    } else {
      same_as_list = false;
      value_txt = set_val;
    }
}

void MenuLCD::up() {
  move_cursor(1);
}

void MenuLCD::down() {
  move_cursor(-1);
}

void MenuLCD::esc(byte* menu_pos) {
  switch (task) {
    case 0:
      *menu_pos = prev_id;
    break;
    case 1:
      browse_mode();
    break;
    case 2:
      browse_mode();     
    break;
  }    
}

void MenuLCD::enter(byte* menu_pos, float* val, float inf, float sup, float stp) {
  byte next = next_id[cur_pos];
  switch (task) {
    case 0:
      if ((next != 255) && (next != 254)) {
        *menu_pos = next;
      } else if (next == 254) {
        change_value_mode(0.0, 0.0, 1.0, 1.0);
        confirm_mode();
      } else if ((next != 254) && (val_ref != 255)) { 
        change_value_mode(*val, inf, sup, stp);
      }
    break;
    case 1:
      if (confirmed()) {
        *val = value;
      }
      browse_mode();
    break;
    case 2:
      confirm_mode();
    break;
  }
}



/*
 * Move up (verse = 1) or down (verse = -1) the cursor position and set the proper line0 and line1 values.
 */
void MenuLCD::move_cursor(int verse) {
  switch (task) {
  case 0:
    if ( abs(verse) == 1) {
    cur_pos = int(rows + cur_pos - verse)%int(rows);
    if ((cur_pos != line0) && (cur_pos != line1)) {
      line0 = (rows + line0 - verse) % (rows);
      line1 = (rows + line0 + 1) % (rows);
     }
    }
    break;
  case 1:
    confirm = !confirm;
    if (next_id[cur_pos] == 254) {
      switch (confirm) {
      case true:
        value = 1.0;
        break;
      case false:
        value = 0.0;
        break;
      }
    }
    break;
  case 2:
    if ((verse == -1)  && (value - stp  >=  inf)) {
        value = value + verse*stp;
    } else if ((verse == 1)  && (value + stp  <=  sup)){
      value = value + verse*stp;
    }
    break;
  }
}
/* 
 * Return the string of the first (line = 0) or second line (line = 1) to print on the LCD screen
 * taking into the corusor positon or the input or confirm mode.
 */
String MenuLCD::get_line(byte line) {
  String ret;
  char str[16] = "";
  switch (line) {
    case 0:
      if (task == 0) {
        if (line0 == cur_pos) {
          ret = "> " + list[line0];
        } else {
          ret = "  " + list[line0];
        }
      } else if (task == 1) {
          ret = confirm_txt;
      } else if (task == 2) { 
          if (!same_as_list) {
            ret = value_txt;
          } else {
            ret = list[cur_pos];
          }
        }
      if (ser_active) {
        Serial.println("__________________");
        Serial.print("|");
        Serial.println(ret);
      }
      break;
    case 1:
      if (task == 0) {
        if (line1 == cur_pos) {
          ret = "> " + list[line1];
        } else {
          ret = "  " + list[line1];
        }
      } else if (task == 1) {
          switch (confirm) {
          case false:
            ret = refuse_txt;
            break;
          case true:
            ret = accept_txt;
            break;
          }
      } else if (task == 2) {
          ret = dtostrf(value, 2, 2, str);
      }
      if (ser_active) {
        Serial.print("|");
        Serial.println(ret);
        Serial.println("__________________");
      }
      break;
  }
  return ret;
}

float MenuLCD::ret_value() {
  return value;
}

void MenuLCD::serial_on() {
  ser_active = true;
}

void MenuLCD::serial_off() {
  ser_active = false;
}

byte MenuLCD::current() {
  return menu_id;
}

byte MenuLCD::previous() {
  return prev_id;
}

byte MenuLCD::next() {
  return next_id[cur_pos];
}

byte MenuLCD::value_ref() {
  return val_ref[cur_pos];
}

byte MenuLCD::mode() {
  return task;
}

void MenuLCD::browse_mode() {
  task = 0;
}

void MenuLCD::confirm_mode() {
  task = 1;
}

bool MenuLCD::confirmed() {
  bool ret = confirm;
  confirm = false;
  return ret;
}

// Initialize the change value mode
void MenuLCD::change_value_mode(float actual_val, float lower, float upper, float step_val=0) {
  if (val_ref[cur_pos] != 255) {
    task = 2;
    value = actual_val;
    if (lower < upper) {
      inf = lower;
      sup = upper;
    } else {
      inf = upper;
      sup = lower;
    }
    if (step_val == 0) {
      stp = (sup-inf)/20;
    } else {
      stp = step_val;
    }
  } else {
    task = 0;
  }
}

#endif
