#include "CO2.h"
#include "global.h"

#include <Arduino.h>

SCD30 airSensor;
ModbusMaster node;

/*
 *  Function:       bool CO2::setup()
 *  Description:    setup the serial for the CO2
 */
bool CO2::setup() {
    CO2_SERIAL.begin(CO2_SERIAL_BAUD);
    bool value_updated = false;
    String recv_string = "";

    // check if CO2 sensor is sending data
    CO2_SERIAL.flush();//clear buffer, wait max 400ms for fresh values
    unsigned long timeout = millis()+600;
    while(millis()<timeout ) {
      if(CO2_SERIAL.available() > 0){
        int inChar = CO2_SERIAL.read();
        #ifdef debug
          serial_debug.write(inChar);
        #endif
        if(inChar == '\n' || inChar == 'Z') {
            recv_string = "";
        }

        if(isDigit(inChar)) {
            recv_string += (char)inChar;
        }

        if(inChar == 'z' && recv_string.length()==5) {
            current_co2 = (recv_string.toInt() * 10);
            value_updated = true;
            co2_type=0;
        }
      }
    }

    //SCD30 i2c
    if(value_updated == false){
      Wire.begin();
      Wire.beginTransmission(0x61);
      uint8_t dummy = Wire.endTransmission();
      if(dummy==0){
        airSensor.begin(); //This will cause readings to occur every two seconds
        value_updated = true;
        co2_type=1;
      }
    }
    //SCD30 UART
    if(value_updated == false){
      uint16_t result;
      CO2_SERIAL.begin(CO2_SERIAL_BAUD_SCD30);
      node.begin(0x61, CO2_SERIAL);
     //****************Set measurement interva**************//
      result = node.writeSingleRegister(0x0025, 0x00002);     // 2s interval
      if (result > 0) {
        serial_debug.println("0x0025 NACK");
      }
      delay(100);
    
      //****************Altitude Compensation**************//
      //result = node.writeSingleRegister(0x0038, 0);
      //*************Trigger continuous measurement******************//
      result = node.writeSingleRegister(0x0036, 0x0000);
      if (result > 0) {
        serial_debug.println("0x0036 NACK");
      }
      if(result==0){
        airSensor.begin(); //This will cause readings to occur every two seconds
        value_updated = true;
        co2_type=2;
      }
    }
    return value_updated;
} // end of setup()

/*
 *  Function:       bool CO2::read()
 *  Description:    read the sensors
 */
bool CO2::read() {
    if( read_CO2() == true) {
        return true;
    } else {
        return false;
    }
} // end of read()

/*
 *  Function:       void CO2::print_data()
 *  Description:    print the data
 */
void CO2::print_data() {

    print_debug(data,                               // data where we store values
                time_data,                          // where we store time data
                CO2_num_of_variables,           // number of sensor we use
                counter_col,                        // counter for coloumn index
                counter_row,                        // counter for row index
                counter_col_overflow);              // flag for coloumn counter overflow

} // end of print_data()

/*
 *  Function:    CO2::exec_timer()
 *  Description: execute read if the timer is right
 */
bool CO2::exec_timer() {

    if(millis() - exec_timer_last >= exec_time) {
        //skip if not initialized
        if(CAN_ID==0){
           #ifdef debug
                serial_debug.println("CO2 (exec_timer) - no init, skipping");
            #endif
          return false;
        }
        
        if(read()) {
            //print_data();
            #ifdef debug
                serial_debug.println("CO2 (exec_timer) - executed read");
            #endif

            exec_timer_last = millis();

            return true;
        }
        return false;
        
    }

    return false;
} // end of exec_timer()

/*
 *  Function:       bool CO2:read_CO2()
 *  Description:    read the CO2 through serial
 */
bool CO2::read_CO2() {
    String recv_string = "";
    bool value_updated = false;
    unsigned long timeout = millis()+26000;

    serial_debug.println(millis());
    serial_debug.println(timeout);
    serial_debug.println(value_updated);
    
    if(co2_type==0){
      CO2_SERIAL.flush();//clear buffer, wait max 400ms for fresh values
      // Z ##### z #####/r/n
      while(millis()<timeout ) {
        if(CO2_SERIAL.available() > 0){
          int inChar = CO2_SERIAL.read();
  
          if(inChar == '\n' || inChar == 'Z') {
              recv_string = "";
          }
  
          if(isDigit(inChar)) {
              recv_string += (char)inChar;
          }
  
          if(inChar == 'z' && recv_string.length()==5) {
              current_co2 = (recv_string.toInt() * 10);
                                              // updating the data into our data array
              update_16bit(data,              // our data array
              time_data,                      // our time data array
              id_co2,                         // pressure ID
              counter_col,                    // coloumn counter
              counter_row,                    // row counter
              counter_col_overflow,           // check if coloumn has overflow 
              CO2_num_of_variables,           // number of variables
              current_co2,                    // the value
              exec_timer_last);               // the last time
              
              value_updated = true;
              break; // exit while loop when reading the measurement
          }
        }
      }
    }
    else if(co2_type==1){
      while(millis()<timeout ) {
        serial_debug.print("read_CO2_i2c"); 
        if(airSensor.dataAvailable()){
          current_co2=airSensor.getCO2();
          value_updated = true;
          break;
        }
        delay(5000);
      }
    }
    else if(co2_type==2){
      double CO2;
      double TEMP;
      double RH;
      
      unsigned int tempU32;
      while(millis()<timeout ) {
        //*************Get data ready status******************//
        node.readHoldingRegisters(0x0027, 0x0001);
        if (node.getResponseBuffer(0x00) == 1) {
          //*************Read measurement******************//
          node.readHoldingRegisters(0x0028, 0x0006);
          tempU32 = (unsigned int)(node.getResponseBuffer(0x00) << 16) | node.getResponseBuffer(0x01);
          CO2 = *(float*)&tempU32;
      
          tempU32 = (unsigned int)(node.getResponseBuffer(0x02) << 16) | node.getResponseBuffer(0x03);
          RH = *(float*)&tempU32;
      
          tempU32 = (unsigned int)(node.getResponseBuffer(0x04) << 16) | node.getResponseBuffer(0x05);
          TEMP = *(float*)&tempU32;

          current_co2=CO2;
          value_updated = true;
          break;
        }
      }
    }
    
    #ifdef debug
         if(value_updated){
            serial_debug.print("CO2 (read_CO2) - CO2:"); 
            serial_debug.print(current_co2,DEC);
            serial_debug.println(" ppm");
          }        
    #endif

    return value_updated;
} // end of read_CO2
