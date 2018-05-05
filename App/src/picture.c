#include "include.h"
#include "picture.h"

int flash_picture[] = {SECTOR_NUM - 1, 0}; //从倒数第二个扇区开始写图片
uint8 save_picture = 0;					   //判断是否要写入图片的标志
uint8 imgbuff1[CAMERA_SIZE];
int picture_choose = 0; //读取图片时的按键选择
int picture_count = 1;  //当前读取的图片的序号
float delete_picture=0;	//是否删除图片的变量
int picture_choose_before=0;
float read_all_picture=0;

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

	flash_picture[1] = 0;
	flash_picture[0] = SECTOR_NUM - 1;
	/*找到可以存放数据的扇区*/
	while (813 == flash_read(flash_picture[0], flash_picture[1] * 4, uint32)) //在每组图片信息的头部flash有标志是否已经存储图片的标志位，存储图片时标志位是813
	{
		next_Write_Location(0);
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
	next_Write_Location(0);
	led_turn (LED0);
	DELAY_MS(200);
	led_turn (LED0);
}

int next_Write_Location(int f) //寻找下一个写入的位置
{
	if (0 == f)
	{
		if (flash_picture[1] >= 250) //一个扇区存储两幅图片的信息，第一幅的起始偏移地址是0，第二幅是250
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
		return 1;
	}
	else if (1 == f)
	{
		if (flash_picture[1] >= 250) //一个扇区存储两幅图片的信息，第一幅的起始偏移地址是0，第二幅是250
		{
			flash_picture[1] = 0;
			flash_picture[0]--;
		}
		else
		{
			flash_picture[1] = 250;
		}
		if (flash_read(flash_picture[0], flash_picture[1] * 4, uint32) != 813 || flash_picture[0] < SECTOR_NUM - 1 - 30)
		{
			flash_picture[1] = 0;
			flash_picture[0] = SECTOR_NUM - 1;
		}
		return (((SECTOR_NUM - 1) - flash_picture[0]) * 2 + (flash_picture[1] > 249) + 1);
	}
        return -1;
}

int before_Write_Location()
{
	int i = 0;
	int j = 0;

	if (flash_picture[1] >= 250)
	{
		flash_picture[1] = 0;
	}
	else
	{
		flash_picture[1] = 250;
		flash_picture[0]++;
	}
	if (flash_picture[0] > SECTOR_NUM - 1)
	{
		flash_picture[1] = 0;
		flash_picture[0] = SECTOR_NUM - 1;
		i = 0;
		while (next_Write_Location(1) != 1)
		{
			i++;
		}
		for (j = 0; j < i; j++)
		{
			next_Write_Location(1);
		}
	}
	return (((SECTOR_NUM - 1) - flash_picture[0]) * 2 + (flash_picture[1] > 249) + 1);
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

void read_Picture_Array()
{
	int i = 0;
	uint32 data = 0;
	Site_t site = {0, 0};
	Size_t size;
	Size_t imgsize = {CAMERA_W, CAMERA_H};
	//size.H = LCD_H;
	//size.W = LCD_W;
	//size.H = LCD_H/2;
	//size.W = LCD_W/2;
	size.H = 60;
	size.W = 80;

	if (1 == picture_choose)
	{
		picture_choose = 10;
		picture_count = before_Write_Location();
	}
	else if (3 == picture_choose)
	{
		picture_choose = 10;
		picture_count = next_Write_Location(1);
	}
	else if (2 == picture_choose)
	{
		if (flash_picture[1] >= 0 && flash_picture[1] < 250)
		{
			flash_picture[1] = 1;
		}
		else
		{
			flash_picture[1] = 251;
		}
		for (i = 0; i < 150; i++) //图片数据
		{
			data = flash_read(flash_picture[0], flash_picture[1] * 4, uint32);
			imgbuff1[4 * i] = data >> 24;
			imgbuff1[4 * i + 1] = (data & 0x00ff0000) >> 16;
			imgbuff1[4 * i + 2] = (data & 0x0000ff00) >> 8;
			imgbuff1[4 * i + 3] = data & 0x000000ff;
			flash_picture[1]++;
		}
		vcan_sendimg(imgbuff1, CAMERA_SIZE);
		LCD_Img_Binary_Z(site, size, imgbuff1, imgsize);
		flash_picture[1] -= 150;
		picture_choose_before=2;
		picture_choose=0;
	}
	else if (4==picture_choose)
	{
		if (flash_picture[1] >= 0 && flash_picture[1] < 250)
		{
			flash_picture[1] = 151;
		}
		else
		{
			flash_picture[1] = 401;
		}
		for (i = 0; i < 45; i++) //图片数据
		{
			if (0 == (i % 3) && i != 0)
				printf("\n");
			if (0 == (i % 15) && i != 0)
				printf("\n");
			if (0 == i)
			{
				printf("\n\n\n\n\n");
				printf("                                   Picture:%d\n",picture_count);
			}
				
			data = flash_read(flash_picture[0], flash_picture[1] * 4, uint32);
			printf("%5d ", (int8)((data >> 24)));
			printf("%5d ", (int8)((data & 0x00ff0000) >> 16));
			printf("%5d ", (int8)((data & 0x0000ff00) >> 8));
			printf("%5d ", (int8)(data & 0x000000ff));
			flash_picture[1]++;
		}
		flash_picture[1] -= 45;
		picture_choose=0;
	}
	else if (10==picture_choose)
	{
		if (flash_picture[1] >= 0 && flash_picture[1] < 250)
		{
			flash_picture[1] = 1;
		}
		else
		{
			flash_picture[1] = 251;
		}
		for (i = 0; i < 150; i++) //图片数据
		{
			data = flash_read(flash_picture[0], flash_picture[1] * 4, uint32);
			imgbuff1[4 * i] = data >> 24;
			imgbuff1[4 * i + 1] = (data & 0x00ff0000) >> 16;
			imgbuff1[4 * i + 2] = (data & 0x0000ff00) >> 8;
			imgbuff1[4 * i + 3] = data & 0x000000ff;
			flash_picture[1]++;
		}
		send_Picture10();
		LCD_Img_Binary_Z(site, size, imgbuff1, imgsize);
		flash_picture[1] -= 150;
		picture_choose_before=10;
		picture_choose=0;
	}
	else if(11==picture_choose)
	{
		picture_count=1;
		picture_choose=10;
		read_Picture_Array();
		picture_choose=4;
		read_Picture_Array();
		printf("\n\n\n---------------------------------------------------------------------------------------------\n\n\n");
		while(next_Write_Location(1)!=1)
		{
			picture_count++;
			picture_choose=10;
			read_Picture_Array();
			picture_choose=4;
			read_Picture_Array();
			printf("\n\n\n---------------------------------------------------------------------------------------------\n\n\n");
		}
		picture_count=1;
		picture_choose=0;
		flash_picture[1] = 0;
		flash_picture[0] = SECTOR_NUM - 1;
	}
}


void send_Picture10()
{
	int i=0;
	int j=0;

	img_extract(img, imgbuff1, CAMERA_SIZE); //解压图像

	printf("\n\n                                   Picture:%d\n",picture_count);
	for(i=0;i<60;i++)
	{
		for(j=0;j<80;j++)
		{
			printf("%d",img[i][j]);
		}
		printf("\n");
	}

	printf("\n\n                                   Picture:%d\n",picture_count);
	for(i=0;i<60;i++)
	{
		for(j=0;j<80;j++)
		{
			printf("%d",img[i][j]);
			if(0==((j+1)%20)) printf(" ");
		}
		printf("\n");
		if(0==((i+1)%10)) printf("\n");
	}
}