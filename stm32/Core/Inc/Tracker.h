#ifndef __TRACKER_H
#define __TRACKER_H

# include "stdint.h"
# include "DCMotor.h"

# define SIZE 30
# define SIZE2 900
# define CELL_LENGTH 30
# define TURN 91
# define INSUN 150
# define PANELNUM 2
enum DIR{N,E,S,W};

class Tracker{
	private:

		bool enf = false ;
		bool ok  = false ;

		uint8_t      FDistance;          // front distance
		uint8_t      LDistance;          // left distance
		uint8_t      RDistance;          // right distance
/*
		uint16_t     topLeft ;          // LDR
		uint16_t     topRight;          // LDR
		uint16_t     bottomLeft;        // LDR
		uint16_t     bottomRight;       // LDR
*/
		//ServoSystem& serSys;
		uint16_t     panelFL ;          // PANEL F
		uint16_t     panelFR ;          // PANEL F
		uint16_t     panelBL;          // PANEL R
		uint16_t     panelBR;        // PANEL L

		int16_t      sunlight_counter = 0;
		int16_t      sunlight_scheduale = 0;

		bool isOP = false ;
		bool watering = false ;

		DCSystem&    dcSys;



		int                matrix[SIZE][SIZE];
		bool               graph [SIZE2][4];
		uint8_t            r,c;                // position
		enum DIR           dir;



	public:

		//Tracker(ServoSystem& sersys,DCSystem& dcsys);
		Tracker(DCSystem& dcsys);

//		void     set_illumination(uint16_t tl , uint16_t tr , uint16_t bl ,uint16_t br);
		void     set_panels(uint16_t tl , uint16_t tr , uint16_t bl , uint16_t br );
		void     set_Fdis(uint8_t);
		void     set_Ldis(uint8_t);
		void     set_Rdis(uint8_t);
		void     set_encoders(TIM_HandleTypeDef* , TIM_HandleTypeDef* );

//		uint16_t get_tl();
//		uint16_t get_tr();
//		uint16_t get_bl();
//		uint16_t get_br();


		uint8_t  moveR(uint16_t by,int cond , uint8_t leftSide[30] ,uint8_t rightSide[30]);
		uint8_t  moveL(uint16_t by,int cond , uint8_t leftSide[30] ,uint8_t rightSide[30]);
		uint8_t  moveF(uint16_t by,int cond , uint8_t leftSide[30] ,uint8_t rightSide[30]);
		uint8_t  moveB(uint16_t by);

//		void scan(uint16_t sens);

		void     set_enf(bool enf);
		void     set_ok (bool ok );

		uint8_t traverse();


		uint8_t getX(){return this->c;}
		uint8_t gety(){return this->r;}
		bool    is_OP(){return this->isOP;}
		uint16_t isSun(){return (uint16_t)((panelFL+panelFR+panelBL+panelBR)/4);}

		void     setWater(){
			this->watering = true ;
		}

		void restart();

		void set_sunlight_counter(int16_t c){this->sunlight_counter = c ;}
		void set_sunlight_scheduale(int16_t s){this->sunlight_scheduale = s;}

		void watering_palnt(){
			if(this->watering){
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET);
				HAL_Delay(2000);
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);
				this->watering = false ;
				/***
				 * to do
				 * watering the plant
				 */
			}
		}


	private:
		void mark_unvisited();
		void init_graph();
		void mark_obstacle(int r , int c);
		int  dijkstra();
		int  go_to(int d, uint8_t leftSide[30] ,uint8_t rightSide[30]);
		int  minDistance(int dist[],bool sptSet[]);
		int  printSolution(int src,int dist[], int n,int parent[]);
		void printPath(int parent[], int j);
		void copy(uint8_t dest1[30] ,uint8_t dest2[30],uint8_t src1[30],uint8_t src2[30]);
		void copy(uint8_t dest1[30] ,uint8_t dest2[30],uint8_t src1,uint8_t src2);
		void count(uint8_t src1[30],uint8_t src2[30],uint8_t* count1,uint8_t* count2,uint8_t lessThan);
		uint8_t go_to_shadow();

		uint16_t max();



};

#endif /* __MAIN_H */
