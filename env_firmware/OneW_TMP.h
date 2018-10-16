#ifndef OneW_TMP_h
#define OneW_TMP_h

#include <Arduino.h>
#include <OneWireSTM.h>
#include "global.h"

#define OneW_TMP_num_of_variables 3
#define ONE_WIRE_BUS 11
#define TEMP_PRECISION 12 //read precision of temp probe sensor, 12b max

class OneW_TMP
{
    public:

		OneW_TMP();
        
		// functions
        bool    setup();
        bool    read();
        void    print_data();
        bool    exec_timer();

        // variables
		int PIN = 0;
		int EN_PIN = 19;
        int CAN_ID;
        int exec_time                   = 120 * 1000;
        unsigned long exec_timer_last   = 0;

        int         counter_col                 = 0;
        int         counter_row                 = 0;
        bool        counter_col_overflow        = false;


        // data
        byte data[OneW_TMP_num_of_variables][COL_LENGTH][8];
        byte     time_data[COL_LENGTH][8];

        // debug
        String name = "OneW_TMP";
    private:

		//Sensors
		OneWire oneWire; //OneWire object
		byte addr[3][8];

		int n_sensors = 0; //Number of connected devices
        
		// functions
        bool        read_temperature(int);

        //variables
        float    current_temperature[3];

};

#endif
