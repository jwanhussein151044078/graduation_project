# include "Tracker.h"

/*
Tracker::Tracker(ServoSystem& sersys , DCSystem& dcsys ):serSys(sersys),dcSys(dcsys){
	for(uint8_t i = 0 ; i < 30 ; i++){
		for(uint8_t j = 0 ; j < 30 ; j++){
			this->matrix[i][j]= -2;
		}
	}
	this->r = (int)(SIZE)/2 ;
	this->c = (int)(SIZE)/2 ;
	this->matrix[r][c]= 0;
	this->dir = N;
	this->init_graph();
}
*/
Tracker::Tracker(DCSystem& dcsys ):dcSys(dcsys){
	for(uint8_t i = 0 ; i < 30 ; i++){
		for(uint8_t j = 0 ; j < 30 ; j++){
			this->matrix[i][j]= -2;
		}
	}
	this->r = (int)(SIZE)/2 ;
	this->c = (int)(SIZE)/2 ;
	this->matrix[r][c]= 0;
	this->dir = N;
	this->init_graph();
}

void Tracker::restart(){
	for(uint8_t i = 0 ; i < 30 ; i++){
		for(uint8_t j = 0 ; j < 30 ; j++){
			this->matrix[i][j]= -2;
		}
	}
	this->init_graph();
}
void Tracker::set_panels(uint16_t tl , uint16_t tr , uint16_t bl , uint16_t br ){
	this->panelFL = tl ;
	this->panelFR = tr ;
	this->panelBL = bl ;
	this->panelBR = br ;
}
/*
void Tracker::set_illumination(uint16_t tl , uint16_t tr , uint16_t bl ,uint16_t br){
	topLeft = tl ;
	topRight = tr ;
	bottomLeft = bl ;
	bottomRight = br ;
}

uint16_t Tracker::get_tl(){
	return topLeft;
}
uint16_t Tracker::get_tr(){
	return topRight;
}
uint16_t Tracker::get_bl(){
	return bottomLeft;
}
uint16_t Tracker::get_br(){
	return bottomRight;
}

void Tracker::scan(uint16_t sens){
	int avgtop    = (int)((topLeft+topRight)/2);
	int avgbottom = (int)((bottomLeft+bottomRight)/2);
	int avgleft   = (int)((topLeft+bottomLeft)/2);
	int avgright  = (int)((topRight+bottomRight)/2);
	int temp1 = avgleft-avgright ;
	int temp2 = avgtop -avgbottom;
	if(temp1<0){
		temp1 = temp1*-1;
	}
	if(temp2<0){
		temp2 = temp2*-1;
	}
	//serSys.setSpeedX(this->map(temp1,0,200,1,5));
	//serSys.setSpeedY(this->map(temp2,0,200,1,5));
	if(avgtop > avgbottom+sens){
		serSys.moveUp();
	}else if(avgtop < avgbottom - sens){
		serSys.moveDown();
	}

	if(avgleft > avgright+sens ){
		if(serSys.moveLeft() == 0){
			dcSys.turnLeft(150);
		}
	}else if(avgleft < avgright-sens){
		if(serSys.moveRight() == 0){
			dcSys.turnRight(150);
		}
	}else{
			dcSys.stop();
	}

}
*/
void Tracker::set_Fdis(uint8_t dis){
	this->FDistance = dis ;
	this->dcSys.set_Fdis(dis);
}
void Tracker::set_Ldis(uint8_t dis){
	this->LDistance = dis ;
	this->dcSys.set_Ldis(dis);
}
void Tracker::set_Rdis(uint8_t dis){
	this->RDistance = dis ;
	this->dcSys.set_Rdis(dis);
}
void Tracker::copy(uint8_t dest1[30] ,uint8_t dest2[30],uint8_t src1[30],uint8_t src2[30]){
	for(int i = 0 ; i < 30 ; i++){
		dest1[i] = src1[i];
		dest2[i] = src2[i];
	}
}

void Tracker::copy(uint8_t dest1[30] ,uint8_t dest2[30],uint8_t src1,uint8_t src2){
	for(int i = 0 ; i < 30 ; i++){
		dest1[i] = src1;
		dest2[i] = src2;
	}
}

void Tracker::count(uint8_t src1[30],uint8_t src2[30],uint8_t* count1,uint8_t* count2,uint8_t lessThan){
	*count1 = 0 ;
	*count2 = 0 ;
	for(uint8_t i = 0 ; i < 30 ; i++){
		if(src1[i] < lessThan){(*count1)++;}
		if(src2[i] < lessThan){(*count2)++;}
	}
}

