#include "arm_msg.h"

//#define MASTER
#define SLAVE


float radians[ARRAY_LEN] = {-3.14, -1, -0.5, 0.78, 1.57, 2.3, 3.004};
float inc_commands[ARRAY_LEN] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0};
int cnt = 0;

ArmMsg msg;
ArmMsg *msg_ptr = &msg;


void setup()
{
  Serial.begin(9600);
}

void loop()
{

#ifdef MASTER
  setMsg(msg_ptr,radians);
  sendMsg(msg_ptr,Serial);
  //printOutputArr(msg_ptr,Serial);
  //printMsg(msg_ptr,Serial);
  delay(50);
#endif

#ifdef SLAVE
  readMsg(msg_ptr,Serial);
  exportMsg(msg_ptr,inc_commands);
  //printInputArr(msg_ptr,Serial);

#endif
}
