#ifndef L0_h
#define L0_h

#include <Arduino.h>
#include <stdint.h>
#include "global.h"

#define L0_num_of_variables 2

class L0 
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

        int     counter_col             = 0;
        int     counter_row             = 0;
        bool    counter_col_overflow    = false;

        // data
        byte data[L0_num_of_variables][COL_LENGTH][8];
        byte     time_data[COL_LENGTH][8];

        // debug
        String  name = "L0";
    private:

        // functions
        bool    read_temperature();
        bool    read_VDD();

        // variables
        float   current_temperature     = 0;
        float   current_vdd             = 0;

        // id of the values
        uint8_t id_temperature          = 0;
        uint8_t id_vdd                  = 1;

};

#endif