uint8_t Tracker::traverse(){

	int flag=0;
	int d = -1 ;
	uint8_t leftSide[30] , rightSide[30];
	uint8_t leftobstacles = 0 , rightobstacles = 0 , stucked = 0;
	uint8_t fdist_obstacle = 15 ;

	int c = 0 ;
	int cc= 0 ;

	if(this->sunlight_counter > this->sunlight_scheduale){
		return this->go_to_shadow();
	}
	this->copy(leftSide, rightSide, this->LDistance, this->RDistance);
	while(1){
		watering_palnt();
		if(!enf && !ok){
			this->isOP = false ;
			return 6 ;
		}
		flag = 0 ;
		leftobstacles = 0 ;
		rightobstacles= 0;
		stucked = 0;
		this->count(leftSide, rightSide, &leftobstacles, &rightobstacles, 30);
		///////////////////////////////////////////////
		/*
		while(c < 10){
							if( this->max() > 0 ){
								cc++;
							}
							c++;
							HAL_Delay(50);
						}
						if(cc > 0 ){
							this->isOP = false ;
							this->mark_unvisited();
							return 5 ;
						}else{
							this->isOP = true ;
						}
		///////////////////////////////////////////////
		this->matrix[this->r][this->c] = this->max() ;
		*/

		this->matrix[this->r][this->c] = this->max() ;
		if(this->matrix[this->r][this->c] >= PANELNUM){
			this->isOP = false ;
			this->mark_unvisited();
			return 5 ;
		}else{
			this->isOP = true ;
		}
		switch(this->dir){
			case N:
				if(rightobstacles > 2 && this->c < SIZE-1){          // obstacle on right
					this->matrix[this->r][this->c+1] = -3 ;
					this->mark_obstacle(this->r, this->c+1);
				}
				if(leftobstacles > 2 && this->c > 0){               // obstacle on the left
					this->matrix[this->r][this->c-1] = -3 ;
					this->mark_obstacle(this->r, this->c-1);
				}
				if(this->FDistance < fdist_obstacle && this->r > 0){               // obstacle in the front
					this->matrix[this->r-1][this->c] = -3 ;
					this->mark_obstacle(this->r-1, this->c);
				}
				break;
			case E:
				if(rightobstacles > 2 && this->r < SIZE-1){          // obstacle on right
					this->matrix[this->r+1][this->c] = -3 ;
					this->mark_obstacle(this->r+1, this->c);
				}
				if(leftobstacles > 2  && this->r > 0){               // obstacle on the left
					this->matrix[this->r-1][this->c] = -3 ;
					this->mark_obstacle(this->r-1, this->c);
				}
				if(this->FDistance < fdist_obstacle && this->c < SIZE-1){               // obstacle in the front
					this->matrix[this->r][this->c+1] = -3 ;
					this->mark_obstacle(this->r, this->c+1);
				}
				break;

			case S:
				if(rightobstacles > 2  && this->c > 0){          // obstacle on right
					this->matrix[this->r][this->c-1] = -3 ;
					this->mark_obstacle(this->r, this->c-1);
				}
				if(leftobstacles > 2  && this->c < SIZE-1){               // obstacle on the left
					this->matrix[this->r][this->c+1] = -3 ;
					this->mark_obstacle(this->r, this->c+1);
				}
				if(this->FDistance < fdist_obstacle && this->r < SIZE-1){               // obstacle in the front
					this->matrix[this->r+1][this->c] = -3 ;
					this->mark_obstacle(this->r+1, this->c);
				}
				break;
			case W:
				if(rightobstacles > 2  && this->r > 0){          // obstacle on right
					this->matrix[this->r-1][this->c] = -3 ;
					this->mark_obstacle(this->r-1, this->c);
				}
				if(leftobstacles > 2  && this->r < SIZE-1){               // obstacle on the left
					this->matrix[this->r+1][this->c] = -3 ;
					this->mark_obstacle(this->r+1, this->c);
				}
				if(this->FDistance < fdist_obstacle && this->c < SIZE-1){               // obstacle in the front
					this->matrix[this->r][this->c+1] = -3 ;
					this->mark_obstacle(this->r, this->c+1);
				}
				break;
			default:return 2;
		}

		flag = this->moveR(CELL_LENGTH,-2 , leftSide, rightSide);
		if(flag == 3){
			HAL_Delay(1000);
			continue;
		}else{stucked++;}

		flag= this->moveF(CELL_LENGTH,-2 ,  leftSide, rightSide);
		if(flag == 3){
			HAL_Delay(1000);
			continue;
		}else{stucked++;}

		flag= this->moveL(CELL_LENGTH,-2 ,  leftSide, rightSide);
		if(flag == 3){
			HAL_Delay(1000);
			continue;
		}else{stucked++;}

		if(stucked == 3){
			d = dijkstra();
			if(d == -1){         // no more cell to visit
				return 3 ;
			}else{
				if(go_to(d,leftSide,rightSide) == 5){
					this->mark_unvisited();
					return 5 ;
				}

			}
		}
	}
	return 1 ;
}

