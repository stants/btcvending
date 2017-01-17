 #include <SoftwareSerial.h>
 #include <Wire.h>
 #include <SPI.h>
#include <SD.h>
#include <LiquidCrystal.h>

//Para deletar arquivo use SD.remove("example.txt");
//Noteiro---------------------------------
int contador = 0;
unsigned long timerx;
bool timerinit = false;
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);  
char filepath[120];
    char path[128];
    char ibuf[5];
    char billavailchar[50];

void lcdinit() { 
 lcd.setCursor(0,0);  
 lcd.print("Notas aceitas:R$  ");  
 lcd.setCursor(0,1);
 strcpy(billavailchar,isBillAvBool(2)?"2,":"");
 strcat(billavailchar,isBillAvBool(5)?"5,":"");
 strcat(billavailchar,isBillAvBool(10)?"10,":"");
 strcat(billavailchar,isBillAvBool(20)?"20,":"");
 strcat(billavailchar,isBillAvBool(50)?"50,":"");
 strcat(billavailchar,isBillAvBool(100)?"100.":"");
 Serial.println(billavailchar);
 if (strlen(billavailchar) < 2) {
  //timerx = millis() - 10000;
  lcd.setCursor(0,0);  
  lcd.print("Nenhuma nota        ");  
  lcd.setCursor(0,1);
  lcd.print("aceita.             ");
 }
 else {
 lcd.print(billavailchar);
 }
}

//Fim LCD---------------------------------
//Printer OpenATM--------------------------------
//This code was ADAPTED from Open Bitcoin ATM, created by John Mayo-Smith from https://openbitcoinatm.wordpress.com/about/. This fragment is licensed under a Creative Commons Attribution 4.0 International License.
 byte cThisChar; //for streaming from SD card
 byte cLastChar; //for streaming from SD card
 char cHexBuf[3]; //for streaming from SD card
 const int HEADER_LEN = 25; //maximum size of bitmap header
  #define DOUBLE_HEIGHT_MASK (1 << 4) //size of pixels
 #define DOUBLE_WIDTH_MASK  (1 << 5) //size of pixels
 const int chipSelect = 53; //SD module
 
 int printer_RX_Pin = 26;  // This is the green wire
 int printer_TX_Pin = 28;  // This is the yellow wire
 
char printDensity = 4; // 15; //text darkening
 char printBreakTime = 4; //15; //text darkening

 
 //Initialize the printer connection

 SoftwareSerial *printer;
 #define PRINTER_WRITE(b) printer->write(b)

void imprimetudo (int contador) {  
  
  printer = new SoftwareSerial(printer_RX_Pin, printer_TX_Pin);
  printer->begin(19200);

  //Modify the print speed and heat
  PRINTER_WRITE(27);
  PRINTER_WRITE(55);
  PRINTER_WRITE(7); //Default 64 dots = 8*('7'+1)
  PRINTER_WRITE(100); //Default 80 or 800us
  PRINTER_WRITE(255); //Default 2 or 20us

  //Modify the print density and timeout
  PRINTER_WRITE(18);
  PRINTER_WRITE(35);
  //int printSetting = (printDensity<<4) | printBreakTime;
  int printSetting = (printBreakTime<<5) | printDensity;
  PRINTER_WRITE(printSetting); //Combination of printDensity and printBreakTime

/* For double height text. Disabled to save paper
  PRINTER_WRITE(27);
  PRINTER_WRITE(33);
  PRINTER_WRITE(DOUBLE_HEIGHT_MASK);
  PRINTER_WRITE(DOUBLE_WIDTH_MASK);
*/
               printer->println("Bitcoin Wallet");

               printer->println("Chave privada: reserve-a.");

               printer->println("Duvidas e suporte: openbtcvending@gmail.com");
               
               printer->println(" ");
               delay(500);
               Serial.print("Path:");
               Serial.println(filepath);
               printBitmap(filepath);
             delay(1000);
             printer->println(" ");
             printer->println(" ");
            printer->flush();
}

