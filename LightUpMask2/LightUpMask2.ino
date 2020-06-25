/*********************************************************************
 This is an example for our nRF51822 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

#include <string.h>
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define PIN 6

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);



/*=========================================================================
    APPLICATION SETTINGS

    FACTORYRESET_ENABLE       Perform a factory reset when running this sketch
   
                              Enabling this will put your Bluefruit LE module
                              in a 'known good' state and clear any config
                              data set in previous sketches or projects, so
                              running this at least once is a good idea.
   
                              When deploying your project, however, you will
                              want to disable factory reset by setting this
                              value to 0.  If you are making changes to your
                              Bluefruit LE device via AT commands, and those
                              changes aren't persisting across resets, this
                              is the reason why.  Factory reset will erase
                              the non-volatile memory where config data is
                              stored, setting it back to factory default
                              values.
       
                              Some sketches that require you to bond to a
                              central device (HID mouse, keyboard, etc.)
                              won't work at all with this feature enabled
                              since the factory reset will clear all of the
                              bonding data stored on the chip, meaning the
                              central device won't be able to reconnect.
    MINIMUM_FIRMWARE_VERSION  Minimum firmware version to have some new features
    MODE_LED_BEHAVIOUR        LED activity, valid options are
                              "DISABLE" or "MODE" or "BLEUART" or
                              "HWUART"  or "SPI"  or "MANUAL"
    -----------------------------------------------------------------------*/
    #define FACTORYRESET_ENABLE         1
    #define MINIMUM_FIRMWARE_VERSION    "0.6.6"
    #define MODE_LED_BEHAVIOUR          "MODE"
/*=========================================================================*/

// Create the bluefruit object, either software serial...uncomment these lines
/*
SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);

Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN,
                      BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);
*/

/* ...or hardware serial, which does not need the RTS/CTS pins. Uncomment this line */
// Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/* ...software SPI, using SCK/MOSI/MISO user-defined SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO,
//                             BLUEFRUIT_SPI_MOSI, BLUEFRUIT_SPI_CS,
//                             BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

// function prototypes over in packetparser.cpp
uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout);
float parsefloat(uint8_t *buffer);
void printHex(const uint8_t * data, const uint32_t numBytes);

// the packet buffer
extern uint8_t packetbuffer[];


boolean audioMode = false;
int audioModeTimer = 0;

const uint16_t textcolors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };


  


