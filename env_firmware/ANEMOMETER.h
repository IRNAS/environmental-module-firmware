#ifndef ANEMOMETER_h
#define ANEMOMETER_h

#include <Arduino.h>
#include "global.h"

#define ANEMOMETER_num_of_variables 1

class ANEMOMETER
{
    public:
        // functions
        bool    setup();
        bool    read();
        void    print_data();
        bool    exec_timer();

        // variables
        int PIN                         = 0;
        int CAN_ID;
        int exec_time                   = 120 * 1000;
        unsigned long exec_timer_last   = 0;

        int         counter_col                 = 0;
        int         counter_row                 = 0;
        bool        counter_col_overflow        = false;

        // print_data
        byte data[ANEMOMETER_num_of_variables][COL_LENGTH][8];
        byte time_data[COL_LENGTH][8];

        // debug
        String name = "ANEMOMETER";

    private:

        // functions
        bool read_windspeed();

        // variables
        float current_windspeed                 = 0;

        // id of value
        uint8_t id_windspeed                    = 0;
};   

#endif
