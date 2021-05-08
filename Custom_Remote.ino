#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <IRremote.h>

// Control variables
int selected = 0;

int main_selected = 0;
int main_scroll = 0;

int candles_selected = 0;
int candles_scroll = 0;

int candles_colors_selected = 0;
int candles_colors_scroll = 0;

bool candles_colors_menu = false;

int bedtv_selected = 0;
int bedtv_scroll = 0;

int bedroku_selected = 0;
int bedroku_scroll = 0;

int rgblamp_selected = 0;
int rgblamp_scroll = 0;

int bedfan_selected = 0;
int bedfan_scroll = 0;

bool bedroku_control_mode = false;

// Device Information
const int num_devices = 5;
const char devices[5][12] PROGMEM = {"Candles", "Bed Roku", "Bed Fan", "Bedroom TV", "RGB Lamp"};

const int num_candles_options = 6;
const char candles_options[6][12] PROGMEM = {"On", "Off", "4H", "8H", "Multi", "-Colors"};
const long int candles_ir_codes[5] PROGMEM = {0x1FE48B7, 0x1FE58A7, 0x1FE807F, 0x1FE40BF, 0x1FEC03F};

const int num_candles_colors = 12;
const char candles_colors[12][12] PROGMEM = {"Red", "Orange", "Yellow", "Green1", "Green2", "Blue1", "Blue2", "Blue3", "Purple", "Pink1", "Pink2", "Pink3"};
const long int candles_colors_ir_codes[12] PROGMEM = {0x1FE20DF, 0x1FEE01F, 0x1FE50AF, 0x1FEA05F, 0x1FE10EF, 0x1FED827, 0x1FEB04F, 0x1FE609F, 0x1FE906F, 0x1FE30CF, 0x1FEF807, 0x1FE708F};

const int num_bedtv_options = 5;
const char bedtv_options[5][12] PROGMEM = {"Power", "Volume+", "Volume-", "Input", "Enter"};
const long int bedtv_ir_codes[5] PROGMEM = {0x61A0F00F, 0x61A030CF, 0x61A0B04F, 0x61A0B847, 0x61A018E7};

const int num_bedroku_options = 3;
const char bedroku_options[3][12] PROGMEM = {"-Control", "Home", "Back"};

const int num_rgblamp_options = 3;
const char rgblamp_options[3][12] PROGMEM = {"On", "Off", "White"};
const long int rgblamp_ir_codes[3] PROGMEM = {0xFFE01F, 0xFF609F, 0xFFD02F};

const int num_bedfan_options = 5;
const char bedfan_options[5][12] PROGMEM = {"Power", "Speed", "Timer", "Rotation", "Wind Type"};
const long int bedfan_ir_codes[5] PROGMEM = {0x1FE58A7, 0x1FE807F, 0x1FEE01F, 0x1FEC03F, 0x1FE906F}; 

// Joystick values
int xval = 500;
int yval = 500;
int button_val = 1;

const int DOWN = 1;
const int UP = 2;
const int RIGHT = 3;
const int LEFT = 4;
const int PRESS = 5;
const int LEFTPRESS = 6;

Adafruit_SSD1306 display(128, 64, &Wire);
IRsend irsend;
void setup() {
  // Init oled display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Set joystick button state
  pinMode(5, INPUT);
  digitalWrite(5, HIGH);

  Serial.begin(9600);
}

void loop() {

  // Change menu handler based on the selected device
  switch (selected) {
    case 0:
      // Main menu
      main_handler();
      break;
    case 1:
      candles_handler();
      break;
    case 2:
      bedroku_handler();
      break;
    case 3:
      bedfan_handler();
      break;
    case 4:
      bedtv_handler();
      break;
    case 5:
      rgblamp_handler();
      break;
  }

  delay(50);
}

int get_joystick_action() {
  // Read joystick values
  xval = analogRead(A6);
  yval = analogRead(A7);
  button_val = digitalRead(5);

  if (xval < 100) {
    return DOWN;
  } else if (xval > 650) {
    return UP;
  } else if (yval > 900) {
    return RIGHT;
  } else if (yval < 250) {
    if (button_val == 0) {
      return LEFTPRESS;
    } else {
      return LEFT;
    }
  } else if (button_val == 0) {
    return PRESS;
  } else {
    return 0;
  }
}