/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void setup(void)
{
  //while (!Serial);  // required for Flora & Micro
  delay(500);

  /* Initialise the module */
  
  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
 

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
   
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }


  /* Disable command echo from Bluefruit */
  ble.echo(false);

  
  /* Print Bluefruit information */
  ble.info();



  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }

  

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set Bluefruit to DATA mode
 
  ble.setMode(BLUEFRUIT_MODE_DATA);


  matrix.begin();
  matrix.setBrightness(80);
  matrix.setTextWrap(false);
  matrix.setTextColor(textcolors[0]);

}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void loop(void)
{

  if(audioMode)
  {
   int lvl = analogRead(A1);
   int vol = abs( 230 - lvl);
   vol = constrain(vol, 0, 25);
   //int col = map(vol, 0, 25, 0, 7);
   matrix.fillScreen(0);
   if(vol>8)
   {
      openmouth1();
   }
   else
   {
      smile1();
   }
   //matrix.fillRect(0, (7-col), 8, col+1, matrix.Color(128, 0, 128));
   //matrix.show();
   delay(10);
   audioModeTimer++;
  }
  /* Wait for new data to arrive */
  if((audioMode && audioModeTimer > 150) || (!audioMode))
  {
    
  uint8_t len = readPacket(&ble, BLE_READPACKET_TIMEOUT);
  if (len == 0) return;

  /* Got a packet! */
  // printHex(packetbuffer, len);

  // Color
  if (packetbuffer[1] == 'C') {
    audioMode = false;
    uint8_t red = packetbuffer[2];
    uint8_t green = packetbuffer[3];
    uint8_t blue = packetbuffer[4];
    matrix.fillScreen(matrix.Color(red, green,blue));
    matrix.show();
  }

  // Buttons
  if (packetbuffer[1] == 'B') {
    uint8_t buttnum = packetbuffer[2] - '0';
    boolean pressed = packetbuffer[3] - '0';
    
    if (pressed) {
      if(buttnum == 1)
      {
        audioMode = true;
      }
      else if(buttnum == 2)
      {
        audioMode = false;
        for(int p = 0; p<8; p++)
        {
        corona1();
        delay(400);
        corona3();
        delay(400);
        }
        matrix.fillScreen(matrix.Color(0,0,0));
        matrix.show();
      }
      else if(buttnum == 3)
      {
        audioMode = false;
        for(int p = 0; p<4;p++)
        {
        creeper1();
        delay(400);
        enderman1();
        delay(400);
        chicken1();
        delay(400);
        }
        matrix.fillScreen(matrix.Color(0,0,0));
        matrix.show();
      }
      else if(buttnum == 4)
      {
        int x    = matrix.width();
        int pass = 0;
        matrix.setTextColor(textcolors[pass]);
        matrix.setTextSize(1);
        matrix.fillScreen(0);
        matrix.show();
        for(int v = 0; v<(150);v++)
        {
        matrix.fillScreen(0);
        matrix.setCursor(x, 0);
        matrix.print(F("Mask Up"));
        if(--x < -45) {
          x = matrix.width();
          pass++;
          matrix.setTextColor(textcolors[pass]);
        }
        matrix.show();
        delay(100);
        }
        matrix.fillScreen(0);
        matrix.show();
      }

      else if(buttnum == 5)
      {
        for(int v = 0; v<8;v++)
        {
        fish1();
        delay(400);
        fish2();
        delay(400);
        }
        matrix.fillScreen(0);
        matrix.show();
      }
      else if(buttnum == 6)
      {
        for(int v=0; v<3;v++)
        {
        fw1();
        delay(150);
        fw2();
        delay(150);
        fw3();
        delay(150);
        fw4();
        delay(150);
        fw5();
        delay(150);
        fw6();
        delay(150);
        fw7();
        delay(150);
        fw8();
        delay(150);
        fw9();
        delay(150);
        fw10();
        delay(150);
        fw11();
        delay(150);
        fw12();
        delay(150);
        fw13();
        delay(150);  
        matrix.fillScreen(0);
        matrix.show();   
        } 
      }
      
    } else {
      
    }
  }

  
  if(audioModeTimer > 50)
  {
      audioModeTimer = 0;
  }

 } // end if audioMode
  
}

void fish1(){
matrix.fillScreen(0);
matrix.drawPixel(1,0, matrix.Color(15,72,154));
matrix.drawPixel(3,1, matrix.Color(15,72,154));
matrix.drawPixel(1,2, matrix.Color(15,72,154));
matrix.drawPixel(3,3, matrix.Color(163,2,132));
matrix.drawPixel(2,4, matrix.Color(163,2,132));
matrix.drawPixel(3,4, matrix.Color(163,2,132));
matrix.drawPixel(4,4, matrix.Color(163,2,132));
matrix.drawPixel(1,5, matrix.Color(163,2,132));
matrix.drawPixel(3,5, matrix.Color(163,2,132));
matrix.drawPixel(4,5, matrix.Color(163,2,132));
matrix.drawPixel(6,5, matrix.Color(163,2,132));
matrix.drawPixel(2,6, matrix.Color(163,2,132));
matrix.drawPixel(3,6, matrix.Color(163,2,132));
matrix.drawPixel(4,6, matrix.Color(163,2,132));
matrix.drawPixel(5,6, matrix.Color(163,2,132));
matrix.drawPixel(3,7, matrix.Color(163,2,132));
matrix.drawPixel(6,7, matrix.Color(163,2,132));
matrix.show();
}

void fish2(){
matrix.fillScreen(0);
matrix.drawPixel(3,0, matrix.Color(15,72,154));
matrix.drawPixel(1,1, matrix.Color(15,72,154));
matrix.drawPixel(2,2, matrix.Color(15,72,154));
matrix.drawPixel(3,3, matrix.Color(163,2,132));
matrix.drawPixel(2,4, matrix.Color(163,2,132));
matrix.drawPixel(3,4, matrix.Color(163,2,132));
matrix.drawPixel(4,4, matrix.Color(163,2,132));
matrix.drawPixel(6,4, matrix.Color(163,2,132));
matrix.drawPixel(1,5, matrix.Color(163,2,132));
matrix.drawPixel(3,5, matrix.Color(163,2,132));
matrix.drawPixel(4,5, matrix.Color(163,2,132));
matrix.drawPixel(5,5, matrix.Color(163,2,132));
matrix.drawPixel(2,6, matrix.Color(163,2,132));
matrix.drawPixel(3,6, matrix.Color(163,2,132));
matrix.drawPixel(4,6, matrix.Color(163,2,132));
matrix.drawPixel(6,6, matrix.Color(163,2,132));
matrix.drawPixel(3,7, matrix.Color(163,2,132));
matrix.show();
}



