// #define USE_AVR_DEBUGGER

#if defined USE_AVR_DEBUGGER
#include "avr_debugger.h"
#include "avr8-stub.h"
#include "app_api.h"
#endif

#include "utils.hpp"

#if USE_XBEE == 2

#include <XBee.h>

#include "traffic_light.h"

/*
This example is for Series 2 XBee
Receives a ZB RX packet and sets a PWM value based on packet data.
Error led is flashed if an unexpected packet is received
*/

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

int statusLed = YELLOWLIGHT_PIN;
int errorLed = REDLIGHT_PIN;
int dataLed = GREENLIGHT_PIN;


void setup() {
  pinMode(statusLed, OUTPUT);
  pinMode(errorLed, OUTPUT);
  pinMode(dataLed,  OUTPUT);

  // start serial
  Serial.begin(9600);
  xbee.begin(Serial);

  #if defined USE_AVR_DEBUGGER
    debug_init();
    delay(3000);
  #endif

  flashLed(statusLed, 3, 50);
}
// continuously reads packets, looking for ZB Receive or Modem Status
void loop() {

    // flashLed(statusLed, 2, 200);
    xbee.readPacket(1000);

    flashLed(dataLed, 2, 200);
    if (xbee.getResponse().isAvailable()) {
      // got something

      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        // got a zb rx packet

        // now fill our zb rx class
        xbee.getResponse().getZBRxResponse(rx);

        if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
            // the sender got an ACK
            flashLed(statusLed, 10, 10);
        } else {
            // we got it (obviously) but sender didn't get an ACK
            flashLed(errorLed, 2, 20);
        }
        // set dataLed PWM to value of the first byte in the data
        analogWrite(dataLed, rx.getData(0));
      } else if (xbee.getResponse().getApiId() == MODEM_STATUS_RESPONSE) {
        xbee.getResponse().getModemStatusResponse(msr);
        // the local XBee sends this response on certain events, like association/dissociation

        if (msr.getStatus() == ASSOCIATED) {
          // yay this is great.  flash led
          flashLed(statusLed, 10, 10);
        } else if (msr.getStatus() == DISASSOCIATED) {
          // this is awful.. flash led to show our discontent
          flashLed(errorLed, 10, 10);
        } else {
          // another status
          flashLed(statusLed, 5, 10);
        }
      } else {
          // not something we were expecting
        flashLed(errorLed, 1, 25);
      }
    } else if (xbee.getResponse().isError()) {
      //nss.print("Error reading packet.  Error code: ");
      //nss.println(xbee.getResponse().getErrorCode());
    }
    // flashLed(statusLed, 2, 200);
}

#endif