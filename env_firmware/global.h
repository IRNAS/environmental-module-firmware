
#include <stdint.h>

#include <Arduino.h>

#define debug
#define candebug
#define serial_debug            Serial
#define serial_debug_speed      115200
#define COL_LENGTH 16

#define NUM_IN_ARRAY(a) (sizeof(a) / sizeof(*a))

void update_16bit(byte data[][COL_LENGTH][8], 
                  byte time_data[COL_LENGTH][8],
                  int var_id,
                  int& col, 
                  int& row, 
                  bool& counter_col_overflow, 
                  int number_of_variables,
                  int value,
                  int current_time);

void print_debug(byte data[][COL_LENGTH][8], 
                 byte time_data[COL_LENGTH][8],
                 int number_of_variables,
                 int col,
                 int row,
                 bool counter_col_overflow);

void clear_data( byte data[][COL_LENGTH][8], 
                   byte time_data[COL_LENGTH][8],
                   int& col, 
                   int& row, 
                   bool& counter_col_overflow,
                   int number_of_variables );

float mapf(float x, float in_min, float in_max, float out_min, float out_max);

void PrintHex83(uint8_t *data, uint8_t length);
