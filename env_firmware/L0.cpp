#include "L0.h"
#include "global.h"

// Library include
#include <STM32L0.h> 

/*
 *  Function:       bool L0::setup()
 *  Description:    setup for L0
 */
bool L0::setup() {
    return true;
} // end of setup()

/*
 *  Function:       bool L0::read()
 *  Description:    read all sensors from L0
 */
bool L0::read() {

    // read the sensors
    if( read_temperature() == true &&
        read_VDD()         == true      ) {

        return true;
    } else {

        return false;
    }

} // end of read()

/*
 *  Function:    void L0::print_data()
 *  Description: print all data from this sensor
 */
void L0::print_data() {

    print_debug(data,                       // data where we store values
                time_data,                  // where we store time data
                L0_num_of_variables,          // number of sensor we use
                counter_col,                // counter for coloumn index
                counter_row,                // counter for row index
                counter_col_overflow);      // flag for coloumn counter overflow

} // end of print_data()

/*
 *  Function:    L0::exec_timer()
 *  Description: execute read if the timer is right
 */
bool L0::exec_timer() {

    if(millis() - exec_timer_last >= exec_time) {
        
        if(read()) {
            //print_data();
            #ifdef debug
                serial_debug.println("L0 (exec_timer) - executed read");
            #endif

            exec_timer_last = millis();

            return true;
        }
        return false;
        
    }

    return false;
} // end of exec_timer()

/**************************************************************************************/

/*
 *  Function:    bool L0:read_temperature()
 *  Description: read the temperature
 */
bool L0::read_temperature() {

    // read the temperature
    current_temperature = STM32L0.getTemperature();

    #ifdef debug
        serial_debug.print("L0 (read_temperature) - temp:"); 
        serial_debug.println(current_temperature);
    #endif

    // converting data (23.56 -> 2356)
    int convert_data = int(current_temperature * 100);

    // updating the data into our data array
    update_16bit(data,                      // our data array
                time_data,
                id_temperature,             // temperature ID
                counter_col,               // coloumn counter
                counter_row,               // row counter
                counter_col_overflow,      // check if coloumn has overflow 
                L0_num_of_variables,
                convert_data,                  // the value
                exec_timer_last);               // the last time

    return true;

} // end of read_temperature()

/*
 *  Function:    bool L0:read_VDD()
 *  Description: read the VDD voltage
 */
bool L0::read_VDD() {

    // read the read_VDD
    current_vdd = STM32L0.getVDDA();

    #ifdef debug
        serial_debug.print("L0 (read_VDD) - VDD:");
        serial_debug.println(current_vdd);
    #endif

    // convert data (3.30 -> 3300)
    int convert_data = int(current_vdd * 100);

    // updating the data into our data array
    update_16bit(data,                      // our data array
                time_data,
                id_vdd,                     // VDD ID
                counter_col,               // coloumn counter
                counter_row,               // row counter
                counter_col_overflow,      // check if coloumn has overflow
                L0_num_of_variables,               
                convert_data,                  // the value
                exec_timer_last);               // the last time
    return true;

} // end of read_VDD()
