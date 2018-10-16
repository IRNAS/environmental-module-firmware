#include "RAIN.h"
#include "global.h"

volatile unsigned long  rain_contacttime            = 0;
int                     current_rain_count          = 0;

/*
 *  Function:       void ISR_RAIN()
 *  Description:    interrupt function for RAIN
 */
void ISR_RAIN() {

    STM32L0.wakeup();

    #ifdef debug
        serial_debug.println("RAIN (ISR_RAIN) - interrupt on rain");
    #endif

    if((millis() - rain_contacttime) > 70) {
        current_rain_count++;
        rain_contacttime = millis();
    }

} // end of ISR_RAIN

/*
 *  Function:       bool RAIN::setup()
 *  Description:    setup pins and interrupt
 */
bool RAIN::setup() {
    pinMode(PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(PIN), ISR_RAIN, FALLING);

    return true;

} // end of setup()

/*
 *  Function:       RAIN::read()
 *  Description:    read rain data
 */
bool RAIN::read() {
    if( read_raincount() == true) {
        
        return true;
    } else {
        return false;
    }
} // end of read()

/*
 *  Function:       RAIN::print_data()
 *  Description:    print data to the console
 */
void RAIN::print_data() {

    print_debug(data,                               // data where we store values
                time_data,                          // where we store time data
                RAIN_num_of_variables,           // number of sensor we use
                counter_col,                        // counter for coloumn index
                counter_row,                        // counter for row index
                counter_col_overflow);

} // end of print_data()

/*
 *  Function:    RAIN::exec_timer()
 *  Description: execute read if the timer is right
 */
bool RAIN::exec_timer() {

    if(millis() - exec_timer_last >= exec_time) {
        
        if(read()) {
            //print_data();
            #ifdef debug
                serial_debug.println("RAIN (exec_timer) - executed read");
            #endif

            exec_timer_last = millis();

            return true;
        }
        return false;
        
    }

    return false;
} // end of exec_timer()

/*
 *  Function:       RAIN::read_raincount()
 *  Description:    update the data with the current rain count
 */
bool RAIN::read_raincount() {

    #ifdef debug
        serial_debug.print("RAIN (read_raincount) - Raincount: ");
        serial_debug.println(current_rain_count);
    #endif

    // updating the data into our data array
    update_16bit(data,                          // our data array
                time_data,                      // our time data array
                id_raincount,                   // RAIN ID
                counter_col,                    // coloumn counter
                counter_row,                    // row counter
                counter_col_overflow,           // check if coloumn has overflow 
                RAIN_num_of_variables,          // number of variables
                current_rain_count,                  // the value
                exec_timer_last);               // the last time

    return true;

} // end of read_raincount()
