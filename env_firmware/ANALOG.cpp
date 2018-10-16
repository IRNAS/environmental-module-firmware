#include "ANALOG.h"
#include "global.h"

/*  
 *  Function:       bool ANALOG::setup() 
 *  Description:    setup for the analog sensor
 */
bool ANALOG::setup() {

    pinMode(EN_PIN, OUTPUT);
    digitalWrite(EN_PIN, LOW);
    delay(100);
    bool value = read_sensor();
    pinMode(PIN, INPUT);

    return value;

} // end of setup()

/*  
 *  Function:       bool ANALOG::read() 
 *  Description:    read the sensor 
 */
bool ANALOG::read() {

    if( read_sensor() == true ) {
        return true;
    } else {
        return false;
    }

} // end of read()

/*  
 *  Function:       void ANALOG::print_data() 
 *  Description:    print all data from sensor
 */
void ANALOG::print_data() {

    print_debug(data,                               // data where we store values
                time_data,                          // where we store time data
                ANALOG_num_of_variables,           // number of sensor we use
                counter_col,                        // counter for coloumn index
                counter_row,                        // counter for row index
                counter_col_overflow);              // flag for coloumn counter overflow
 
} // end of print_data()

/*
 *  Function:    ANALOG::exec_timer()
 *  Description: execute read if the timer is right
 */
bool ANALOG::exec_timer() {

    if(millis() - exec_timer_last >= exec_time) {
        
        if(read()) {
            //print_data();
            #ifdef debug
                serial_debug.println("ANALOG (exec_timer) - executed read");
            #endif

            exec_timer_last = millis();

            return true;
        }
        return false;
        
    }

    return false;
} // end of exec_timer()

/*  
 *  Function:       bool ANALOG::read_sensor()
 *  Description:    read the sensor data
 */
bool ANALOG::read_sensor() {
    //digitalWrite(EN_PIN, HIGH);
    delay(300);

    long value = 0;
    
    for(int i=0; i<256; i++){
      value+=analogRead(PIN);
      delay(1);
    }
    int current_value = value/256;
    //digitalWrite(EN_PIN, LOW);

    // out of range -> CHANGE THE RANGE
    //if(current_value < 100 || current_value > 4000 ) {
      //  return false;
    //}

    // updating the data into our data array
    update_16bit(data,                          // our data array
                time_data,                      // our time data array
                id_value,                       // value ID
                counter_col,                    // coloumn counter
                counter_row,                    // row counter
                counter_col_overflow,           // check if coloumn has overflow 
                ANALOG_num_of_variables,        // number of variables
				        current_value,                  // the value
				        exec_timer_last);					      // the last time

    return true;

} // end of read_sensor()
