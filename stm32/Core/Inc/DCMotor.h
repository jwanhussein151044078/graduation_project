/*
 * DCMotor.h
 *
 *  Created on: Apr 16, 2021
 *      Author: jwan
 */

#ifndef INC_DCMOTOR_H_
#define INC_DCMOTOR_H_
#include "stm32f429xx.h"
#include "stm32f4xx_hal.h"
# include "MPU6050.h"
#include "stdint.h"

enum STATE{MOVINGFORWARD , MOVINGBACKWORD , TURNINGLEFT , TURNINGRIGHT , IDEL };

class DCMotor{
	private:
		uint8_t             Speed ;
		GPIO_TypeDef        *gpio_typedef1 , *gpio_typedef2 ;
		uint16_t            pinforward , pinbackword ;
		volatile uint32_t   otherVariable;
		volatile uint32_t&  CCRx ;


	public:
		DCMotor(volatile uint32_t& ccrx);
		void setGPIO(GPIO_TypeDef* GPIOx1 ,GPIO_TypeDef* GPIOx2 ,uint16_t pin1  ,uint16_t pin2);
		void setCCRx(volatile uint32_t& ccrx);
		void moveForword(uint8_t speed);
		void moveBackword(uint8_t speed);
		void stop();
		void setSpeed(uint8_t speed);
		void speed_up(uint8_t by);
		void speed_down(uint8_t by);
		void togglepin();
		void resetpin();



		uint8_t getSpeed();
};

class DCSystem{
	private:
		uint8_t      FDistance;          // front distance
		uint8_t      LDistance;          // left distance
		uint8_t      RDistance;          // right distance

		uint32_t*     encoderL;
		uint32_t*     encoderR;

		DCMotor& R;
		DCMotor& L;
		MPU6050* mpu ;
		TIM_HandleTypeDef* tim;

		STATE    state ;

		int map(int s , int a1 , int a2 , int b1 , int b2 );
	public:
		DCSystem(DCMotor& r , DCMotor& l);
		void setCCRx(volatile uint32_t& ccrx_X , volatile uint32_t& ccrx_Y);
		void setMPU(MPU6050* mpu );
		void setTIM(TIM_HandleTypeDef* tim);
		void moveForword(uint8_t speed);
		void moveBackword(uint8_t speed);
		void turnLeft(uint8_t speed);
		void turnRight(uint8_t speed);
		void stop();
		void increaseSpeed(uint8_t by);
		void decreaseSpeed(uint8_t by);
		void set_Fdis(uint8_t);
		void set_Ldis(uint8_t);
		void set_Rdis(uint8_t);
		void set_encoders(uint32_t* left ,uint32_t* right );



		uint16_t advance(uint16_t by , uint8_t array[30],uint8_t rightSide[30]);
		uint16_t returnBy(uint16_t by);

		float turnBy (int by );
		float turnBy2(int by );
		STATE getState();

		uint8_t getSpeed();
};

#endif /* INC_DCMOTOR_H_ */
