// CAN Send Example
//

#include <mcp_can.h>
#include <SPI.h>

SYSTEM_THREAD(ENABLED);
SerialLogHandler logHandler(LOG_LEVEL_TRACE);


#define CAN0_INT A1                              // Set INT to pin A1
MCP_CAN CAN0(A2);                               // Set CS to pin A2

unsigned long lastRequest = 0;


const uint8_t SERVICE_CURRENT_DATA = 0x01; // also known as mode 1

// These are the CAN IDs (11-bit) for OBD-II requests to the primary ECU 
// and the CAN ID for the response. 
const uint32_t OBD_CAN_REQUEST_ID      = 0x7DF;
const uint32_t OBD_CAN_REPLY_ID        = 0x7E8;

// Note: SAE PID codes are 8 bits. Proprietary ones are 16 bits.
const uint8_t PID_ENGINE_RPM          = 0x0C;
const uint8_t PID_VEHICLE_SPEED       = 0x0D;

byte data[8] = {0x02, SERVICE_CURRENT_DATA, PID_ENGINE_RPM, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc};

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];

char msgString[128];

void setup()
{
  //Serial.begin(115200);

  // Wait 10 seconds for USB debug serial to be connected (plus 1 more)
  waitFor(Serial.isConnected, 10000);
  delay(1000);

  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) Log.info("MCP2515 Initialized Successfully!");
  else Log.error("Error Initializing MCP2515...");

  CAN0.setMode(MCP_NORMAL);   // Change to normal mode to allow messages to be transmitted
}

void loop()
{
  if(!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)
    
    if((rxId & 0x80000000) == 0x80000000)     // Determine if ID is standard (11 bits) or extended (29 bits)
      Log.trace("Extended ID: 0x%.8lX  DLC: %1d", (rxId & 0x1FFFFFFF), len);
    else
      Log.trace("Standard ID: 0x%.3lX       DLC: %1d", rxId, len);
  
  
    if((rxId & 0x40000000) == 0x40000000){    // Determine if message is a remote request frame.
      sprintf(msgString, " REMOTE REQUEST FRAME");
      Log.trace(msgString);
    } else {
      char *cp = msgString;
      cp += sprintf(cp, " Data: ");
      for(byte i = 0; i<len; i++){
        cp += sprintf(cp, " %02x", rxBuf[i]);
      }
      Log.trace(msgString);
    }
  }

  
  if (millis() - lastRequest >= 1000) {
      lastRequest = millis();

    // send data:  ID = 0x100, Standard CAN Frame, Data length = 8 bytes, 'data' = array of data bytes to send
    byte sndStat = CAN0.sendMsgBuf(OBD_CAN_REQUEST_ID, 0, 8, data);
    if(sndStat == CAN_OK){
        Log.trace("Message Sent Successfully!");
    } else {
        Log.error("Error Sending Message %d", sndStat);
    }
  }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
