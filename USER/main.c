#include "stm32f10x.h"
#include <stdio.h>

#include "mb.h"
#include "mbutils.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//输入寄存器起始地址
#define REG_INPUT_START       0x0000
//输入寄存器数量
#define REG_INPUT_NREGS       8
//保持寄存器起始地址
#define REG_HOLDING_START     0x0000
//保持寄存器数量
#define REG_HOLDING_NREGS     8

//线圈起始地址
#define REG_COILS_START       0x0000
//线圈数量
#define REG_COILS_SIZE        16

//开关寄存器起始地址
#define REG_DISCRETE_START    0x0000
//开关寄存器数量
#define REG_DISCRETE_SIZE     16

/* Private macro -------------------------------------------------------------*/
//LED控制
#define LED1_OFF()          GPIO_SetBits(GPIOB,GPIO_Pin_9)
#define LED1_ON()           GPIO_ResetBits(GPIOB,GPIO_Pin_9)

#define LED2_OFF()          GPIO_SetBits(GPIOB,GPIO_Pin_8)
#define LED2_ON()           GPIO_ResetBits(GPIOB,GPIO_Pin_8)

#define LED3_OFF()          GPIO_SetBits(GPIOB,GPIO_Pin_7)
#define LED3_ON()           GPIO_ResetBits(GPIOB,GPIO_Pin_7)

#define LED4_OFF()          GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define LED4_ON()           GPIO_ResetBits(GPIOB,GPIO_Pin_6)

//拨码开关控制
#define BUTTON1_READ()      GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_8)
#define BUTTON2_READ()      GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_9)
#define BUTTON3_READ()      GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_10)
#define BUTTON4_READ()      GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_11)
#define BUTTON5_READ()      GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_12)
#define BUTTON6_READ()      GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_13)
#define BUTTON7_READ()      GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_14)
#define BUTTON8_READ()      GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_15)

//继电器控制
#define Relay1_ON()         GPIO_SetBits(GPIOC,GPIO_Pin_0)
#define Relay1_OFF()        GPIO_ResetBits(GPIOC,GPIO_Pin_0)

#define Relay2_ON()         GPIO_SetBits(GPIOC,GPIO_Pin_1)
#define Relay2_OFF()        GPIO_ResetBits(GPIOC,GPIO_Pin_1)

#define Relay3_ON()         GPIO_SetBits(GPIOC,GPIO_Pin_2)
#define Relay3_OFF()        GPIO_ResetBits(GPIOC,GPIO_Pin_2)

#define Relay4_ON()         GPIO_SetBits(GPIOC,GPIO_Pin_3)
#define Relay4_OFF()        GPIO_ResetBits(GPIOC,GPIO_Pin_3)


/* Private variables ---------------------------------------------------------*/
//输入寄存器内容
uint16_t usRegInputBuf[REG_INPUT_NREGS] = {0x1000,0x1001,0x1002,0x1003,0x1004,0x1005,0x1006,0x1007};
//寄存器起始地址
uint16_t usRegInputStart = REG_INPUT_START;

//保持寄存器内容
uint16_t usRegHoldingBuf[REG_HOLDING_NREGS] = {0x147b,0x3f8e,0x147b,0x400e,0x1eb8,0x4055,0x147b,0x408e};
//保持寄存器起始地址
uint16_t usRegHoldingStart = REG_HOLDING_START;

//线圈状态
uint8_t ucRegCoilsBuf[REG_COILS_SIZE / 8] = {0x01,0x02};
//开关输入状态
uint8_t ucRegDiscreteBuf[REG_DISCRETE_SIZE / 8] = {0x01,0x02};