void smile1(){
matrix.fillScreen(0);
matrix.drawPixel(1,0, matrix.Color(4,4,4));
matrix.drawPixel(1,3, matrix.Color(255,255,255));
matrix.drawPixel(2,3, matrix.Color(255,255,255));
matrix.drawPixel(5,3, matrix.Color(255,255,255));
matrix.drawPixel(6,3, matrix.Color(255,255,255));
matrix.drawPixel(1,4, matrix.Color(255,255,255));
matrix.drawPixel(2,4, matrix.Color(255,255,255));
matrix.drawPixel(5,4, matrix.Color(255,255,255));
matrix.drawPixel(6,4, matrix.Color(255,255,255));
matrix.drawPixel(1,5, matrix.Color(255,255,255));
matrix.drawPixel(2,5, matrix.Color(255,255,255));
matrix.drawPixel(3,5, matrix.Color(255,255,255));
matrix.drawPixel(4,5, matrix.Color(255,255,255));
matrix.drawPixel(5,5, matrix.Color(255,255,255));
matrix.drawPixel(6,5, matrix.Color(255,255,255));
matrix.drawPixel(1,6, matrix.Color(255,255,255));
matrix.drawPixel(2,6, matrix.Color(255,255,255));
matrix.drawPixel(3,6, matrix.Color(255,255,255));
matrix.drawPixel(4,6, matrix.Color(255,255,255));
matrix.drawPixel(5,6, matrix.Color(255,255,255));
matrix.drawPixel(6,6, matrix.Color(255,255,255));
matrix.show();
}

void openmouth1(){
matrix.fillScreen(0);
matrix.drawPixel(3,0, matrix.Color(255,255,255));
matrix.drawPixel(4,0, matrix.Color(255,255,255));
matrix.drawPixel(2,1, matrix.Color(255,255,255));
matrix.drawPixel(4,1, matrix.Color(4,4,4));
matrix.drawPixel(5,1, matrix.Color(255,255,255));
matrix.drawPixel(1,2, matrix.Color(255,255,255));
matrix.drawPixel(5,2, matrix.Color(4,4,4));
matrix.drawPixel(6,2, matrix.Color(255,255,255));
matrix.drawPixel(1,3, matrix.Color(255,255,255));
matrix.drawPixel(5,3, matrix.Color(4,4,4));
matrix.drawPixel(6,3, matrix.Color(255,255,255));
matrix.drawPixel(1,4, matrix.Color(255,255,255));
matrix.drawPixel(5,4, matrix.Color(4,4,4));
matrix.drawPixel(6,4, matrix.Color(255,255,255));
matrix.drawPixel(2,5, matrix.Color(255,255,255));
matrix.drawPixel(5,5, matrix.Color(255,255,255));
matrix.drawPixel(3,6, matrix.Color(255,255,255));
matrix.drawPixel(4,6, matrix.Color(255,255,255));
matrix.show();
}


void corona1(){
matrix.fillScreen(0);
matrix.drawPixel(2,1, matrix.Color(208,44,60));
matrix.drawPixel(5,1, matrix.Color(208,44,60));
matrix.drawPixel(3,2, matrix.Color(254,252,253));
matrix.drawPixel(4,2, matrix.Color(254,252,253));
matrix.drawPixel(1,3, matrix.Color(208,44,60));
matrix.drawPixel(2,3, matrix.Color(254,252,253));
matrix.drawPixel(3,3, matrix.Color(254,252,253));
matrix.drawPixel(4,3, matrix.Color(254,252,253));
matrix.drawPixel(5,3, matrix.Color(254,252,253));
matrix.drawPixel(2,4, matrix.Color(254,252,253));
matrix.drawPixel(3,4, matrix.Color(254,252,253));
matrix.drawPixel(4,4, matrix.Color(254,252,253));
matrix.drawPixel(5,4, matrix.Color(254,252,253));
matrix.drawPixel(6,4, matrix.Color(208,44,60));
matrix.drawPixel(3,5, matrix.Color(254,252,253));
matrix.drawPixel(4,5, matrix.Color(254,252,253));
matrix.drawPixel(2,6, matrix.Color(208,44,60));
matrix.drawPixel(5,6, matrix.Color(208,44,60));
matrix.show();
}


