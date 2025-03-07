#include<SoftwareSerial.h>
#include<LiquidCrystal_I2C.h>
#include<Keypad.h>

LiquidCrystal_I2C lcd (0x3F, 20, 4);

SoftwareSerial gsm (3, 2);

const byte row = 4;
const byte col = 3;

const byte rowPins[row] = {4, 5, 6, 7};
const byte colPins[col] = {8, 9, 10};


char keySet[row][col] = {

  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}

};

Keypad myKeypad = Keypad(makeKeymap(keySet), rowPins, colPins, row, col);
char key;

const byte passwordSize = 7;
char mainPassword[passwordSize];
char inputPassword[passwordSize];
unsigned long otp;
byte count = 0; byte flag=0; 

byte relay = 11;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  gsm.begin(9600);

  Serial.println("Initializing Netwrok...");

  gsm.println("AT");
  checkSerial();

  gsm.println("AT+CSQ");
  checkSerial();

  gsm.println("AT+CCID");
  checkSerial();

  gsm.println("AT+CREG?");
  checkSerial();

  gsm.println("AT+CBC");
  checkSerial();

  gsm.println("AT+COPS?");
  checkSerial();

  gsm.println("AT+CMGF=1"); // Initializes the text mode
  checkSerial();

  gsm.println("AT+CNMI=2,2,0,0,0"); // Decides how newly arrived messages will be handled
  checkSerial();

  //  gsm.println("AT+CMGD=1,4");
  //  checkSerial();

  pinMode(relay, OUTPUT);


  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("WELCOME 2 RAFI'S");
  lcd.setCursor(0, 1);
  lcd.print("SMART DOOR LOCK");
  delay(4000);
  lcd.setCursor(0, 0);
  lcd.print("DOOR IS LOCKED: ");
  lcd.setCursor(0, 1);
  lcd.print("CLK * TO GET OTP");
  delay(3000);

}

void loop() {
  // put your main code here, to run repeatedly:

  key = myKeypad.getKey();


  if (key == '*') {

    otp = random(100000, 999999);
    sprintf(mainPassword, "%lu", otp);
    Serial.println(mainPassword);
    
    msg1();
    
    lcd.setCursor(0, 0);
    lcd.print("OTP SENT 2 PHONE");
    lcd.setCursor(0, 1);
    lcd.print("ENTER OTP:      ");
    key = 0;
    flag=1; 

  }

  if ((flag==1) && (key)) {
    if (key != '*')  {

      lcd.setCursor(count + 10, 1);
      lcd.print('*');
      inputPassword[count] = key;
      count++;
    }
  }


  if (count == passwordSize - 1) {

    if (strcmp(mainPassword, inputPassword) == 0) {

      digitalWrite(relay, HIGH);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("PASSWORD MATCHED");
      lcd.setCursor(0, 1);
      lcd.print("DOOR UNLOCKED");
      delay(5000);
      digitalWrite(relay, LOW);

      lcd.clear(); 
      lcd.setCursor(0, 0);
      lcd.print("DOOR IS LOCKED: ");
      lcd.setCursor(0, 1);
      lcd.print("CLK * TO GET OTP");

    }

    else {

      lcd.setCursor(0, 0);
      lcd.print("PASS MISSMATCHED");
      lcd.setCursor(0, 1);
      lcd.print("TRY AGAIN       ");
      
      msg2();
      flag=0;
      delay(3000);
      
      otp = random(100000, 999999);
      sprintf(mainPassword, "%lu", otp);
      lcd.clear(); 
      lcd.setCursor(0, 0);
      lcd.print("DOOR IS LOCKED: ");
      lcd.setCursor(0, 1);
      lcd.print("CLK * TO GET OTP");

    }
    
    count = 0;
  }


}

//  otp = random(100000, 999999);
//  sprintf(mainPassword, "%lu", otp);
//  Serial.print("Your main password is: ");
//  Serial.println(mainPassword);
//  delay(1000);



void msg1 () {

  delay(500);

  gsm.println("AT+CMGF=1");
  checkSerial();

  gsm.println("AT+CMGS=\"+8801988448287\""); // Set Destination Phone Number
  checkSerial();

  gsm.print("Your Door Lock OTP is : "); // Set Message Content
  gsm.println(mainPassword); // Set Message Content
  checkSerial();

  gsm.write(26);

}

void msg2 () {

  delay(500);

  gsm.println("AT+CMGF=1");
  checkSerial();

  gsm.println("AT+CMGS=\"+8801988448287\""); // Set Destination Phone Number
  checkSerial();

  gsm.print("Warning! Incorrect Password Entered at the Door Lock."); // Set Message Content
  checkSerial();

  gsm.write(26);

}

void checkSerial() {

  delay(500); // Used to ensure enough lag time between the at commands

  //  while (Serial.available())
  //    gsm.write(Serial.read());

  while (gsm.available())
    Serial.write(gsm.read());

}
