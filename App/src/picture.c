#include "include.h"
#include "picture.h"

int flash_picture[] = {SECTOR_NUM - 1, 0}; //从倒数第二个扇区开始写图片
uint8 save_picture = 0;					   //判断是否要写入图片的标志
uint8 imgbuff1[CAMERA_SIZE];

void save_Picture()
{
	if (1 == save_picture)
	{
		flash_Picture();
		save_picture = 0; //清空标志位
		enable_irq(PORTD_IRQn);
	}
}

void flash_Picture() //将图片的信息写入flash
{
	int i = 0;
	uint32 data;

	/*找到可以存放数据的扇区*/
	while (813 == flash_read(flash_picture[0], flash_picture[1] * 4, uint32)) //在每组图片信息的头部flash有标志是否已经存储图片的标志位，存储图片时标志位是813
	{
		next_Write_Location();
	}
	/*写入对应的数据*/
	if (0 == flash_picture[1]) //如果是一个新的扇区
	{
		flash_erase_sector(flash_picture[0]); //擦除该扇区，也就是扇区初始化
	}
	for (i = -1; i < 150; i++) //图片数据
	{
		if (-1 == i) //写入头标志
		{
			flash_write(flash_picture[0], flash_picture[1] * 4, 813);
			flash_picture[1]++;
			continue;
		}
		data = ((uint32)imgbuff[4 * i] << 24) + ((uint32)imgbuff[4 * i + 1] << 16) + ((uint32)imgbuff[4 * i + 2] << 8) + ((uint32)imgbuff[4 * i + 3]);
		flash_write(flash_picture[0], flash_picture[1] * 4, data);
		flash_picture[1]++;
	}

	for (i = 0; i < 15; i++)
	{
		data = ((uint32)middleline[4 * i] << 24) + ((uint32)middleline[4 * i + 1] << 16) + ((uint32)middleline[4 * i + 2] << 8) + ((uint32)middleline[4 * i + 3]);
		flash_write(flash_picture[0], flash_picture[1] * 4, data);
		flash_picture[1]++;
	}
	for (i = 0; i < 15; i++)
	{
		data = ((uint32)left_black[4 * i] << 24) + ((uint32)left_black[4 * i + 1] << 16) + ((uint32)left_black[4 * i + 2] << 8) + ((uint32)left_black[4 * i + 3]);
		flash_write(flash_picture[0], flash_picture[1] * 4, data);
		flash_picture[1]++;
	}
	for (i = 0; i < 15; i++)
	{
		data = ((uint32)right_black[4 * i] << 24) + ((uint32)right_black[4 * i + 1] << 16) + ((uint32)right_black[4 * i + 2] << 8) + ((uint32)right_black[4 * i + 3]);
		flash_write(flash_picture[0], flash_picture[1] * 4, data);
		flash_picture[1]++;
	}
	next_Write_Location();
}

void next_Write_Location() //寻找下一个写入的位置
{
	if (flash_picture[1] >= 250) //一个扇区存储两幅图片的信息，第一幅的起始偏移地址是0，第二幅是1000
	{
		flash_picture[1] = 0;
		flash_picture[0]--;
	}
	else
	{
		flash_picture[1] = 250;
	}
	if (flash_picture[0] < SECTOR_NUM - 1 - 30) //如果写入超过的30个扇区
	{
		flash_picture[1] = 0;
		flash_picture[0] = SECTOR_NUM - 1;
	}
}


void delete_Picture()
{
	flash_picture[1] = 0;
	flash_picture[0] = SECTOR_NUM - 1;
	while (813 == flash_read(flash_picture[0], flash_picture[1] * 4, uint32))
	{
		flash_erase_sector(flash_picture[0]);
		flash_picture[0]--;
	}
	flash_picture[1] = 0;
	flash_picture[0] = SECTOR_NUM - 1;
}


void read_Picture()
{
	int i = 0;
	uint32 data = 0;

	flash_picture[1] = 0;
	flash_picture[0] = SECTOR_NUM - 1;
	flash_picture[1]++;		  //跳过标志位
	for (i = 0; i < 150; i++) //图片数据
	{
		data = flash_read(flash_picture[0], flash_picture[1] * 4, uint32);
		imgbuff1[4*i] = data>>24;
		imgbuff1[4*i+1] = (data & 0x00ff0000)>>16;
		imgbuff1[4*i+2] = (data & 0x0000ff00)>>8 ;
		imgbuff1[4*i+3] = data & 0x000000ff;
		flash_picture[1]++;
	}
	flash_picture[1] = 0;
	flash_picture[0] = SECTOR_NUM - 1;
}

// void read_Picture()
// {
// 	int i=0;
// 	uint8 data=0;

// 	flash_picture[1] = 0;
// 	flash_picture[0] = SECTOR_NUM - 1;
// 	while (813 == flash_read(flash_picture[0], flash_picture[1] * 4, uint32)) //在每组图片信息的头部flash有标志是否已经存储图片的标志位，存储图片时标志位是813
// 	{
// 		flash_picture[1]++; //跳过标志位
// 		for (i = 0; i < 780; i++) //图片数据
// 		{
// 			data=flash_read(flash_picture[0], flash_picture[1] * 4, uint32);
// 			vcan_sendimg(&data,sizeof(data));
// 			flash_picture[1]++;
// 		}
// 		next_Write_Location();
// 	}
// 	flash_picture[1] = 0;
// 	flash_picture[0] = SECTOR_NUM - 1;
// }