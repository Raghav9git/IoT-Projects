  #include<Servo.h>
  #include<DHT.h>              // for DHT-11 sensor
  #include<SoftwareSerial.h>   // to send data from DHT-11 to mobile

  SoftwareSerial BT(10,11);    // RX and TX pins 

  Servo myservo;

  int irPin = 7;
  int phPin = A0;
  int stat = 0;    // for ir
  int angle;
  String input = "";     // for receiving any value of servo form mobile app

  #define DHTPIN1 9       // for DHT-11 temp-humidity sensor
  #define DHTPIN2 11
  #define DHTTYPE DHT11

  DHT dht1(DHTPIN1,DHTTYPE);
  DHT dht2(DHTPIN2,DHTTYPE);

  void setup()
  {
    myservo.attach(4);
    myservo.write(0);           // start with curtain closed

    pinMode(7,INPUT);
    pinMode(A0,INPUT);

    dht1.begin();
    dht2.begin();

    Serial.begin(9600);
    BT.begin(9600);      // for sending data from DHT-11 to mobile via bluetooth
   
  }

  void loop()
  {

    bool openCurtain = false;             // false for curtain to stay closed as default
                                          // used bool so that input from all sensors don't contradict and give chaotic output
  // openCurtain just checks the conditions of true or false,
  // curtain is controlled using myservo at the end of all logic

    // photoresistor

    Serial.println(analogRead(A0));
    delay(500);
    if(analogRead(A0) > 700)
    {
      openCurtain = true;
    }
    else if(analogRead(A0) < 100)
    {
      openCurtain = false;
    }
    
    
    // Infrared Sensor 

    stat = digitalRead(irPin);
    if(stat == HIGH)
    {
      openCurtain = true;                  // LOW state not mentioned due to default closed setting
    }
    

    // DHT-11 temp-humidity sensor inside room 

    float temp1 = dht1.readTemperature(); // read temp value from sensor in celsius
    float hum1 = dht1.readHumidity();     // read humidity value from sensor in percentage

    if(isnan(temp1) && isnan(hum1))       // checks wheather the read value is valid or not , isnan = is not a number
    {
      Serial.println("Failed to read from sensor!");
      return;
    }

    if (temp1 > 26 || hum1 < 30 || (temp1 > 26 && hum1 > 60))
    {
      openCurtain = true;                             // hum1<30 dry weather
    }                                                 // hum1>60 humid weather


    // for mobile app connected via bluetooth

    if (BT.available()) 
    {
    String BTinput = BT.readStringUntil('\n');       //This command reads the full line at once, storing it in the variable input as a String.
    BTinput.trim();                                    // trims the unwanted spaces,tabs,newlines
    if(BTinput == "o")
    {
      openCurtain = true;
    }
    else if(BTinput == "c")
    {
      openCurtain = false;
    }
    else
    {
      int angle = BTinput.toInt();      // toInt() is a function to convert received string from mobile into integer
      if(angle >= 0 && angle <= 180)  // angle for sliding bar in mobile app
      {
        myservo.write(angle);
        delay(2000); 
        return;
      }
    }
  }


    // 7. Apply final decision
    if (openCurtain) 
    {
      myservo.write(180);
    }  
    else 
    {
      myservo.write(0);
    }  

  

    // DHT-11 temp-humidity sensor outside room to send data to mobile device
    float temp2 = dht2.readTemperature(); // read temp value from sensor in celsius
    float hum2 = dht2.readHumidity();     // read humidity value from sensor in percentage

    if(isnan(temp2) || isnan(hum2))
    {
      Serial.println("Failed to read from sensor!");
      return;
    }
    else
    {
      BT.print("temperature outside  : ");
      BT.println(temp2);
      BT.print("humidity outside  : ");
      BT.println(hum2);
      delay(2000);
    }
  
}

    
