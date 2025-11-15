#ifndef HT1621_h
#define HT1621_h

#include <Arduino.h>

class HT1621 {
public:
  HT1621(int data_pin, int wr_pin, int cs_pin) : 
    _data_pin(data_pin), _wr_pin(wr_pin), _cs_pin(cs_pin) {}

  void begin() {
    pinMode(_data_pin, OUTPUT);
    pinMode(_wr_pin, OUTPUT);
    pinMode(_cs_pin, OUTPUT);
    digitalWrite(_cs_pin, HIGH);
    
    sendCommand(0x01); // System Enable
    sendCommand(0x28); // BIAS 1/3, 4 commons
    sendCommand(0x18); // On-chip RC oscillator
    sendCommand(0x06); // WDT Disable
    sendCommand(0x03); // LCD ON
  }

  void lcdOff() {
    sendCommand(0x02); // LCD OFF
  }

  void lcdOn() {
    sendCommand(0x03); // LCD ON
  }

  void sysOff() {
    sendCommand(0x00); // System Disable
  }

  void sysOn() {
    sendCommand(0x01); // System Enable
  }

  void clear() {
    for (byte i = 0; i < 16; i++) {
      write(i * 2, 0x00);
    }
  }

  void displayNumber(int number) {
    clear();
    if (number < 0 || number > 99) return;

    byte tens = number / 10;
    byte ones = number % 10;

    if (number > 9) {
      writeDigit(0, tens);
    }
    writeDigit(1, ones);
  }

  void turnAllOn() {
    for (byte i = 0; i < 16; i++) {
      write(i * 2, 0xFF);
    }
  }

  void writeDigit(int position, int digit, bool colon_on = false) {
    // Custom segment map based on your display's logic table.
    // Bit mapping: (B, G, C, Colon, A, F, E, D)
    byte segments[] = {
      0xAF, // 0
      0xA0, // 1
      0xCB, // 2
      0xE9, // 3
      0xE4, // 4
      0x6D, // 5
      0x6F, // 6
      0xA8, // 7
      0xEF, // 8
      0xED  // 9
    };
    if (digit >= 0 && digit <= 9) {
      byte data = segments[digit];
      if (colon_on) {
        data |= 0b00010000; // Turn on the colon bit (P4)
      }
      // Address mapping: Digit 1->Addr 0, Digit 2->Addr 2, etc.
      write(position * 2, data);
    }
  }

private:
  int _data_pin, _wr_pin, _cs_pin;

  void sendCommand(byte command) {
    digitalWrite(_cs_pin, LOW);
    writeBits(0b100, 3);
    writeBits(command, 8);
    writeBits(0, 1); // Don't care bit for command
    digitalWrite(_cs_pin, HIGH);
  }

  void write(byte address, byte data) {
    digitalWrite(_cs_pin, LOW);
    writeBits(0b101, 3);
    writeBits(address, 6);
    writeBits(data, 8);
    digitalWrite(_cs_pin, HIGH);
  }

  void writeBits(unsigned int data, byte bits) {
    for (byte i = 0; i < bits; i++) {
      digitalWrite(_wr_pin, LOW);
      delayMicroseconds(2);
      if (data & (1 << (bits - 1 - i))) {
        digitalWrite(_data_pin, HIGH);
      } else {
        digitalWrite(_data_pin, LOW);
      }
      digitalWrite(_wr_pin, HIGH);
      delayMicroseconds(2);
    }
  }
};

#endif