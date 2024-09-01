using ValueArray = uint8_t[2];
void handleButton(MultiPurposeButton &button, const MIDIAddress &shortPressAddr,
                  ValueArray shortPressValues, const MIDIAddress &longPressAddr,
                  ValueArray longPressValues, bool &shortPressStatus,
                  bool &longPressStatus, bool bankCycle) {
  switch (button.update()) {
  case button.None:
    break;
  case button.PressStart:
    break;
  case button.ShortPressRelease:
    shortPressStatus = !shortPressStatus;
    midi.sendCC(shortPressAddr,
                shortPressStatus ? shortPressValues[1] : shortPressValues[0]);
    break;
  case button.LongPress:
    if (bankCycle) {
      currentBank =
          (currentBank + 1) %
          (sizeof(banks) / sizeof(banks[0])); // Cambiar al siguiente banco
      updateBankIndicators(); // Actualizar LEDs para reflejar el banco actual
      fastBlink(ledPin, internalLedState);
    } else {
      internalLedState = !internalLedState; // Cambiar el estado del LED
      longPressStatus = !longPressStatus;
      midi.sendCC(longPressAddr,
                  longPressStatus ? longPressValues[1] : longPressValues[0]);
      digitalWrite(ledPin, internalLedState ? HIGH : LOW);
    }
    break;
  }
}

// Función para manejar el botón pushbutton
void handlePushButton() {
  if (pushbutton.update() == Button::Falling) {
    midi.sendCC(banks[currentBank].btn2ShortPressAddress,
                banks[currentBank].btn2ShortPressValues[1]);
  }
}

void handlePot(
    AH::FilteredAnalog<7, filteringAmount, uint32_t, analog_t> &analog,
    int CCNumber) {
  if (analog.update())
    midi.sendCC({CCNumber, Channel_1}, analog.getValue());
}
