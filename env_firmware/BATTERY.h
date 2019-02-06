#ifndef BATTERY_h
#define BATTERY_h

#include <Arduino.h>
#include "global.h"

#define BAT_num_of_variables 4
#define BAT_I2C_ADDRESS 0x55

class BAT
{
    public:

        // functions
        bool setup();
        bool read();
        void print_data();
        bool    exec_timer();

        // variables
        int PIN                         = 0;
        int CAN_ID;
        int exec_time                   = 180 * 1000;
        unsigned long exec_timer_last   = 0;

        int         counter_col                 = 0;
        int         counter_row                 = 0;
        bool        counter_col_overflow        = false;


        // print_data
        byte data[BAT_num_of_variables][COL_LENGTH][8];
        byte time_data[COL_LENGTH][8];

        // debug
        String name = "BAT";

    private:

        // functions
        bool read_voltage();
        bool read_temperature();
        bool read_current();
        bool read_soc();

        // variables
        int current_voltage                   = 0;
        int current_temperature               = 0;
        int current_current                   = 0;
        int current_soc                       = 0;

        // id of values
        uint8_t id_voltage                      = 0;
        uint8_t id_temperature                  = 1;
        uint8_t id_current                      = 2;
        uint8_t id_soc                          = 3;


};

#endif
