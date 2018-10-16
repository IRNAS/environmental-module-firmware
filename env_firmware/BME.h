#ifndef BME_h
#define BME_h

#include <Arduino.h>
#include "global.h"

#define BME280_num_of_variables 3
#define BME_I2C_ADDRESS 0x77 // 0x76 if at U2

class BME
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
        int exec_time                   = 120 * 1000;
        unsigned long exec_timer_last   = 0;

        int         counter_col                 = 0;
        int         counter_row                 = 0;
        bool        counter_col_overflow        = false;


        // print_data
        byte data[BME280_num_of_variables][COL_LENGTH][8];
        byte time_data[COL_LENGTH][8];

        // debug
        String name = "BME280";

    private:

        // functions
        bool read_pressure();
        bool read_temperature();
        bool read_humidity();

        // variables
        float current_pressure                  = 0;
		float current_temperature               = 0;
		float current_humidity                  = 0;

        // id of values
        uint8_t id_pressure                     = 0;
        uint8_t id_temperature                  = 1;
        uint8_t id_humidity                     = 2;

};

#endif
