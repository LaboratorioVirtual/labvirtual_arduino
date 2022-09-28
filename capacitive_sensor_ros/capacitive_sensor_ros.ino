#include <ros.h>
#include <labvirtual_messages/Sensor5TE.h>
#include <labvirtual_messages/Sensor5TEArray.h>

// ----------------------
const int AirValue = 620;
const int WaterValue = 310;
int soilMoistureValue = 0;
int soilmoisturepercent = 0;
// ----------------------
float umidade = 0, temperatura = 0, condutividade = 0;
// ----------------------

ros::NodeHandle nh;

labvirtual_messages::Sensor5TE sensor;
labvirtual_messages::Sensor5TEArray sensores;
ros::Publisher pub_arduino_data("arduino_data", &sensores);

void ler_sensor() {
  soilMoistureValue = analogRead(A0);
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
  if (soilmoisturepercent >= 100) {
    soilmoisturepercent = 100;
  } else if (soilmoisturepercent <= 0) {
    soilmoisturepercent = 0;
  } else {
    soilmoisturepercent = soilmoisturepercent;
  }

  sensor.id = 0;
  sensor.umidade = soilmoisturepercent;
  sensor.temperatura = 32.0;
  sensor.condutividade = 12.0;
  sensores.sensors[0] = sensor;
  
  sensor.id = 1;
  sensor.umidade = 12.6;
  sensor.temperatura = 23.0;
  sensor.condutividade = 21.0;
  sensores.sensors[1] = sensor;

  pub_arduino_data.publish(&sensores);
}

void setup() {
  nh.initNode();
  nh.advertise(pub_arduino_data);
}

long publisher_timer;

void loop() {
  if (millis() > publisher_timer) {
    ler_sensor(); 

    publisher_timer = millis() + 60000;
  }

  nh.spinOnce();
}
