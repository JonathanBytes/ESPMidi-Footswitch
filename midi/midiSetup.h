
#include <Control_Surface.h>

// Instanciar la interfaz MIDI sobre USB
HairlessMIDI_Interface midi;

int analog1Pin = 33;
int analog2Pin = 35; // 32 used by wifi
int analog3Pin = 34;
int analog4Pin = 39; // Labeled as VN on board

// Create a filtered analog object:
FilteredAnalog<7,        // Output precision in bits
               4,        // The amount of filtering
               uint32_t, // The integer type for the filter calculations
               analog_t  // The integer type for the upscaled analog values
               >
    analog1 = analog1Pin;

FilteredAnalog<7,        // Output precision in bits
               4,        // The amount of filtering
               uint32_t, // The integer type for the filter calculations
               analog_t  // The integer type for the upscaled analog values
               >
    analog2 = analog2Pin;

FilteredAnalog<7,        // Output precision in bits
               4,        // The amount of filtering
               uint32_t, // The integer type for the filter calculations
               analog_t  // The integer type for the upscaled analog values
               >
    analog3 = analog3Pin;

FilteredAnalog<7,        // Output precision in bits
               4,        // The amount of filtering
               uint32_t, // The integer type for the filter calculations
               analog_t  // The integer type for the upscaled analog values
               >
    analog4 = analog4Pin;

// Crear objetos de botón
Button pushbutton{23};
MultiPurposeButton btn1{16}, btn2{4};

// Pines para LEDs
const pin_t ledPin = 2;
const pin_t bankLeds[] = {17, 5, 18, 19};

struct MIDI_Bank {
  MIDIAddress btn1ShortPressAddress;
  uint8_t btn1ShortPressValues[2];
  bool btn1ShortPressStatus;
  MIDIAddress btn1LongPressAddress;
  uint8_t btn1LongPressValues[2];
  bool btn1LongPressStatus;

  MIDIAddress btn2ShortPressAddress;
  uint8_t btn2ShortPressValues[2];
  bool btn2ShortPressStatus;
  MIDIAddress btn2LongPressAddress;
  uint8_t btn2LongPressValues[2];
  bool btn2LongPressStatus;
};

// Configuración de bancos
MIDI_Bank banks[] = {
    {{69, Channel_1},
     {9, 9},
     false,
     {77, Channel_1},
     {0, 127},
     false,
     {69, Channel_1},
     {8, 8},
     false,
     {81, Channel_1},
     {0, 0},
     false},
    {{70, Channel_1},
     {0, 127},
     false,
     {78, Channel_1},
     {0, 127},
     false,
     {71, Channel_1},
     {0, 127},
     false,
     {82, Channel_1},
     {0, 127},
     false},
    {{72, Channel_1},
     {0, 127},
     false,
     {79, Channel_1},
     {0, 127},
     false,
     {73, Channel_1},
     {0, 127},
     false,
     {83, Channel_1},
     {0, 127},
     false},
    {{74, Channel_1},
     {0, 127},
     false,
     {80, Channel_1},
     {0, 127},
     false,
     {75, Channel_1},
     {0, 127},
     false,
     {84, Channel_1},
     {0, 127},
     false},
    // Se pueden agregar más bancos aquí
};

bool internalLedState = LOW;
uint8_t currentBank = 0; // Empezar con el primer banco
