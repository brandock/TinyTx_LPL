/* TinyTx_DS18B20_LPL
 * TinyTx Temp Sensor with RFM69_LPL radio library
 * Brandon Baldock August 2023
 * 
 * This is the original TinyTx sketch by Nathan Chantrell, modified to work with the OpenEnergyMonitor fork of the
 * RFM69 libarary from Felix Rusu of LowPowerLab. //https://github.com/openenergymonitor/RFM69_LPL
 * 
 * I used the OpenEnergyMonitor default encryption key (89txbe4p8aik5kt3) and network (210). 
 * I changed the ACK_TIME definition to the RFM69 default "retryWaitTime" of 30 (originally 10).
 * I changed the RETRY_LIMIT definition to the RFM69 "retries" default of 2 (originally 5).
 * I removed the RETRY_PERIOD definition. How soon to retry is not user-defined in RFM69.
 * I removed the waitForAck() function because this is handled in RFM69 by sendWithRetry.
 * 
 * Use Spence Konde's ATTinyCore to compile the firmware.
 * I switched to the ATTinyCore pin notation (using "PIN_PA0" rather than "10", for example) to reduce confusion.
 * I have tried to leave everything else untouched.
 */

//----------------------------------------------------------------------------------------------------------------------
// TinyTX - An ATtiny84 and RFM12B Wireless Temperature Sensor Node
// By Nathan Chantrell. For hardware design see http://nathan.chantrell.net/tinytx
//
// Using the Dallas DS18B20 temperature sensor
//
// Licenced under the Creative Commons Attribution-ShareAlike 3.0 Unported (CC BY-SA 3.0) licence:
// http://creativecommons.org/licenses/by-sa/3.0/
//
// Requires Arduino IDE with arduino-tiny core: http://code.google.com/p/arduino-tiny/
// and small change to OneWire library, see: http://arduino.cc/forum/index.php/topic,91491.msg687523.html#msg687523
//----------------------------------------------------------------------------------------------------------------------

#include <OneWire.h> // http://www.pjrc.com/teensy/arduino_libraries/OneWire.zip
#include <DallasTemperature.h> // http://download.milesburton.com/Arduino/MaximTemperature/DallasTemperature_LATEST.zip
#include <JeeLib.h> // https://github.com/openenergymonitor/RFM69_LPL
#include <RFM69_LPL.h>      //https://github.com/openenergymonitor/RFM69_LPL
RFM69 radio;

ISR(WDT_vect) { Sleepy::watchdogEvent(); } // interrupt handler for JeeLabs Sleepy power saving

#define myNodeID 1        // RF12 node ID in the range 1-30
#define network 210       // RF12 Network group
#define freq RF12_433MHZ  // Frequency of RFM12B module

#define ENCRYPTKEY    "89txbe4p8aik5kt3" //OpenEnergyMonitor sample encrypt key is 89txbe4p8aik5kt3

#define USE_ACK           // Enable ACKs, comment out to disable
#define RETRY_LIMIT 2     // Maximum number of times to retry
#define ACK_TIME 30       // Number of milliseconds to wait for an ack

#define ONE_WIRE_BUS PIN_PA0    // DS18B20 Temperature sensor is connected on D10/ATtiny pin 13
#define ONE_WIRE_POWER PIN_PA1  // DS18B20 Power pin is connected on D9/ATtiny pin 12

OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance

DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature

//########################################################################################################################
//Data Structure to be sent
//########################################################################################################################

 typedef struct {
      int temp; // Temperature reading
      int supplyV;  // Supply voltage
 } Payload;

 Payload tinytx;

//--------------------------------------------------------------------------------------------------
// Send payload data via RF
//-------------------------------------------------------------------------------------------------
 static void rfwrite(){
  #ifdef USE_ACK
  //sendWithRetry(uint16_t toAddress, const void* buffer, uint8_t bufferSize, uint8_t retries=2, uint8_t retryWaitTime=RFM69_ACK_TIMEOUT);
   radio.sendWithRetry(0, (const void*)(&tinytx), sizeof(tinytx), RETRY_LIMIT, ACK_TIME);
   radio.sleep();              // Put RF module to sleep
   return;

  #else
   radio.send(0, (const void*)(&tinytx), sizeof(tinytx), false);
   radio.sleep();              // Put RF module to sleep
   return;
  #endif
 }

//--------------------------------------------------------------------------------------------------
// Read current supply voltage
//--------------------------------------------------------------------------------------------------
 long readVcc() {
   bitClear(PRR, PRADC); ADCSRA |= bit(ADEN); // Enable the ADC
   long result;
   // Read 1.1V reference against Vcc
   #if defined(__AVR_ATtiny84__) 
    ADMUX = _BV(MUX5) | _BV(MUX0); // For ATtiny84
   #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);  // For ATmega328
   #endif 
   delay(2); // Wait for Vref to settle
   ADCSRA |= _BV(ADSC); // Convert
   while (bit_is_set(ADCSRA,ADSC));
   result = ADCL;
   result |= ADCH<<8;
   result = 1126400L / result; // Back-calculate Vcc in mV
   ADCSRA &= ~ bit(ADEN); bitSet(PRR, PRADC); // Disable the ADC to save power
   return result;
} 
//########################################################################################################################

void setup() {
  
  radio.setPins(PIN_PB1,PIN_PA5,PIN_PA6,PIN_PA4); // (SSpin, MOSIpin, MISOpin, SCKpin)
  radio.initialize(freq,myNodeID,network);        // Initialize RFM12 with settings defined above 
  radio.encrypt(ENCRYPTKEY); 
  radio.sleep();                                  // Put the RFM12 to sleep

  pinMode(ONE_WIRE_POWER, OUTPUT); // set power pin for DS18B20 to output
  
  PRR = bit(PRTIM1); // only keep timer 0 going
  
  ADCSRA &= ~ bit(ADEN); bitSet(PRR, PRADC); // Disable the ADC to save power

}

void loop() {
  
  digitalWrite(ONE_WIRE_POWER, HIGH); // turn DS18B20 sensor on
  
  //Sleepy::loseSomeTime(5); // Allow 5ms for the sensor to be ready
  delay(5); // The above doesn't seem to work for everyone (why?)
 
  sensors.begin(); //start up temp sensor
  sensors.requestTemperatures(); // Get the temperature
  tinytx.temp=(sensors.getTempCByIndex(0)*100); // Read first sensor and convert to integer, reversed at receiving end
  
  digitalWrite(ONE_WIRE_POWER, LOW); // turn DS18B20 off
  
  tinytx.supplyV = readVcc(); // Get supply voltage

  rfwrite(); // Send data via RF 

  Sleepy::loseSomeTime(60000); //JeeLabs power save function: enter low power mode for 60 seconds (valid range 16-65000 ms)

}
