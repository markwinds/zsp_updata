#include "include.h"


//char img_bucket[ROW][COL];

int tran_O_B()
{
    int i=0;
    int j=0;

    for(i=0;i<ROW;i++)
    {
        for(j=0;j<COL;j++)
        {
            //img_bucket[i][j]=img[OB_rivise[i][j].x][OB_rivise[i][j].y];
        }
    }
    return 1;
}



void init_B()
{
    int i=0;
    int j=0;

    for(i=0;i<ROW;i++)
    {
        for(j=0;j<COL;j++)
        {
            //img_bucket[i][j]=2;
        }
    }
}


void show_B()
{
    int i=0;
    int j=0;

    for(i=0;i<ROW;i++)
    {
        for(j=0;j<COL;j++)
        {
            //if(img_bucket[i][j]!=1)
            // {
            //     printf(" ");
            // }
            // else
            //     printf("1");
        }
        printf("\n");
    }
}