uint8_t Tracker::moveR(uint16_t by,int cond , uint8_t leftSide[30] ,uint8_t rightSide[30]){
	uint16_t advanced = 0 ;
	uint8_t templ[30],tempr[30];
	uint8_t flag = 3;
	this->copy(templ, tempr, leftSide, rightSide);
	switch(this->dir){
		case N:
			if(this->c < SIZE-1 && this->matrix[this->r][this->c+1] == cond){
				dcSys.turnBy(-TURN);
				HAL_Delay(250);
				advanced = dcSys.advance(by,leftSide,rightSide);
				if(advanced < by){
					HAL_Delay(250);
					dcSys.returnBy(advanced);
					HAL_Delay(250);
					dcSys.turnBy(TURN);
					HAL_Delay(250);
					this->matrix[this->r][this->c+1] = -3 ;
					this->mark_obstacle(this->r, this->c+1);
					flag = 1;                                // obstacle was found while advancing
				}else{
					this->dir = (enum DIR)((this->dir+1)%4);
					this->c++;
				}
			}else{
				flag = 0;                                    // can nont move right
			}
			break;
		case E:
			if(this->r < SIZE-1 && this->matrix[this->r+1][this->c] == cond){
				dcSys.turnBy(-TURN);
				HAL_Delay(250);
				advanced = dcSys.advance(by,leftSide,rightSide);
				if(advanced < by){
					HAL_Delay(250);
					dcSys.returnBy(advanced);
					HAL_Delay(250);
					dcSys.turnBy(TURN);
					HAL_Delay(250);
					this->matrix[this->r+1][this->c] = -3 ;
					this->mark_obstacle(this->r+1, this->c);
					flag = 1;                                // obstacle was found while advancing
				}else{
					this->dir = (enum DIR)((this->dir+1)%4);
					this->r++;
				}
			}else{
				flag = 0;
			}
			break;
		case S:
			if(this->c > 0 && this->matrix[this->r][this->c-1] == cond){
				dcSys.turnBy(-TURN);
				HAL_Delay(250);
				advanced = dcSys.advance(by,leftSide,rightSide);
				if(advanced < by){
					HAL_Delay(250);
					dcSys.returnBy(advanced);
					HAL_Delay(250);
					dcSys.turnBy(TURN);
					HAL_Delay(250);
					this->matrix[this->r][this->c-1] = -3 ;
					this->mark_obstacle(this->r, this->c-1);
					flag = 1;                                // obstacle was found while advancing
				}else{
					this->dir = (enum DIR)((this->dir+1)%4);
					this->c--;
				}
			}else{
				flag = 0;                                   // can nont move right
			}
			break;
		case W:
			if(this->r > 0 && this->matrix[this->r-1][this->c] == cond){
				dcSys.turnBy(-TURN);
				HAL_Delay(250);
				advanced = dcSys.advance(by,leftSide,rightSide);
				if(advanced < by){
					HAL_Delay(250);
					dcSys.returnBy(advanced);
					HAL_Delay(250);
					dcSys.turnBy(TURN);
					HAL_Delay(250);
					this->matrix[this->r-1][this->c] = -3 ;
					this->mark_obstacle(this->r-1, this->c);
					flag = 1;                               // obstacle was found while advancing
				}else{
					this->dir = (enum DIR)((this->dir+1)%4);
					this->r--;
				}
			}else{
				flag = 0;
			}
			break;
		default:return 2 ;
	}
	if(flag == 1){
		this->copy(leftSide, rightSide, templ, tempr);
	}
	return flag;
}

