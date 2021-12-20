#include "arm_msg.h"

void printInputArr(ArmMsg *message, HardwareSerial& SerialObj)
{
    SerialObj.print("Input arr: ");
    for (int i = 0; i < MSG_LEN_WCRC; i++)
    {
        SerialObj.print(message->input_arr[i]);
        SerialObj.print('-');
    }
    SerialObj.print('\n');
}

void printOutputArr(ArmMsg *message, HardwareSerial& SerialObj){
    SerialObj.print("Output arr: ");
    for (int i = 0; i < MSG_LEN_WCRC; i++)
    {
        SerialObj.print(message->output_arr[i]);
        SerialObj.print('-');
    }
    SerialObj.print('\n');
}

void printMsg(ArmMsg *message, HardwareSerial& SerialObj)
{

    SerialObj.print(message->start_byte);
    for (int i = 0; i < ARRAY_LEN; i++)
    {
        SerialObj.print(message->directions[i]);
        SerialObj.print('-');
    }
    for (int i = 0; i < ARRAY_LEN; i++)
    {
        SerialObj.print(message->magnitudes[i]);
        SerialObj.print('-');
    }
    SerialObj.print(message->end_byte);
    SerialObj.print('\n');
}

void printRadians(ArmMsg *message, HardwareSerial& SerialObj)
{
    for (int i = 0; i < ARRAY_LEN; i++)
    {
        float radian = message->magnitudes[i];
        radian = (message->directions[i] > 0 ? radian : radian * (-1.0));
        radian = radian * REMAPPING_COEF;
        SerialObj.print(radian);
        SerialObj.print('|');
    }
    SerialObj.print('\n');
}

void setMsg(ArmMsg *message, float src_list[])
{
    int mapped;
    for (int i = 0; i < ARRAY_LEN; i++)
    {
        if (src_list[i] > 0)
            message->directions[i] = 1;
        else
            message->directions[i] = 0;
    }
    for (int i = 0; i < ARRAY_LEN; i++)
    {
        mapped = (int)(MAPPING_COEF * fabs(src_list[i]));
        message->magnitudes[i] = mapped;
    }
    
    createOutputArr(message);
}

void exportMsg(ArmMsg *message, float dest_list[]){
    float remapped;
    for (int i = 0; i < ARRAY_LEN; i++){
        remapped = message->magnitudes[i];
        remapped = (message->directions[i] > 0 ? remapped : remapped * (-1.0));
        remapped = remapped * REMAPPING_COEF;
        dest_list[i] = remapped;
    }
}

void createOutputArr(ArmMsg *message){
    message->output_arr[0] = message->start_byte;
    for (int i = 0; i < ARRAY_LEN; i++)
    {
        message->output_arr[1 + i] = message->directions[i];
    }
    for (int i = 0; i < ARRAY_LEN * 2; i++)
    {
        message->output_arr[2 * i + ARRAY_LEN + 1] = highByte(message->magnitudes[i]);
        message->output_arr[2 * i + ARRAY_LEN + 2] = lowByte(message->magnitudes[i]);
    }
    message->output_arr[MSG_LEN - 1] = message->end_byte;
    message->msg_crc = getCRC(message->output_arr);
    message->output_arr[MSG_LEN] = highByte(message->msg_crc);
    message->output_arr[MSG_LEN + 1] = lowByte(message->msg_crc);
    //message->output_arr[MSG_LEN - 1] = highByte(message->msg_crc);
    //message->output_arr[MSG_LEN] = lowByte(message->msg_crc);
    //message->output_arr[MSG_LEN + 1] = message->end_byte;
}

void sendMsg(ArmMsg *message,  HardwareSerial& SerialObj)
{
    for (int i = 0; i < MSG_LEN + 2; i++)
    {
        SerialObj.write(message->output_arr[i]);
        delay(1);
    }
}

void readMsg(ArmMsg *message, HardwareSerial& SerialObj)
{
    int inc_byte;
    unsigned new_crc;
    static boolean receive_flg = false;
    static uint8_t receive_cnt = 0;
    delay(2);
    inc_byte = SerialObj.read();
    if ((uint8_t)inc_byte == message->start_byte)
    {
        message->input_arr[receive_cnt] = inc_byte;
        receive_flg = true;
        receive_cnt++;
    }
    if (receive_flg && inc_byte != message->start_byte && inc_byte != message->end_byte)
    {
        message->input_arr[receive_cnt] = inc_byte;
        receive_cnt++;
    }
    if (inc_byte == message->end_byte)
    {
        message->input_arr[receive_cnt++] = inc_byte;
        inc_byte = SerialObj.read();
        message->input_arr[receive_cnt++] = inc_byte;
        delay(1);
        inc_byte = SerialObj.read();
        message->input_arr[receive_cnt++] = inc_byte;
        

        //CRC check
        new_crc = getCRC(message->input_arr);
        message->msg_crc = (message->input_arr[MSG_LEN_WCRC - 2] << 8) | message->input_arr[MSG_LEN_WCRC - 1];
        //message->msg_crc = (message->input_arr[MSG_LEN_WCRC - 3] << 8) | message->input_arr[MSG_LEN_WCRC - 2];
        if (new_crc == message->msg_crc)
        {
            assignNewVars(message,SerialObj);
            //printInputArr(message,SerialObj);
        }
        receive_cnt = 0;
        receive_flg = false;
    }
}

void assignNewVars(ArmMsg *message, HardwareSerial& SerialObj)
{
     //if (message->input_arr[0] == 'S' && message->input_arr[MSG_LEN_WCRC-1] == 'F')
    if (message->input_arr[0] == 'S' && message->input_arr[MSG_LEN - 1] == 'F')
    {
        for (int i = 0; i < ARRAY_LEN; i++)
        {
            message->directions[i] = message->input_arr[i + 1];
        }
        for (int i = 0; i < ARRAY_LEN; i++)
        {
            message->magnitudes[i] = (message->input_arr[2 * i + ARRAY_LEN + 1] << 8) | message->input_arr[2 * i + ARRAY_LEN + 2];
        }
    }

    // This line is used for debugging purposes
    printRadians(message,SerialObj);
}

unsigned getCRC(byte buf[MSG_LEN + 2])
{
    unsigned int crc = 0xFFFF;
    //for (int pos = 0; pos < MSG_LEN; pos++)
    for (int pos = 0; pos < MSG_LEN-1; pos++)
    {
        crc ^= (unsigned int)buf[pos];
        for (int i = 8; i != 0; i--)
        {
            if ((crc & 0x0001) != 0)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}