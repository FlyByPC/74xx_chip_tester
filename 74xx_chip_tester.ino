//Pin definitions
#define DATAPIN 2
#define CLOCKPIN 15
#define RESETPIN 17

//Set up TFT Display
#include<TFT_eSPI.h>
#include<SPI.h>
TFT_eSPI tft = TFT_eSPI();
#define TFT_GREY 0x7BEF
#define XSIZE 239
#define YSIZE 135

//Macroscopic delay times (ms)
#define TESTDELAYTIME 0 //Time between tests, if any

//Comms timings (all in MICROSECONDS, not ms!)
#define RESETTIME 20 //Should be even
#define BITTIME 20 //Should be even

void setup() {
  // put your setup code here, to run once:
  pinMode(CLOCKPIN, OUTPUT);
  digitalWrite(CLOCKPIN, LOW);
  pinMode(DATAPIN, OUTPUT);
  digitalWrite(DATAPIN, LOW);
  pinMode(RESETPIN, OUTPUT);
  digitalWrite(RESETPIN, LOW);

  Serial.begin(115200);
  delay(1000);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(2, 2, 2);

  uint8_t testWord = 0x00;
  uint8_t reverseWord = 0x00;
  uint32_t totalTests=0;
  uint32_t notTotal=0;
  uint32_t xorTotal=0;
  uint32_t xnorTotal=0;
  uint32_t orTotal=0;
  uint32_t norTotal=0;
  uint32_t andTotal=0;
  uint32_t nandTotal=0;

  for(int gates = 1; gates < 7; gates++){
    Serial.println(gates);
    for(uint32_t AA=0;AA<2;AA++){ //This nest sets up the 'truth table' that we will be testing on
      for(uint32_t BB=0;BB<2;BB++){ //The ports read xx000000 and we are testing on the 0s
        for(uint32_t CC=0;CC<2;CC++){
          int testTemp=0;   //For scores
          
          //The physical system has requires both versions of the test word
          testWord = ((0b1 && AA) << 5) + ((0b1 && BB) << 4) + ((0b1 && CC) << 3) + ((0b1 && AA) << 2) + ((0b1 && BB) << 1) + ((0b1 && CC) << 0);
          reverseWord = ((0b1 && CC) << 5) + ((0b1 && BB) << 4) + ((0b1 && AA) << 3) + ((0b1 && CC) << 2) + ((0b1 && BB) << 1) + ((0b1 && AA) << 0);

          doReset();

          //This writes out the data to the PIC, First which PIC, then I/OMask (always 0), then the test masks
          sendByte(0x01);
          sendByte(0x00);
          sendByte(0x00);
          sendByte(testWord);
          sendByte(reverseWord);

          //We read the two inputs ports back from the PIC
          uint8_t PICportA = readByte();
          uint8_t PICportB = readByte();

          //And put it together for later use
          uint16_t PICout = (PICportA << 8) + PICportB;
          Serial.println(PICout, BIN);
          //Test for NOT
          testTemp=notScore(PICout, gates);
          if(testTemp==1) notTotal++;

          //Test for XOR
          testTemp=xorScore(PICout, gates);
          if(testTemp==1) xorTotal++;

          //Test for XNOR
          testTemp=xnorScore(PICout, gates);
          if(testTemp==1) xnorTotal++;

          //Test for OR
          testTemp=orScore(PICout, gates);
          if(testTemp==1) orTotal++; 

          //Test for AND
          testTemp=andScore(PICout, gates);
          if(testTemp==1) andTotal++; 

          //Test for NAND
          testTemp=nandScore(PICout, gates);
          if(testTemp==1) nandTotal++; 

          //Test for NOR
          testTemp=norScore(PICout, gates);
          if(testTemp==1) norTotal++; 
          
          delay(TESTDELAYTIME);

          totalTests++;
        }
      }
    }
    //If you would like a summary of the tests done, uncomment the following code (you may also have to tweak the tft display settings)
    Serial.printf("SUMMARY\n");
    Serial.printf("-------\n");
    Serial.printf("NOT: %lu : %lu\n",notTotal,totalTests);
    Serial.printf("XOR: %lu : %lu\n",xorTotal,totalTests);
    Serial.printf("XNOR: %lu : %lu\n",xnorTotal,totalTests);
    Serial.printf("OR: %lu : %lu\n",orTotal,totalTests);
    Serial.printf("NOR: %lu : %lu\n",norTotal,totalTests);
    Serial.printf("AND: %lu : %lu\n",andTotal,totalTests);
    Serial.printf("NAND: %lu : %lu\n",nandTotal,totalTests);
    Serial.println();

    //Display what chip the tests concluded on
    
    tft.printf("Gate: %lu : ", gates);
    if(notTotal == totalTests) tft.println("7404 - NOT");
    else if(xorTotal == totalTests) tft.println("7486 - XOR");
    else if(xnorTotal == totalTests) tft.println("747266 - XNOR");
    else if(orTotal == totalTests) tft.println("7432 - OR");
    else if(norTotal == totalTests) tft.println("7402 - NOR");
    else if(andTotal == totalTests) tft.println("7408 - AND");
    else if(nandTotal == totalTests) tft.println("7400 - NAND");
    else tft.println("Gate Is Bad");
    //Note that this currently tests the whole chip at once and cannot test individual gates
    
    //reset these values
    totalTests=0;
    notTotal=0;
    xorTotal=0;
    xnorTotal=0;
    orTotal=0;
    norTotal=0;
    andTotal=0;
    nandTotal=0;
  }
}

