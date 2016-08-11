#include "stm32f10x.h"
#include <stdio.h>

#include "mb.h"
#include "mbutils.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//����Ĵ�����ʼ��ַ
#define REG_INPUT_START       0x0000
//����Ĵ�������
#define REG_INPUT_NREGS       8
//���ּĴ�����ʼ��ַ
#define REG_HOLDING_START     0x0000
//���ּĴ�������
#define REG_HOLDING_NREGS     8

//��Ȧ��ʼ��ַ
#define REG_COILS_START       0x0000
//��Ȧ����
#define REG_COILS_SIZE        16

//���ؼĴ�����ʼ��ַ
#define REG_DISCRETE_START    0x0000
//���ؼĴ�������
#define REG_DISCRETE_SIZE     16

/* Private macro -------------------------------------------------------------*/
//LED����
#define LED1_OFF()          GPIO_SetBits(GPIOB,GPIO_Pin_9)
#define LED1_ON()           GPIO_ResetBits(GPIOB,GPIO_Pin_9)

#define LED2_OFF()          GPIO_SetBits(GPIOB,GPIO_Pin_8)
#define LED2_ON()           GPIO_ResetBits(GPIOB,GPIO_Pin_8)

#define LED3_OFF()          GPIO_SetBits(GPIOB,GPIO_Pin_7)
#define LED3_ON()           GPIO_ResetBits(GPIOB,GPIO_Pin_7)

#define LED4_OFF()          GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define LED4_ON()           GPIO_ResetBits(GPIOB,GPIO_Pin_6)

//���뿪�ؿ���
#define BUTTON1_READ()      GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_8)
#define BUTTON2_READ()      GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_9)
#define BUTTON3_READ()      GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_10)
#define BUTTON4_READ()      GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_11)
#define BUTTON5_READ()      GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_12)
#define BUTTON6_READ()      GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_13)
#define BUTTON7_READ()      GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_14)
#define BUTTON8_READ()      GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_15)

//�̵�������
#define Relay1_ON()         GPIO_SetBits(GPIOC,GPIO_Pin_0)
#define Relay1_OFF()        GPIO_ResetBits(GPIOC,GPIO_Pin_0)

#define Relay2_ON()         GPIO_SetBits(GPIOC,GPIO_Pin_1)
#define Relay2_OFF()        GPIO_ResetBits(GPIOC,GPIO_Pin_1)

#define Relay3_ON()         GPIO_SetBits(GPIOC,GPIO_Pin_2)
#define Relay3_OFF()        GPIO_ResetBits(GPIOC,GPIO_Pin_2)

#define Relay4_ON()         GPIO_SetBits(GPIOC,GPIO_Pin_3)
#define Relay4_OFF()        GPIO_ResetBits(GPIOC,GPIO_Pin_3)


/* Private variables ---------------------------------------------------------*/
//����Ĵ�������
uint16_t usRegInputBuf[REG_INPUT_NREGS] = {0x1000,0x1001,0x1002,0x1003,0x1004,0x1005,0x1006,0x1007};
//�Ĵ�����ʼ��ַ
uint16_t usRegInputStart = REG_INPUT_START;

//���ּĴ�������
uint16_t usRegHoldingBuf[REG_HOLDING_NREGS] = {0x147b,0x3f8e,0x147b,0x400e,0x1eb8,0x4055,0x147b,0x408e};
//���ּĴ�����ʼ��ַ
uint16_t usRegHoldingStart = REG_HOLDING_START;

//��Ȧ״̬
uint8_t ucRegCoilsBuf[REG_COILS_SIZE / 8] = {0x01,0x02};
//��������״̬
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
  * @brief  ��ѭ��
  * @param  None
  * @retval None
  */
int main(void)
{

  //��ʼ�� RTUģʽ �ӻ���ַΪ1 USART1 9600 ��У��
  eMBInit(MB_RTU, 0x01, 0x01, 38400, MB_PAR_NONE);
  //����FreeModbus 
  eMBEnable();                
  while (1)
  {
    //����ѭ��
    eMBPoll(); 
    //LED ����ѭ��
    //LED_Poll(); 
    //
    //Button_Poll();
    //
    //Relay_Poll();
  }
}

/**
  * @brief  ��ʼ��ʱ�ӣ���ҪΪ�˳�ʼ��Systick
  * @param  None
  * @retval None
  */
void RCC_Config(void)
{
  //Systickʱ��ÿ1ms����һ��
  if (SysTick_Config(SystemCoreClock / 1000))
  { 
    //
    while (1);
  }  
}

/**
  * @brief  ����Ĵ���������������Ĵ����ɶ���������д��
  * @param  pucRegBuffer  ��������ָ��
  *         usAddress     �Ĵ�����ʼ��ַ
  *         usNRegs       �Ĵ�������
  * @retval eStatus       �Ĵ���״̬
  */
