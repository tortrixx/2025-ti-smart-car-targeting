#include "GW8.h"
#include "OLED.h"
#include "motor.h"
#define GWi2C &hi2c1

#define GW_GRAY_ADDR GW_GRAY_ADDR_DEF
uint8_t GW8_init(void)
{
	/* 数字数据模式, 设置完毕之后, 每次读取一个8bit的数据, 每个位表示1-8探头的状态 */
	uint8_t i2c_cmd = GW_GRAY_DIGITAL_MODE;
	uint8_t ret_transmit = HAL_I2C_Master_Transmit(GWi2C, GW_GRAY_ADDR << 1, &i2c_cmd, 1, 100);
	if (ret_transmit != HAL_OK) {
		// i2c 错误
		/* 处理错误 */
		return 1;
	}
	
	/* 处理正确	*/
		return 0;
}

uint8_t GW8_get(void)
{
	uint8_t recv_value = 0;
	HAL_I2C_Master_Receive(GWi2C, GW_GRAY_ADDR << 1, &recv_value, 1, 100);
  return recv_value;

}
//OLED_Set_Pos(0,0);
//OLED_WR_DATA(GWGEET);
void GW8_OLED_Show(uint8_t* GW8_64Date,uint8_t size)
{
uint8_t GW_i,GW_ii;
if(size==0)
{
	for(GW_i=0;GW_i<=63;GW_i++)
	{
		for(GW_ii=0;GW_ii<8;GW_ii++)
		{
			OLED_Set_Pos(GW_i,GW_ii);
			if((GW8_64Date[GW_i]>>GW_ii)%2==1)
			{
				OLED_WR_DATA(0XFF);
			}
			else
				OLED_WR_DATA(0X00);
		}
	}
}
else
{
	for(GW_i=0;GW_i<=63;GW_i++)
	{
		for(GW_ii=0;GW_ii<8;GW_ii++)
		{
			OLED_Set_Pos(GW_i*2,GW_ii);
			if((GW8_64Date[GW_i]>>GW_ii)%2==1)
			{
				OLED_WR_DATA(0XFF);
			}
			else
				OLED_WR_DATA(0X00);
		}
		for(GW_ii=0;GW_ii<8;GW_ii++)
		{
			OLED_Set_Pos(GW_i*2+1,GW_ii);
			if((GW8_64Date[GW_i]>>GW_ii)%2==1)
			{
				OLED_WR_DATA(0XFF);
			}
			else
				OLED_WR_DATA(0X00);
		}
	}
}
}

void GW8_OLED_Show_ONLY(uint8_t GW8_64Date)
{
uint8_t GW_i=0,GW_ii;

		for(GW_ii=0;GW_ii<8;GW_ii++)
		{
			OLED_Set_Pos(GW_i,GW_ii);
			if((GW8_64Date>>GW_ii)%2==1)
			{
				OLED_WR_DATA(0XFF);
			}
			else
				OLED_WR_DATA(0X00);
		}
}


float line_er=0,L_line_er=0,SUM_line_er=0,LIN_TURN_CNT=0;
uint8_t findline_pid_scanner(uint8_t* GW8_64Date,uint8_t turn)
{

	uint8_t GW_i,GW_ii,CNT=0,flag=0,flag1=0;
	float er_64[64]={0},l_PID_OUT;
		for(GW_i=0;GW_i<=63;GW_i++)
		{
			CNT=0,flag=0;
			
			/////////////直走
			if(turn==0)
			{
			for(GW_ii=0;GW_ii<8;GW_ii++)
			{
				if((GW8_64Date[GW_i]>>GW_ii)%2==0)
				{
					if(GW_ii<=3)
						er_64[GW_i]=er_64[GW_i]+0.5*(GW_ii-4);
					else
						er_64[GW_i]=er_64[GW_i]+0.5*(GW_ii-3);
					flag=1;
					if(CNT>0)
					{
						flag1=1;
						return flag1;
					}
					
				}
				else 
				{
					if(flag==1)
						CNT++;
				}	
			}
			}
			/////////////左转
			if(turn=='A')
			{
			for(GW_ii=0;GW_ii<6;GW_ii++)
			{
				if((GW8_64Date[GW_i]>>GW_ii)%2==0)
				{
					if(GW_ii<=3)
						er_64[GW_i]=er_64[GW_i]+0.5*(GW_ii-4);
					else
						er_64[GW_i]=er_64[GW_i]+0.5*(GW_ii-3);
					flag=1;
					if(CNT>0)
					{
						flag1=1;
						return flag1;
					}
					
				}
				else 
				{
					if(flag==1)
						CNT++;
				}	
			}
			}
			/////////////右转
			if(turn=='B')
			{
			for(GW_ii=2;GW_ii<8;GW_ii++)
			{
				if((GW8_64Date[GW_i]>>GW_ii)%2==0)
				{
					if(GW_ii<=3)
						er_64[GW_i]=er_64[GW_i]+0.5*(GW_ii-4);
					else
						er_64[GW_i]=er_64[GW_i]+0.5*(GW_ii-3);
					flag=1;
					if(CNT>0)
					{
						flag1=1;
						return flag1;
					}
					
				}
				else 
				{
					if(flag==1)
						CNT++;
				}	
			}
			}
			/////////////
		}
		
			
		if(flag1==0)
		{
			if(line_er>1||line_er<-1)
				SUM_line_er=SUM_line_er+line_er;
			else
				SUM_line_er=0;
			L_line_er=line_er;
			line_er=(er_64[7]+er_64[15]+er_64[23]+er_64[31])/4;
			l_PID_OUT=12.5*line_er+0.05*SUM_line_er-8.5*(L_line_er-line_er);
			Tv_A=20-l_PID_OUT,Tv_B=20+l_PID_OUT;
		}
		return 0;
}

