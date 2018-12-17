#include <SPI.h>
#include <AIR430BoostFCC.h>
#include <aJSON.h>
#include <LCD_Launchpad.h>


const char analogPin = 23;
const char* Nom = "Amal";
const char* Capteur = "Light";
LCD_LAUNCHPAD myLCD;


void scrollText(String text)
{
  Serial.println("Scrolling Text to terminal & LCD:");
  myLCD.displayText(text);
  Serial.println(text);
  delay(400);
  while(text != ""){
    myLCD.clear();
    text = text.substring(1);
    Serial.println(text);
    myLCD.displayText(text);
    delay(175);
  }
}

void setup()
{

  Serial.begin(9600);
  Serial.println("\n");

  // setup initial address, channel, and TX power.
  Radio.begin(0x01, CHANNEL_1, POWER_MAX);

  // Configure the on-board LCD of the MSP430FR6989
  myLCD.init();

;

  // Print sensor type to the LCD screen
  String sensorType = String(Capteur)+" sensor";
  scrollText(sensorType);

  // Configure RED_LED, for  notification of RF TX
  pinMode(RED_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);   // set the LED off
  Serial.println("Radio started! Setup complete.");
}

void loop()
{
  //Blink LED & TX segment on LCD to signify start of new sensor reading + RF TX
  digitalWrite(RED_LED, HIGH);
  myLCD.showSymbol(LCD_SEG_TX, 1);

  // Encode sensor readings into JSON
  /*Desired JSON encoded format:
  {
    "d":{
      "Name":"  ",
      "Sensor":"  ",
      "Data": 1234
    }
  }
  */
  aJsonObject *msg = aJson.createObject();
  aJsonObject *d = aJson.createObject();
  aJson.addItemToObject(msg, "d", d);
  aJson.addStringToObject(d, "Name", Nom);
  aJson.addStringToObject(d, "Sensor", Capteur);
  int sensorValue = analogRead(analogPin);
  aJson.addNumberToObject(d, "Data", sensorValue);

  char* payload = aJson.print(msg);
  aJson.deleteItem(msg);
  Radio.transmit(ADDRESS_BROADCAST, (uint8_t*)payload, 60);

  // Print latest sensor readings to LCD
  myLCD.displayText("    ");
  myLCD.displayText(String(sensorValue));

  // Print JSON-encoded payload to terminal, then free char array
  Serial.print("TX (DATA): ");
  Serial.println(payload);
  free(payload);

  // Transmission success! Toggle off LED & clear TX segment on LCD
  digitalWrite(RED_LED, LOW);
  myLCD.showSymbol(LCD_SEG_TX, 0);

  // Go to LPM3 for 1 second
  //sleepSeconds(1);
  delay(250);
}
