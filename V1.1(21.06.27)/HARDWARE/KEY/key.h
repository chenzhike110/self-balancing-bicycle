#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
#define KEY1 PBin(14)
#define KEY2 PBin(15)
void KEY_Init(void);          //������ʼ��
u8 key_read(void);
#endif 
