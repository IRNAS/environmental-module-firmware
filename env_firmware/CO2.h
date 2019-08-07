#ifndef CO2_h
#define CO2_h

#include <Arduino.h>
#include "global.h"
#include "ModbusMaster.h"
#include "SparkFun_SCD30_Arduino_Library.h" 

#define CO2_num_of_variables    1

#define CO2_SERIAL              Serial1
#define CO2_SERIAL_BAUD         9600
#define CO2_SERIAL_BAUD_SCD30   19200



class CO2 
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
        int exec_time                   = 180 * 1000;
        unsigned long exec_timer_last   = 0;

        int     counter_col                 = 0;
        int     counter_row                 = 0;
        bool    counter_col_overflow        = false;


        // data
        byte data[CO2_num_of_variables][COL_LENGTH][8];
        byte     time_data[COL_LENGTH][8];

        // debug
        String name = "CO2";
        
    private:

        // functions
        bool    read_CO2();

        // variables
        int     current_co2                 = 0;

        // id of values
        uint8_t id_co2                      = 0;
        uint8_t co2_type = 0;
        // 0 is ExplorIR
        // 1 is SCD30 I2C
        // 2 is SCD30 UART


};

#endif
