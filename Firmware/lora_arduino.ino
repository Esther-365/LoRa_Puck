#include <RadioLib.h> //Library for the radio setup 
#include <SPI.h>

//RP2040 pins--------GPIO
const int LED_PIN = 16; //Led pin 
const int NSS_PIN = 17;
const int CLK_PIN = 18;
const int MOSI_PIN = 19;
const int MISO_PIN = 20;
const int NRST_PIN = 22;
const int BUSY_PIN = 23;
const int DIO1_PIN = 24;


//LoRa Settings, must be identical on transmitter and receiver
/*const float LORA_FREQ_MHZ = 915.0;
const float LORA_BW_KHZ = 125.0; //BW - BandWidth. longer communication range, higher receiver sensitivity, but slower data rate of transmission
const int LORA_SF = 9; //SF - Spreading Factor. check notes
const int LORA_CR = 7;//CR - Coding Rate 
const int LORA_SYNC_WORD = 0x12; //private network
const int LORA_TX_DBM = 17;
const int LORA_PREAMBLE = 8;*/
 
String message;
//https://github.com/jgromes/RadioLib/blob/master/examples/SX126x 
SX1262 radio = new Module(NSS_PIN, DIO1_PIN, NRST_PIN, BUSY_PIN,SPI);
//The RP2040 has SPI(SPI0) and SPI1, we are letting the radio know that we are using SPI0 to communicate. In cases where there aren't multiples, you prolly won't need to do that.  

void radioBegin()
{
//I'm setting this manually because the pins I used differs from the Arduino's defaults.
  //check c:\Users\PC\AppData\Local\Arduino15\packages\rp2040\hardware\rp2040\6.3.2\variants\rpipico\pins_arduino.h for the defaults.
  SPI.setSCK(CLK_PIN);
  SPI.setTX(MOSI_PIN); //MOSI
  SPI.setRX(MISO_PIN); //MISO
  SPI.begin(); //initializes the RP2040 SPI0 controller
}

//int state = radio.begin(LORA_FREQ_MHZ, LORA_BW_KHZ, LORA_SF, LORA_CR, LORA_SYNC_WORD, LORA_TX_DBM, LORA_PREAMBLE, 0.0 );//My project uses a crystal not a TXCO hence the 0.0 // you can use the ConfigLoRa_t approach for this as well, it's brief and takes less time writing.
ConfigLoRa_t config;
void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT); 
  digitalWrite(LED_PIN, LOW); //Set it to low as default, a successful message will change it to HIGH and light the LED.

  radioBegin();
  radio.tcxoVoltage = RadioLibTCXOVoltage_t::VoltageNone; //My project uses a crystal not a TXCO

  config.frequency = 915;
  //config.power = 17;

  int state = radio.begin(config); 

  if (state == RADIOLIB_ERR_NONE) 
  {
    Serial.println(F("success! Radio initialized"));
  } 
  else 
  {
    Serial.print(F("Radio initialization failed "));
    Serial.println(state);
    while (true) { delay(100); }
  }
  
  radio.setDio2AsRfSwitch(true); //configures the SX1262 to the PE4259(RF switch) using DIO2

  state = radio.transmit("Hello Peeps"); 

  if (state != RADIOLIB_ERR_NONE)
  {
    Serial.println("First transit failed, trying again");
    delay(500);

    state = radio.transmit("Hello Peeps"); 

    if(state == RADIOLIB_ERR_NONE)
    {
      Serial.println("Transmission successfuk");
    }

    else
    {
      Serial.print(F("Transmit failed after retry, error: "));
      Serial.println(state);
      while(true){delay(100);}
    }
  }
  else
  {
    Serial.println("Transmission successful");
  }
}

void loop() 
{
  // put your main code here, to run repeatedly:
  if (Serial.available() == 0)
  {
    return;
  }

  message = Serial.readStringUntil('\n');
  message.trim();

  if(message.length() > 0)
  {
  //radio.transmit expects a uint8_t, .c_str gives const char, we parse it.
  //int state = radio.transmit(byteBuffer, sizeof(byteBuffer));
  int state = radio.transmit(reinterpret_cast<const uint8_t*> (message.c_str()), message.length());

  if (state == RADIOLIB_ERR_NONE)
  {
    digitalWrite(LED_PIN,HIGH); //indicates that the message was successfully transmitted

    for (int i = 0; i < 2; i++) 
    {
      //2 quick flashes to indicate that it has finished transmitting
      digitalWrite(LED_PIN,HIGH);
      delay(100);
      digitalWrite(LED_PIN,LOW);
      delay(100);
    }   
  }
  else
  {
    Serial.print("Transmission failed, error: ");
    Serial.println(state);
  }

  Serial.println("Ready for another message, waazzaa");
  }
}

//I don't have the other LoRa for receiving so for now it's just transmitting.
