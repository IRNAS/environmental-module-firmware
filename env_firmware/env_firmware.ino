// Modules
#include "can_module.h"
#include "Sensor.h"
#include "global.h"
#include <STM32L0.h> 
#include "TimerMillis.h"

// LED light error status
// Fast blinking - CAN error
//##########################################
// Uncomment appropriate line based on the number on your module. For example for device with number 1, uncomment line #define TOP1 and comment all the others. 
// Also, change the number for SET_CAN_ID in the can_module.h accordingly. I.e. for module 1, set #define SET_CAN_ID 0x100, for module 2 set it to #define SET_CAN_ID 0x200.
#define TOP1
//#define MID2
//#define MID3
//#define BOTTOM4

//##########################################
// Sensor objects
Sensor *s_L0 = new mySensor<L0>(1);
Sensor *s_TSL = new mySensor<TSL2591_sensor>(2);
Sensor *s_BME = new mySensor<BME>(3);
Sensor *s_ANEMOMETER = new  mySensor<ANEMOMETER>(4, A4); // Pin PA0, Analog channel 4, enable pin 25
Sensor *s_RAIN = new mySensor<RAIN>(5, 4);
Sensor *s_CO2 = new mySensor<CO2>(6);
Sensor *s_TDR = new mySensor<TDR>(7,19);
//note there is a flaw in implementation where TDR, OneW_TMP and ANALOG need to be called at the same interval as they share a common power enable pin
Sensor *s_BAT = new mySensor<BAT>(8);
Sensor *s_OneW_TMP = new mySensor<OneW_TMP>(9);
Sensor *s_A0 = new mySensor<ANALOG>(10,A4); // Pin PA0, Analog channel 4
Sensor *s_A1 = new mySensor<ANALOG>(11,A1); // Pin PA5, Analog channel 1

#ifdef TOP1
  Sensor *sensors[] = {s_L0, s_TSL, s_BME, s_CO2, s_RAIN, s_ANEMOMETER}; 
  int n_sensors = 6; //Change number of sensors
#endif

#ifdef MID2
  Sensor *sensors[] = {s_L0, s_TSL, s_BME};
  int n_sensors = 3; //Change number of sensors
#endif

#ifdef MID3
  Sensor *sensors[] = {s_L0, s_TSL, s_BME};
  int n_sensors = 3; //Change number of sensors
#endif

#ifdef BOTTOM4
  Sensor *sensors[] = {s_L0, s_TSL, s_BME, s_BAT, s_TDR, s_A0, s_A1, s_OneW_TMP};
  int n_sensors = 8; //Change number of sensors
#endif

TimerMillis watchdog;

void callbackWatchdog(void)
{
  STM32L0.wdtReset();
}

//CAN module
CAN_MODULE module_CAN(SET_CAN_ID);//this does not work

int active_can_id[8] = {0};
uint8_t gpio_sharing_counter = 0;
char active_sensor_addresses[64];
char sensor_exect_timer_counter = 0;