uint8_t Tracker::moveL(uint16_t by,int cond ,uint8_t leftSide[30] ,uint8_t rightSide[30]){
	uint16_t advanced = 0 ;
	uint8_t templ[30],tempr[30];
	uint8_t flag = 3;
	this->copy(templ, tempr, leftSide, rightSide);
	switch(this->dir){
	case N:
		if(this->c > 0 && this->matrix[this->r][this->c-1] == cond){
			dcSys.turnBy(TURN);
			HAL_Delay(250);
			advanced = dcSys.advance(by,leftSide,rightSide);
			if(advanced < by){
				HAL_Delay(250);
				dcSys.returnBy(advanced);
				HAL_Delay(250);
				dcSys.turnBy(-TURN);
				HAL_Delay(250);
				this->matrix[this->r][this->c-1] = -3 ;
				this->mark_obstacle(this->r, this->c-1);
				flag = 1;                                // obstacle was found while advancing
			}else{
				this->dir = (enum DIR)((this->dir+3)%4);
				this->c--;
			}
		}else{
			flag = 0;                                    // can nont move right
		}
		break;
	case E:
		if(this->r > 0 && this->matrix[this->r-1][this->c] == cond){
			dcSys.turnBy(TURN);
			HAL_Delay(250);
			advanced = dcSys.advance(by,leftSide,rightSide);
			if(advanced < by){
				HAL_Delay(250);
				dcSys.returnBy(advanced);
				HAL_Delay(250);
				dcSys.turnBy(-TURN);
				HAL_Delay(250);
				this->matrix[this->r-1][this->c] = -3 ;
				this->mark_obstacle(this->r-1, this->c);
				flag = 1;                                // obstacle was found while advancing
			}else{
				this->dir = (enum DIR)((this->dir+3)%4);
				this->r--;
			}
		}else{
			flag = 0;
		}
		break;
	case S:
		if(this->c < SIZE-1 && this->matrix[this->r][this->c+1] == cond){
			dcSys.turnBy(TURN);
			HAL_Delay(250);
			advanced = dcSys.advance(by,leftSide,rightSide);
			if(advanced < by){
				HAL_Delay(250);
				dcSys.returnBy(advanced);
				HAL_Delay(250);
				dcSys.turnBy(-TURN);
				HAL_Delay(250);
				this->matrix[this->r][this->c+1] = -3 ;
				this->mark_obstacle(this->r, this->c+1);
				flag = 1;                                // obstacle was found while advancing
			}else{
				this->dir = (enum DIR)((this->dir+3)%4);
				this->c++;
			}
		}else{
			flag = 0;                                    // can nont move right
		}
		break;
	case W:
		if(this->r < SIZE-1 && this->matrix[this->r+1][this->c] == cond){
			dcSys.turnBy(TURN);
			HAL_Delay(250);
			advanced = dcSys.advance(by,leftSide,rightSide);
			if(advanced < by){
				HAL_Delay(250);
				dcSys.returnBy(advanced);
				HAL_Delay(250);
				dcSys.turnBy(-TURN);
				HAL_Delay(250);
				this->matrix[this->r+1][this->c] = -3 ;
				this->mark_obstacle(this->r+1, this->c);
				flag = 1;                                // obstacle was found while advancing
			}else{
				this->dir = (enum DIR)((this->dir+3)%4);
				this->r++;
			}
		}else{
			flag = 0;
		}
		break;
	default:return 2;
	}
	if(flag == 1){
		this->copy(leftSide, rightSide, templ, tempr);
	}
	return flag;
}

uint8_t Tracker::moveF(uint16_t by,int cond ,uint8_t leftSide[30] ,uint8_t rightSide[30]){
	uint16_t advanced = 0 ;
	uint8_t templ[30],tempr[30];
	uint8_t flag = 3;
	this->copy(templ, tempr, leftSide, rightSide);
	switch(this->dir){
	case N:
		if(this->r > 0 && this->matrix[this->r-1][this->c] == cond){
			advanced = dcSys.advance(by,leftSide,rightSide);
			if(advanced < by){
				HAL_Delay(250);
				dcSys.returnBy(advanced);
				HAL_Delay(250);
				this->matrix[this->r-1][this->c] = -3 ;
				this->mark_obstacle(this->r-1, this->c);
				flag = 1;
			}else{
				this->r--;
			}
		}else{
			flag = 0;
		}
		break;
	case E:
		if(this->c < SIZE-1 && this->matrix[this->r][this->c+1] == cond){
			advanced = dcSys.advance(by,leftSide,rightSide);
			if(advanced < by){
				HAL_Delay(250);
				dcSys.returnBy(advanced);
				HAL_Delay(250);
				this->matrix[this->r][this->c+1] = -3 ;
				this->mark_obstacle(this->r, this->c+1);
				flag = 1;
			}else{
				this->c++;
			}
		}else{
			flag = 0;
		}
		break;
	case S:
		if(this->r < SIZE-1 && this->matrix[this->r+1][this->c] == cond){
			advanced = dcSys.advance(by,leftSide,rightSide);
			if(advanced < by){
				HAL_Delay(250);
				dcSys.returnBy(advanced);
				HAL_Delay(250);
				this->matrix[this->r+1][this->c] = -3 ;
				this->mark_obstacle(this->r+1, this->c);
				flag = 1;
			}else{
				this->r++;
			}
		}else{
			flag = 0;
		}
		break;
	case W:
		if(this->c > 0 && this->matrix[this->r][this->c-1] == cond){
			advanced = dcSys.advance(by,leftSide,rightSide);
			if(advanced < by){
				HAL_Delay(250);
				dcSys.returnBy(advanced);
				HAL_Delay(250);
				this->matrix[this->r][this->c-1] = -3 ;
				this->mark_obstacle(this->r, this->c-1);
				flag = 1;
			}else{
				this->c--;
			}
		}else{
			flag = 0;
		}

		break;
	default:return 2;
	}
	if(flag == 1){
		this->copy(leftSide, rightSide, templ, tempr);
	}
	return flag;
}

