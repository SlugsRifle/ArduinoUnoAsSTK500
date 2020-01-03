#include "SPI.h"
#include "command.h"

#define PIN_RESET 10
#define PIN_MOSI  11
#define PIN_MISO  12
#define PIN_SCK   13

#define BAUDRATE  115200
#define SPI_CLOCK 115200

#define HW_VER 0x02
#define SW_MAJOR 0x02
#define SW_MINOR 0x0a

volatile uint8_t rbuf[275];
volatile uint8_t sbuf[275];
volatile uint32_t addr;

void setup() {
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_MOSI, OUTPUT);
  pinMode(PIN_MISO, INPUT);
  pinMode(PIN_SCK, OUTPUT);
  
  digitalWrite(PIN_RESET, HIGH);
  
  Serial.begin(BAUDRATE);
  
  SPI.beginTransaction(SPISettings(SPI_CLOCK, MSBFIRST, SPI_MODE0));
  SPI.setDataMode(SPI_MODE0);
}

void loop(void) {
  uint8_t sn = recvMessage();
  if (rbuf[0] == CMD_SIGN_ON) {
    signOn(sn);
  } else if (rbuf[0] == CMD_SET_PARAMETER) {
    setParameter(sn);
  } else if (rbuf[0] == CMD_GET_PARAMETER) {
    getParameter(sn);
  } else if (rbuf[0] == CMD_LOAD_ADDRESS) {
    loadAddress(sn);
  } else if (rbuf[0] == CMD_ENTER_PROGMODE_ISP) {
    enterProgramMode(sn);
  } else if (rbuf[0] == CMD_LEAVE_PROGMODE_ISP) {
    leaveProgramMode(sn);
  } else if (rbuf[0] == CMD_CHIP_ERASE_ISP) {
    chipErase(sn);
  } else if (rbuf[0] == CMD_PROGRAM_FLASH_ISP) {
    programFlash(sn);
  } else if (rbuf[0] == CMD_READ_FLASH_ISP) {
    readFlash(sn);
  } else if (rbuf[0] == CMD_PROGRAM_FUSE_ISP) {
    //programFuse(sn);
  } else if (rbuf[0] == CMD_READ_FUSE_ISP) {
    readFuse(sn);
  } else if (rbuf[0] == CMD_PROGRAM_LOCK_ISP) {
    //programLock(sn);
  } else if (rbuf[0] == CMD_READ_LOCK_ISP) {
    readLock(sn);
  } else if (rbuf[0] == CMD_READ_SIGNATURE_ISP) {
    readSignature(sn);
  } else if (rbuf[0] == CMD_READ_OSCCAL_ISP) {
    readOSCCAL(sn);
  } else if (rbuf[0] == CMD_SPI_MULTI) {
    //SPIMulti(sn);
  } 
}

uint8_t getch() {
  while (!Serial.available());
  return Serial.read();
}

void signOn(uint8_t sn) {
  sbuf[0] = CMD_SIGN_ON; //0x01
  sbuf[1] = STATUS_CMD_OK; //0x00
  sbuf[2] = 8;
  sbuf[3] = 'S';
  sbuf[4] = 'T';
  sbuf[5] = 'K';
  sbuf[6] = '5';
  sbuf[7] = '0';
  sbuf[8] = '0';
  sbuf[9] = '_';
  sbuf[10] = '2';
  sendMessage(sn,11);
}

//Not Implemented
void setParameter(uint8_t sn) {
  sbuf[0] = CMD_SET_PARAMETER;
  sbuf[1] = STATUS_CMD_OK;

  if (rbuf[1] == PARAM_VTARGET) { //0x95
    sbuf[2] = 0x00;
  } else if (rbuf[1] == PARAM_VADJUST) { //0x95
    sbuf[2] = 0x00;
  } else if (rbuf[1] == PARAM_OSC_PSCALE) { //0x96
    sbuf[2] = 0x00;
  } else if (rbuf[1] == PARAM_OSC_CMATCH) { //0x97
    sbuf[2] = 0x00;
  } else if (rbuf[1] == PARAM_SCK_DURATION) { //0x98
    sbuf[2] = 0x00;
  } else if (rbuf[1] == PARAM_RESET_POLARITY) { //0x98
    sbuf[2] = 0x00;
  } else if (rbuf[1] == PARAM_CONTROLLER_INIT) { //0x98
    sbuf[2] = 0x00;
  } else {
    sbuf[1] = STATUS_CMD_FAILED;
    sendMessage(sn, 2);
    return;
  }
  sendMessage(sn, 2);
}