void setup() {

  // setup serial
  #ifdef debug
    serial_debug.begin(serial_debug_speed);
  #endif

  pinMode(22, OUTPUT);
  digitalWrite(22, LOW);
  STM32L0.wdtEnable(18000);
  watchdog.start(callbackWatchdog, 0, 8500);

  // setup all modules
  if(!module_CAN.setup()) {
    #ifdef debug
      serial_debug.println("CAN init - failed.");
      while(1){
      digitalWrite(22, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(100);                       // wait for a second
      digitalWrite(22, LOW);    // turn the LED off by making the voltage LOW
      delay(100);                       // wait for a second
        }
    #endif
  } else {
    #ifdef debug
      serial_debug.println("CAN init - success.");
    #endif
  }
  // setup all the sensors
  for(int i=0; i<n_sensors; i++)
  {
    // setup sensor
    device_setup(sensors[i]);
    active_can_id[i] = sensors[i]->getCANidValue();
    // clear data
    sensors[i]->clearData();
  }
  digitalWrite(22, HIGH);
} // end of setup

void loop() {
  // EXEC sending, we received something on can
  if(module_CAN.return_local_send_data_int()>0) {
    //clean up the indicator to process
    module_CAN.execute_int_can = false;

    #ifdef debug
      serial_debug.print("loop() - CAN_RXID chosen (ID): ");
      serial_debug.println(module_CAN.return_CAN_RXID(), HEX);
    #endif

    //get the sensor number without the evice prefix
    int global_id = module_CAN.get_sensor_CAN_id(module_CAN.return_CAN_RXID());
    int sensor_idx = sensor_lookup(global_id); //Find position of the specific sensor

    // check if the sensor is present on the device  
    if(sensor_idx<0){
       #ifdef debug
          serial_debug.println("loop() - sensor not found");
       #endif
       module_CAN.send_empty();
    }
    //check if the sensor is present on the device but not active
    else if(sensors[sensor_idx]->getCANid()==0){
      #ifdef debug
          serial_debug.println("loop() - CAN message empty");
       #endif
      module_CAN.send_empty();
    }
    else{
      // optional dump of sensor data
      //sensors[sensor_idx]->printData();

      //check if scan request or data request
      if(module_CAN.return_local_send_data_int()==1){
        //scan request
        #ifdef debug
          serial_debug.println("loop() - CAN message present");
        #endif
        module_CAN.send_present();
      }
      //check if reset request or data request
      else if(module_CAN.return_local_send_data_int()==2){
        //scan request
        #ifdef debug
          serial_debug.println("loop() - CAN message reset");
        #endif
        reset_devices();
      }
      //otherwise send the reply with data
      else{
        //send data
        if(send_data(sensors[sensor_idx])){
  
          #ifdef debug
            serial_debug.println("loop() - CAN message sent, clearing...");
          #endif
  
          // clear data
          sensors[sensor_idx]->clearData();
  
        }
        else{
          #ifdef debug
            serial_debug.println("loop() - CAN message ERROR, not clearing...");
          #endif
        }
      }
    }

    module_CAN.execute_int_can = true;
    module_CAN.set_local_send_data_int(0);

  } else if(module_CAN.return_local_send_data_int() == 0 && module_CAN.execute_int_can == true) {
      if(sensor_exect_timer_counter < n_sensors){
        if(sensors[sensor_exect_timer_counter]->getCANid() != 0)
        {
          sensors[sensor_exect_timer_counter]->execTimer();
        }
        sensor_exect_timer_counter++;
        }
       else{
        sensor_exect_timer_counter=0;
        // sleep devices
        digitalWrite(22, LOW);
        sleep_devices();
        digitalWrite(22, HIGH);
        }
    } // end of if
} // end of loop

/*
 *  Function:     void device_setup(bool status, String name)
 *  Description:  helper function for setting up device
 */
void device_setup(Sensor *s)
{
  #ifdef debug
      serial_debug.print("device_setup() - device ");
      serial_debug.print(s->getName());
  #endif

  // if we get a false status (not inited)
  if(!s->setup()) {

    #ifdef debug
      serial_debug.println(" - failed");
    #endif
    // Set sencor CAN ID to 0, meaning it is not initialized
    s->setCANid(0);

    //while(1); // blocks firmware indefinitely, testing purpose only
  } else {
    module_CAN.set_sensor_CAN_id(s->getCANid(),s->getGlobalId()); // set-up can to listen
    #ifdef debug
      serial_debug.print(" can id 0x");
      serial_debug.print(s->getCANid(),HEX);
      serial_debug.println(" -  inited!");
    #endif

  }
} // end of device_setup

/*
 *  Function:     void sensor_read(bool status)
 *  Description:  helper function for reading the sensor
 */
void device_read(Sensor *s) {
  #ifdef debug
      serial_debug.print("device_read() - device ");
      serial_debug.print(s->getName());
  #endif

  // if we get a false status (didnt read it good)
  if(!s->sensorRead()) {
    #ifdef debug
      serial_debug.println(" - failed.");
    #endif
  } else {
    #ifdef debug
      serial_debug.println(" - success.");
    #endif
  }

} // end of device_read

bool send_data(Sensor *s){

  byte data[s->getNumberOfVariables()][COL_LENGTH][8];
  byte time_data[COL_LENGTH][8];
  s->updateValues(data, time_data);

  //serial_debug.println("send_data()");
  /*print_debug(
    data,
    time_data,
    s->getNumberOfVariables(),
    s->getCounterCol(),
    s->getCounterRow(),
    s->getCounterColOverflow()
  );*/

  bool status_sent = module_CAN.send_data(
    data,
    time_data,
    s->getNumberOfVariables(),
    s->getCounterCol(),
    s->getCounterRow(),
    s->getCounterColOverflow(),
    s->getLastTime()
  );
  return status_sent;
}

/*
 * Find sensor position in the sensor array with the specified global id idx.
 */
int sensor_lookup(int idx)
{
  int pos = -1;
  for(int i=0; i<n_sensors; i++){
    if(sensors[i]->getGlobalId() == idx)
    {
      pos = i;
      break;
    }
  }
  return pos;
}

void sleep_devices() {

  #ifdef debug
    serial_debug.println("sleep_devices() - sleeping for 15s");
  #endif
  
  //backup deactivate
  gpio_sharing_counter=0;//decrement upon deactivation
  digitalWrite(19, gpio_sharing_counter);

  //module_CAN.enable_sleep();
  // STM32L0
  STM32L0.stop(15 * 1000);

}

void reset_devices() {
  #ifdef debug
    serial_debug.println("reset_devices() - full system reset");
  #endif
  delay(5000);
  STM32L0.reset();
}