uint8_t flag_lukou__scanner=0;
uint8_t lukou_scanner(uint8_t GW_GETT)
{
	if(GW_GETT==0X00)//路口
	{
		flag_lukou__scanner=1;
		return 1;
	}
	else if(flag_lukou__scanner==1&&GW_GETT!=0X00)//过了路口
	{
		return 2;
		flag_lukou__scanner=0;
	}
	return 0;
}


//////////////////////////////////////
uint8_t turn_findline_flag=0;
uint8_t findline_pid(uint8_t GW8_64Date,uint8_t turn,float EX)
{
if(GW8_64Date!=0XFF)
{
uint8_t GW_i,GW_ii,CNT=0,flag=0,flag1=0;
float l_PID_OUT,er_NOM;
if(turn==0||turn_findline_flag==0)
{	
for(GW_ii=0;GW_ii<8;GW_ii++)
	{
		if((GW8_64Date>>GW_ii)%2==0)
		{
		if(GW_ii<=3)
			er_NOM=er_NOM+0.5*(GW_ii-4);
		else
			er_NOM=er_NOM+0.5*(GW_ii-3);
		flag=1;
			if(CNT>0)
				{
				flag1=3;
				return flag1;
				}	
		}
		else 
			{
			if(flag==1)
				CNT++;
			}	
	}
	if(flag1!=3)
	{
	if(line_er>1||line_er<-1)
		SUM_line_er=SUM_line_er+line_er;
	else
		SUM_line_er=0;
		L_line_er=line_er;
		line_er=er_NOM;
		l_PID_OUT=12.5*line_er+0.05*SUM_line_er-8.5*(L_line_er-line_er);
		Tv_A=EX-l_PID_OUT,Tv_B=EX+l_PID_OUT;
	}
}
else if(turn=='A')
{	
for(GW_ii=0;GW_ii<6;GW_ii++)
	{
		if((GW8_64Date>>GW_ii)%2==0)
		{
		if(GW_ii<=3)
			er_NOM=er_NOM+0.5*(GW_ii-4);
		else
			er_NOM=er_NOM+0.5*(GW_ii-3);
		flag=1;
			if(CNT>0)
				{
				flag1=3;
				return flag1;
				}	
		}
		else 
			{
			if(flag==1)
				CNT++;
			}	
	}
		if(flag1!=3)
	{
	if(line_er>1||line_er<-1)
		SUM_line_er=SUM_line_er+line_er;
	else
		SUM_line_er=0;
		L_line_er=line_er;
		line_er=er_NOM;
		l_PID_OUT=12.5*line_er+0.5*SUM_line_er-8.5*(L_line_er-line_er);
		Tv_A=20-l_PID_OUT,Tv_B=20+l_PID_OUT;
	if(line_er==0)turn_findline_flag++;
	if(turn_findline_flag>2){turn_findline_flag=0;return 2;}
	}
}
else if(turn=='B')
{	
for(GW_ii=4;GW_ii<8;GW_ii++)
	{
		if((GW8_64Date>>GW_ii)%2==0)
		{
		if(GW_ii<=3)
			er_NOM=er_NOM+0.5*(GW_ii-4);
		else
			er_NOM=er_NOM+0.5*(GW_ii-3);
		flag=1;
			if(CNT>0)
				{
				flag1=3;
				return flag1;
				}	
		}
		else 
			{
			if(flag==1)
				CNT++;
			}	
	}
		if(flag1!=3)
	{
	if(line_er>1||line_er<-1)
		SUM_line_er=SUM_line_er+line_er;
	else
		SUM_line_er=0;
		L_line_er=line_er;
		line_er=er_NOM;
		l_PID_OUT=12.5*line_er+0.05*SUM_line_er-8.5*(L_line_er-line_er);
		Tv_A=20-l_PID_OUT,Tv_B=20+l_PID_OUT;
	if(line_er==0)turn_findline_flag++;
	if(turn_findline_flag>2){turn_findline_flag=0;return 2;}
	}
}
}
return 0;
}	

uint8_t findline_pid_TEST(uint8_t GW8_64Date,float EX)
{
uint8_t GW_i,GW_ii,CNT=0,flag=0,flag1=0;
float l_PID_OUT,er_NOM;
for(GW_ii=0;GW_ii<8;GW_ii++)
	{
		if((GW8_64Date>>GW_ii)%2==0)
		{
		if(GW_ii<=3)
			er_NOM=er_NOM+0.5*(GW_ii-4);
		else
			er_NOM=er_NOM+0.5*(GW_ii-3);
		flag=1;
			if(CNT>0)
				{
				flag1=3;
				return flag1;
				}	
		}
		else 
			{
			if(flag==1)
				CNT++;
			}	
	}
	if(line_er>1||line_er<-1)
		SUM_line_er=SUM_line_er+line_er;
	else
		SUM_line_er=0;
		L_line_er=line_er;
		line_er=er_NOM;
		l_PID_OUT=12.5*line_er+0.05*SUM_line_er-8.5*(L_line_er-line_er);
		Tv_A=EX-l_PID_OUT,Tv_B=EX+l_PID_OUT;
return 0;
}	





