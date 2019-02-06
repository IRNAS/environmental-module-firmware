#include "ANEMOMETER.h"
#include "global.h"

/*  
 *  Function:       bool ANEMOMETER::setup() 
 *  Description:    setup for the anemometer
 */
bool ANEMOMETER::setup() {

    pinMode(PIN, INPUT);
    pinMode(EN_PIN, OUTPUT);
    analogReadResolution(12);
    
    return read_windspeed();

} // end of setup()

/*  
 *  Function:       bool ANEMOMETER::read() 
 *  Description:    read the anemometer
 */
bool ANEMOMETER::read() {

    if( read_windspeed() == true ) {
        return true;
    } else {
        return false;
    }

} // end of read()

/*  
 *  Function:       void ANEMOMETER::print_data() 
 *  Description:    print all data from anemometer
 */
void ANEMOMETER::print_data() {

    print_debug(data,                               // data where we store values
                time_data,                          // where we store time data
                ANEMOMETER_num_of_variables,           // number of sensor we use
                counter_col,                        // counter for coloumn index
                counter_row,                        // counter for row index
                counter_col_overflow);              // flag for coloumn counter overflow
 
} // end of print_data()

/*
 *  Function:    ANEMOMETER::exec_timer()
 *  Description: execute read if the timer is right
 */
bool ANEMOMETER::exec_timer() {

    if(millis() - exec_timer_last >= exec_time) {
        
        if(read()) {
            //print_data();
            #ifdef debug
                serial_debug.println("ANEMOMETER (exec_timer) - executed read");
            #endif

            exec_timer_last = millis();

            return true;
        }
        return false;
        
    }

    return false;
} // end of exec_timer()

/*  
 *  Function:       bool ANEMOMETER::read_windspeed()
 *  Description:    read the windspeed
 */
bool ANEMOMETER::read_windspeed() {
    digitalWrite(EN_PIN, HIGH);
    delay(300);
    float value = 0;
    //read values a while and average
    for(int i=0; i<256; i++){
      value+=analogRead(PIN);
      delay(1);
    }
    //calculate average and convert to mV
    uint16_t current_value = value*3300/256/4095;
        
    digitalWrite(EN_PIN, LOW);
    
    // out of range, we know to receive values betwee 0.4V and 2.0V, checking for that with some margin
    if(current_value < 200 || current_value > 2500) {
        return false;
    }

    #ifdef debug
        serial_debug.print("ANEMOMETER (read_windspeed) - Wind analog:");
        serial_debug.println(current_value);
    #endif

    //int int_an_converted = int(an_converted * 100);

    // updating the data into our data array
    update_16bit(data,                          // our data array
                time_data,                      // our time data array
                id_windspeed,                // temperature ID
                counter_col,                    // coloumn counter
                counter_row,                    // row counter
                counter_col_overflow,           // check if coloumn has overflow 
                ANEMOMETER_num_of_variables,       // number of variables
                current_value,                  // the value
                exec_timer_last);               // the last time

    return true;

} // end of read_windspeed()
