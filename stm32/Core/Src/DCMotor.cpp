/*
 * DCMotor.cpp
 *
 *  Created on: Apr 16, 2021
 *      Author: jwan
 */


#include "DCMotor.h"


DCMotor::DCMotor(volatile uint32_t& ccrx):CCRx(ccrx){
	this->Speed         = 0 ;
	this->pinbackword   = 0 ;
	this->pinforward    = 0 ;

}

void DCMotor::setGPIO(GPIO_TypeDef* GPIOx1 ,GPIO_TypeDef* GPIOx2 ,uint16_t pin1  ,uint16_t pin2){
	this->gpio_typedef1 = GPIOx1 ;
	this->gpio_typedef2 = GPIOx2 ;
	this->pinforward    = pin1 ;
	this->pinbackword   = pin2 ;
}

void DCMotor::setCCRx(volatile uint32_t& ccrx){
	*(&this->otherVariable + 1) = (volatile uint32_t)&ccrx;
	this->CCRx = this->Speed;
}

void DCMotor::moveForword(uint8_t speed){
	if(speed > 255){
		this->Speed = 255 ;
	}else{
		this->Speed = speed;
	}
	HAL_GPIO_WritePin(gpio_typedef2, pinbackword, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(gpio_typedef1, pinforward, GPIO_PIN_SET);
	this->CCRx = speed;
}

void DCMotor::moveBackword(uint8_t speed){
	if(speed > 255){
		this->Speed = 255 ;
	}else{
		this->Speed = speed;
	}
	HAL_GPIO_WritePin(gpio_typedef1, pinforward, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(gpio_typedef2, pinbackword, GPIO_PIN_SET);
	this->CCRx = speed;
}

void DCMotor::stop(){
	HAL_GPIO_TogglePin(gpio_typedef1, pinforward);
	HAL_GPIO_TogglePin(gpio_typedef2, pinbackword);
	HAL_Delay(40);
	HAL_GPIO_WritePin(gpio_typedef1, pinforward, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(gpio_typedef2, pinbackword, GPIO_PIN_RESET);
	this->CCRx = 0;
	this->Speed = 0 ;
}
void DCMotor::togglepin(){
	HAL_GPIO_TogglePin(gpio_typedef1, pinforward);
	HAL_GPIO_TogglePin(gpio_typedef2, pinbackword);
}
void DCMotor::resetpin(){
	HAL_GPIO_WritePin(gpio_typedef1, pinforward, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(gpio_typedef2, pinbackword, GPIO_PIN_RESET);
	this->CCRx = 0;
	this->Speed = 0 ;
}
void DCMotor::setSpeed(uint8_t speed){
	if(speed < 256){
		this->Speed = speed ;
		this->CCRx = speed;
	}
}

void DCMotor::speed_up(uint8_t by){
	uint16_t temp = by+this->Speed;
	if(temp < 256){
		this->Speed += by ;
	}else{
		this->Speed = 255 ;
	}
	this->CCRx = this->Speed;
}

void DCMotor::speed_down(uint8_t by){
	int temp = this->Speed-by;
	if(temp >= 0){
		this->Speed -= by ;
	}else{
		this->Speed = 0 ;
	}
	this->CCRx = this->Speed;
}
uint8_t DCMotor::getSpeed(){
	return this->Speed;
}
/******************************************************************************************************************************************************/

DCSystem::DCSystem(DCMotor& r , DCMotor& l):R(r),L(l){
	this->state         = IDEL ;
}

void DCSystem::setCCRx(volatile uint32_t& ccrx_X , volatile uint32_t& ccrx_Y){
	R.setCCRx(ccrx_X);
	L.setCCRx(ccrx_Y);
}

void DCSystem::moveForword(uint8_t speed){
	this->state = MOVINGFORWARD ;
	R.moveForword(0);
	L.moveForword(0);
	R.setSpeed(speed);
	L.setSpeed(speed);
}

void DCSystem::moveBackword(uint8_t speed){
	this->state = MOVINGBACKWORD ;
	R.moveBackword(0);
	L.moveBackword(0);
	R.setSpeed(speed);
	L.setSpeed(speed);
}
void DCSystem::turnLeft(uint8_t speed){
	this->state = TURNINGLEFT ;
	R.moveForword(speed);
	L.moveBackword(speed);
}
void DCSystem::turnRight(uint8_t speed){
	this->state = TURNINGRIGHT ;
	R.moveBackword(speed);
	L.moveForword(speed);
}

void DCSystem::stop(){
	this->state = IDEL ;
	L.togglepin();
	R.togglepin();
	HAL_Delay(40);
	this->L.resetpin();
	this->R.resetpin();



}
STATE DCSystem::getState(){
	return this->state;
}

void DCSystem::increaseSpeed(uint8_t by){
	this->L.speed_up(by);
	this->R.speed_up(by);
}

void DCSystem::decreaseSpeed(uint8_t by){
	this->L.speed_down(by);
	this->R.speed_down(by);
}

uint8_t DCSystem::getSpeed(){
	return this->L.getSpeed();
}

void DCSystem::set_Fdis(uint8_t dis){
	this->FDistance = dis ;
}
void DCSystem::set_Ldis(uint8_t dis){
	this->LDistance = dis ;
}
void DCSystem::set_Rdis(uint8_t dis){
	this->RDistance = dis ;
}

void DCSystem::set_encoders(uint32_t* left ,uint32_t* right ){
	this->encoderL = left ;
	this->encoderR = right ;
}
uint16_t DCSystem::advance(uint16_t by , uint8_t leftSide[30] ,uint8_t rightSide[30]){
	uint8_t MINSPEED = 130 ;
	uint16_t advanced = 0 ,temp = 0;
	uint32_t pulses = (uint32_t)by*17.7;
	int error = 0 ;
	int abserror = 0;
	*encoderL = 0 ;
	*encoderR = 0 ;
	leftSide[0] = this->LDistance;
	rightSide[0] = this->RDistance;
	this->moveForword(MINSPEED);
	while(*encoderL < pulses && *encoderR < pulses){
		if(this->FDistance < 10){
			break;
		}
		error = *encoderL-*encoderR;
		if(error >= 1 || error <= -1){
			if(error < 0){
				abserror = error * -1;
				this->L.speed_up(abserror);
				this->R.speed_down(abserror);
			}
			else{
				abserror = error;
				this->R.speed_up(abserror);
				this->L.speed_down(abserror);
			}
		}
		else{
			this->moveForword(MINSPEED);
		}
		temp = (uint16_t)((*encoderL)/17.7);
		if(temp > advanced){
			advanced = temp ;
			leftSide[temp%30] = this->LDistance;
			rightSide[temp%30] = this->RDistance;
		}


	}
	this->stop();
	advanced = (uint16_t)((*encoderL)/17.7);
	return advanced;
}

uint16_t DCSystem::returnBy(uint16_t by){
	uint8_t MINSPEED = 130 ;
	uint16_t advanced = 0 ;
	uint32_t pulses = (uint32_t)by*17.7;
	int error = 0 ;
	int abserror = 0;
	*encoderL = 0 ;
	*encoderR = 0 ;
	this->moveBackword(MINSPEED);
	while(*encoderL < pulses && *encoderR < pulses){

		error = *encoderL-*encoderR;
		if(error > 1 || error < -1){
			if(error < 0){
				abserror = error * -1;
				this->L.speed_up(abserror);
				this->R.speed_down(abserror);
			}
			else{
				abserror = error;
				this->R.speed_up(abserror);
				this->L.speed_down(abserror);
			}
		}else{
			this->moveBackword(MINSPEED);
		}
	}
	this->stop();
	advanced = (uint16_t)((*encoderL)/17.7);
	return advanced;
}
float DCSystem::turnBy(int by ){

	uint8_t speed = 180 ;
	uint16_t pt = 0 ;
	uint16_t nt = 0 ;
	uint16_t dt = 0 ;
	float turn = 0;
	float GZ = 0 ;
	//int error ;
	//int abserror = 0;

	*encoderL = 0 ;
	*encoderR = 0 ;
	mpu->reset();
	HAL_Delay(100);
	if(mpu->init()){
		//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
		if(by > 0){
//			this->turnLeft(speed);
			while(1){
				if(mpu->MPU_getGyroZ(&GZ)){
					//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
					if(GZ < 0){GZ *= -1.0;}
					//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
					this->turnLeft(speed);
					nt = __HAL_TIM_GET_COUNTER(this->tim);
					if(nt <= pt){
						dt = (10000 - pt) + nt ;
					}else {
						dt = nt - pt ;
					}
					pt = nt ;
					turn += (float)(GZ*dt/100000.0) ;
					if(turn > by){
						break ;
					}
/*
					error = *encoderL-*encoderR;
					if(error >= 1 || error <= -1){
						if(error < 0){
							abserror = error * -1;
							this->L.speed_up(abserror);
							this->R.speed_down(abserror);
						}
						else{
							abserror = error;
							this->R.speed_up(abserror);
							this->L.speed_down(abserror);
						}
					}else{
						this->turnLeft(speed);
					}
*/
					HAL_Delay(2);
				}else{
					//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
					//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
					this->stop();
					//mpu->reset();
					return turnBy(by-turn);
				}
			}
			this->stop();
			//mpu->reset();
			/*
			if(turn > by+2.5){
				return turnBy(by-turn);
			}*/
		}else if (by < 0){
//			this->turnRight(speed);
			while(1){
				if(mpu->MPU_getGyroZ(&GZ)){
					//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
					if(GZ < 0){GZ *= -1.0;}
					//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
					this->turnRight(speed);
					nt = __HAL_TIM_GET_COUNTER(this->tim);
					if(nt <= pt){
						dt = (10000 - pt) + nt ;
					}else {
						dt = nt - pt ;
					}
					pt = nt ;
					turn += (float)(GZ*dt/100000.0) ;
					if(turn > -by){
						break;
					}
/*
					error = *encoderL-*encoderR;
					if(error >= 1 || error <= -1){
						if(error < 0){
							abserror = error * -1;
							this->L.speed_up(abserror);
							this->R.speed_down(abserror);
						}
						else{
							abserror = error;
							this->R.speed_up(abserror);
							this->L.speed_down(abserror);
						}
					}else{
						this->turnRight(speed);
					}
*/
					HAL_Delay(2);
				}else{
					this->stop();
					//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
					return turnBy(by+turn);
				}
			}
			this->stop();
			//mpu->reset();
			/*
			if(turn < by-2.5){
				return turnBy(by-turn);
			}*/
		}
		//this->stop();
		//mpu->reset();
		return turn ;
	}else{
		//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
		return turnBy(by);
	}
	return 0 ;
}
/**
 * if by > 0 turn left
 * else turn right
 */
float DCSystem::turnBy2(int by ){
	uint16_t advanced = 0;
	int error = 0 ;
	int abserror = 0;
	if(by < 0){

		uint16_t pulses = (uint16_t)(2*3.14*17*(-by/360.0)*17.7);
		*encoderL = 0 ;
		*encoderR = 0 ;
		this->turnRight(160);
		while(*encoderL < pulses && *encoderR < pulses){
			error = *encoderL-*encoderR;
			if(error > 1 || error < -1){
				if(error < 0){
					abserror = error * -1;
					this->L.speed_up(abserror);
					this->R.speed_down(abserror);
				}
				else{
					abserror = error;
					this->R.speed_up(abserror);
					this->L.speed_down(abserror);
				}
			}else{
				this->turnRight(130);
			}
		}
		this->stop();

	}else if (by > 0){
		uint16_t pulses = (uint16_t)(2*3.14*17*(by/360.0)*17.7);
		*encoderL = 0 ;
		*encoderR = 0 ;
		this->turnLeft(160);
		while(*encoderL < pulses && *encoderR < pulses){
			error = *encoderL-*encoderR;
			if(error > 1 || error < -1){
				if(error < 0){
					abserror = error * -1;
					this->L.speed_up(abserror);
					this->R.speed_down(abserror);
				}
				else{
					abserror = error;
					this->R.speed_up(abserror);
					this->L.speed_down(abserror);
				}
			}else{
				this->turnLeft(130);
			}

		}
		this->stop();
	}
	return advanced;
}
int DCSystem::map(int s , int a1 , int a2 , int b1 , int b2 ){
	return (int)(b1+((s-a1)*(b2-b1)/(a2-a1)));
}

void DCSystem::setMPU(MPU6050* mpu ){
	this->mpu = mpu ;
}
void DCSystem::setTIM(TIM_HandleTypeDef* tim){
	this->tim = tim ;
}