void getParameter(uint8_t sn) {
  sbuf[0] = CMD_GET_PARAMETER;
  sbuf[1] = STATUS_CMD_OK;

  if (rbuf[1] == PARAM_BUILD_NUMBER_LOW) { //0x80
    sbuf[2] = 0x00;
  } else if (rbuf[1] == PARAM_BUILD_NUMBER_HIGH) { //0x81
    sbuf[2] = 0x01;
  } else if (rbuf[1] == PARAM_HW_VER) { //0x90
    sbuf[2] = HW_VER;
  } else if (rbuf[1] == PARAM_SW_MAJOR) { //0x91 
    sbuf[2] = SW_MAJOR;
  } else if (rbuf[1] == PARAM_SW_MINOR) { //0x92
    sbuf[2] = SW_MINOR;
  } else if (rbuf[1] == PARAM_VTARGET) { //0x95
    sbuf[2] = 0x32; //50
  } else if (rbuf[1] == PARAM_VADJUST) { //0x95
    sbuf[2] = 0x19; //25
  } else if (rbuf[1] == PARAM_OSC_PSCALE) { //0x96
    sbuf[2] = 0x02;
  } else if (rbuf[1] == PARAM_OSC_CMATCH) { //0x97
    sbuf[2] = 0x01;
  } else if (rbuf[1] == PARAM_SCK_DURATION) { //0x98
    sbuf[2] = 0x02;
  } else if (rbuf[1] == PARAM_TOPCARD_DETECT) { //0x9a
    sbuf[2] = 0x55;
  } else if (rbuf[1] == PARAM_CONTROLLER_INIT) { //0x9f
    sbuf[2] = 0x00;
  } else {
    sbuf[1] = STATUS_CMD_FAILED;
    sendMessage(sn, 2);
    return;
  }
  sendMessage(sn, 3);
}

void loadAddress(uint8_t sn) {
  //uint8_t a1 = rbuf[1], a2 = rbuf[2], a3 = rbuf[3], a4 = rbuf[4];
  uint8_t *adr = (uint8_t*)&addr;
  for (uint8_t i = 0; i < 4; ++i) {
    adr[3-i] = rbuf[1 + i];
  }
  
  sbuf[0] = CMD_LOAD_ADDRESS;
  sbuf[1] = STATUS_CMD_OK;
  sendMessage(sn, 2);
} 

void enterProgramMode(uint8_t sn) {
  uint8_t timeout = rbuf[1], stabDelay = rbuf[2], cmdexeDelay = rbuf[3], synchLoops = rbuf[4],
  byteDelay = rbuf[5], pollValue = rbuf[6], pollIndex = rbuf[7], c1 = rbuf[8], c2 = rbuf[9],
  c3 = rbuf[10], c4 = rbuf[11];
  
  sbuf[0] = CMD_ENTER_PROGMODE_ISP;
  sbuf[1] = STATUS_CMD_OK;

  digitalWrite(PIN_RESET, LOW);
  digitalWrite(PIN_SCK, LOW);
  delay(50);
  
  digitalWrite(PIN_RESET, HIGH);
  delayMicroseconds(100);
  digitalWrite(PIN_RESET, LOW);
  delayMicroseconds(100);
  digitalWrite(PIN_RESET, HIGH);
  delayMicroseconds(100);
  digitalWrite(PIN_RESET, LOW);
  
  delay(50);
  //spiTransaction(0xAC, 0x53, 0x00, 0x00);
  //spiTransaction(c1, c2, c3, c4);
  spiTransaction(c1, c2, 0x00, 0x00);
  delay(cmdexeDelay);
  sendMessage(sn, 2);
}

void leaveProgramMode(uint8_t sn) {
  uint8_t preDelay = rbuf[1], postDelay = rbuf[2];
  sbuf[0] = CMD_LEAVE_PROGMODE_ISP;
  sbuf[1] = STATUS_CMD_OK;

  delay(preDelay);

  digitalWrite(PIN_RESET, HIGH);

  delay(postDelay);
  
  sendMessage(sn, 2);
}

uint8_t chipErase(uint8_t sn) {
  uint8_t eraseDelay = rbuf[1], pollMethod = rbuf[2], c1 = rbuf[3], c2 = rbuf[4], c3 = rbuf[5], c4 = rbuf[6];
  
  sbuf[0] = CMD_CHIP_ERASE_ISP;
  sbuf[1] = STATUS_CMD_OK;  
  
  spiTransaction(c1, c2, c3, c4);
  delay(eraseDelay);
  sendMessage(sn, 2);
}

