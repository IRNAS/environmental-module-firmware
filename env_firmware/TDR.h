#ifndef TDR_h
#define TDR_h

#include <Arduino.h>
#include <Wire.h>
#include "global.h"

#define TDR_num_of_variables 5

class TDR
{
    public:

        bool    setup();
        bool    read();
        void    print_data();
        bool    exec_timer();

        // variables
        int PIN                         = 0;
        int EN_PIN                         = 19;
        int CAN_ID;
        int exec_time                   = 120 * 1000;
        int exec_time_read              = 8 * 1000;
        int exec_time_stage              = 1;
        unsigned long exec_timer_last   = 0;

        int         counter_col                 = 0;
        int         counter_row                 = 0;
        bool        counter_col_overflow        = false;


        // data
        byte data[TDR_num_of_variables][COL_LENGTH][8];
        byte     time_data[COL_LENGTH][8];

        // debug
        String name = "TDR";

    private:

        // functions
        boolean    read_allTDR();

		float read_data[10][10]; //General data structure for up to 4 sensors

        // variables
        int         current_vol_w_content       = 0;
        int         current_soil_temp           = 0;
        int         current_soil_perm           = 0;
        int         current_soil_elec           = 0;
        int         current_other_data          = 0;

        // id of values
        uint8_t     id_vol_w_content            = 0;
        uint8_t     id_soil_temp                = 1;
        uint8_t     id_soil_perm                = 2;  
        uint8_t     id_soil_elec                = 3;
        uint8_t     id_other_data               = 4;


};

#endif