uint8_t Tracker::moveB(uint16_t by){
	switch(this->dir){
	case N:
		if(this->r < SIZE-1 && this->matrix[this->r+1][this->c] >= 0){
			dcSys.returnBy(by);
			this->r++;
			return 3 ;
		}
		break;
	case E:
		if(this->c > 0 && this->matrix[this->r][this->c-1] >= 0){
			dcSys.returnBy(by);
			this->c--;
			return 3 ;
		}
		break;
	case S:
		if(this->r > 0 && this->matrix[this->r-1][this->c] >= 0){
			dcSys.returnBy(by);
			this->r--;
			return 3 ;
		}

		break;
	case W:
		if(this->c < SIZE-1 && this->matrix[this->r][this->c+1] >= 0){
			dcSys.returnBy(by);
			this->c++;
			return 3 ;
		}

		break;
	default:return 2 ;
	}
	return 1 ;
}

void Tracker::init_graph(){

	for (int i = 0; i < SIZE2; ++i){
		for (int j = 0; j < 4; ++j){
			graph[i][j] = 0;
	    }
	}
	for (int i = 0; i < SIZE2; ++i){
		if(i-1 >= 0 && ((int)(i/SIZE) == (int)((i-1)/SIZE))){
	       graph[i][1] = 1 ;
	    }
	    if(i-SIZE >= 0 && ((int)(i/SIZE) > (int)((i-SIZE)/SIZE))){
	    	graph[i][0] = 1 ;
	    }
	    if(((int)(i/SIZE) == (int)((i+1)/SIZE))){
        	graph[i][2] = 1 ;
        }
        if(i+SIZE < SIZE2 && ((int)(i/SIZE) < (int)((i+SIZE)/SIZE))){
        	graph[i][3] = 1 ;
        }
    }
}

void Tracker::mark_obstacle(int r , int c){
	int v = r*SIZE + c ;
	if(graph[v][0]){
		graph[v][0]      = false ;
		graph[v-SIZE][3] = false ;
	}
	if(graph[v][1]){
		graph[v][1]   = false ;
		graph[v-1][2] = false ;
	}
	if(graph[v][2]){
		graph[v][2]   = false;
		graph[v+1][1] = false;
	}
	if(graph[v][3]){
		graph[v][3]      = false ;
		graph[v+SIZE][0] = false ;
	}
}


int Tracker::dijkstra(){
	int src = this->r*SIZE + this->c ;
    int dist[SIZE2];
    bool sptSet[SIZE2];
    int parent[SIZE2];
    parent[src] = -1;
    for (int i = 0; i < SIZE2; i++)
    {
        dist[i] = 2147483647;
        sptSet[i] = false;
    }
    dist[src] = 0;
    for (int count = 0; count < SIZE2 - 1; count++){
        int u = minDistance(dist, sptSet);
        sptSet[u] = true;
        //printf("u = %d\t",u );
	        if(u- SIZE >= 0){
            if(!sptSet[u- SIZE]){
                if(graph[u][0] && dist[u] + graph[u][0] < dist[u- SIZE]){
                    //printf("v = %d\t",u- SIZE );
                    parent[u- SIZE] = u;
                    dist[u- SIZE] = dist[u] + graph[u][0];
                }
            }
        }
        if(u- 1 >= 0){
            if(!sptSet[u- 1]){
                if(graph[u][1] && dist[u] + graph[u][1] < dist[u- 1]){
                    //printf("v = %d\t",u- 1 );
                    parent[u- 1] = u;
                    dist[u- 1] = dist[u] + graph[u][1];
                }

            }
        }
        if(u + 1 < SIZE2){
            if(!sptSet[u + 1]){
                if(graph[u][2] && dist[u] + graph[u][2] < dist[u + 1]){
                    //printf("v = %d\t",u + 1 );
                    parent[u + 1] = u;
                    dist[u + 1] = dist[u] + graph[u][2];
                }

            }
        }
        if(u + SIZE < SIZE2){
           if(!sptSet[u + SIZE]){
                if(graph[u][3] && dist[u] + graph[u][3] < dist[u + SIZE]){
                    //printf("v = %d\t",u + SIZE );
                    parent[u + SIZE] = u;
                    dist[u + SIZE] = dist[u] + graph[u][3];
                }
            }
        }
        //printf("\n");
    }
    //printf("\n");
    int d = printSolution(src,dist, SIZE2, parent) ;
    //printf("%d\n",d);
    return d ;
}

