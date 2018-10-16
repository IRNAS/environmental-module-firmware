#include "global.h"

#include <Arduino.h>

/*
 *  Function: void update_16bit(...)
 *  Description: add `value` to your `data` array (converting to 16bit (2x8bit))
 *  Parameters:
 *      - uint8_t data[][COL_LENGTH][8]          - the array where we store our data
 *      - int var_id                    - the variables ID (to find it in the array)
 *      - int* col                      - the coloumn variable (pointer because we modif it) can be any 
 *      - int* row                      - the row variable (pointer because we modif it), must be 8 bytes
 *      - bool* counter_col_overflow    - the counter coloumn overflown flag (set when we have filled all 64bits)
 *      - int value                     - the value we add to the data array
 */
void update_16bit( byte data[][COL_LENGTH][8], 
                   byte time_data[COL_LENGTH][8],
                   int var_id, 
                   int& col, 
                   int& row, 
                   bool& counter_col_overflow, 
                   int number_of_variables,
                   int value,
                   int last_time) {
    
    // writing the data
    data[var_id][col][row]  = lowByte(value);
    data[var_id][col][row+1]  = highByte(value);

    // get the seconds since 
    int delta_time = int((millis() - last_time) / 100);

    // write the time data
    time_data[col][row]     = lowByte(delta_time);
    time_data[col][row + 1]     = highByte(delta_time);

    /*#ifdef debug
      serial_debug.print("update_16bit() - var_id ");
      serial_debug.print(var_id);
      serial_debug.print(" col ");
      serial_debug.print(col);
      serial_debug.print(" row ");
      serial_debug.print(row);
      serial_debug.print(" value l ");
      serial_debug.print(lowByte(value),HEX);
      serial_debug.print(" value h ");
      serial_debug.print(highByte(value),HEX);
      serial_debug.print(" time l ");
      serial_debug.print(lowByte(delta_time),HEX);
      serial_debug.print(" time h ");
      serial_debug.println(highByte(delta_time),HEX);
    #endif*/

    // if we are processing the last variable
    if(number_of_variables == var_id + 1) { 

        // increment the row
        row+=2;

        // if it is more than 7 (because we are fitting 4 data points 4x2 into 8bits)
        if(row > 7) {
            row = 0;                            // go back to null
            col++;                              // go to the next coloumn`
            
            // if we are overflowing
            if(col > COL_LENGTH-1) {
                col = 0;                        // go back 
                counter_col_overflow = 1;       // set the overflow flag
            } 
        }
    }
}

/*
 *  Function: void print_debug(...)
 *  Description: print the data array
 *  Parameters:  
 *      - uint8_t data[][COL_LENGTH][8]          - the array we print from
 *      - int number_of_variables       - number of variables it has (first [] (we could add auto detection of this))
 *      - int col                       - the current coloumn counter
 *      - int row                       - the current row counter
 *      - bool counter_col_overflow     - if we have filled all 64bit
 */
void print_debug(byte data[][COL_LENGTH][8], 
                 byte time_data[COL_LENGTH][8],
                 int number_of_variables,
                 int col,
                 int row,
                 bool counter_col_overflow) {

    // go through all the variables
    for(int var=0; var < number_of_variables; var++) {
        
        // print the variable number 
        serial_debug.print("Variable num: ");serial_debug.println(var);

        for(int c=0; c <= col; c++) {
          char tmp[16];
                      
            serial_debug.print("Data[");
            serial_debug.print(c);
            serial_debug.print("]: ");     
            PrintHex83(data[var][c],8);
            serial_debug.println();

            serial_debug.print("Time[");
            serial_debug.print(c);
            serial_debug.print("]: "); 
            PrintHex83(time_data[c],8);
            serial_debug.println();
        }

    }

    serial_debug.println();
}

void clear_data( byte data[][COL_LENGTH][8], 
                   byte time_data[COL_LENGTH][8],
                   int& col, 
                   int& row, 
                   bool& counter_col_overflow, 
                   int number_of_variables
                   ) {

    /*#ifdef debug
      serial_debug.print("clear_data() - sizeof ");
      serial_debug.print(sizeof(data[0]));
      serial_debug.print(" number_of_variables ");
      serial_debug.println(number_of_variables);
    #endif*/
    
    for(int var=0; var < number_of_variables; var++) {
      memset(data[var], 0, sizeof(data[var]));
    }
    
    /*#ifdef debug
      serial_debug.print("clear_data() - sizeof ");
      serial_debug.print(sizeof(time_data[0]));
      serial_debug.print(" length ");
      serial_debug.println(sizeof(*time_data));
    #endif*/

    for(int var=0; var < sizeof(*time_data); var++) {
      memset(time_data[var], 0, sizeof(time_data[var]));
    }
    
    col = 0;
    row = 0;
    counter_col_overflow = false;

}

/*
 *  Function:       float mapf(...)
 *  Description:    same as map but with floats
 */
float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void PrintHex83(uint8_t *data, uint8_t length) // prints 8-bit data in hex
{
 char tmp[length*2+1];
 byte first ;
 int j=0;
 for (uint8_t i=0; i<length; i++)
 {
   first = (data[i] >> 4) | 48;
   if (first > 57) tmp[j] = first + (byte)39;
   else tmp[j] = first ;
   j++;

   first = (data[i] & 0x0F) | 48;
   if (first > 57) tmp[j] = first + (byte)39;
   else tmp[j] = first;
   j++;
 }
 tmp[length*2] = 0;
 serial_debug.println(tmp);
}
