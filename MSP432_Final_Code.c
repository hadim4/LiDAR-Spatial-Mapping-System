// Final Project C code - Mohammad Hadin

#include <stdint.h>
#include "PLL.h"
#include "SysTick.h"
#include "uart.h"
#include "onboardLEDs.h"
#include "tm4c1294ncpdt.h"
#include "VL53L1X_api.h"

#define I2C_MCS_ACK             0x00000008  // Data Acknowledge Enable
#define I2C_MCS_DATACK          0x00000008  // Acknowledge Data
#define I2C_MCS_ADRACK          0x00000004  // Acknowledge Address
#define I2C_MCS_STOP            0x00000004  // Generate STOP
#define I2C_MCS_START           0x00000002  // Generate START
#define I2C_MCS_ERROR           0x00000002  // Error
#define I2C_MCS_RUN             0x00000001  // I2C Master Enable
#define I2C_MCS_BUSY            0x00000001  // I2C Busy
#define I2C_MCR_MFE             0x00000010  // I2C Master Function Enable

#define MAXRETRIES              5           // number of receive attempts before giving up
void PortJ_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R8;			
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R8) == 0){};	
  GPIO_PORTJ_DIR_R &= ~0x03;    								
  GPIO_PORTJ_DEN_R |= 0x03;     									
	
	GPIO_PORTJ_PCTL_R &= ~0x000000F0;	 								
	GPIO_PORTJ_AMSEL_R &= ~0x03;											
	GPIO_PORTJ_PUR_R |= 0x03;													
	}

void I2C_Init(void){
  SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R0;                // activate I2C0
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;          	// activate port B
  while((SYSCTL_PRGPIO_R&0x0002) == 0){};				// ready?

    GPIO_PORTB_AFSEL_R |= 0x0C;           				// 3) enable alt funct on PB2,3       0b00001100
    GPIO_PORTB_ODR_R |= 0x08;             				// 4) enable open drain on PB3 only

    GPIO_PORTB_DEN_R |= 0x0C;             				// 5) enable digital I/O on PB2,3
//    GPIO_PORTB_AMSEL_R &= ~0x0C;          			// 7) disable analog functionality on PB2,3

                                                                            // 6) configure PB2,3 as I2C
//  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFF00FF)+0x00003300;
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFF00FF)+0x00002200;    //TED
    I2C0_MCR_R = I2C_MCR_MFE;                      							// 9) master function enable
    I2C0_MTPR_R = 0b0000000000000101000000000111011;                       	// 8) configure for 100 kbps clock (added 8 clocks of glitch suppression ~50ns)
//    I2C0_MTPR_R = 0x3B;                                        			// 8) configure for 100 kbps clock
        
}

//The VL53L1X needs to be reset using XSHUT.  We will use PG0
void PortG_Init(void){
    //Use PortG0
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R6;                // activate clock for Port N
    while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R6) == 0){};       // allow time for clock to stabilize
    GPIO_PORTG_DIR_R &= 0x00;                                      // make PG0 in (HiZ)
  GPIO_PORTG_AFSEL_R &= ~0x01;                                     // disable alt funct on PG0
  GPIO_PORTG_DEN_R |= 0x01;                                        // enable digital I/O on PG0
                                                                   // configure PG0 as GPIO
  //GPIO_PORTN_PCTL_R = (GPIO_PORTN_PCTL_R&0xFFFFFF00)+0x00000000;
  GPIO_PORTG_AMSEL_R &= ~0x01;                                     // disable analog functionality on PN0

    return;
}

//XSHUT     This pin is an active-low shutdown input; 
//					the board pulls it up to VDD to enable the sensor by default. 
//					Driving this pin low puts the sensor into hardware standby. This input is not level-shifted.
void VL53L1X_XSHUT(void){
    GPIO_PORTG_DIR_R |= 0x01;                            // make PG0 out
    GPIO_PORTG_DATA_R &= 0b11111110;                     //PG0 = 0
    FlashAllLEDs();
    SysTick_Wait10ms(10);
    GPIO_PORTG_DIR_R &= ~0x01;                           // make PG0 input (HiZ)
    
}

//STEPPER MOTOR INPUT
void PortH_Init(void){
	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R7;			
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R7) == 0){};	
	GPIO_PORTH_DIR_R |= 0x0F;        							
  GPIO_PORTH_AFSEL_R &= ~0x0F;     							
  GPIO_PORTH_DEN_R |= 0x0F;        						
																								
  GPIO_PORTH_AMSEL_R &= ~0x0F;     					
	return;
}