int Tracker::minDistance(int dist[],bool sptSet[]){
    int min = 2147483647 ;
    int min_index;
	for (int v = 0; v < SIZE2; v++){
		if (sptSet[v] == false && dist[v] <= min){
        	min = dist[v];
	        min_index = v;
	    }
	}
	return min_index;
}

int Tracker::printSolution(int src,int dist[], int n,int parent[]){
	int min = 2147483647 ;
	int ind = -1;
    for (int i = 0; i < SIZE2; i++){
        if(i != src && dist[i] > 0 && dist[i] < SIZE2 && dist[i] < min && matrix[((int)(i/SIZE))][i%SIZE] == -2){
        	min = dist[i] ;
        	ind = i ;
        }
    }
    if(ind != -1){
       	printPath(parent, ind);
    }
    return ind ;
}

void Tracker::printPath(int parent[], int j){
    if (parent[j] == - 1){return;}
    printPath(parent, parent[j]);
    matrix[((int)(j/SIZE))][j%SIZE] = -1 ;
}

int  Tracker::go_to(int d, uint8_t leftSide[30] ,uint8_t rightSide[30]){
	int flag=0;
	uint8_t leftobstacles = 0 , rightobstacles = 0 , stucked = 0;
	uint8_t fdist_obstacle = 15 ;
	int c = 0 ;
	int cc = 0 ;
	if(this->dir == N ){
		if(this->r < SIZE && matrix[r+1][c] == -1){
			this->dir = (enum DIR)((this->dir+2)%4);
			dcSys.turnBy((TURN*2)-1);
		}
	}else if(this->dir == E){
		if(this->c > 0 && matrix[r][c-1] == -1){
			this->dir = (enum DIR)((this->dir+2)%4);
			dcSys.turnBy((TURN*2)-1);
		}
	}else if(this->dir == S){
		if(this->r > 0 && matrix[r-1][c] == -1){
			this->dir = (enum DIR)((this->dir+2)%4);
			dcSys.turnBy((TURN*2)-1);
		}
	}else if(this->dir == W){
		if(this->c < SIZE && matrix[r][c+1] == -1){
			this->dir = (enum DIR)((this->dir+2)%4);
			dcSys.turnBy(-180);
		}
	}

	while(1){
		watering_palnt();
		if(!enf && !ok){
			this->isOP = false ;
			return 6 ;
		}
		flag = 0 ;
		leftobstacles = 0 ;
		rightobstacles= 0;
		stucked = 0;
		this->count(leftSide, rightSide, &leftobstacles, &rightobstacles, 30);
/*
		//////////////////////////////
				while(c < 10){
					if( this->max() > 0 ){
						cc++;
					}
					c++;
					HAL_Delay(50);
				}
				if(cc > 0 ){
					this->isOP = false ;
					//this->mark_unvisited();
					return 5 ;
				}else{
					this->isOP = true ;
				}
				this->matrix[this->r][this->c] = this->max() ;
		//////////////////////////////
*/

		this->matrix[this->r][this->c] = this->max() ;
		if(this->matrix[this->r][this->c] >= PANELNUM){                   //  sunlight found
			this->isOP = false  ;
			return 5 ;
		}else{
			this->isOP = true  ;
		}

		switch(this->dir){
			case N:
				if(rightobstacles > 2 && this->c < SIZE-1){          // obstacle on right
					this->matrix[this->r][this->c+1] = -3 ;
					this->mark_obstacle(this->r, this->c+1);
				}
				if(leftobstacles > 2 && this->c > 0){               // obstacle on the left
					this->matrix[this->r][this->c-1] = -3 ;
					this->mark_obstacle(this->r, this->c-1);
				}
				if(this->FDistance < fdist_obstacle && this->r > 0){               // obstacle in the front
					this->matrix[this->r-1][this->c] = -3 ;
					this->mark_obstacle(this->r-1, this->c);
				}
				break;
			case E:
				if(rightobstacles > 2 && this->r < SIZE-1){          // obstacle on right
					this->matrix[this->r+1][this->c] = -3 ;
					this->mark_obstacle(this->r+1, this->c);
				}
				if(leftobstacles > 2  && this->r > 0){               // obstacle on the left
					this->matrix[this->r-1][this->c] = -3 ;
					this->mark_obstacle(this->r-1, this->c);
				}
				if(this->FDistance < fdist_obstacle && this->c < SIZE-1){               // obstacle in the front
					this->matrix[this->r][this->c+1] = -3 ;
					this->mark_obstacle(this->r, this->c+1);
				}
				break;

			case S:
				if(rightobstacles > 2  && this->c > 0){          // obstacle on right
					this->matrix[this->r][this->c-1] = -3 ;
					this->mark_obstacle(this->r, this->c-1);
				}
				if(leftobstacles > 2  && this->c < SIZE-1){               // obstacle on the left
					this->matrix[this->r][this->c+1] = -3 ;
					this->mark_obstacle(this->r, this->c+1);
				}
				if(this->FDistance < fdist_obstacle && this->r < SIZE-1){               // obstacle in the front
					this->matrix[this->r+1][this->c] = -3 ;
					this->mark_obstacle(this->r+1, this->c);
				}
				break;
			case W:
				if(rightobstacles > 2  && this->r > 0){          // obstacle on right
					this->matrix[this->r-1][this->c] = -3 ;
					this->mark_obstacle(this->r-1, this->c);
				}
				if(leftobstacles > 2  && this->r < SIZE-1){               // obstacle on the left
					this->matrix[this->r+1][this->c] = -3 ;
					this->mark_obstacle(this->r+1, this->c);
				}
				if(this->FDistance < fdist_obstacle && this->c < SIZE-1){               // obstacle in the front
					this->matrix[this->r][this->c+1] = -3 ;
					this->mark_obstacle(this->r, this->c+1);
				}
				break;
			default:return 2;
		}
		flag = this->moveR(CELL_LENGTH,-1 , leftSide, rightSide);
		if(flag == 3){
			HAL_Delay(1000);
			continue;
		}else{stucked++;}

		flag= this->moveF(CELL_LENGTH,-1 ,  leftSide, rightSide);
		if(flag == 3){
			HAL_Delay(1000);
			continue;
		}else{stucked++;}

		flag= this->moveL(CELL_LENGTH,-1 ,  leftSide, rightSide);
		if(flag == 3){
			HAL_Delay(1000);
			continue;
		}else{stucked++;}

		if(stucked == 3){
			return 0 ;
		}
	}
	return 1 ;
}
uint16_t Tracker::max(){
	uint16_t i = 0;
	/*
	if (panelF >= panelL && panelF >= panelR){
		return panelF ;
	}else if(panelL >= panelF && panelL >= panelR){
		return panelL ;
	}else{
		return panelR ;
	}
	*/
	if(panelFL >= INSUN){
		i++;
	}
	if(panelFR >= INSUN){
		i++;
	}
	if(panelBL >= INSUN){
		i++;
	}
	if(panelBR >= INSUN){
		i++;
	}
	return i;
}