void loop() { //no loop; the funcional code from the setup could be put here to cause it to run continually

}

//Sends the reset signal
void doReset(){
  digitalWrite(RESETPIN, HIGH);
  delayMicroseconds(RESETTIME/2);
  digitalWrite(RESETPIN, LOW);
  delayMicroseconds(RESETTIME/2);
}

//Cycles the clock
void cycleClock(){
  delayMicroseconds(BITTIME);
  digitalWrite(CLOCKPIN, HIGH);
  delayMicroseconds(BITTIME);
  digitalWrite(CLOCKPIN, LOW);  
}  

//Code to send one Byte of data to the PIC
void sendByte(uint8_t theByte){

  //Set relevant Pins LOW, and make sure that we are outputing data
  digitalWrite(CLOCKPIN, LOW);
  digitalWrite(DATAPIN, LOW);
  pinMode(DATAPIN, OUTPUT);

  //For each bit of the Byte send that bit - this sends MSB first (left to right)
  for (int size = 7 ; size >= 0 ; size--){
    if(((theByte >> size) & 0b1) == 1){
      digitalWrite(DATAPIN, HIGH);
    } else{
      digitalWrite(DATAPIN, LOW); 
    }
    cycleClock();
  }

  //Clear the Data Pin and set it to read for later use
  digitalWrite(DATAPIN, LOW);
  pinMode(DATAPIN, INPUT);
}

//Code to read one Byte of data from the PIC
uint8_t readByte(){
  //setup the variables that we will be using
  int bitCount=0;
  uint8_t result=0x00;
  //Make sure that the Microcontroller is reading data, not writing it
  pinMode(DATAPIN, INPUT);
  //A different way to say 'For each bit of the byte, read it' also MSB
  while (bitCount<8){
    digitalWrite(CLOCKPIN, LOW);
    delayMicroseconds(BITTIME/2);
    digitalWrite(CLOCKPIN, HIGH);
    delayMicroseconds(BITTIME/2); //Need to wait after sending clock high
    if(digitalRead(DATAPIN)==HIGH){result = result | (0x80 >> bitCount);}
    bitCount++;
    delayMicroseconds(BITTIME/2); //Probably not needed
    digitalWrite(CLOCKPIN, LOW);
    }//while
  return (result);
}

//Code to pull an indivdual bit from a 2 byte number - this is used for gate testing
uint8_t getBit(uint16_t d, uint8_t bit){
  if(d & (0x01 << bit)){return(1);}
  return(0);
}

//check for NOT Chip
int notScore(uint16_t a, int gate){
  int score=0;

  switch(gate){
    case 1: 
      if(getBit(a,0)!=getBit(a,1)){score++;}
      break;
    case 2:
      if(getBit(a,2)!=getBit(a,3)){score++;}
      break;
    case 3: 
      if(getBit(a,4)!=getBit(a,5)){score++;}
      break;
    case 4: 
      if(getBit(a,8)!=getBit(a,9)){score++;}
      break;
    case 5: 
      if(getBit(a,10)!=getBit(a,11)){score++;}
      break;
    case 6: 
      if(getBit(a,12)!=getBit(a,13)){score++;}
      break;
    default: 
      0;
      break;
  }

  return(score);
}//notScore()