//POPT N - D1/D2
void PortN_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R12;				
	while((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R12) == 0){};	
	GPIO_PORTN_DIR_R |= 0x03;        							
  GPIO_PORTN_AFSEL_R &= ~0x03;     						
  GPIO_PORTN_DEN_R |= 0x03;        							
																									
  
  GPIO_PORTN_AMSEL_R &= ~0x03;    
	}

//POPT F - D3/D4
void PortF_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;				
	while((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R5) == 0){};	
	GPIO_PORTF_DIR_R |= 0x11;        							
  GPIO_PORTF_AFSEL_R &= ~0x11;     						
  GPIO_PORTF_DEN_R |= 0x11;        							
																									
  
  GPIO_PORTF_AMSEL_R &= ~0x11;   
	}

	
//FUNCTIONS TO TURN ON AND OFF ALL ONBOARD LEDS
void D1_ON() {
	GPIO_PORTN_DATA_R |= 0b00000010;  // Turn ON LED (PN1)
	}
void D1_OFF() {
	GPIO_PORTN_DATA_R &= ~0b00000010;  // Turn OFF LED (PN1)
}
void D2_ON() {
	GPIO_PORTN_DATA_R |= 0b00000001;  // Turn ON LED (PN0)
	}
void D2_OFF() {
	GPIO_PORTN_DATA_R &= ~0b00000001;  // Turn OFF LED (PN0)
	}
void D3_ON() {
	GPIO_PORTF_DATA_R |= 0b00010000;  // Turn ON LED (PF4)
	}
void D3_OFF() {
	GPIO_PORTF_DATA_R &= ~0b00010000;  // Turn OFF LED (PF4)
	}
void D4_ON() {
	GPIO_PORTF_DATA_R |= 0b00000001;  // Turn ON LED (PF0)
	}
void D4_OFF() {
	GPIO_PORTF_DATA_R &= ~0b00000001;  // Turn ON LED (PF0)
	}

//STEPPER MOTOR LOGIC FOR PORT H - FULL STEP

//CLOWKWISE ROTATION
void CW(){																	
	uint32_t delay = 5;													
	
		GPIO_PORTH_DATA_R = 0b1001;
		SysTick_Wait10ms(delay);										
		GPIO_PORTH_DATA_R = 0b0011;													
		SysTick_Wait10ms(delay);
		GPIO_PORTH_DATA_R = 0b0110;												
		SysTick_Wait10ms(delay);
		GPIO_PORTH_DATA_R = 0b1100;												
		SysTick_Wait10ms(delay);
}

//COUNTERCLOCKWISE ROTATION
void CCW(){																		
	uint32_t delay = 5;															
	
		GPIO_PORTH_DATA_R = 0b1100;
		SysTick_Wait10ms(delay);											
		GPIO_PORTH_DATA_R = 0b0110;												
		SysTick_Wait10ms(delay);
		GPIO_PORTH_DATA_R = 0b0011;												
		SysTick_Wait10ms(delay);
		GPIO_PORTH_DATA_R = 0b1001;													
		SysTick_Wait10ms(delay);
}	

//*********************************************************************************************************
//*********************************************************************************************************
//***********					MAIN Function				*****************************************************************
//*********************************************************************************************************
//*********************************************************************************************************
uint16_t	dev = 0x29;			//address of the ToF sensor as an I2C slave peripheral
int status=0;

int main(void) {
  uint8_t byteData, sensorState=0, myByteArray[10] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} , i=0;
  uint16_t wordData;
  uint16_t Distance;
  uint16_t SignalRate;
  uint16_t AmbientRate;
  uint16_t SpadNum; 
  uint8_t RangeStatus;
  uint8_t dataReady;
	int motor = 0; 
	int dir = 0;   
	int x = 0; 
	int counter = 0; 
	int steps=0;


	//initialize
	
	PLL_Init();	
	SysTick_Init();
	onboardLEDs_Init();
	I2C_Init();
	UART_Init();
	PortH_Init();                                                                
	PortJ_Init();
	PortN_Init();
	PortF_Init();
	
	// hello world!
	UART_printf("Program Begins\r\n");
	int mynumber = 1;
	sprintf(printf_buffer,"2DX ToF Program Studio Code %d\r\n",mynumber);
	UART_printf(printf_buffer);