void draw_basic_menu(const char* title, const char options[15][12], const int num_options, int selected_option, int scroll) {
  // Header
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(title);

  // Device list
  for (int i = scroll; i < scroll + 3; ++i) {

    // Break if i is out of bounds
    if (i >= num_options) break;

    display.setCursor(0, display.getCursorY() + 10);

    char option[12];
    strcpy_P(option, (char*)(options+i));
    
    if (i == selected_option) {
      display.print(option);
      display.println("<");
    } else {
      display.println(option);
    }

  }
  display.display();
}

void main_handler() {
  // Input handling
  int joystick_action = get_joystick_action();
  bool is_input = false;

  if (joystick_action != 0) {
    is_input = true;
  }

  if (joystick_action == DOWN && main_selected + 1 < num_devices) {
    if (main_selected > 0) {
      ++main_scroll;
    }
    ++main_selected;
  } else if (joystick_action == UP && main_selected > 0) {
    if (main_selected > 1) {
      --main_scroll;
    }
    --main_selected;
  } else if (joystick_action == PRESS) {
    selected = main_selected + 1;
  }

  draw_basic_menu("Devices:", devices, num_devices, main_selected, main_scroll);

  // Delay if there was input
  if (is_input) {
    delay(300);
  }

}

void candles_handler() {
  // Input handling
  int joystick_action = get_joystick_action();
  bool is_input = false;

  if (joystick_action != 0) {
    is_input = true;
  }

  if (!candles_colors_menu) {
    if (joystick_action == DOWN && candles_selected + 1 < num_candles_options) {
      if (candles_selected > 0) {
        ++candles_scroll;
      }
      ++candles_selected;
  
    } else if (joystick_action == UP && candles_selected > 0) {
      if (candles_selected > 1) {
        --candles_scroll;
      }
      --candles_selected;
  
    } else if (joystick_action == LEFT) {
      selected = 0;
  
    } else if (joystick_action == PRESS) {
      if (candles_selected != 5) {
        irsend.sendNECMSB(pgm_read_dword(candles_ir_codes + candles_selected), 32); 
      } else {
        candles_colors_menu = true;
      }
    }
  } else {
    if (joystick_action == DOWN && candles_colors_selected + 1 < num_candles_colors) {
      if (candles_colors_selected > 0) {
        ++candles_colors_scroll;
      }
      ++candles_colors_selected;
  
    } else if (joystick_action == UP && candles_colors_selected > 0) {
      if (candles_colors_selected > 1) {
        --candles_colors_scroll;
      }
      --candles_colors_selected;
  
    } else if (joystick_action == LEFT) {
      candles_colors_menu = false;
    } else if (joystick_action == PRESS) {
      irsend.sendNECMSB(pgm_read_dword(candles_colors_ir_codes + candles_colors_selected), 32);
    }
 }

  
  // Draw either the menu of color options or the menu of candles control options
  if (candles_colors_menu) {
    draw_basic_menu("Colors:", candles_colors, num_candles_colors, candles_colors_selected, candles_colors_scroll);
  } else {
    draw_basic_menu("Candles:", candles_options, num_candles_options, candles_selected, candles_scroll);
  }

  if (is_input) {
    delay(300);
  }

}

void bedtv_handler() {
  int joystick_action = get_joystick_action();
  bool is_input = false;

  if (joystick_action != 0) {
    is_input = true;
  }

  if (joystick_action == DOWN && bedtv_selected + 1 < num_bedtv_options) {
    if (bedtv_selected > 0) {
      ++bedtv_scroll;
    }
    ++bedtv_selected;
  } else if (joystick_action == UP && bedtv_selected > 0) {
    if (bedtv_selected > 1) {
      --bedtv_scroll;
    }
    --bedtv_selected;
  } else if (joystick_action == PRESS) {
    irsend.sendNECMSB(pgm_read_dword(bedtv_ir_codes + bedtv_selected), 32);
  } else if (joystick_action == LEFT) {
    selected = 0;
  }

  draw_basic_menu("Bedroom TV:", bedtv_options, num_bedtv_options, bedtv_selected, bedtv_scroll);

  // Delay if there was input
  if (is_input) {
    delay(300);
  }
}

