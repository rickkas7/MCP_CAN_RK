// Demo: Dual CAN-BUS Shields, Data Pass-through
// Written by: Cory J. Fowler
// January 31st 2014
// This examples the ability of this library to support more than one MCP2515 based CAN interface.


#include <mcp_can.h>
#include <SPI.h>

unsigned long rxId;
byte len;
byte rxBuf[8];

byte tx0Buf[] = {0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55};
byte tx1Buf[] = {0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA};

MCP_CAN MyCAN0(10);                              // CAN0 interface usins CS on digital pin 10
MCP_CAN MyCAN1(9);                               // CAN1 interface using CS on digital pin 9

void setup()
{
  Serial.begin(115200);
  
  // init CAN0 bus, baudrate: 250k@16MHz
  if(MyCAN0.begin(MCP_EXT, CAN_250KBPS, MCP_16MHZ) == CAN_OK){
  Serial.print("CAN0: Init OK!\r\n");
  MyCAN0.setMode(MCP_NORMAL);
  } else Serial.print("CAN0: Init Fail!!!\r\n");
  
  // init CAN1 bus, baudrate: 250k@16MHz
  if(MyCAN1.begin(MCP_EXT, CAN_250KBPS, MCP_16MHZ) == CAN_OK){
  Serial.print("CAN1: Init OK!\r\n");
  MyCAN1.setMode(MCP_NORMAL);
  } else Serial.print("CAN1: Init Fail!!!\r\n");
  
  SPI.setClockDivider(SPI_CLOCK_DIV2);         // Set SPI to run at 8MHz (16MHz / 2 = 8 MHz)
  
  MyCAN0.sendMsgBuf(0x1000000, 1, 8, tx0Buf);
  MyCAN1.sendMsgBuf(0x1000001, 1, 8, tx1Buf);
}

void loop(){  
  if(!digitalRead(2)){                         // If pin 2 is low, read CAN0 receive buffer
    MyCAN0.readMsgBuf(&rxId, &len, rxBuf);       // Read data: len = data length, buf = data byte(s)
    MyCAN1.sendMsgBuf(rxId, 1, len, rxBuf);      // Immediately send message out CAN1 interface
  }
  if(!digitalRead(3)){                         // If pin 3 is low, read CAN1 receive buffer
    MyCAN1.readMsgBuf(&rxId, &len, rxBuf);       // Read data: len = data length, buf = data byte(s)
    MyCAN0.sendMsgBuf(rxId, 1, len, rxBuf);      // Immediately send message out CAN0 interface
  }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
