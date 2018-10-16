#ifndef can_module_h
#define can_module_h

#include <Arduino.h>
#include <SPI.h>
#include <STM32L0.h>
#include "global.h"

#include <mcp_can.h>
#include <mcp_can_dfs.h>

#define CAN_PIN_INT 2               // interrupt pin PB5
#define CAN_PIN_NSS 10               // chip-select PB12-NSS pin

#define CAN_MASTER_ID 0x001

#define CAN_SPEED CAN_125KBPS       // CAN_125KBPS
#define CAN_MHZ   MCP_16MHZ          // MCP_16MHZ, MCP_20MHZ

#define SET_CAN_ID 0x200

class CAN_MODULE
{
    public:

        // functions
        CAN_MODULE(int ID = SET_CAN_ID);
        bool setup();

        int set_sensor_CAN_id(int& ID, int num);
        int get_sensor_CAN_id(int ID);

        bool send_data( byte data[][COL_LENGTH][8],
                        byte time_data[COL_LENGTH][8],
                        int number_of_variables,
                        int col,
                        int row,
                        bool counter_col_overflow,
						int last_time);
        bool send_empty();
        bool send_present();
        bool enable_sleep();

        void ISR_CAN();

        //Variables
        int CAN_DEVICE_ID = 0x400;
        bool execute_int_can    = true;

        // debug
        String name = "CAN";

        int return_local_send_data_int();
        void set_local_send_data_int(int status);
        long unsigned int return_CAN_RXID();
        void set_CAN_RXID(long unsigned int ID);

    private:

};

#endif
