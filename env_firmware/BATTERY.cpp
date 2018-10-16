#include "BATTERY.h"
#include "global.h"

#include "bq34z100.h"
#include <Wire.h>

bq34z100 _BAT;

/*
 *  Function:       bool BAT::setup()
 *  Description:    setup for BAT
 */
bool BAT::setup() {
    if(_BAT.setup(0x101,3,17400,12369,1000)) {
        return false;
    } else {
        return true;
    }
} // end of setup()

/*
 *  Function:       bool BAT::read()
 *  Description:    read all sensor data from BAT
 */
bool BAT::read() {

    if( read_voltage() == true &&
        read_temperature() == true &&
        read_current() == true &&
        read_soc() == true         ) {
        
        return true;
    } else {
        return false;
    }
} // end of read()

/*
 *  Function:       bool BAT::print_data()
 *  Description:    print all data
 */
void BAT::print_data() {
    
    print_debug(data,                               // data where we store values
                time_data,                          // where we store time data
                BAT_num_of_variables,           // number of sensor we use
                counter_col,                        // counter for coloumn index
                counter_row,                        // counter for row index
                counter_col_overflow);              // flag for coloumn counter overflow
 
} // end of print_data()

/*
 *  Function:    BAT::exec_timer()
 *  Description: execute read if the timer is right
 */
bool BAT::exec_timer() {

    if(millis() - exec_timer_last >= exec_time) {
        
        if(read()) {
            //print_data();
            #ifdef debug
                serial_debug.println("BAT (exec_timer) - executed read");
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
 *  Function:       bool BAT::read_voltage()
 *  Description:    read voltage data
 */
bool BAT::read_voltage() {

    current_voltage = _BAT.getVoltage();

    #ifdef debug
        serial_debug.print("BAT (read_voltage) - Voltage:");
        serial_debug.println(current_voltage);
    #endif

    // updating the data into our data array
    update_16bit(data,                          // our data array
                time_data,                      // our time data array
                id_voltage,                     // voltage ID
                counter_col,                    // coloumn counter
                counter_row,                    // row counter
                counter_col_overflow,           // check if coloumn has overflow 
                BAT_num_of_variables,           // number of variables
                current_voltage,                  // the value
                exec_timer_last);               // the last time

    return true;

}// end of read_voltage()

/*
 *  Function:       bool BAT::read_temperature()
 *  Description:    read the temperature value
 */
bool BAT::read_temperature() {

    current_temperature = _BAT.getTemp();

    #ifdef debug
        serial_debug.print("BAT (read_temperature) - Temperature:");
        serial_debug.println(current_temperature);
    #endif

    // updating the data into our data array
    update_16bit(data,                          // our data array
                time_data,                      // our time data array
                id_temperature,                 // temperature ID
                counter_col,                    // coloumn counter
                counter_row,                    // row counter
                counter_col_overflow,           // check if coloumn has overflow 
                BAT_num_of_variables,           // number of variables
                current_temperature,                  // the value
                exec_timer_last);               // the last time
    return true;

} // end of read_temperature()

/*
 *  Function:       bool BAT::read_current()
 *  Description:    read the current data
 */
bool BAT::read_current() {

    current_current = _BAT.getCurrent();

    #ifdef debug
        serial_debug.print("BAT (read_current) - Current:");
        serial_debug.println(current_current);
    #endif

    // updating the data into our data array
    update_16bit(data,                          // our data array
                time_data,                      // our time data array
                id_current,                     // current ID
                counter_col,                    // coloumn counter
                counter_row,                    // row counter
                counter_col_overflow,           // check if coloumn has overflow 
                BAT_num_of_variables,           // number of variables
                current_current,                  // the value
                exec_timer_last);               // the last time
    return true;
} // end of read_current()

/*
 *  Function:       bool BAT::read_soc()
 *  Description:    read the soc data
 */
bool BAT::read_soc() {

    current_soc = _BAT.getSOC();

    #ifdef debug
        serial_debug.print("BAT (read_soc) - SOC:");
        serial_debug.println(current_soc);
    #endif

    // updating the data into our data array
    update_16bit(data,                          // our data array
                time_data,                      // our time data array
                id_soc,                         // soc ID
                counter_col,                    // coloumn counter
                counter_row,                    // row counter
                counter_col_overflow,           // check if coloumn has overflow 
                BAT_num_of_variables,           // number of variables
                current_soc,                  // the value
                exec_timer_last);               // the last time

    return true;
} // end of read_current()