void corona3(){
matrix.fillScreen(0);
matrix.drawPixel(3,1, matrix.Color(208,44,60));
matrix.drawPixel(1,2, matrix.Color(208,44,60));
matrix.drawPixel(3,2, matrix.Color(254,252,253));
matrix.drawPixel(4,2, matrix.Color(254,252,253));
matrix.drawPixel(6,2, matrix.Color(208,44,60));
matrix.drawPixel(2,3, matrix.Color(254,252,253));
matrix.drawPixel(3,3, matrix.Color(254,252,253));
matrix.drawPixel(4,3, matrix.Color(254,252,253));
matrix.drawPixel(5,3, matrix.Color(254,252,253));
matrix.drawPixel(2,4, matrix.Color(254,252,253));
matrix.drawPixel(3,4, matrix.Color(254,252,253));
matrix.drawPixel(4,4, matrix.Color(254,252,253));
matrix.drawPixel(5,4, matrix.Color(254,252,253));
matrix.drawPixel(1,5, matrix.Color(208,44,60));
matrix.drawPixel(3,5, matrix.Color(254,252,253));
matrix.drawPixel(4,5, matrix.Color(254,252,253));
matrix.drawPixel(6,5, matrix.Color(208,44,60));
matrix.drawPixel(4,6, matrix.Color(208,44,60));
matrix.show();
}


void creeper1(){
matrix.fillScreen(0);
matrix.drawPixel(0,0, matrix.Color(18,122,47));
matrix.drawPixel(1,0, matrix.Color(49,227,66));
matrix.drawPixel(2,0, matrix.Color(18,122,47));
matrix.drawPixel(3,0, matrix.Color(18,122,47));
matrix.drawPixel(4,0, matrix.Color(49,227,66));
matrix.drawPixel(5,0, matrix.Color(18,122,47));
matrix.drawPixel(6,0, matrix.Color(18,122,47));
matrix.drawPixel(7,0, matrix.Color(49,227,66));
matrix.drawPixel(0,1, matrix.Color(18,122,47));
matrix.drawPixel(3,1, matrix.Color(49,227,66));
matrix.drawPixel(4,1, matrix.Color(18,122,47));
matrix.drawPixel(7,1, matrix.Color(18,122,47));
matrix.drawPixel(0,2, matrix.Color(49,227,66));
matrix.drawPixel(3,2, matrix.Color(49,227,66));
matrix.drawPixel(4,2, matrix.Color(18,122,47));
matrix.drawPixel(7,2, matrix.Color(49,227,66));
matrix.drawPixel(0,3, matrix.Color(18,122,47));
matrix.drawPixel(1,3, matrix.Color(49,227,66));
matrix.drawPixel(2,3, matrix.Color(18,122,47));
matrix.drawPixel(5,3, matrix.Color(49,227,66));
matrix.drawPixel(6,3, matrix.Color(18,122,47));
matrix.drawPixel(7,3, matrix.Color(49,227,66));
matrix.drawPixel(0,4, matrix.Color(18,122,47));
matrix.drawPixel(1,4, matrix.Color(18,122,47));
matrix.drawPixel(6,4, matrix.Color(49,227,66));
matrix.drawPixel(7,4, matrix.Color(18,122,47));
matrix.drawPixel(0,5, matrix.Color(49,227,66));
matrix.drawPixel(1,5, matrix.Color(49,227,66));
matrix.drawPixel(6,5, matrix.Color(18,122,47));
matrix.drawPixel(7,5, matrix.Color(18,122,47));
matrix.drawPixel(0,6, matrix.Color(18,122,47));
matrix.drawPixel(1,6, matrix.Color(18,122,47));
matrix.drawPixel(3,6, matrix.Color(18,122,47));
matrix.drawPixel(4,6, matrix.Color(49,227,66));
matrix.drawPixel(6,6, matrix.Color(18,122,47));
matrix.drawPixel(7,6, matrix.Color(49,227,66));
matrix.drawPixel(0,7, matrix.Color(49,227,66));
matrix.drawPixel(1,7, matrix.Color(18,122,47));
matrix.drawPixel(2,7, matrix.Color(18,122,47));
matrix.drawPixel(3,7, matrix.Color(49,227,66));
matrix.drawPixel(4,7, matrix.Color(49,227,66));
matrix.drawPixel(5,7, matrix.Color(18,122,47));
matrix.drawPixel(6,7, matrix.Color(49,227,66));
matrix.drawPixel(7,7, matrix.Color(18,122,47));
matrix.show();
}


