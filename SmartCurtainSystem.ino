// Smart curtain system

/*1)opens up when sunlight is high in morning
  2)opens up during low humidity in room
  3)opens up during high temp in room
  4)opens when person is close
  5)closes when person is away
  6)can be controlled manually by mobile app anytime we want
  7)if outside DHT reads cold,by app give status and button to open 
  8)if outside DHT reads hot, by app give status and button to close
  9) closes when sunlight is null in night
  
  Sensors and components
  ir sensor - 4 , 5
  photoresistor - 1 , 9
  inside DHT-11 sensor - 2 , 3 
  outside DHT-11 sensor - 7 , 8
  bluetooth module 6 , 7 , 8
  servo - all
  */



  #include<Servo.h>
  #include<DHT.h>              // for DHT-11 sensor
  #include<SoftwareSerial.h>   // to send data from DHT-11 to mobile
  SoftwareSerial BT(10,11);    // RX and TX pins 
  Servo myservo;
  int irPin = 7;
  int phPin = A0;
  int stat = 0;    // for ir
  char val;        // for bluetooth
  int angle;
  String input = "";     // for receiving any value of servo form mobile app

  #define DHTPIN1 9       // fpr DHT-11 temp-humidity sensor
  #define DHTPIN2 11
  #define DHTTYPE DHT11

  DHT dht1(DHTPIN1,DHTTYPE);
  DHT dht2(DHTPIN2,DHTTYPE);

  void setup()
  {
    myservo.attach(4);
    pinMode(7,INPUT);
    pinMode(A0,INPUT);
    dht1.begin();
    dht2.begin();
    Serial.begin(9600);
    BT.begin(9600);      // fro sending data from DHT-11 to mobile via bluetooth
    myservo.write(0);
  }

  void loop()
  {

    // photoresistor

    Serial.println(analogRead(A0));
    delay(500);
    if(analogRead(A0) > 700)
    {
      myservo.write(180);
    }
    else if(analogRead(A0) < 100)
    {
      myservo.write(0);
    }
    else
    {
      myservo.write(90);
    }
    
    // Infrared Sensor 

    stat = digitalRead(irPin);
    if(stat == HIGH)
    {
      myservo.write(180);
    }
    else
    {
      myservo.write(0);
    }

    // DHT-11 temp-humidity sensor inside room to move curtain

    float temp1 = dht1.readTemperature(); // read temp value from sensor in celsius
    float hum1 = dht1.readHumidity();     // read humidity value from sensor in percentage

    if(isnan(temp1) || isnan(hum1))       // checks wheather the read value is valid or not
    {
      Serial.println("Failed to read from sensor!");
      return;
    }

    if(temp1 > 26 && hum1 > 60)
    {
      myservo.write(180);
    }
    else
    {
      myservo.write(0);
    }
    delay(2000);

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
    }
    delay(2000);

    // for mobile app connected via bluetooth

    while(Serial.available() > 0)
    {
      val = Serial.read();
      angle = Serial.read();
      Serial.println(val);
    }
    if(val == 'o')
    {
      myservo.write(180);
    }
    else if(val == 'c')
    {
      myservo.write(0);
    }
    
    int angle = input.toInt();      // toInt() is a function to convert received string from mobile into integer
    if(angle >= 0 && angle <= 180)
    {
      myservo.write(angle);
    }
  }


