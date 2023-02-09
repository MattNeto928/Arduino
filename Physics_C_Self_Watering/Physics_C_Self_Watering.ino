int sensor = A0;
int pumpPin = 11;
int moistureThreshold = 500;
int moistureLevel;

void setup() {
  Serial.begin(9600);                     // Initializing Serial port for printing data
  pinMode(pumpPin, OUTPUT);               // Initializing output pin for pump
}

void loop() {
  moistureLevel = analogRead(sensor);     // Getting moisture value from sensor
  Serial.println(moistureLevel);          // Printing out moisture

  if(moistureLevel > moistureThreshold)   // Turning on/off pump if there the soil is not moist/moist enough, respectively
  {  
    pulseWater();
  }
   else
   {
    digitalWrite(pumpPin, LOW);
    Serial.println("NOT PUMPING");
   }

  delay(250);                            // Delaying loop by 5 seconds
}

void pulseWater()
{
  Serial.println("PUMPING");
  digitalWrite(pumpPin, HIGH);
  delay(1500);
  digitalWrite(pumpPin, LOW);
  delay(3000);
}
