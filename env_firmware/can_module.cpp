#include "can_module.h"
#include "global.h"
#include "TimerMillis.h"

CAN_MODULE thisClass;

MCP_CAN CAN_BUS(CAN_PIN_NSS); // can bus object

int local_send_data_int = 0;
long unsigned int CAN_RXID = 0;
unsigned char len = 0;
unsigned char rxBuf[8];

TimerMillis CANreset;

void callbackCANreset(void)
{
    //reset if no can communication has been present in the last X time
    #ifdef debug
      serial_debug.println("callbackCANreset() - full system reset");
    #endif
    delay(5000);
    STM32L0.reset();
}

CAN_MODULE::CAN_MODULE(int ID){
  CAN_DEVICE_ID = ID;
}

int CAN_MODULE::return_local_send_data_int() {
    return local_send_data_int;
}

void CAN_MODULE::set_local_send_data_int(int status) {
    local_send_data_int = status;
}

long unsigned int CAN_MODULE::return_CAN_RXID() {
    return CAN_RXID;
}

void CAN_MODULE::set_CAN_RXID(long unsigned int ID) {
    CAN_RXID = ID;
}

/*
 *  Function: static void ISR()
 *  Description: it is a function that is a refrence to ISR_CAN()
 */
static void ISR() {
    thisClass.ISR_CAN();
}

/*
 *  Function:       void ISR_CAN()
 *  Description:    Interrupt for CAN
 */
void CAN_MODULE::ISR_CAN() {

    // wakeup the STM32L0
    STM32L0.wakeup();

    if(execute_int_can) {
        CANreset.restart(0,CAN_SAFETY_MARGIN*60*1000);
        #ifdef debug
            //serial_debug.println("can_module (ISR_CAN) - ISR");
        #endif

        // read the id and to clear the interrupt flag
        CAN_BUS.readMsgBuf(&CAN_RXID,&len, rxBuf);
        if((CAN_RXID&0xff00) == CAN_DEVICE_ID) {
            local_send_data_int   = 1;
            //check if scan request or data request
            if(rxBuf[0]==0x02){
              local_send_data_int   = 1;
            }
            //check if reset request
            else if(rxBuf[0]==0xab){
              //implement reset
              local_send_data_int   = 2;
            }
            else{
              local_send_data_int   = 3;
            }
        } else {
            local_send_data_int   = 0;
            CAN_RXID        = 0;
        }
    } // end of if
} // end of ISR_CAN()

/*
 *  Function:       bool CAN_MODULE::setup()
 *  Description:    setup the CAN module
 */
bool CAN_MODULE::setup() {

    if( CAN_BUS.begin(MCP_ANY, CAN_SPEED, CAN_MHZ) == CAN_OK &&
        CAN_BUS.setMode(MCP_NORMAL) == MCP2515_OK ) {

        // set the sleep wakeup
        CAN_BUS.setSleepWakeup(1);

        // setup the interrupt pin
        pinMode(CAN_PIN_INT, INPUT);
        attachInterrupt(digitalPinToInterrupt(CAN_PIN_INT), ISR, FALLING);
        //implement the safety timer, 120 minutes
        CANreset.start(callbackCANreset,0,CAN_SAFETY_MARGIN*60*1000);
        return true;
    } else {
        return false;
    }

} // end of setup()

bool CAN_MODULE::enable_sleep() {
  CAN_BUS.setMode(MCP_SLEEP);
  return true;
} 

void CAN_MODULE::self_check() {
  #ifdef debug
    serial_debug.print("self_check() - check CAN: 0x");
    serial_debug.println(CAN_BUS.checkReceive(),HEX);
  #endif
} 

/*
 *  Function:       bool CAN_MODULE::send_wmprty(...)
 *  Description:    send empty reply
 */
bool CAN_MODULE::send_empty() {
    byte send_data_status   = CAN_OK;       // data status
    byte data_begin[2];                    // the array we send first time
    data_begin[0]       = 0;               // tell master that it should expect 8 coloumns
    data_begin[1]       = 0;               // send master how many vairables there are * coloumn
    #ifdef debug
      serial_debug.println("can - send_empty()");
    #endif
    // send the begin data
    // send data:  ID = 0x100, Standard CAN Frame, Data length = 8 bytes, 'data' = array of data bytes to send
    send_data_status = CAN_BUS.sendMsgBuf(CAN_MASTER_ID, 0, 0, data_begin);

    if(send_data_status != CAN_OK)
        return false;
    return true;

} // end of send_empty()

/*
 *  Function:       bool CAN_MODULE::send_present(...)
 *  Description:    send reply that sensor is present
 */
