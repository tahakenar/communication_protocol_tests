#ifndef ARM_MSG_H
#define ARM_MSG_H

#include <Arduino.h>
#include <HardwareSerial.h>

#define ARRAY_LEN 7
#define MSG_LEN 3 * ARRAY_LEN + 2
#define MSG_LEN_WCRC MSG_LEN + 2
#define MAPPING_COEF 2048 / 3.1416
#define REMAPPING_COEF 3.1416 / 2048

/*
    - This struct is used to store data regarding position controlled actuators and transmit it via serial port.

    - start_byte and end_byte is used by receiver to start reading and finish reading.

    - Directions are stored in the directions[] array. 1 : positive / 0 : negative

    - magnitudes[] array stores the radian magnitude which is mapped as 'radian / 3.14  =  magnitude / 2048'.

    - input_arr[] and output_arr[] are used to send and read struct data respectively.

    - CRC check is used as a control algorithm to detect any corruptions while transmitting. If any corruption is detected
      that message is not considered by the receiver.

    - A message is created and stored in output_arr[] and it is read as the same way it was created. An example is included:

              83-0-0-0-1-1-1-1-7-254-2-139-1-69-1-252-3-255-5-219-7-166-70-77-28
              START BYTE - DIRECTION BYTES - MAGNITUDES (AS HIGH & LOW BYTES) - END BYTE - CRC BYTES (2)
*/
typedef struct
{
  uint8_t start_byte = 'S';
  uint8_t directions[ARRAY_LEN];
  uint16_t magnitudes[ARRAY_LEN];
  uint8_t end_byte = 'F';
  uint8_t input_arr[MSG_LEN + 2];
  uint8_t output_arr[MSG_LEN + 2];
  unsigned int msg_crc;
  
} ArmMsg;

void printInputArr(ArmMsg *message, HardwareSerial& SerialObj);
// Prints the input array of message

void printOutputArr(ArmMsg *message, HardwareSerial& SerialObj);
// Prints the output array of message

void printMsg(ArmMsg *message, HardwareSerial& SerialObj);
// Prints the contents of message (to check if everything is correct)

void printRadians(ArmMsg *message, HardwareSerial& SerialObj);
// Prints the assigned radians 

void setMsg(ArmMsg *message, float src_list[]);
// Creates msg

//TODO: Export radians
void exportMsg(ArmMsg *message, float dest_list[]);
// Exports data in the msg

void createOutputArr(ArmMsg *message);
// Creates output msg

void sendMsg(ArmMsg *message, HardwareSerial& SerialObj);
// Sends msg through uart media

void readMsg(ArmMsg *message, HardwareSerial& SerialObj);
// Reads msg from uart media and assings incoming data to the struct

void assignNewVars(ArmMsg *message, HardwareSerial& SerialObj);
// Assings new input array to the struct

unsigned getCRC(byte buf[MSG_LEN + 2]);

#endif