eMBErrorCode 
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
  eMBErrorCode    eStatus = MB_ENOERR;
  int16_t         iRegIndex;
  
  //��ѯ�Ƿ��ڼĴ�����Χ��
  //Ϊ�˱��⾯�棬�޸�Ϊ�з�������
  if( ( (int16_t)usAddress >= REG_INPUT_START ) \
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
  {
    //��ò���ƫ���������β�����ʼ��ַ-����Ĵ����ĳ�ʼ��ַ
    iRegIndex = ( int16_t )( usAddress - usRegInputStart );
    //�����ֵ
    while( usNRegs > 0 )
    {
      //��ֵ���ֽ�
      *pucRegBuffer++ = ( uint8_t )( usRegInputBuf[iRegIndex] >> 8 );
      //��ֵ���ֽ�
      *pucRegBuffer++ = ( uint8_t )( usRegInputBuf[iRegIndex] & 0xFF );
      //ƫ��������
      iRegIndex++;
      //�������Ĵ��������ݼ�
      usNRegs--;
    }
  }
  else
  {
    //���ش���״̬���޼Ĵ���  
    eStatus = MB_ENOREG;
  }

  return eStatus;
}

/**
  * @brief  ���ּĴ��������������ּĴ����ɶ����ɶ���д
  * @param  pucRegBuffer  ������ʱ--��������ָ�룬д����ʱ--��������ָ��
  *         usAddress     �Ĵ�����ʼ��ַ
  *         usNRegs       �Ĵ�������
  *         eMode         ������ʽ��������д
  * @retval eStatus       �Ĵ���״̬
  */
eMBErrorCode 
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
  //����״̬
  eMBErrorCode    eStatus = MB_ENOERR;
  //ƫ����
  int16_t         iRegIndex;
  
  //�жϼĴ����ǲ����ڷ�Χ��
  if( ( (int16_t)usAddress >= REG_HOLDING_START ) \
     && ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
  {
    //����ƫ����
    iRegIndex = ( int16_t )( usAddress - usRegHoldingStart );
    
    switch ( eMode )
    {
      //��������  
      case MB_REG_READ:
        while( usNRegs > 0 )
        {
          *pucRegBuffer++ = ( uint8_t )( usRegHoldingBuf[iRegIndex] >> 8 );
          *pucRegBuffer++ = ( uint8_t )( usRegHoldingBuf[iRegIndex] & 0xFF );
          iRegIndex++;
          usNRegs--;
        }
        break;

      //д������ 
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
    //���ش���״̬
    eStatus = MB_ENOREG;
  }
  
  return eStatus;
}


/**
  * @brief  ��Ȧ�Ĵ�������������Ȧ�Ĵ����ɶ����ɶ���д
  * @param  pucRegBuffer  ������---��������ָ�룬д����--��������ָ��
  *         usAddress     �Ĵ�����ʼ��ַ
  *         usNRegs       �Ĵ�������
  *         eMode         ������ʽ��������д
  * @retval eStatus       �Ĵ���״̬
  */
eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
  //����״̬
  eMBErrorCode    eStatus = MB_ENOERR;
  //�Ĵ�������
  int16_t         iNCoils = ( int16_t )usNCoils;
  //�Ĵ���ƫ����
  int16_t         usBitOffset;

  //���Ĵ����Ƿ���ָ����Χ��
  if( ( (int16_t)usAddress >= REG_COILS_START ) &&
        ( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )
  {
    //����Ĵ���ƫ����
    usBitOffset = ( int16_t )( usAddress - REG_COILS_START );
    switch ( eMode )
    {
      //������
      case MB_REG_READ:
        while( iNCoils > 0 )
        {
          *pucRegBuffer++ = xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,
                                          ( uint8_t )( iNCoils > 8 ? 8 : iNCoils ) );
          iNCoils -= 8;
          usBitOffset += 8;
        }
        break;

      //д����
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
  //����״̬
  eMBErrorCode    eStatus = MB_ENOERR;
  //�����Ĵ�������
  int16_t         iNDiscrete = ( int16_t )usNDiscrete;
  //ƫ����
  uint16_t        usBitOffset;

  //�жϼĴ���ʱ�����ƶ���Χ��
  if( ( (int16_t)usAddress >= REG_DISCRETE_START ) &&
        ( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE ) )
  {
    //���ƫ����
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
  * @brief  LED��ʼ��
  * @param  None
  * @retval None
  */
void LED_Config(void)
{
  //����һ��GPIO�ṹ��
  GPIO_InitTypeDef  GPIO_InitStructure; 

  //��ʼ��GPIOBʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB ,ENABLE);
  //GPIOB
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 ; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
}

/**
  * @brief  LEDѭ�����
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
  * @brief  ���뿪�س�ʼ��
  * @param  None
  * @retval None
  */
void Button_Config(void)
{
  //����һ��GPIO�ṹ��
  GPIO_InitTypeDef  GPIO_InitStructure; 

  //��ʼ��GPIOBʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE ,ENABLE);
  //GPIOB
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | \
                                GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   //��������
  GPIO_Init(GPIOE, &GPIO_InitStructure); 
}

/**
  * @brief  ���뿪�ؼ��ѭ��
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
  //����һ��GPIO�ṹ��
  GPIO_InitTypeDef  GPIO_InitStructure; 

  //��ʼ��GPIOBʱ��
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