void Tracker::mark_unvisited(){
	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j) {
			if(this->matrix[i][j] == -2 || this->matrix[i][j] == -1){
				this->matrix[i][j] = 0 ;
			}
		}
	}

	if(this->r > 0 ){
		if ( this->c > 0){
			if(this->matrix[this->r-1][this->c-1] != -3){
				this->matrix[this->r-1][this->c-1] = -2 ;
			}
		}
		if ( this->c < SIZE){
			if(this->matrix[this->r-1][this->c+1] != -3){
				this->matrix[this->r-1][this->c+1] = -2 ;
			}
		}
		if(this->matrix[this->r-1][this->c] != -3){
			this->matrix[this->r-1][this->c] = -2 ;
		}
	}
	if(this->r < SIZE ){
		if ( this->c > 0){
			if(this->matrix[this->r+1][this->c-1] != -3){
				this->matrix[this->r+1][this->c-1] = -2 ;
			}
		}
		if ( this->c < SIZE){
			if(this->matrix[this->r+1][this->c+1] != -3){
				this->matrix[this->r+1][this->c+1] = -2 ;
			}
		}
		if(this->matrix[this->r+1][this->c] != -3){
			this->matrix[this->r+1][this->c] = -2 ;
		}
	}
	if(this->c > 0){
		if(this->matrix[this->r][this->c-1] != -3){
			this->matrix[this->r][this->c-1] = -2 ;
		}
	}
	if(this->c < SIZE){
		if(this->matrix[this->r][this->c+1] != -3){
			this->matrix[this->r][this->c+1] = -2 ;
		}
	}
}