void enderman1(){
matrix.fillScreen(0);

matrix.drawPixel(0,4, matrix.Color(196,83,225));
matrix.drawPixel(1,4, matrix.Color(100,6,151));
matrix.drawPixel(2,4, matrix.Color(196,83,225));
matrix.drawPixel(5,4, matrix.Color(196,83,225));
matrix.drawPixel(6,4, matrix.Color(100,6,151));
matrix.drawPixel(7,4, matrix.Color(196,83,225));

matrix.show();
}


void chicken1(){
matrix.fillScreen(0);
matrix.drawPixel(0,0, matrix.Color(231,231,231));
matrix.drawPixel(1,0, matrix.Color(231,231,231));
matrix.drawPixel(2,0, matrix.Color(231,231,231));
matrix.drawPixel(3,0, matrix.Color(231,231,231));
matrix.drawPixel(4,0, matrix.Color(231,231,231));
matrix.drawPixel(5,0, matrix.Color(231,231,231));
matrix.drawPixel(6,0, matrix.Color(231,231,231));
matrix.drawPixel(7,0, matrix.Color(231,231,231));
matrix.drawPixel(0,1, matrix.Color(231,231,231));
matrix.drawPixel(1,1, matrix.Color(231,231,231));
matrix.drawPixel(2,1, matrix.Color(231,231,231));
matrix.drawPixel(3,1, matrix.Color(231,231,231));
matrix.drawPixel(4,1, matrix.Color(231,231,231));
matrix.drawPixel(5,1, matrix.Color(231,231,231));
matrix.drawPixel(6,1, matrix.Color(254,252,251));
matrix.drawPixel(7,1, matrix.Color(254,252,251));
matrix.drawPixel(0,2, matrix.Color(254,252,251));
matrix.drawPixel(2,2, matrix.Color(254,252,251));
matrix.drawPixel(3,2, matrix.Color(253,252,246));
matrix.drawPixel(4,2, matrix.Color(253,252,246));
matrix.drawPixel(5,2, matrix.Color(253,252,246));
matrix.drawPixel(7,2, matrix.Color(253,252,246));
matrix.drawPixel(0,3, matrix.Color(253,252,246));
matrix.drawPixel(1,3, matrix.Color(253,252,246));
matrix.drawPixel(2,3, matrix.Color(253,252,246));
matrix.drawPixel(3,3, matrix.Color(253,252,246));
matrix.drawPixel(4,3, matrix.Color(253,252,246));
matrix.drawPixel(5,3, matrix.Color(253,252,246));
matrix.drawPixel(6,3, matrix.Color(253,252,246));
matrix.drawPixel(7,3, matrix.Color(253,252,246));
matrix.drawPixel(0,4, matrix.Color(253,252,246));
matrix.drawPixel(1,4, matrix.Color(253,252,246));
matrix.drawPixel(2,4, matrix.Color(208,188,31));
matrix.drawPixel(3,4, matrix.Color(208,188,31));
matrix.drawPixel(4,4, matrix.Color(208,188,31));
matrix.drawPixel(5,4, matrix.Color(208,188,31));
matrix.drawPixel(6,4, matrix.Color(253,252,246));
matrix.drawPixel(7,4, matrix.Color(253,252,246));
matrix.drawPixel(0,5, matrix.Color(253,252,246));
matrix.drawPixel(1,5, matrix.Color(253,252,246));
matrix.drawPixel(2,5, matrix.Color(208,188,31));
matrix.drawPixel(3,5, matrix.Color(208,188,31));
matrix.drawPixel(4,5, matrix.Color(208,188,31));
matrix.drawPixel(5,5, matrix.Color(208,188,31));
matrix.drawPixel(6,5, matrix.Color(253,252,246));
matrix.drawPixel(7,5, matrix.Color(253,252,246));
matrix.drawPixel(0,6, matrix.Color(253,252,246));
matrix.drawPixel(1,6, matrix.Color(253,252,246));
matrix.drawPixel(2,6, matrix.Color(253,252,246));
matrix.drawPixel(3,6, matrix.Color(222,44,69));
matrix.drawPixel(4,6, matrix.Color(222,44,69));
matrix.drawPixel(5,6, matrix.Color(253,252,246));
matrix.drawPixel(6,6, matrix.Color(253,252,246));
matrix.drawPixel(7,6, matrix.Color(253,252,246));
matrix.drawPixel(0,7, matrix.Color(253,252,246));
matrix.drawPixel(1,7, matrix.Color(253,252,246));
matrix.drawPixel(2,7, matrix.Color(253,252,246));
matrix.drawPixel(3,7, matrix.Color(222,44,69));
matrix.drawPixel(4,7, matrix.Color(222,44,69));
matrix.drawPixel(5,7, matrix.Color(253,252,246));
matrix.drawPixel(6,7, matrix.Color(253,252,246));
matrix.drawPixel(7,7, matrix.Color(253,252,246));
matrix.show();
}

