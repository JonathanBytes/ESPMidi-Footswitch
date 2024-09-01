#include <Arduino_Helpers.h>
#include <AH/Hardware/FilteredAnalog.hpp>
#include <Control_Surface.h>

// Instanciar la interfaz MIDI sobre USB
HairlessMIDI_Interface midi;

// Create a filtered analog object:
FilteredAnalog<7,       // Output precision in bits
               4,        // The amount of filtering
               uint32_t, // The integer type for the filter calculations
               analog_t  // The integer type for the upscaled analog values
              >
  analog1 = 33;

FilteredAnalog<7,       // Output precision in bits
               4,        // The amount of filtering
               uint32_t, // The integer type for the filter calculations
               analog_t  // The integer type for the upscaled analog values
              >
  analog2 = 35;

FilteredAnalog<7,       // Output precision in bits
               4,        // The amount of filtering
               uint32_t, // The integer type for the filter calculations
               analog_t  // The integer type for the upscaled analog values
              >
  analog3 = 34;

FilteredAnalog<7,       // Output precision in bits
               4,        // The amount of filtering
               uint32_t, // The integer type for the filter calculations
               analog_t  // The integer type for the upscaled analog values
              >
  analog4 = 39;

void handlePot(AH::FilteredAnalog<7, 4, uint32_t, analog_t> &analog, int CCNumber) {
  if (analog.update())
    midi.sendCC({CCNumber, Channel_1}, analog.getValue());
}

void setup() {
  Serial.begin(115200);

  // Select the correct ADC resolution
  analog1.setupADC();
  analog2.setupADC();
  analog3.setupADC();
  analog4.setupADC();
  // Initialize the filter to whatever the value on the input is right now
  // (otherwise, the filter is initialized to zero and you get transients)
  analog1.resetToCurrentValue();
  analog2.resetToCurrentValue();
  analog3.resetToCurrentValue();
  analog4.resetToCurrentValue();

  midi.begin(); // Inicializar la interfaz MIDI

}

void loop() {
  
  handlePot(analog1, 91);
  // handlePot(analog2, 92);
  //handlePot(analog3, 93);
  handlePot(analog4, 94);

  midi.update(); // Manejar o descartar la entrada MIDI
}