/* Private function prototypes -----------------------------------------------*/
void RCC_Config(void);
void LED_Config(void);
void LED_Poll(void);
void Button_Config(void);
void Button_Poll(void);
void Relay_Config(void);
void Relay_Poll(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  主循环
  * @param  None
  * @retval None
  */
int main(void)
{

  //初始化 RTU模式 从机地址为1 USART1 9600 无校验
  eMBInit(MB_RTU, 0x01, 0x01, 38400, MB_PAR_NONE);
  //启动FreeModbus 
  eMBEnable();                
  while (1)
  {
    //不断循环
    eMBPoll(); 
    //LED 控制循环
    //LED_Poll(); 
    //
    //Button_Poll();
    //
    //Relay_Poll();
  }
}

/**
  * @brief  初始化时钟，主要为了初始化Systick
  * @param  None
  * @retval None
  */
void RCC_Config(void)
{
  //Systick时钟每1ms触发一次
  if (SysTick_Config(SystemCoreClock / 1000))
  { 
    //
    while (1);
  }  
}

/**
  * @brief  输入寄存器处理函数，输入寄存器可读，但不可写。
  * @param  pucRegBuffer  返回数据指针
  *         usAddress     寄存器起始地址
  *         usNRegs       寄存器长度
  * @retval eStatus       寄存器状态
  */
eMBErrorCode 
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
  eMBErrorCode    eStatus = MB_ENOERR;
  int16_t         iRegIndex;
  
  //查询是否在寄存器范围内
  //为了避免警告，修改为有符号整数
  if( ( (int16_t)usAddress >= REG_INPUT_START ) \
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
  {
    //获得操作偏移量，本次操作起始地址-输入寄存器的初始地址
    iRegIndex = ( int16_t )( usAddress - usRegInputStart );
    //逐个赋值
    while( usNRegs > 0 )
    {
      //赋值高字节
      *pucRegBuffer++ = ( uint8_t )( usRegInputBuf[iRegIndex] >> 8 );
      //赋值低字节
      *pucRegBuffer++ = ( uint8_t )( usRegInputBuf[iRegIndex] & 0xFF );
      //偏移量增加
      iRegIndex++;
      //被操作寄存器数量递减
      usNRegs--;
    }
  }
  else
  {
    //返回错误状态，无寄存器  
    eStatus = MB_ENOREG;
  }

  return eStatus;
}

/**
  * @brief  保持寄存器处理函数，保持寄存器可读，可读可写
  * @param  pucRegBuffer  读操作时--返回数据指针，写操作时--输入数据指针
  *         usAddress     寄存器起始地址
  *         usNRegs       寄存器长度
  *         eMode         操作方式，读或者写
  * @retval eStatus       寄存器状态
  */
eMBErrorCode 
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
  //错误状态
  eMBErrorCode    eStatus = MB_ENOERR;
  //偏移量
  int16_t         iRegIndex;
  
  //判断寄存器是不是在范围内
  if( ( (int16_t)usAddress >= REG_HOLDING_START ) \
     && ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
  {
    //计算偏移量
    iRegIndex = ( int16_t )( usAddress - usRegHoldingStart );
    
    switch ( eMode )
    {
      //读处理函数  
      case MB_REG_READ:
        while( usNRegs > 0 )
        {
          *pucRegBuffer++ = ( uint8_t )( usRegHoldingBuf[iRegIndex] >> 8 );
          *pucRegBuffer++ = ( uint8_t )( usRegHoldingBuf[iRegIndex] & 0xFF );
          iRegIndex++;
          usNRegs--;
        }
        break;

      //写处理函数 
      case MB_REG_WRITE:
        while( usNRegs > 0 )
        {
          usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
          usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
          iRegIndex++;
          usNRegs--;
        }
        break;
     }
  }
  else
  {
    //返回错误状态
    eStatus = MB_ENOREG;
  }
  
  return eStatus;
}


/**
  * @brief  线圈寄存器处理函数，线圈寄存器可读，可读可写
  * @param  pucRegBuffer  读操作---返回数据指针，写操作--返回数据指针
  *         usAddress     寄存器起始地址
  *         usNRegs       寄存器长度
  *         eMode         操作方式，读或者写
  * @retval eStatus       寄存器状态
  */
eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
  //错误状态
  eMBErrorCode    eStatus = MB_ENOERR;
  //寄存器个数
  int16_t         iNCoils = ( int16_t )usNCoils;
  //寄存器偏移量
  int16_t         usBitOffset;

  //检查寄存器是否在指定范围内
  if( ( (int16_t)usAddress >= REG_COILS_START ) &&
        ( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )
  {
    //计算寄存器偏移量
    usBitOffset = ( int16_t )( usAddress - REG_COILS_START );
    switch ( eMode )
    {
      //读操作
      case MB_REG_READ:
        while( iNCoils > 0 )
        {
          *pucRegBuffer++ = xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,
                                          ( uint8_t )( iNCoils > 8 ? 8 : iNCoils ) );
          iNCoils -= 8;
          usBitOffset += 8;
        }
        break;

      //写操作
      case MB_REG_WRITE:
        while( iNCoils > 0 )
        {
          xMBUtilSetBits( ucRegCoilsBuf, usBitOffset,
                        ( uint8_t )( iNCoils > 8 ? 8 : iNCoils ),
                        *pucRegBuffer++ );
          iNCoils -= 8;
        }
        break;
    }

  }
  else
  {
    eStatus = MB_ENOREG;
  }
  return eStatus;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
  //错误状态
  eMBErrorCode    eStatus = MB_ENOERR;
  //操作寄存器个数
  int16_t         iNDiscrete = ( int16_t )usNDiscrete;
  //偏移量
  uint16_t        usBitOffset;

  //判断寄存器时候再制定范围内
  if( ( (int16_t)usAddress >= REG_DISCRETE_START ) &&
        ( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE ) )
  {
    //获得偏移量
    usBitOffset = ( uint16_t )( usAddress - REG_DISCRETE_START );
    
    while( iNDiscrete > 0 )
    {
      *pucRegBuffer++ = xMBUtilGetBits( ucRegDiscreteBuf, usBitOffset,
                                      ( uint8_t)( iNDiscrete > 8 ? 8 : iNDiscrete ) );
      iNDiscrete -= 8;
      usBitOffset += 8;
    }

  }
  else
  {
    eStatus = MB_ENOREG;
  }
  return eStatus;
}