void fw1(){
matrix.fillScreen(0);
matrix.drawPixel(1,7, matrix.Color(254,211,220));
matrix.show();
}
void fw2(){
matrix.fillScreen(0);
matrix.drawPixel(1,6, matrix.Color(254,211,220));
matrix.drawPixel(1,7, matrix.Color(239,72,160));
matrix.drawPixel(5,7, matrix.Color(160,133,252));
matrix.show();
}
void fw3(){
matrix.fillScreen(0);
matrix.drawPixel(1,5, matrix.Color(254,211,220));
matrix.drawPixel(1,6, matrix.Color(239,72,160));
matrix.drawPixel(5,6, matrix.Color(160,133,252));
matrix.drawPixel(1,7, matrix.Color(163,2,132));
matrix.drawPixel(5,7, matrix.Color(82,45,247));
matrix.show();
}
void fw4(){
matrix.fillScreen(0);
matrix.drawPixel(1,4, matrix.Color(254,211,220));
matrix.drawPixel(1,5, matrix.Color(239,72,160));
matrix.drawPixel(5,5, matrix.Color(160,133,252));
matrix.drawPixel(1,6, matrix.Color(163,2,132));
matrix.drawPixel(5,6, matrix.Color(82,45,247));
matrix.drawPixel(5,7, matrix.Color(27,23,102));
matrix.show();
}
void fw5(){
matrix.fillScreen(0);
matrix.drawPixel(1,3, matrix.Color(254,211,220));
matrix.drawPixel(0,4, matrix.Color(254,211,220));
matrix.drawPixel(1,4, matrix.Color(239,72,160));
matrix.drawPixel(2,4, matrix.Color(254,211,220));
matrix.drawPixel(5,4, matrix.Color(160,133,252));
matrix.drawPixel(1,5, matrix.Color(254,211,220));
matrix.drawPixel(5,5, matrix.Color(82,45,247));
matrix.drawPixel(5,6, matrix.Color(27,23,102));
matrix.show();
}
void fw6(){
matrix.fillScreen(0);
matrix.drawPixel(1,2, matrix.Color(254,211,220));
matrix.drawPixel(0,3, matrix.Color(254,211,220));
matrix.drawPixel(1,3, matrix.Color(239,72,160));
matrix.drawPixel(2,3, matrix.Color(254,211,220));
matrix.drawPixel(5,3, matrix.Color(160,133,252));
matrix.drawPixel(0,4, matrix.Color(239,72,160));
matrix.drawPixel(1,4, matrix.Color(163,2,132));
matrix.drawPixel(2,4, matrix.Color(239,72,160));
matrix.drawPixel(3,4, matrix.Color(254,211,220));
matrix.drawPixel(5,4, matrix.Color(82,45,247));
matrix.drawPixel(0,5, matrix.Color(254,211,220));
matrix.drawPixel(1,5, matrix.Color(239,72,160));
matrix.drawPixel(2,5, matrix.Color(254,211,220));
matrix.drawPixel(5,5, matrix.Color(27,23,102));
matrix.drawPixel(1,6, matrix.Color(254,211,220));
matrix.show();
}
void fw7(){
matrix.fillScreen(0);
matrix.drawPixel(1,1, matrix.Color(254,211,220));
matrix.drawPixel(1,2, matrix.Color(239,72,160));
matrix.drawPixel(3,2, matrix.Color(254,211,220));
matrix.drawPixel(5,2, matrix.Color(160,133,252));
matrix.drawPixel(0,3, matrix.Color(239,72,160));
matrix.drawPixel(1,3, matrix.Color(163,2,132));
matrix.drawPixel(2,3, matrix.Color(239,72,160));
matrix.drawPixel(4,3, matrix.Color(160,133,252));
matrix.drawPixel(5,3, matrix.Color(82,45,247));
matrix.drawPixel(6,3, matrix.Color(160,133,252));
matrix.drawPixel(0,4, matrix.Color(163,2,132));
matrix.drawPixel(2,4, matrix.Color(163,2,132));
matrix.drawPixel(3,4, matrix.Color(239,72,160));
matrix.drawPixel(4,4, matrix.Color(254,211,220));
matrix.drawPixel(5,4, matrix.Color(160,133,252));
matrix.drawPixel(0,5, matrix.Color(239,72,160));
matrix.drawPixel(1,5, matrix.Color(163,2,132));
matrix.drawPixel(2,5, matrix.Color(239,72,160));
matrix.drawPixel(1,6, matrix.Color(239,72,160));
matrix.drawPixel(3,6, matrix.Color(254,211,220));
matrix.drawPixel(1,7, matrix.Color(254,211,220));
matrix.show();
}
void fw8(){
matrix.fillScreen(0);
matrix.drawPixel(1,0, matrix.Color(254,211,220));
matrix.drawPixel(1,1, matrix.Color(239,72,160));
matrix.drawPixel(4,1, matrix.Color(254,211,220));
matrix.drawPixel(5,1, matrix.Color(160,133,252));
matrix.drawPixel(1,2, matrix.Color(163,2,132));
matrix.drawPixel(3,2, matrix.Color(239,72,160));
matrix.drawPixel(4,2, matrix.Color(160,133,252));
matrix.drawPixel(5,2, matrix.Color(82,45,247));
matrix.drawPixel(6,2, matrix.Color(160,133,252));
matrix.drawPixel(0,3, matrix.Color(163,2,132));
matrix.drawPixel(2,3, matrix.Color(163,2,132));
matrix.drawPixel(3,3, matrix.Color(160,133,252));
matrix.drawPixel(4,3, matrix.Color(82,45,247));
matrix.drawPixel(5,3, matrix.Color(27,23,102));
matrix.drawPixel(6,3, matrix.Color(82,45,247));
matrix.drawPixel(7,3, matrix.Color(160,133,252));
matrix.drawPixel(3,4, matrix.Color(163,2,132));
matrix.drawPixel(4,4, matrix.Color(239,72,160));
matrix.drawPixel(5,4, matrix.Color(82,45,247));
matrix.drawPixel(6,4, matrix.Color(160,133,252));
matrix.drawPixel(0,5, matrix.Color(163,2,132));
matrix.drawPixel(2,5, matrix.Color(163,2,132));
matrix.drawPixel(5,5, matrix.Color(160,133,252));
matrix.drawPixel(1,6, matrix.Color(163,2,132));
matrix.drawPixel(3,6, matrix.Color(239,72,160));
matrix.drawPixel(1,7, matrix.Color(239,72,160));
matrix.drawPixel(4,7, matrix.Color(254,211,220));
matrix.show();
}
void fw9(){
matrix.fillScreen(0);
matrix.drawPixel(1,0, matrix.Color(239,72,160));
matrix.drawPixel(5,0, matrix.Color(160,133,252));
matrix.drawPixel(1,1, matrix.Color(163,2,132));
matrix.drawPixel(3,1, matrix.Color(160,133,252));
matrix.drawPixel(4,1, matrix.Color(239,72,160));
matrix.drawPixel(5,1, matrix.Color(82,45,247));
matrix.drawPixel(7,1, matrix.Color(160,133,252));
matrix.drawPixel(3,2, matrix.Color(163,2,132));
matrix.drawPixel(4,2, matrix.Color(82,45,247));
matrix.drawPixel(5,2, matrix.Color(27,23,102));
matrix.drawPixel(6,2, matrix.Color(82,45,247));
matrix.drawPixel(2,3, matrix.Color(160,133,252));
matrix.drawPixel(3,3, matrix.Color(82,45,247));
matrix.drawPixel(4,3, matrix.Color(27,23,102));
matrix.drawPixel(6,3, matrix.Color(27,23,102));
matrix.drawPixel(7,3, matrix.Color(82,45,247));
matrix.drawPixel(3,4, matrix.Color(163,2,132));
matrix.drawPixel(4,4, matrix.Color(82,45,247));
matrix.drawPixel(5,4, matrix.Color(27,23,102));
matrix.drawPixel(6,4, matrix.Color(82,45,247));
matrix.drawPixel(3,5, matrix.Color(160,133,252));
matrix.drawPixel(5,5, matrix.Color(82,45,247));
matrix.drawPixel(7,5, matrix.Color(160,133,252));
matrix.drawPixel(3,6, matrix.Color(163,2,132));
matrix.drawPixel(5,6, matrix.Color(160,133,252));
matrix.drawPixel(1,7, matrix.Color(163,2,132));
matrix.drawPixel(4,7, matrix.Color(239,72,160));
matrix.show();
}
void fw10(){
matrix.fillScreen(0);
matrix.drawPixel(1,0, matrix.Color(163,2,132));
matrix.drawPixel(2,0, matrix.Color(160,133,252));
matrix.drawPixel(5,0, matrix.Color(82,45,247));
matrix.drawPixel(3,1, matrix.Color(82,45,247));
matrix.drawPixel(4,1, matrix.Color(163,2,132));
matrix.drawPixel(5,1, matrix.Color(27,23,102));
matrix.drawPixel(7,1, matrix.Color(82,45,247));
matrix.drawPixel(4,2, matrix.Color(27,23,102));
matrix.drawPixel(6,2, matrix.Color(27,23,102));
matrix.drawPixel(1,3, matrix.Color(160,133,252));
matrix.drawPixel(2,3, matrix.Color(82,45,247));
matrix.drawPixel(3,3, matrix.Color(27,23,102));
matrix.drawPixel(7,3, matrix.Color(27,23,102));
matrix.drawPixel(4,4, matrix.Color(27,23,102));
matrix.drawPixel(6,4, matrix.Color(27,23,102));
matrix.drawPixel(3,5, matrix.Color(82,45,247));
matrix.drawPixel(5,5, matrix.Color(27,23,102));
matrix.drawPixel(7,5, matrix.Color(82,45,247));
matrix.drawPixel(2,6, matrix.Color(160,133,252));
matrix.drawPixel(5,6, matrix.Color(82,45,247));
matrix.drawPixel(4,7, matrix.Color(163,2,132));
matrix.drawPixel(5,7, matrix.Color(160,133,252));
matrix.show();
}
void fw11(){
matrix.fillScreen(0);
matrix.drawPixel(2,0, matrix.Color(82,45,247));
matrix.drawPixel(5,0, matrix.Color(27,23,102));
matrix.drawPixel(3,1, matrix.Color(27,23,102));
matrix.drawPixel(7,1, matrix.Color(27,23,102));
matrix.drawPixel(0,3, matrix.Color(160,133,252));
matrix.drawPixel(1,3, matrix.Color(82,45,247));
matrix.drawPixel(2,3, matrix.Color(27,23,102));
matrix.drawPixel(3,5, matrix.Color(27,23,102));
matrix.drawPixel(7,5, matrix.Color(27,23,102));
matrix.drawPixel(2,6, matrix.Color(82,45,247));
matrix.drawPixel(5,6, matrix.Color(27,23,102));
matrix.drawPixel(1,7, matrix.Color(160,133,252));
matrix.drawPixel(5,7, matrix.Color(82,45,247));
matrix.show();
}
void fw12(){
matrix.fillScreen(0);
matrix.drawPixel(2,0, matrix.Color(27,23,102));
matrix.drawPixel(0,3, matrix.Color(82,45,247));
matrix.drawPixel(1,3, matrix.Color(27,23,102));
matrix.drawPixel(2,6, matrix.Color(27,23,102));
matrix.drawPixel(1,7, matrix.Color(82,45,247));
matrix.drawPixel(5,7, matrix.Color(27,23,102));
matrix.show();
}

void fw13(){
matrix.fillScreen(0);
matrix.drawPixel(0,3, matrix.Color(27,23,102));
matrix.drawPixel(1,7, matrix.Color(27,23,102));
matrix.show();
}