void printBitmap(char *filename){
  int nBytes = 0;
  int iBitmapWidth = 0 ;
  int iBitmapHeight = 0 ;
  File tempFile = SD.open(filename);

        for(int h = 0; h < HEADER_LEN; h++){
        
          cLastChar = cThisChar;
          if(tempFile.available()) cThisChar = tempFile.read(); 
    
              //read width of bitmap
              if(cLastChar == '0' && cThisChar == 'w'){
                if(tempFile.available()) cHexBuf[0] = tempFile.read(); 
                if(tempFile.available()) cHexBuf[1] = tempFile.read(); 
                  cHexBuf[2] = '\0';
                  
                  iBitmapWidth = (byte)strtol(cHexBuf, NULL, 16); 
                  //Serial.println("bitmap width");
                  //Serial.println(iBitmapWidth);           
              }
    
              //read height of bitmap
              if(cLastChar == '0' && cThisChar == 'h'){
               
                 if(tempFile.available()) cHexBuf[0] = tempFile.read(); 
                 if(tempFile.available()) cHexBuf[1] = tempFile.read(); 
                  cHexBuf[2] = '\0';
                  
                  iBitmapHeight = (byte)strtol(cHexBuf, NULL, 16);
                  //Serial.println("bitmap height");
                  //Serial.println(iBitmapHeight); 
              }
      }
      
  
      PRINTER_WRITE(0x0a); //line feed

      
      Serial.println("Print bitmap image");
      //set Bitmap mode
      PRINTER_WRITE(18); //DC2 -- Bitmap mode
      PRINTER_WRITE(42); //* -- Bitmap mode
      PRINTER_WRITE(iBitmapHeight); //r
      PRINTER_WRITE((iBitmapWidth+7)/8); //n (round up to next byte boundary
  
  
      //print 
      while(nBytes < (iBitmapHeight * ((iBitmapWidth+7)/8))){ 
        if(tempFile.available()){
            cLastChar = cThisChar;
            cThisChar = tempFile.read(); 
        
                if(cLastChar == '0' && cThisChar == 'x'){
      
                    cHexBuf[0] = tempFile.read(); 
                    cHexBuf[1] = tempFile.read(); 
                    cHexBuf[2] = '\0';
                    //Serial.println(cHexBuf);
                    
                    PRINTER_WRITE((byte)strtol(cHexBuf, NULL, 16)); 
                    nBytes++;
                }
        }  
          
      }

       
      PRINTER_WRITE(10); //Paper feed

      Serial.println("Print bitmap done");


    tempFile.close();
    Serial.println("file closed");

    //tempFile.close();

    SD.remove(filename);

 
 
   // update transaction log file
    //if (! SD.exists(LOG_FILE)) {
      // only open a new file if it doesn't exist
       

    //}
    
  return;
}


//Fim Printer OpenATM---------------------------------

//Início função checagem de nota se tem bitcoin correspondente
void isBillAvailable(int value) {
  //while (true) {
    strcpy(path, "/");
    itoa(value, ibuf, 10);
    strcat(path,ibuf);
    File sdopen = SD.open(path);
    File entry =  sdopen.openNextFile();
    if (! entry) {
      // no more files
      Serial.print("Nao tem nota de ");
      Serial.println(ibuf);
      entry.close();
      sdopen.close();
      strcpy(filepath, "");
    //  break;
    }
    if (entry) {
    Serial.print("Tem sim nota de ");
    Serial.println(ibuf);
    strcpy(filepath,path);
    strcat(filepath,"/");
    strcat(filepath,entry.name());
    sdopen.close();
    entry.close();
    //break;
    }
    
    entry.close();
  //}
}

bool isBillAvBool(int value) {
  //while (true) {
    //char path[128];
    //char ibuf[5];
    strcpy(path, "/");
    itoa(value, ibuf, 10);
    strcat(path,ibuf);
    //Serial.println(path);
    File sdopen = SD.open(path);
    File entry =  sdopen.openNextFile();
    if (! entry) {
      // no more files
      Serial.print("Nao tem nota de ");
      Serial.println(ibuf);
      entry.close();
      sdopen.close();
      return false;
    }
    if (entry) {
    Serial.print("Tem sim nota de ");
      Serial.println(ibuf);
    //String filen = entry.name();
    entry.close();
    sdopen.close();
    return true;
    }
  //}
}
//Fim da função checagem de nota se tem bitcoin correspondente

void setup()
{
// Noteiro
  pinMode(23, OUTPUT);
  pinMode(A0, INPUT); // Busy bill acceptor pin
 Serial.begin(9600);
 Serial1.begin(9600);
 digitalWrite(23, LOW);
 //Fim noteiro
//SD card start
  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
  }
//SD card end
//LCD
lcd.begin(16, 2);
lcdinit();
//Fim LCD

}

void loop()
{
  
//Noteiro  
 if( Serial1.available() > 0)
 {    
  if (Serial1.read() == 0 ) {
    contador++;
    //DEBUG of contador variable
   Serial.println(contador);
   //DEBUG of busy bill acceptor pin
   //Serial.println("Busy: " + String(digitalRead(A0)));
   Serial.flush();
   }
   
   }
   else {
    if (contador != 0 && digitalRead(A0) == HIGH) {
      Serial1.flush();
      Serial.print("Nota:");
      Serial.println(contador);
      //Serial.println("Busy: " + String(digitalRead(A0)));
      lcd.setCursor(0,0);
      char toshow[20];
      //char ibuf[5];
      itoa(contador, ibuf,10);
      strcpy(toshow, "Nota de R$");
      strcat(toshow, ibuf);
      strcat(toshow, ",00     ");
      lcd.print(toshow);
      lcd.setCursor(0,1);
      lcd.print("inserida.           ");
      timerx = millis();
      timerinit = true;
      isBillAvailable(contador);
      if (isBillAvBool(contador)) {
        imprimetudo(contador);
      }
      //billavail = "false";
      contador = 0;
      Serial.flush();
    }
 }
 //Fim do noteiro

if ( timerinit == true && (millis() - timerx) > 13000) {
  lcdinit();
  timerinit = false;
}
 
}
