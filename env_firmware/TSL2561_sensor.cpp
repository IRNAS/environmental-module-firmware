#include "TSL2561_sensor.h"
#include "global.h"

#include <TSL2561.h>

// object from TSL2561 library
TSL2561 _TSL2561(0x39); 

/*
 *  Function:       bool TSL2561_sensor::setup()
 *  Description:    setup for TSL2561 
 */
bool TSL2561_sensor::setup() {

    if(_TSL2561.begin()) {

        //_TSL2561.setGain(TSL2561_GAIN_0X);                    // set no gain (for bright situtations)
        _TSL2561.setGain(TSL2561_GAIN_16X);                     // set 16x gain (for dim situations)

        _TSL2561.setTiming(TSL2561_INTEGRATIONTIME_13MS);       // shortest integration time (bright light)
        //_TSL2561.setTiming(TSL2561_INTEGRATIONTIME_101MS);    // medium integration time (medium light)
        //_TSL2561.setTiming(TSL2561_INTEGRATIONTIME_402MS);    // longest integration time (dim light)

        return true;
    } else {
        return false;
    }
    
} // end of setup()

/*
 *  Function:       bool TS::read()
 *  Description:    read all sensor data from TSL
 */
bool TSL2561_sensor::read() {

    if( read_luminosity()   == true &&
        read_fullspectrum() == true &&
        read_infrared()     == true        ) {

        return true;
    } else {

        return false;
    }
} // end of read()

/*
 *  Function:       void TSL2561_sensor::print_data()
 *  Description:    print all sensor data
 */
void TSL2561_sensor::print_data() {

    print_debug(data,                               // data where we store values
                time_data,                          // where we store time data
                TSL2561_num_of_variables,           // number of sensor we use
                counter_col,                        // counter for coloumn index
                counter_row,                        // counter for row index
                counter_col_overflow);              // flag for coloumn counter overflow

} // end of print_data()

/*
 *  Function:    TSL2561_sensor::exec_timer()
 *  Description: execute read if the timer is right
 */
bool TSL2561_sensor::exec_timer() {

    if(millis() - exec_timer_last >= exec_time) {
        
        if(read()) {
            //print_data();
            #ifdef debug
                serial_debug.println("TSL (exec_timer) - executed read");
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
 *  Function:       bool TSL2561_sensor::read_luminosity()
 *  Description:    read the luminosity value
 */
bool TSL2561_sensor::read_luminosity() {

    current_luminosity = _TSL2561.getLuminosity(TSL2561_VISIBLE);
   
    #ifdef debug
        serial_debug.print("TSL (read_luminosity) - luminosity:"); 
        serial_debug.println(current_luminosity);
    #endif

    // updating the data into our data array
    update_16bit(data,                          // our data array
                time_data,                      // our time data array
                id_luminosity,                  // luminosity ID
                counter_col,                    // coloumn counter
                counter_row,                    // row counter
                counter_col_overflow,           // check if coloumn has overflow 
                TSL2561_num_of_variables,       // number of variables
                current_luminosity,                  // the value
                exec_timer_last);               // the last time

    return true;

} // end of read_luminosity()

/*
 *  Function:       bool TSL2561_sensor::read_fullspectrum()
 *  Description:    read the fullspectrum value
 */
bool TSL2561_sensor::read_fullspectrum() {

    current_fullspectrum = _TSL2561.getLuminosity(TSL2561_FULLSPECTRUM);
   
    #ifdef debug
        serial_debug.print("TSL (read_fullspectrum) - Full spectrum:"); 
        serial_debug.println(current_fullspectrum);
    #endif

    // updating the data into our data array
    update_16bit(data,                          // our data array
                time_data,                      // our time data array
                id_fullspectrum,                // fullspectrum ID
                counter_col,                    // coloumn counter
                counter_row,                    // row counter
                counter_col_overflow,           // check if coloumn has overflow 
                TSL2561_num_of_variables,       // number of variables
                current_fullspectrum,                  // the value
                exec_timer_last);               // the last time

    return true;

} // end of read_fullspectrum()

/*
 *  Function:       bool TSL2561_sensor::read_infrared()
 *  Description:    read the infrared value
 */
bool TSL2561_sensor::read_infrared() {

    current_infrared = _TSL2561.getLuminosity(TSL2561_INFRARED);
   
    #ifdef debug
        serial_debug.print("TSL (read_infrared) - Full infrared:"); 
        serial_debug.println(current_infrared);
    #endif

    // updating the data into our data array
    update_16bit(data,                          // our data array
                time_data,                      // our time data array
                id_infrared,                    // infrared ID
                counter_col,                    // coloumn counter
                counter_row,                    // row counter
                counter_col_overflow,           // check if coloumn has overflow 
                TSL2561_num_of_variables,       // number of variables
                current_infrared,                  // the value
                exec_timer_last);               // the last time

    return true;

} // end of read_infrared()