/**
  * @brief  LED初始化
  * @param  None
  * @retval None
  */
void LED_Config(void)
{
  //定义一个GPIO结构体
  GPIO_InitTypeDef  GPIO_InitStructure; 

  //初始化GPIOB时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB ,ENABLE);
  //GPIOB
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 ; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
}

/**
  * @brief  LED循环检测
  * @param  None
  * @retval None
  */
void LED_Poll(void)
{
  uint8_t LED_Status = ucRegCoilsBuf[0];
  
  if(LED_Status & 0x01) {LED1_ON();} else {LED1_OFF();}
  if(LED_Status & 0x02) {LED2_ON();} else {LED2_OFF();}
  if(LED_Status & 0x04) {LED3_ON();} else {LED3_OFF();}
  if(LED_Status & 0x08) {LED4_ON();} else {LED4_OFF();}
}

/**
  * @brief  拨码开关初始化
  * @param  None
  * @retval None
  */
void Button_Config(void)
{
  //定义一个GPIO结构体
  GPIO_InitTypeDef  GPIO_InitStructure; 

  //初始化GPIOB时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE ,ENABLE);
  //GPIOB
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | \
                                GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   //上拉输入
  GPIO_Init(GPIOE, &GPIO_InitStructure); 
}

/**
  * @brief  拨码开关检测循环
  * @param  None
  * @retval None
  */
void Button_Poll(void)
{
  //if(BUTTON1_READ()) {ucRegDiscreteBuf[0] |= 0x01;} else {ucRegDiscreteBuf[0] &=~ 0x01;}
  uint8_t Button_Status = 0x00;
  
  BUTTON1_READ()?(Button_Status &=~ 0x01):(Button_Status |= 0x01);
  BUTTON2_READ()?(Button_Status &=~ 0x02):(Button_Status |= 0x02); 
  BUTTON3_READ()?(Button_Status &=~ 0x04):(Button_Status |= 0x04);
  BUTTON4_READ()?(Button_Status &=~ 0x08):(Button_Status |= 0x08); 
  
  ucRegDiscreteBuf[0] = Button_Status;
}


void Relay_Config(void)
{
  //定义一个GPIO结构体
  GPIO_InitTypeDef  GPIO_InitStructure; 

  //初始化GPIOB时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC ,ENABLE);
  //GPIOB
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 ; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure); 

}


void Relay_Poll(void)
{
  
  uint8_t Relay_Status = ucRegCoilsBuf[0];
  
  if(Relay_Status & 0x01) {Relay1_ON();} else {Relay1_OFF();}
  if(Relay_Status & 0x02) {Relay2_ON();} else {Relay2_OFF();}
  if(Relay_Status & 0x04) {Relay3_ON();} else {Relay3_OFF();}
  if(Relay_Status & 0x08) {Relay4_ON();} else {Relay4_OFF();}
}
/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