void bedroku_handler() {

  int joystick_action = get_joystick_action();
  bool is_input = false;
  if (joystick_action != 0) {
      is_input = true;
    }
  

  if (bedroku_control_mode) {
    if (joystick_action == DOWN) {
      irsend.sendNECMSB(0x5743CC33, 32);
    } else if (joystick_action == UP) {
      irsend.sendNECMSB(0x57439867, 32);
    } else if (joystick_action == LEFT) {
      irsend.sendNECMSB(0x57437887, 32);
    } else if (joystick_action == RIGHT) {
      irsend.sendNECMSB(0x5743B44B, 32);
    } else if (joystick_action == PRESS) {
      irsend.sendNECMSB(0x574354AB, 32);
    } else if (joystick_action == LEFTPRESS) {
      bedroku_control_mode = false;
    }

    // Delay if there was input
    if (is_input) {
      delay(100);
    }
  } else {
    if (joystick_action == DOWN && bedroku_selected + 1 < num_bedroku_options) {
      if (bedroku_selected > 0) {
        ++bedroku_scroll;
      }
      ++bedroku_selected;
    } else if (joystick_action == UP && bedroku_selected > 0) {
      if (bedroku_selected > 1) {
        --bedroku_scroll;
      }
      --bedroku_selected;
    } else if (joystick_action == PRESS) {
      if (bedroku_selected == 1) {
        irsend.sendNECMSB(0x5743C03F, 32);
      } else if (bedroku_selected == 2) {
        irsend.sendNECMSB(0x57436699, 32);
      } else if (bedroku_selected == 0) {
        bedroku_control_mode = true;
      }
    } else if (joystick_action == LEFT) {
      selected = 0;
    }

    draw_basic_menu("Bedroom Roku:", bedroku_options, num_bedroku_options, bedroku_selected, bedroku_scroll);

    // Delay if there was input
    if (is_input) {
      delay(300);
    }

  }
}

void rgblamp_handler() {
  int joystick_action = get_joystick_action();
  bool is_input = false;

  if (joystick_action != 0) {
    is_input = true;
  }

  if (joystick_action == DOWN && rgblamp_selected + 1 < num_rgblamp_options) {
    if (rgblamp_selected > 0) {
      ++rgblamp_scroll;
    }
    ++rgblamp_selected;
  } else if (joystick_action == UP && rgblamp_selected > 0) {
    if (rgblamp_selected > 1) {
      --rgblamp_scroll;
    }
    --rgblamp_selected;
  } else if (joystick_action == PRESS) {
    irsend.sendNECMSB(pgm_read_dword(rgblamp_ir_codes + rgblamp_selected), 32);
  } else if (joystick_action == LEFT) {
    selected = 0;
  }

  draw_basic_menu("RGB Lamp:", rgblamp_options, num_rgblamp_options, rgblamp_selected, rgblamp_scroll);

  // Delay if there was input
  if (is_input) {
    delay(300);
  }
}

void bedfan_handler() {
  int joystick_action = get_joystick_action();
  bool is_input = false;

  if (joystick_action != 0) {
    is_input = true;
  }

  if (joystick_action == DOWN && bedfan_selected + 1 < num_bedfan_options) {
    if (bedfan_selected > 0) {
      ++bedfan_scroll;
    }
    ++bedfan_selected;
  } else if (joystick_action == UP && bedfan_selected > 0) {
    if (bedfan_selected > 1) {
      --bedfan_scroll;
    }
    --bedfan_selected;
  } else if (joystick_action == PRESS) {
    irsend.sendNECMSB(pgm_read_dword(bedfan_ir_codes + bedfan_selected), 32);
  } else if (joystick_action == LEFT) {
    selected = 0;
  }

  draw_basic_menu("Bedroom Fan:", bedfan_options, num_bedfan_options, bedfan_selected, bedfan_scroll);

  // Delay if there was input
  if (is_input) {
    delay(300);
  }
}
