/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : MC_Keys.c
* Author             : IMS Systems Lab 
* Date First Issued  : 11/28/2007
* Description        : This file handles Joystick and button management
********************************************************************************
* History:
* 11/28/07 v1.0
* 05/29/08 v2.0
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, 
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING 
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOURCE CODE IS PROTECTED BY A LICENSE.
* FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
* IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "stm32f10x_MClib.h"
#include "MC_Globals.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define KEY_UP_PORT GPIOD
#define KEY_UP_BIT  GPIO_Pin_8

#define KEY_DOWN_PORT GPIOD
#define KEY_DOWN_BIT  GPIO_Pin_14

#define KEY_RIGHT_PORT GPIOE
#define KEY_RIGHT_BIT  GPIO_Pin_0

#define KEY_LEFT_PORT GPIOE
#define KEY_LEFT_BIT  GPIO_Pin_1

#define KEY_SEL_PORT GPIOD
#define KEY_SEL_BIT  GPIO_Pin_12

#define USER_BUTTON_PORT GPIOB
#define USER_BUTTON_BIT  GPIO_Pin_9

#define  SEL_FLAG        (u8)0x02
#define  RIGHT_FLAG      (u8)0x04
#define  LEFT_FLAG       (u8)0x08
#define  UP_FLAG         (u8)0x10
#define  DOWN_FLAG       (u8)0x20

//Variable increment and decrement

#define SPEED_INC_DEC    (u16)10
#define hKp_Gain_INC_DEC  (u16)100
#define hKi_Gain_INC_DEC  (u16)100
#define hKd_Gain_INC_DEC  (u16)50

#define TORQUE_INC_DEC   (u16)250
#define FLUX_INC_DEC     (u16)250

/* Private macro -------------------------------------------------------------*/

u8 KEYS_Read (void);

/* Private variables ---------------------------------------------------------*/

static u8 bKey;
static u8 bPrevious_key;
static u8 bKey_Flag;

u8 bMenu_index ;