bool CAN_MODULE::send_present() {
    byte send_data_status   = CAN_OK;       // data status
    byte data_begin[4];                    // the array we send first time
    data_begin[0]       = 0;               // tell master that it should expect 8 coloumns
    data_begin[1]       = 0;               // send master how many vairables there are * coloumn
    data_begin[2]       = 0;
    data_begin[3]       = 0;
    #ifdef debug
      serial_debug.println("can - send_present()");
    #endif
    // send the begin data
    // send data:  ID = 0x100, Standard CAN Frame, Data length = 8 bytes, 'data' = array of data bytes to send
    send_data_status = CAN_BUS.sendMsgBuf(CAN_MASTER_ID, 0, 4, data_begin);

    if(send_data_status != CAN_OK)
        return false;
    return true;

} // end of send_present()


/*
 *  Function:       bool CAN_MODULE::send_data(...)
 *  Description:    send the data
 */
bool CAN_MODULE::send_data( byte data[][COL_LENGTH][8],
                            byte time_data[COL_LENGTH][8],
                            int number_of_variables,
                            int col,
                            int row,
                            bool counter_col_overflow,
							int last_time) {

    byte send_data_status   = CAN_OK;       // data status
    byte data_begin[4];                     // the array we send first time

    // setup the begin data
    if(counter_col_overflow) {
        data_begin[0]       = COL_LENGTH;   // tell master that it should expect 8 coloumns
        col                 = COL_LENGTH-1; // for the for loop
    } else {
        data_begin[0]       = col+1;        // tell master how many coloumns to expect
    }

    data_begin[1] = number_of_variables;    // send master how many vairables there are * coloumn

    // time offset between can read and lst measurement
    int delta_time = int((millis() - last_time) / 100);

    // write the time data
    data_begin[2]     = lowByte(delta_time);
    data_begin[3]     = highByte(delta_time);

    // send the begin data
    send_data_status = CAN_BUS.sendMsgBuf(CAN_MASTER_ID, 0, 4, data_begin);
    //can0  001   [8]  90 10 00 00 63 71 00 08
    /*#ifdef candebug
      serial_debug.print("can0  ");
      serial_debug.print(CAN_MASTER_ID,HEX);
      serial_debug.print("   [");
      serial_debug.print(2);
      serial_debug.print("]  ");
      serial_debug.print(data_begin[0],HEX);
      serial_debug.print(" ");
      serial_debug.println(data_begin[1],HEX);
    #endif*/

    if(send_data_status != CAN_OK)
        return false;

    // SENDING THE DATA
    // loop through the variables - data[x][][]
    for(int variables=0; variables<data_begin[1]; variables++) {
        // loop through the coloumn - data[][x][]

          /*for(int c=0; c <= col; c++) {
          char tmp[16];

            serial_debug.print("Data[");
            serial_debug.print(c);
            serial_debug.print("]: ");
            PrintHex83(data[variables][c],8);
            serial_debug.println();

            serial_debug.print("Time[");
            serial_debug.print(c);
            serial_debug.print("]: ");
            PrintHex83(time_data[c],8);
            serial_debug.println();
        }*/

        for(int coloumn=0; coloumn<col+1; coloumn++) {
            // not all 8bits are available, so just send as many as available
            if(coloumn == col && counter_col_overflow == false) {

                send_data_status = CAN_BUS.sendMsgBuf(CAN_MASTER_ID, 0, row, data[variables][coloumn]);
                if(send_data_status != CAN_OK)
                    return false;

                send_data_status = CAN_BUS.sendMsgBuf(CAN_MASTER_ID, 0, row, time_data[coloumn]);
                if(send_data_status != CAN_OK)
                    return false;

            } else {

                send_data_status = CAN_BUS.sendMsgBuf(CAN_MASTER_ID, 0, 8, data[variables][coloumn]);
                if(send_data_status != CAN_OK)
                    return false;

                send_data_status = CAN_BUS.sendMsgBuf(CAN_MASTER_ID, 0, 8, time_data[coloumn]);
                if(send_data_status != CAN_OK)
                    return false;

            }
        } // end of coloumn for loop
    } // end of variables for loop

    return true;

} // end of send_data()

/*
 *  Function:       int CAN_MODULE::set_sensor_CAN_id(int& ID, int num)
 *  Description:    set the sensors CAN id automatically based on num
 */
int CAN_MODULE::set_sensor_CAN_id(int& ID, int num) {

    // example: ID = 0x100 | 1 -> 0x101
    ID = CAN_DEVICE_ID | num;
    return ID;

} // end of set_sensor_CAN_id()

/*
 *  Function:       int CAN_MODULE::get_sensor_CAN_id(int num)
 *  Description:    return the sensor ID from the num
 */
int CAN_MODULE::get_sensor_CAN_id(int ID) {
   //serial_debug.println(ID);
   //serial_debug.println(ID | CAN_DEVICE_ID);
   int value = (ID | CAN_DEVICE_ID)&0x00ff;
   return value;

} // end of get_sensor_CAN_id()