//check for OR Chip
int orScore(uint16_t a, int gate){
  int score=0;
  
  switch(gate){
    case 1: 
      if((getBit(a,1)|getBit(a,2))==getBit(a,0)){score++;}
      break;
    case 2: 
      if((getBit(a,5)|getBit(a,4))==getBit(a,3)){score++;}
      break;
    case 3: 
      if((getBit(a,8)|getBit(a,9))==getBit(a,10)){score++;}
      break;
    case 4: 
      if((getBit(a,11)|getBit(a,12))==getBit(a,13)){score++;}
      break;
    default: 
      0;
      break;
  }
  
  return(score);
}//orScore()

//check for XOR Chip
int xorScore(uint16_t a, int gate){
  int score=0;
  
  switch(gate){
    case 1: 
      if((getBit(a,1)^getBit(a,2))==getBit(a,0)){score++;}
      break;
    case 2: 
      if((getBit(a,5)^getBit(a,4))==getBit(a,3)){score++;}
      break;
    case 3: 
      if((getBit(a,8)^getBit(a,9))==getBit(a,10)){score++;}
      break;
    case 4: 
      if((getBit(a,11)^getBit(a,12))==getBit(a,13)){score++;}
      break;
    default: 
      0;
      break;
  }
  
  return(score);
}//xorScore()

//check for XNOR Chip
int xnorScore(uint16_t a, int gate){
  int score=0;
  
  switch(gate){
    case 1: 
      if((getBit(a,1)^getBit(a,2))!=getBit(a,0)){score++;}
      break;
    case 2: 
      if((getBit(a,5)^getBit(a,4))!=getBit(a,3)){score++;}
      break;
    case 3: 
      if((getBit(a,8)^getBit(a,9))!=getBit(a,10)){score++;}
      break;
    case 4: 
      if((getBit(a,11)^getBit(a,12))!=getBit(a,13)){score++;}
      break;
    default: 
      0;
      break;
  }

  return(score);
}//xnorScore()

//check for NOR Chip
int norScore(uint16_t a, int gate){
  int score=0;
  
  switch(gate){
    case 1: 
      if((getBit(a,0)|getBit(a,1))!=getBit(a,2)){score++;}
      break;
    case 2: 
      if((getBit(a,3)|getBit(a,4))!=getBit(a,5)){score++;}
      break;
    case 3: 
      if((getBit(a,10)|getBit(a,9))!=getBit(a,8)){score++;}
      break;
    case 4: 
      if((getBit(a,13)|getBit(a,12))!=getBit(a,11)){score++;}
      break;
    default: 
      0;
      break;
  }
  
  return(score);
}//norScore()

//check for AND Chip
int andScore(uint16_t a, int gate){
  int score=0;
  
  switch(gate){
    case 1: 
      if((getBit(a,1)&getBit(a,2))==getBit(a,0)){score++;}
      break;
    case 2: 
      if((getBit(a,5)&getBit(a,4))==getBit(a,3)){score++;}
      break;
    case 3: 
      if((getBit(a,8)&getBit(a,9))==getBit(a,10)){score++;}
      break;
    case 4: 
      if((getBit(a,11)&getBit(a,12))==getBit(a,13)){score++;}
      break;
    default: 
      0;
      break;
  }
  
  return(score);
}//andScore()

//check for NAND Chip
int nandScore(uint16_t a, int gate){
  int score=0;
  
  switch(gate){
    case 1: 
      if((getBit(a,1)&getBit(a,2))!=getBit(a,0)){score++;}
      break;
    case 2: 
      if((getBit(a,5)&getBit(a,4))!=getBit(a,3)){score++;}
      break;
    case 3: 
      if((getBit(a,8)&getBit(a,9))!=getBit(a,10)){score++;}
      break;
    case 4: 
      if((getBit(a,11)&getBit(a,12))!=getBit(a,13)){score++;}
      break;
    default: 
      0;
      break;
  }
  
  return(score);
}//nandScore()