/*******************************************************************************
* Function Name  : KEYS_Init
* Description    : Init GPIOs for joystick/button management
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void KEYS_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
    
  /* Enable GPIOA, GPIOB, GPIOC, GPIOE clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | 
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_GPIOE, ENABLE);
 
  GPIO_StructInit(&GPIO_InitStructure);
  
  /* Joystick GPIOs configuration*/
  
  GPIO_InitStructure.GPIO_Pin = KEY_UP_BIT;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(KEY_UP_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = KEY_DOWN_BIT;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(KEY_DOWN_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = KEY_RIGHT_BIT;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(KEY_RIGHT_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = KEY_LEFT_BIT;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(KEY_LEFT_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = KEY_SEL_BIT;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(KEY_SEL_PORT, &GPIO_InitStructure);
  
  /* User button GPIO configuration */
  
  GPIO_InitStructure.GPIO_Pin = USER_BUTTON_BIT;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(USER_BUTTON_PORT, &GPIO_InitStructure);
}
  
/*******************************************************************************
* Function Name  : KEYS_Read
* Description    : Reads key from demoboard.
* Input          : None
* Output         : None
* Return         : Return RIGHT, LEFT, SEL, UP, DOWN, KEY_HOLD or NOKEY
*******************************************************************************/
u8 KEYS_Read ( void )
{
  /* "RIGHT" key is pressed */
  if(!GPIO_ReadInputDataBit(KEY_RIGHT_PORT, KEY_RIGHT_BIT))
  {
    if (bPrevious_key == RIGHT) 
    {
      return KEY_HOLD;
    }
    else
    {
      bPrevious_key = RIGHT;
      return RIGHT;
    }
  }
  /* "LEFT" key is pressed */
  else if(!GPIO_ReadInputDataBit(KEY_LEFT_PORT, KEY_LEFT_BIT))
  {
    if (bPrevious_key == LEFT) 
    {
      return KEY_HOLD;
    }
    else
    {
      bPrevious_key = LEFT;
      return LEFT;
    }
  }
  /* "SEL" key is pressed */
   if(!GPIO_ReadInputDataBit(KEY_SEL_PORT, KEY_SEL_BIT))
  {
    if (bPrevious_key == SEL) 
    {
      return KEY_HOLD;
    }
    else
    {
      if ( (TB_DebounceDelay_IsElapsed() == FALSE) && (bKey_Flag & SEL_FLAG == SEL_FLAG) )
      {
        return NOKEY;
      }
      else
      {
      if ( (TB_DebounceDelay_IsElapsed() == TRUE) && ( (bKey_Flag & SEL_FLAG) == 0) ) 
      {
        bKey_Flag |= SEL_FLAG;
        TB_Set_DebounceDelay_500us(100); // 50 ms debounce
      }
      else if ( (TB_DebounceDelay_IsElapsed() == TRUE) && ((bKey_Flag & SEL_FLAG) == SEL_FLAG) )
      {
        bKey_Flag &= (u8)(~SEL_FLAG);
        bPrevious_key = SEL;
        return SEL;
      }
      return NOKEY;
      }
    }
  }
  /* "SEL" key is pressed */
  else if(!GPIO_ReadInputDataBit(USER_BUTTON_PORT, USER_BUTTON_BIT))
  {
    if (bPrevious_key == SEL) 
    {
      return KEY_HOLD;
    }
    else
    {
      if ( (TB_DebounceDelay_IsElapsed() == FALSE) && (bKey_Flag & SEL_FLAG == SEL_FLAG) )
      {
        return NOKEY;
      }
      else
      {
      if ( (TB_DebounceDelay_IsElapsed() == TRUE) && ( (bKey_Flag & SEL_FLAG) == 0) ) 
      {
        bKey_Flag |= SEL_FLAG;
        TB_Set_DebounceDelay_500us(100); // 50 ms debounce
      }
      else if ( (TB_DebounceDelay_IsElapsed() == TRUE) && ((bKey_Flag & SEL_FLAG) == SEL_FLAG) )
      {
        bKey_Flag &= (u8)(~SEL_FLAG);
        bPrevious_key = SEL;
        return SEL;
      }
      return NOKEY;
      }
    }
  }
   /* "UP" key is pressed */
  else if(!GPIO_ReadInputDataBit(KEY_UP_PORT, KEY_UP_BIT))
  {
    if (bPrevious_key == UP) 
    {
      return KEY_HOLD;
    }
    else
    {
      bPrevious_key = UP;
      return UP;
    }
  }
  /* "DOWN" key is pressed */
  else if(!GPIO_ReadInputDataBit(KEY_DOWN_PORT, KEY_DOWN_BIT))
  {
    if (bPrevious_key == DOWN) 
    {
      return KEY_HOLD;
    }
    else
    {
      bPrevious_key = DOWN;
      return DOWN;
    }
  }
  
  /* No key is pressed */
  else
  {
    bPrevious_key = NOKEY;
    return NOKEY;
  }
}



/*******************************************************************************
* Function Name  : KEYS_process
* Description    : Process key 
* Input          : Key code
* Output         : None
* Return         : None
*******************************************************************************/

void KEYS_process(void)
{
bKey = KEYS_Read();    // read key pushed (if any...)

switch (bMenu_index)
      {        
        case(CONTROL_MODE_MENU_1):
          switch(bKey)
          {
            case UP:
            case DOWN:
              wGlobal_Flags ^= CLOSED_LOOP;
              PID_Init(&PID_Torque_InitStructure, &PID_Flux_InitStructure, &PID_Speed_InitStructure);
              bMenu_index = CONTROL_MODE_MENU_6;
            break;
            
            case RIGHT:
                bMenu_index = REF_SPEED_MENU;
            break;
            
            case LEFT:
#ifdef DAC_FUNCTIONALITY              
              bMenu_index = DAC_PB1_MENU;
#else
              bMenu_index = POWER_STAGE_MENU;
#endif
            break;
            
            case SEL:
              if (State == RUN)
              {
                State = STOP;               
              }
              else if (State== START)
              {
                State = STOP; 
              }
                else if(State == IDLE)
                {
                  State = INIT;
                  bMenu_index = REF_SPEED_MENU;
                }  
              
            break;
            default:
            break;
          }
        break;
        
        case(REF_SPEED_MENU):
          switch(bKey)
          {
            case UP:
              if (hSpeed_Reference <= MAX_FREQ)
              {
                hSpeed_Reference += SPEED_INC_DEC;
              }
            break;
              
            case DOWN:
              if (hSpeed_Reference >= -MAX_FREQ)
              {
                hSpeed_Reference -= SPEED_INC_DEC;
              }
            break;
           
            case RIGHT:
                bMenu_index = P_SPEED_MENU;
            break;

            case LEFT:
              if (State == IDLE)
              {
                bMenu_index = CONTROL_MODE_MENU_1;
              }
              else
              {
#ifdef DAC_FUNCTIONALITY              
              bMenu_index = DAC_PB1_MENU;
#else              
              bMenu_index = POWER_STAGE_MENU;
#endif
              }              
            break;
            
            case SEL:
              if (State == RUN)
              {
                State = STOP;               
              }
              else if(State == START)
              {
                State = STOP;
              }              
                else if(State == IDLE)
                {
                  State = INIT;
                }   
            break;
            default:
            break;
          }
        break;          
        
        case(P_SPEED_MENU):    
          switch(bKey)
          {
            case UP:
              if (PID_Speed_InitStructure.hKp_Gain <= 32500)
              {
                PID_Speed_InitStructure.hKp_Gain += hKp_Gain_INC_DEC;
              }
            break;
              
            case DOWN:
              if (PID_Speed_InitStructure.hKp_Gain >= hKp_Gain_INC_DEC)
              {
              PID_Speed_InitStructure.hKp_Gain -= hKp_Gain_INC_DEC;
              }
            break;
           
            case RIGHT:
                bMenu_index = I_SPEED_MENU;
            break;

            case LEFT:
                bMenu_index = REF_SPEED_MENU;             
            break;
            
            case SEL:
              if (State == RUN)
              {
                State = STOP;               
              }
              else if (State== START)
              {
                State = STOP; 
              }
                else if(State == IDLE)
                {
                  State = INIT;
                }   
            break;
          default:
            break;
          }
        break;

        case(I_SPEED_MENU):    
          switch(bKey)
          {
            case UP:
              if (PID_Speed_InitStructure.hKi_Gain <= 32500)
              {
                PID_Speed_InitStructure.hKi_Gain += hKi_Gain_INC_DEC;
              }
            break;
              
            case DOWN:
              if (PID_Speed_InitStructure.hKi_Gain >= hKi_Gain_INC_DEC)
              {
              PID_Speed_InitStructure.hKi_Gain -= hKi_Gain_INC_DEC;
              }
            break;
           
            case RIGHT:
#ifdef DIFFERENTIAL_TERM_ENABLED                 
                bMenu_index = D_SPEED_MENU;
#else
                bMenu_index = P_TORQUE_MENU;
#endif                
            break;

            case LEFT:
              bMenu_index = P_SPEED_MENU;
            break;
            
            case SEL:
              if (State == RUN)
              {
                State = STOP;               
              }
              else if (State== START)
              {
                State = STOP; 
              }
                else if(State == IDLE)
                {
                  State = INIT;
                }   
            break;
          default:
            break;
          }
        break;        
        
#ifdef DIFFERENTIAL_TERM_ENABLED       
        case(D_SPEED_MENU):    
          switch(bKey)
          {
            case UP:
              if (PID_Speed_InitStructure.hKd_Gain <= 32500)
              {
                PID_Speed_InitStructure.hKd_Gain += hKd_Gain_INC_DEC;
              }
            break;
              
            case DOWN:
              if (PID_Speed_InitStructure.hKd_Gain >= hKd_Gain_INC_DEC)
              {
              PID_Speed_InitStructure.hKd_Gain -= hKd_Gain_INC_DEC;
              }
            break;
           
            case RIGHT:                
                bMenu_index = P_TORQUE_MENU;               
            break;
            
            case LEFT:
              bMenu_index = I_SPEED_MENU;
            break;
            
            case SEL:
              if (State == RUN || State == START)
              {
                State = STOP;               
              }
              else if(State == IDLE)
              {
                State = INIT;
              }   
            break;
          default:
            break;
          }
        break;   
#endif
        case(P_TORQUE_MENU):    
          switch(bKey)
          {
            case UP:
              if (PID_Torque_InitStructure.hKp_Gain <= 32500)
              {
                PID_Torque_InitStructure.hKp_Gain += hKp_Gain_INC_DEC;
              }
            break;
              
            case DOWN:
              if (PID_Torque_InitStructure.hKp_Gain >= hKp_Gain_INC_DEC)
              {
              PID_Torque_InitStructure.hKp_Gain -= hKp_Gain_INC_DEC;
              }
            break;
           
            case RIGHT:
                bMenu_index = I_TORQUE_MENU;
            break;
             
            case LEFT:         
              if ((wGlobal_Flags & CLOSED_LOOP) == CLOSED_LOOP)
              {
#ifdef DIFFERENTIAL_TERM_ENABLED              
                bMenu_index = D_SPEED_MENU;
#else
                bMenu_index = I_SPEED_MENU; 
#endif                
              }
              else
              {
                bMenu_index = ID_REF_MENU;
              }              
            break;
            
            case SEL:
              if (State == RUN)
              {
                State = STOP;               
              }
              else if (State== START)
                {
                  State = STOP; 
                }
                  else if(State == IDLE)
                  {
                    State = INIT;
                  }   
            break;
          default:
            break;
          }
        break;
        
        case(I_TORQUE_MENU):    
          switch(bKey)
          {
            case UP:
              if (PID_Torque_InitStructure.hKi_Gain <= 32500)
              {
                PID_Torque_InitStructure.hKi_Gain += hKi_Gain_INC_DEC;
              }
            break;
              
            case DOWN:
              if (PID_Torque_InitStructure.hKi_Gain >= hKi_Gain_INC_DEC)
              {
              PID_Torque_InitStructure.hKi_Gain -= hKi_Gain_INC_DEC;
              }
            break;
           
            case RIGHT:
#ifdef DIFFERENTIAL_TERM_ENABLED                 
                bMenu_index = D_TORQUE_MENU;
#else
                bMenu_index = P_FLUX_MENU;
#endif                
            break;

            case LEFT:
              bMenu_index = P_TORQUE_MENU;
            break;
            
            case SEL:
              if (State == RUN)
              {
                State = STOP;               
              }
                else if (State== START)
                {
                  State = STOP; 
                }
                  else if(State == IDLE)
                  {
                    State = INIT;
                  }   
            break;
          default:
            break;
          }
        break;
        
#ifdef DIFFERENTIAL_TERM_ENABLED       
        case(D_TORQUE_MENU):    
          switch(bKey)
          {
            case UP:
              if (PID_Torque_InitStructure.hKd_Gain <= 32500)
              {
                PID_Torque_InitStructure.hKd_Gain += hKd_Gain_INC_DEC;
              }
            break;
              
            case DOWN:
              if (PID_Torque_InitStructure.hKd_Gain >= hKd_Gain_INC_DEC)
              {
              PID_Torque_InitStructure.hKd_Gain -= hKd_Gain_INC_DEC;
              }
            break;
           
            case RIGHT:                
                bMenu_index = P_FLUX_MENU;               
            break;
            
            case LEFT:
              bMenu_index = I_TORQUE_MENU;
            break;
            
            case SEL:
              if (State == RUN || State == START)
              {
                State = STOP;               
              }
              else if(State == IDLE)
              {
                State = INIT;
              }   
            break;
          default:
            break;
          }
        break;   
#endif 
        case(P_FLUX_MENU):    
          switch(bKey)
          {
            case UP:
              if (PID_Flux_InitStructure.hKp_Gain <= 32500)
              {
                PID_Flux_InitStructure.hKp_Gain += hKp_Gain_INC_DEC;
              }
            break;
              
            case DOWN:
              if (PID_Flux_InitStructure.hKp_Gain >= hKp_Gain_INC_DEC)
              {
                PID_Flux_InitStructure.hKp_Gain -= hKp_Gain_INC_DEC;
              }
            break;
           
            case RIGHT:
                bMenu_index = I_FLUX_MENU;
            break;
             
            case LEFT:         
#ifdef  DIFFERENTIAL_TERM_ENABLED
              bMenu_index = D_TORQUE_MENU;
#else
              bMenu_index = I_TORQUE_MENU;
#endif              
            break;
            
            case SEL:
              if (State == RUN)
              {
                State = STOP;               
              }
                else if (State== START)
                {
                  State = STOP; 
                }
                  else if(State == IDLE)
                  {
                    State = INIT;
                  }   
            break;
          default:
            break;
          }
        break;
        
        case(I_FLUX_MENU):    
          switch(bKey)
          {
            case UP:
              if (PID_Flux_InitStructure.hKi_Gain <= 32500)
              {
                PID_Flux_InitStructure.hKi_Gain += hKi_Gain_INC_DEC;
              }
            break;
              
            case DOWN:
              if (PID_Flux_InitStructure.hKi_Gain >= hKi_Gain_INC_DEC)
              {
              PID_Flux_InitStructure.hKi_Gain -= hKi_Gain_INC_DEC;
              }
            break;
           
            case RIGHT:
#ifdef DIFFERENTIAL_TERM_ENABLED                 
                bMenu_index = D_FLUX_MENU;
#else
                bMenu_index = POWER_STAGE_MENU;
#endif                
            break;

            case LEFT:
              bMenu_index = P_FLUX_MENU;
            break;
            
            case SEL:
              if (State == RUN)
              {
                State = STOP;               
              }
                else if (State== START)
                {
                  State = STOP; 
                }
                  else if(State == IDLE)
                  {
                    State = INIT;
                  }   
            break;
          default:
            break;
          }
        break;
        
#ifdef DIFFERENTIAL_TERM_ENABLED       
        case(D_FLUX_MENU):    
          switch(bKey)
          {
            case UP:
              if (PID_Flux_InitStructure.hKd_Gain <= 32500)
              {
                PID_Flux_InitStructure.hKd_Gain += hKd_Gain_INC_DEC;
              }
            break;
              
            case DOWN:
              if (PID_Flux_InitStructure.hKd_Gain >= hKd_Gain_INC_DEC)
              {
              PID_Flux_InitStructure.hKd_Gain -= hKd_Gain_INC_DEC;
              }
            break;
           
            case RIGHT:                
                bMenu_index = POWER_STAGE_MENU;               
            break;
            
            case LEFT:              
              bMenu_index = I_FLUX_MENU;
            break;
            
            case SEL:
              if (State == RUN || State == START)
              {
                State = STOP;               
              }
              else if(State == IDLE)
              {
                State = INIT;
              }   
            break;
          default:
            break;
          }
        break;   
#endif 
        case(POWER_STAGE_MENU):
          switch(bKey)
          {
            case RIGHT:       

#ifdef DAC_FUNCTIONALITY
              bMenu_index = DAC_PB0_MENU;
#else 
              if ((wGlobal_Flags & CLOSED_LOOP == CLOSED_LOOP))
              {
                if (State == IDLE)
                {
                  bMenu_index = CONTROL_MODE_MENU_1;
                }
                else
                {
                  bMenu_index = REF_SPEED_MENU;
                }
              }
              else //Open Loop
              {
                if (State == IDLE)
                {
                  bMenu_index = CONTROL_MODE_MENU_6;
                }
                else
                {
                  bMenu_index = IQ_REF_MENU;
                }
              }
#endif  
            break;
            
            case LEFT:
#ifdef DIFFERENTIAL_TERM_ENABLED            
              bMenu_index = D_FLUX_MENU;
#else
              bMenu_index = I_FLUX_MENU;                          
#endif              
            break;
            
            case SEL:
              if (State == RUN)
              {
                State = STOP;               
              }
                else if (State== START)
                {
                  State = STOP; 
                }
                  else if(State == IDLE)
                  {
                    State = INIT;
                  }   
            break;
          default:
            break;
          }
        break; 
        
        case(CONTROL_MODE_MENU_6):
          switch(bKey)
          {
            case UP:
            case DOWN:
              wGlobal_Flags ^= CLOSED_LOOP;
              bMenu_index = CONTROL_MODE_MENU_1;
            break;
            
            case RIGHT:
                bMenu_index = IQ_REF_MENU;
            break;
            
            case LEFT:
#ifdef DAC_FUNCTIONALITY              
              bMenu_index = DAC_PB1_MENU;
#else              
              bMenu_index = POWER_STAGE_MENU;
#endif 
            break;
            
            case SEL:
              if (State == RUN)
              {
                State = STOP;               
              }
                else if (State== START)
                {
                  State = STOP; 
                }
                  else if(State == IDLE)
                  {
                    State = INIT;
                    bMenu_index = IQ_REF_MENU;
                  }   
            break;
            default:
            break;
          }
        break;  
        
        case(IQ_REF_MENU):
          switch(bKey)
          {
            case UP:
              if (hTorque_Reference <= NOMINAL_TORQUE-TORQUE_INC_DEC)
              {
                hTorque_Reference += TORQUE_INC_DEC;
              }
            break;
            
            case DOWN:
            if (hTorque_Reference >= -NOMINAL_TORQUE + TORQUE_INC_DEC)
            {
              hTorque_Reference -= TORQUE_INC_DEC;
            }
            break;
            
            case RIGHT:
                bMenu_index = ID_REF_MENU;
            break;
            
            case LEFT:
              if(State == IDLE)
              {
                bMenu_index = CONTROL_MODE_MENU_6;
              }
              else
              {
#ifdef DAC_FUNCTIONALITY              
              bMenu_index = DAC_PB1_MENU;
#else              
              bMenu_index = POWER_STAGE_MENU;
#endif 
              }
            break;
            
            case SEL:
              if (State == RUN)
              {
                State = STOP;               
              }
                else if (State== START)
                {
                  State = STOP; 
                }
                  else if(State == IDLE)
                  {
                    State = INIT;
                  }   
            break;
            default:
            break;
          }
        break;    
        
        case(ID_REF_MENU):
          switch(bKey)
          {
            case UP:
              if (hFlux_Reference <= NOMINAL_FLUX-FLUX_INC_DEC)
              {
                hFlux_Reference += FLUX_INC_DEC;
              }
            break;
            
            case DOWN:
              if (hFlux_Reference >= FLUX_INC_DEC)
              {
                hFlux_Reference -= FLUX_INC_DEC;
              }
            break;
            
            case RIGHT:
              bMenu_index = P_TORQUE_MENU;
            break;
            
            case LEFT:
              bMenu_index = IQ_REF_MENU;
            break;
            
            case SEL:
              if (State == RUN)
              {
                State = STOP;               
              }
                else if (State== START)
                {
                  State = STOP; 
                }
                  else if(State == IDLE)
                  {
                    State = INIT;
                  }   
            break;
            default:
            break;
          }
        break;

#ifdef DAC_FUNCTIONALITY  
        case(DAC_PB0_MENU):    
          switch(bKey)
          {
            case UP:
              MCDAC_Output_Choice(1,DAC_CH1);
            break;
            
            case DOWN:
              MCDAC_Output_Choice(-1,DAC_CH1);
            break;  
           
            case RIGHT:                
                bMenu_index = DAC_PB1_MENU;               
            break;
            
            case LEFT:
              bMenu_index = POWER_STAGE_MENU;
            break;
            
            case SEL:
              if (State == RUN || State == START)
              {
                State = STOP;               
              }
              else if(State == IDLE)
              {
                State = INIT;
              }   
            break;
          default:
            break;
          }
        break;   

        case(DAC_PB1_MENU):    
          switch(bKey)
          {
            case UP:
              MCDAC_Output_Choice(1,DAC_CH2);
            break;
            
            case DOWN:
              MCDAC_Output_Choice(-1,DAC_CH2);
            break;  
           
            case RIGHT:                
            if ((wGlobal_Flags & CLOSED_LOOP == CLOSED_LOOP))
              {
                if (State == IDLE)
                {
                  bMenu_index = CONTROL_MODE_MENU_1;
                }
                else
                {
                  bMenu_index = REF_SPEED_MENU;
                }
              }
              else //Open Loop
              {
                if (State == IDLE)
                {
                  bMenu_index = CONTROL_MODE_MENU_6;
                }
                else
                {
                  bMenu_index = IQ_REF_MENU;
                }
              }               
            break;
            
            case LEFT:             
              bMenu_index = DAC_PB0_MENU;
            break;
            
            case SEL:
              if (State == RUN || State == START)
              {
                State = STOP;               
              }
              else if(State == IDLE)
              {
                State = INIT;
              }   
            break;
          default:
            break;
          }
        break; 
#endif 
      default:
        break; 
      }
}

/*******************************************************************************
* Function Name  : KEYS_ExportbKey
* Description    : Export bKey variable
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u8 KEYS_ExportbKey(void)
{
  return(bKey);
}
                   
/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/

