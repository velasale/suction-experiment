/*!
   @file mprls_simpletest.ino

   A basic test of the sensor with default settings

   Designed specifically to work with the MPRLS sensor from Adafruit
   ----> https://www.adafruit.com/products/3965

   These sensors use I2C to communicate, 2 pins (SCL+SDA) are required
   to interface with the breakout.

   Adafruit invests time and resources providing this open source code,
   please support Adafruit and open-source hardware by purchasing
   products from Adafruit!

   Written by Limor Fried/Ladyada for Adafruit Industries.

   MIT license, all text here must be included in any redistribution.

*/

const bool  USE_ROSSERIAL = true;
const uint8_t NUM_CUPS = 3;          /*Number of suction cups*/

#include <Wire.h>
#include <time.h>
#include "Adafruit_MPRLS.h"

//You don't *need* a reset and EOC pin for most usees, so we set to -1 and don't connect
#define RESET_PIN -1
#define EOC_PIN   -1
#define VALVE 13
Adafruit_MPRLS mpr = Adafruit_MPRLS(RESET_PIN, EOC_PIN);


unsigned long currentMillis;


/*************************** Base ROS Setup ******************************/
// General ROS packages/nodes
#include <ros.h>
ros::NodeHandle nh;





/*************************** ROS Services Setup **************************/
#include <std_srvs/Trigger.h>
void closeValveService(const std_srvs::Trigger::Request &req,
                       std_srvs::TriggerResponse &res) {
  res.success = closeValve();
}
ros::ServiceServer<std_srvs::Trigger::Request,
    std_srvs::Trigger::Response>
    service_close("closeValve", &closeValveService);


void openValveService(const std_srvs::Trigger::Request &req,
                      std_srvs::TriggerResponse &res) {
  res.success = openValve();
}
ros::ServiceServer<std_srvs::Trigger::Request,
    std_srvs::Trigger::Response>
    service_open("openValve", &openValveService);




/*************************** ROS Publishers Setup **************************/
#include <std_msgs/Float32.h>
#include <std_msgs/UInt16.h>

//std_msgs::Float32 press_msg;
//ros::Publisher publisher_pressure("/gripper/pressure", &press_msg);

std_msgs::UInt16 press_msg[NUM_CUPS];
ros::Publisher publisher_pressure[NUM_CUPS] {
  ros::Publisher("/gripper/pressure/sc1", &press_msg[0]),
  ros::Publisher("/gripper/pressure/sc2", &press_msg[1]),
  ros::Publisher("/gripper/pressure/sc3", &press_msg[2])
};




// Constants
const int PCAADR = 0x70;
//const int sensorAddress = 0x18;
const int VALVE_DELAY = 10;

// Variables
long publisher_timer = 10;
uint16_t pressure_hPa;


/**************************** Setup ***************************************/
void setup() {
  // initialize serial:
  Serial.begin(115200);     // 57600, 76800, 115200

  // Initialize VALVE pin as output
  pinMode(VALVE, OUTPUT);

  // Initialize pressure sensor
  mpr.begin();

  // Initialize ROS stuff
  if (USE_ROSSERIAL) {

    // Initialize ROS node
    nh.getHardware()->setBaud(57600);     //Needs to match the one set in the PC's launch file
    nh.initNode();

    // Initialize ROS publishers
    for (uint8_t i = 0 ; i < NUM_CUPS; i++) {
      nh.advertise(publisher_pressure[i]);
    }

    // ROS services
    nh.advertiseService(service_open);
    nh.advertiseService(service_close);
  }

  digitalWrite(VALVE, LOW);

}


/***************************** Loop ****************************************/
void loop() {

  //  TODO Parallel

  if (USE_ROSSERIAL) {
    nh.spinOnce();
  }

  for (uint8_t i = 0; i < NUM_CUPS; i++) {

    currentMillis = millis();

    pcaselect(i);
    pressure_hPa = mpr.readPressure();
    press_msg[i].data = pressure_hPa;


    if (USE_ROSSERIAL) {
      publisher_pressure[i].publish(&press_msg[i]);
    }
    else {
      //Print the time in ms just to have an idea of long the sensor takes to measure press.
      Serial.print(millis() - currentMillis);
      Serial.print(" ");

      Serial.print(press_msg[i].data);
      Serial.print(" ");
    }

  }

  if (!USE_ROSSERIAL) {
    Serial.println("\n");
  }

}

/****************************** Control Functions ***************************/
bool closeValve() {
  bool success = true;
  delay(VALVE_DELAY);
  digitalWrite(VALVE, LOW);
  delay(VALVE_DELAY);
  return success;
}


bool openValve() {
  bool success = true;
  delay(VALVE_DELAY);
  digitalWrite(VALVE, HIGH);
  delay(VALVE_DELAY);
  return success;
}

/**************************** I2C Multiplexer *********************************/
// Reference: https://learn.adafruit.com/adafruit-pca9546-4-channel-i2c-multiplexer/arduino
void pcaselect(uint8_t i) {
  if (i > 3) return;

  Wire.beginTransmission(PCAADR);
  Wire.write(1 << i);
  Wire.endTransmission();
}