uint8_t Tracker::go_to_shadow(){
	this->restart();
	int flag=0;
	int d = -1 ;
	uint8_t leftSide[30] , rightSide[30];
	uint8_t leftobstacles = 0 , rightobstacles = 0 , stucked = 0;
	uint8_t fdist_obstacle = 15 ;


	int c = 0 ;
	int cc = 0;

	this->copy(leftSide, rightSide, this->LDistance, this->RDistance);
	while(1){
		watering_palnt();

		flag = 0 ;
		leftobstacles = 0 ;
		rightobstacles= 0;
		stucked = 0;
		this->count(leftSide, rightSide, &leftobstacles, &rightobstacles, 30);
/*
		//////////////////////////////
		while(c < 10){
			if( this->max() > 0 ){
				cc++;
			}
			c++;
			HAL_Delay(50);
		}
		if(cc == 0 ){
			this->isOP = false ;
			//this->mark_unvisited();
			return 7 ;
		}else{
			this->isOP = true ;
		}
		this->matrix[this->r][this->c] = this->max() ;
		//////////////////////////////
*/
		this->matrix[this->r][this->c] = this->max() ;
		if(this->matrix[this->r][this->c] <= 1){
			this->isOP = false ;
			//this->mark_unvisited();
			return 7 ;
		}else{
			this->isOP = true ;
		}

		switch(this->dir){
			case N:
				if(rightobstacles > 2 && this->c < SIZE-1){          // obstacle on right
					this->matrix[this->r][this->c+1] = -3 ;
					this->mark_obstacle(this->r, this->c+1);
				}
				if(leftobstacles > 2 && this->c > 0){               // obstacle on the left
					this->matrix[this->r][this->c-1] = -3 ;
					this->mark_obstacle(this->r, this->c-1);
				}
				if(this->FDistance < fdist_obstacle && this->r > 0){               // obstacle in the front
					this->matrix[this->r-1][this->c] = -3 ;
					this->mark_obstacle(this->r-1, this->c);
				}
				break;
			case E:
				if(rightobstacles > 2 && this->r < SIZE-1){          // obstacle on right
					this->matrix[this->r+1][this->c] = -3 ;
					this->mark_obstacle(this->r+1, this->c);
				}
				if(leftobstacles > 2  && this->r > 0){               // obstacle on the left
					this->matrix[this->r-1][this->c] = -3 ;
					this->mark_obstacle(this->r-1, this->c);
				}
				if(this->FDistance < fdist_obstacle && this->c < SIZE-1){               // obstacle in the front
					this->matrix[this->r][this->c+1] = -3 ;
					this->mark_obstacle(this->r, this->c+1);
				}
				break;

			case S:
				if(rightobstacles > 2  && this->c > 0){          // obstacle on right
					this->matrix[this->r][this->c-1] = -3 ;
					this->mark_obstacle(this->r, this->c-1);
				}
				if(leftobstacles > 2  && this->c < SIZE-1){               // obstacle on the left
					this->matrix[this->r][this->c+1] = -3 ;
					this->mark_obstacle(this->r, this->c+1);
				}
				if(this->FDistance < fdist_obstacle && this->r < SIZE-1){               // obstacle in the front
					this->matrix[this->r+1][this->c] = -3 ;
					this->mark_obstacle(this->r+1, this->c);
				}
				break;
			case W:
				if(rightobstacles > 2  && this->r > 0){          // obstacle on right
					this->matrix[this->r-1][this->c] = -3 ;
					this->mark_obstacle(this->r-1, this->c);
				}
				if(leftobstacles > 2  && this->r < SIZE-1){               // obstacle on the left
					this->matrix[this->r+1][this->c] = -3 ;
					this->mark_obstacle(this->r+1, this->c);
				}
				if(this->FDistance < fdist_obstacle && this->c < SIZE-1){               // obstacle in the front
					this->matrix[this->r][this->c+1] = -3 ;
					this->mark_obstacle(this->r, this->c+1);
				}
				break;
			default:return 2;
		}

		flag = this->moveR(CELL_LENGTH,-2 , leftSide, rightSide);
		if(flag == 3){
			HAL_Delay(1000);
			continue;
		}else{stucked++;}

		flag= this->moveF(CELL_LENGTH,-2 ,  leftSide, rightSide);
		if(flag == 3){
			HAL_Delay(1000);
			continue;
		}else{stucked++;}

		flag= this->moveL(CELL_LENGTH,-2 ,  leftSide, rightSide);
		if(flag == 3){
			HAL_Delay(1000);
			continue;
		}else{stucked++;}

		if(stucked == 3){
			d = dijkstra();
			if(d == -1){         // no more cell to visit
				return 3 ;
			}else{
				if(go_to(d,leftSide,rightSide) == 5){
					this->mark_unvisited();
					return 5 ;
				}

			}
		}
	}
	return 1 ;
}

void Tracker::set_enf(bool enf){
	this->enf = enf;
}

void Tracker::set_ok (bool ok ){
	this->ok = ok ;
}