void programFlash(uint8_t sn) {
  uint8_t numByteH = rbuf[1], numByteL = rbuf[2], mode = rbuf[3], pdelay = rbuf[4], c1 = rbuf[5], c2 = rbuf[6], c3 = rbuf[7], poll1 = rbuf[8], poll2 = rbuf[9];
  uint8_t *ad = (uint8_t*)&addr;
  uint16_t numByte = numByteH;
  numByte <<= 8;
  numByte |= numByteL;
  sbuf[0] = CMD_PROGRAM_FLASH_ISP;
  sbuf[1] = STATUS_CMD_OK;
  if (mode & 0x01) {
    for (uint16_t i = 0; i < numByte; i += 2) {    
      spiTransaction(c1, 0x00, ad[0], rbuf[10 + i]);
      spiTransaction(c1 | 0x08, 0x00, ad[0], rbuf[11 + i]);
      ++addr;
    }
    --addr;
    //writePage
    if (mode & 0x80 == 0x80) {
      spiTransaction(c2, ad[1], ad[0], 0x00);
    }  
    //valuePolling
    if ((mode & 0x20)) {
      while(spiTransaction(c3, ad[1], ad[0], 0x00) != poll1);
    }
    ++addr;
    //RDY/BSY Polling
    if ((mode & 0x40)) {
      while(spiTransaction(0xf0, 0x00, 0x00, 0x00) & 0x01);
    }
    //Delay
    if ((mode & 0x10)) {
      delay(pdelay);
    }
  } else {
    
  }
  sendMessage(sn, 2);
}

uint8_t readFlash(uint8_t sn) {
  uint8_t numByteH = rbuf[1], numByteL = rbuf[2], c1 = rbuf[3];
  uint8_t *ad = (uint8_t*)&addr;
  uint16_t numByte = numByteH;
  numByte <<= 8;
  numByte |= numByteL;
  sbuf[0] = CMD_READ_FLASH_ISP;
  sbuf[1] = STATUS_CMD_OK;

  for (uint16_t i = 0; i < numByte; i += 2) {
    sbuf[i + 2] = spiTransaction(c1, ad[1], ad[0], 0x00);
    sbuf[i + 3] = spiTransaction(c1 | 0x08, ad[1], ad[0], 0x00);
    ++addr;
  }
  sbuf[numByte + 2] = STATUS_CMD_OK;
  sendMessage(sn, numByte + 3);
}

uint8_t programFuse(uint8_t sn) {
  uint8_t c1 = rbuf[1], c2 = rbuf[2], c3 = rbuf[3], c4 = rbuf[4];
  
  sbuf[0] = CMD_PROGRAM_FUSE_ISP;
  sbuf[1] = STATUS_CMD_OK;
  sbuf[2] = STATUS_CMD_OK;
  
  spiTransaction(c1, c2, c3, c4);
  sendMessage(sn, 3);
}

uint8_t readFuse(uint8_t sn) {
  uint8_t retAdr = rbuf[1], c1 = rbuf[2], c2 = rbuf[3], c3 = rbuf[4], c4 = rbuf[5];
  
  sbuf[0] = CMD_READ_FUSE_ISP;
  sbuf[1] = STATUS_CMD_OK;
  sbuf[3] = STATUS_CMD_OK;
  
  sbuf[2] = spiTransaction(c1, c2, c3, c4);
  sendMessage(sn, 4);
}

uint8_t programLock(uint8_t sn) {
  uint8_t c1 = rbuf[1], c2 = rbuf[2], c3 = rbuf[3], c4 = rbuf[4];
  
  sbuf[0] = CMD_PROGRAM_LOCK_ISP;
  sbuf[1] = STATUS_CMD_OK;
  sbuf[2] = STATUS_CMD_OK;
  
  spiTransaction(c1, c2, c3, c4);
  sendMessage(sn, 3);
}

uint8_t readLock(uint8_t sn) {
  uint8_t retAdr = rbuf[1], c1 = rbuf[2], c2 = rbuf[3], c3 = rbuf[4], c4 = rbuf[5];
  
  sbuf[0] = CMD_READ_LOCK_ISP;
  sbuf[1] = STATUS_CMD_OK;
  sbuf[3] = STATUS_CMD_OK;
  
  sbuf[2] = spiTransaction(c1, c2, c3, c4);
  sendMessage(sn, 4);
}

