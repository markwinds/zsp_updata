#ifndef _PICTURE_H_
#define _PICTURE_H_


#include "include.h"

extern int flash_picture[];			//记录写入picture时的扇区和偏移地址
extern uint8 save_picture;			//是否写入图片的标志位
extern uint8 imgbuff1[CAMERA_SIZE];  


extern void flash_Picture();
extern void save_Picture();
extern void delete_Picture();
extern void read_Picture();

#endif