/* Those basic I2C read functions can be used to check your own I2C functions */
	status = VL53L1X_GetSensorId(dev, &wordData);

	sprintf(printf_buffer,"(Model_ID, Module_Type)=0x%x\r\n",wordData);
	UART_printf(printf_buffer);

	// 1 Wait for device booted
	while(sensorState==0){
		status = VL53L1X_BootState(dev, &sensorState);
		SysTick_Wait10ms(10);
  }
	FlashAllLEDs();
	UART_printf("ToF Chip Booted!\r\n Please Wait...\r\n");
	
	status = VL53L1X_ClearInterrupt(dev); /* clear interrupt has to be called to enable next interrupt*/
	
  /* 2 Initialize the sensor with the default setting  */
  status = VL53L1X_SensorInit(dev);
	Status_Check("SensorInit", status);

	
  /* 3 Optional functions to be used to change the main ranging parameters according the application requirements to get the best ranging performances */
//  status = VL53L1X_SetDistanceMode(dev, 2); /* 1=short, 2=long */
//  status = VL53L1X_SetTimingBudgetInMs(dev, 100); /* in ms possible values [20, 50, 100, 200, 500] */
//  status = VL53L1X_SetInterMeasurementInMs(dev, 200); /* in ms, IM must be > = TB */

    // 4 What is missing -- refer to API flow chart
    VL53L1X_StartRanging(dev);    // This function has to be called to enable the ranging

	
	while(1) {
        // Check for PJ0 press (motor toggle)
        if ((GPIO_PORTJ_DATA_R & 0x01) == 0) {
            SysTick_Wait10ms(10);
            if ((GPIO_PORTJ_DATA_R & 0x01) == 0) {
                motor = !motor;
                
                if (motor) {
                    D2_ON();
                    dir = 0;
                    counter = 0;
                    steps = 0;
                } else {
                    D1_OFF(); 
                    D2_OFF();
                    D3_OFF();
                    GPIO_PORTH_DATA_R = 0x00;  // STOP MOTOR
                }
                while ((GPIO_PORTJ_DATA_R & 0x01) == 0) { }
            }
        }

        // Check for PJ1 press (direction toggle)
        else if ((GPIO_PORTJ_DATA_R & 0x02) == 0) {
            SysTick_Wait10ms(10);
            if ((GPIO_PORTJ_DATA_R & 0x02) == 0) {
                dir = !dir;
                if (dir) {
                    D2_OFF();
                    counter = 0; // Reset counter when direction changes
                } else {
                    D2_ON();
                }
                while ((GPIO_PORTJ_DATA_R & 0x02) == 0) { }
            }
        }

        // Motor control and distance measurement
        if (motor) {
    for (int i = 0; i < 3; i++) {  // Repeat entire process 3 times
        counter = 0;   // Reset counter for each scan cycle
        steps = 0;     // Reset steps for each scan cycle
        dir = 1;       // Ensure direction is clockwise at the start of each scan

        // Rotate clockwise for one full revolution (512 steps)
        while (counter < 512) {  
            CCW();  // Rotate clockwise
            counter++;
            steps++;
            
            // Take measurement every 16 steps
            if (counter % 16 == 0) {
                D1_ON();  // Measurement LED
                x++;

                // --- ToF Sensor Measurement --- //
                dataReady = 0;
                while (dataReady == 0) {
                    status = VL53L1X_CheckForDataReady(dev, &dataReady);
                    FlashLED3(1);
                    VL53L1_WaitMs(dev, 5);
                }
                dataReady = 0;

                status = VL53L1X_GetRangeStatus(dev, &RangeStatus);
                status = VL53L1X_GetDistance(dev, &Distance);
                status = VL53L1X_GetSignalRate(dev, &SignalRate);
                status = VL53L1X_GetAmbientRate(dev, &AmbientRate);
                status = VL53L1X_GetSpadNb(dev, &SpadNum);
                status = VL53L1X_ClearInterrupt(dev);

                // Send distance to UART
                sprintf(printf_buffer, "%.1f\r\n", (float)Distance);
                UART_printf(printf_buffer);
                D1_OFF();
            }
        }

        // Reverse direction after full rotation
        dir = 0;  // Switch to counterclockwise
        D4_ON();  // Optional: Indicate direction change
				D2_OFF();
        // Return to home position (rotate counterclockwise)
        while (counter != 0 ) {
            CW();
            counter--;
					
        }

        // Ensure all LEDs are off before next iteration
        D2_OFF();
        D4_OFF();
    }

    // Disable motor after completing all 3 scans
    motor = 0;
}

    }

    return 0;
}	
