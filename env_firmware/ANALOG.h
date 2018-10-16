#ifndef ANALOG_h
#define ANALOG_h

#include <Arduino.h>
#include "global.h"

#define ANALOG_num_of_variables 1

class ANALOG
{
    public:
        // functions
        bool    setup();
        bool    read();
        void    print_data();
        bool    exec_timer();

        // variables
        int PIN                         = 0;
        int EN_PIN                         = 19;
        int CAN_ID;
        int exec_time                   = 120 * 1000;
        unsigned long exec_timer_last   = 0;

        int         counter_col                 = 0;
        int         counter_row                 = 0;
        bool        counter_col_overflow        = false;

        // print_data
        byte data[ANALOG_num_of_variables][COL_LENGTH][8];
        byte time_data[COL_LENGTH][8];

        // debug
        String name = "ANALOG";

    private:

        // functions
        bool read_sensor();

        // variables
        uint16_t current_value                 = 0;

        // id of value
        uint8_t id_value                    = 0;
};   

#endif