uint8_t readSignature(uint8_t sn) {
  uint8_t retAdr = rbuf[1], c1 = rbuf[2], c2 = rbuf[3], c3 = rbuf[4], c4 = rbuf[5];
  
  sbuf[0] = CMD_READ_SIGNATURE_ISP;
  sbuf[1] = STATUS_CMD_OK;
  sbuf[3] = STATUS_CMD_OK;
  
  //sbuf[2] = spiTransaction(c1, c2, c3, c4);
  sbuf[2] = spiTransaction(c1, 0x00, c3, 0x00);
  //sbuf[2] = spiTransaction(0x30, 0x00, c3, 0x00);
  sendMessage(sn, 4);
}

uint8_t readOSCCAL(uint8_t sn) {
  uint8_t retAdr = rbuf[1], c1 = rbuf[2], c2 = rbuf[3], c3 = rbuf[4], c4 = rbuf[5];
  
  sbuf[0] = CMD_READ_OSCCAL_ISP;
  sbuf[1] = STATUS_CMD_OK;
  sbuf[3] = STATUS_CMD_OK;
  
  sbuf[2] = spiTransaction(c1, c2, c3, c4);
  sendMessage(sn, 4);
}

//Not Implemented
uint8_t SPIMulti(uint8_t sn) {
  uint8_t numTx = rbuf[1], numRx = rbuf[2], rxStartAddr = rbuf[3];
  
  sbuf[0] = CMD_SPI_MULTI;
  sbuf[1] = STATUS_CMD_OK;
  //sbuf[3] = STATUS_CMD_OK;
  
  //sbuf[2] = spiTransaction(c1, c2, c3, c4);
  sendMessage(sn, 4);
}

//Not Implement TimeOut and CheckSumError
uint8_t recvMessage() {
  uint8_t sn = 0;
  uint8_t st = 0,ch = 0, cs = 0;
  uint16_t ms,count = 0;
  while(1) {
    if (st == START) {
      ch = getch();
      cs ^= ch;
      if (ch == MESSAGE_START) { //0x1b
        st = 1;
      }
    } else if (st == GET_SEQEUNCE_NUMBER) {
      ch = getch();
      cs ^= ch;
      sn = ch;
      st = 2;
      /*if (ch == sn) {
        st = 2;
        } else {
        st = 0;
      }*/
    } else if (st == GET_MESSAGE_SIZE_1) {
      ch = getch();
      ms = ch;
      cs ^= ch;
      ms <<= 8;
      st = 3;
    } else if (st == GET_MESSAGE_SIZE_2) {
      ch = getch();
      cs ^= ch;
      ms |= ch;
      st = 4;
    } else if (st == GET_TOKEN) { //0x0e
      ch = getch();
      cs ^= ch;
      if (ch == TOKEN) {
        st = 5;
        } else {
        st = 0;
      }
    } else if (st == GET_DATA) {
      if (count < ms) {
        ch = getch();
        rbuf[count++] = ch;
        cs ^= ch;
        } else {
        st = 6;
      }
    } else if (st == GET_CHECKSUM) {
      ch = getch();
      return sn;
      /*if (cs == ch) {
        return;
      } else {
        break;
      }*/
    }
  }
  return 0;
}

void sendMessage(uint8_t sn, uint16_t size) {
  uint8_t hsize, lsize, cs;
  
  hsize = (size >> 8) & 0xff;
  lsize = size & 0xff;
  
  cs = MESSAGE_START ^ TOKEN ^ sn ^ hsize ^ lsize;
  
  for (int i = 0; i < size; ++i) {
    cs ^= sbuf[i];
  }
  
  Serial.write(MESSAGE_START);
  Serial.write(sn);
  Serial.write(hsize);
  Serial.write(lsize);  
  Serial.write(TOKEN);
  Serial.flush();
  for (int i = 0; i < size; ++i) {
    Serial.write(sbuf[i]);
  }
  Serial.flush();
  Serial.write(cs);
  Serial.flush();
}

uint8_t spiTransaction(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
  SPI.transfer(a);
  SPI.transfer(b);
  SPI.transfer(c);
  return SPI.transfer(d);
}

uint8_t calcSDKDuration(uint32_t freq) {
  uint8_t sck_dur;
  if (freq >= 1843200)
    sck_dur = 0;
  else if (freq >= 460800)
    sck_dur = 1;
  else if (freq >= 115200)
    sck_dur = 2;
  else if (freq >= 57600)
    sck_dur = 3;
  else
    sck_dur = ceil(1/(2 * 12.0 * freq * 135.63e-9) - 10 / 12.0);
  return min(254, sck_dur);
}
