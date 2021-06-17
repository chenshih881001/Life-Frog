#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define check_target 0
#define prioritization_method 1
#define unstuck_method 2

//Prioritization:
//1. Move to T
//2. South -> West -> North -> East
//3. Unstuck

#define Xmax 51
#define Ymax 51

#define FALSE 0
#define TRUE 1

#define south 1
#define west 2
#define north 3
#define east 4
#define modeSwitch 5

#define land 0
#define water 1

int state = 0;
int direction = 0;

int Ytarget, Xtarget;
int driveMode = land;
int target_located = 0, spot_located = 0, loop_break1 = 0, loop_break2 = 0;

// Christian's code integrated
char world2[Ymax][Xmax];
char waterland[Ymax][Xmax];
bool worldiscopied=false, stuck=false, driveLand=true;
int tx, ty, rx, ry, state2, xmax=0, ymax=0, moveto, movetoold, rxold = 0, ryold = 0,lastopposidemoveto=0, failcounter=0;

void targeted_move()
{
	bool go=false;

	if(!worldiscopied)		//Copy map into the other maps
	{
			for(int i = 0; i<Xmax; i++)
			{
				for(int j = 0; j<Ymax; j++)
					world2[i][j] = map[j][i];		// the orientation is flipped for Christian's code lmao
			}

			memcpy(waterland, world2, sizeof(world2));
			printf("\n");
			tx = Xtarget;
			ty = Ytarget;
			rx = Xcurrent;
			ry = Ycurrent;
			xx = Xorigin;
			xy = Yorigin;
			worldiscopied=true;
	}
		printf("Robot: x=%i, y=%i;      Target: x=%i, y=%i;\n",rx,ry,tx,ty);
		rxold=rx;
		ryold=ry;

		do{
		switch(state2)
		{
				case 0:
						printf("State 0\n");
						 if(abs(tx-rx)>=abs(ty-ry))
						{
								if(tx-rx>0)
								{
										moveto=2;
										rx++;
								}
								else
								{
										moveto=4;
										rx--;
								}
						}
						else
						{
								if(ty-ry>0)
								{
										moveto=3;
										ry++;
								}
								else
								{
										moveto=1;
										ry--;
								}
						}
						if(world2[rx][ry]=='#'||world2[rx][ry]=='F'||world2[rx][ry]=='*')
						{
								state2=1;
								rx=rxold;
								ry=ryold;
						}
						else
						{
								world2[rxold][ryold]='F';
								go=true;
								stuck=false;
						}
						break;

				case 1:
						printf("State 1\n");
						if(moveto==2||moveto==4) // Try other direction that leads to T
						{
								if(ty-ry>0)
								{
										moveto=3;
										ry++;
								}
								else
								{
										moveto=1;
										ry--;
								}
						}
						else
						{
								if(tx-rx>0)
								{
										moveto=2;
										rx++;
								}
								else
								{
										moveto=4;
										rx--;
								}
						}
						if(world2[rx][ry]=='#'||world2[rx][ry]=='F'||world2[rx][ry]=='*')
						{
								state2=2;
								rx=rxold;
								ry=ryold;
						}
						else
						{
								world2[rxold][ryold]='F';
								state2=0;
								go=true;
								stuck=false;
						}
						break;

				case 2:
						printf("State 2\n");
						int round=0, bestdirection[2][4], initialmoveto=moveto;
						do{
								rx=rxold;
								ry=ryold;
								if(moveto==2||moveto==4) // Try other direction that doesn't leads to T
								{
										if(ty-ry<=0)
										{
												moveto=3;
												ry++;
										}
										else
										{
												moveto=1;
												ry--;
										}
								}
								else
								{
										if(tx-rx<=0)
										{
												moveto=2;
												rx++;
										}
										else
										{
												moveto=4;
												rx--;
										}
								}
								if(round<2)
								{
										bestdirection[round][1]=moveto;
										bestdirection[round][2]=rx;
										bestdirection[round][3]=ry;
										if((world2[rx][ry]!='*'&&world2[rx][ry]!='#'&&world2[rx][ry]!='F')&&(world2[rx][ry--]=='#'||world2[rx--][ry]=='#'||world2[rx][ry++]=='#'||world2[rx++][ry]=='#'||world2[rx][ry--]=='*'||world2[rx--][ry]=='*'||world2[rx][ry++]=='*'||world2[rx++][ry]=='*')) //Reset this if for next task
										{
												bestdirection[round][0]=2;
										}
										else if(world2[rx][ry]!='#'&&world2[rx][ry]!='F'&&world2[rx][ry]!='*')
										{
												bestdirection[round][0]=1;
										}
										else
										{
												bestdirection[round][0]=0;
										}
								}
								round++;
						}while(round<3);
						if(bestdirection[0][0]==0&&bestdirection[1][0]==0)
						{
								state2=3;
								moveto=initialmoveto;
						}
						else
						{
								if(bestdirection[0][0]>bestdirection[1][0])
								{
										moveto=bestdirection[0][1];
										rx=bestdirection[0][2];
										ry=bestdirection[0][3];
								}
								else
								{
										moveto=bestdirection[1][1];
										rx=bestdirection[1][2];
										ry=bestdirection[1][3];
								}
								world2[rxold][ryold]='F';
								state2=0;
								go=true;
								stuck=false;
						}
						break;

				case 3:
						printf("State 3\n");
						if(!stuck)
						{
								printf("Stuck %c\n", world2[rx][ry]);
								stuck=true;
								if(world2[rx][ry]=='#')
								{
										if(moveto==1)
										{
												movetoold=4;
										}
										else
										{
												movetoold=moveto-1;
										}
								}
								if(world2[rx][ry]=='F')
								{
										movetoold=moveto;
								}
						}
						rx=rxold;
						ry=ryold;

						switch(movetoold){
								case 1:
										ry--;
										break;
								case 2:
										rx++;
										break;
								case 3:
										ry++;
										break;
								case 4:
										rx--;
										break;
						}
						//printf("lastopposidemoveto %i, movetoold %i, place %c\n", lastopposidemoveto, movetoold, world2[rx][ry]);
						if(world2[rx][ry]=='*'||world2[rx][ry]=='#'||(lastopposidemoveto==movetoold&&failcounter<4))
						{
								rx=rxold;
								ry=ryold;
								if(movetoold==1)
								{
										movetoold=4;
								}
								else
								{
										movetoold--;
								}
								failcounter++;
						}
						else
						{
								switch(movetoold){
								case 1:
										lastopposidemoveto=3;
										break;
								case 2:
										lastopposidemoveto=4;
										break;
								case 3:
										lastopposidemoveto=1;
										break;
								case 4:
										lastopposidemoveto=2;
										break;
								}

								world2[rxold][ryold]='F';
								state2=0;
								go=true;
								moveto=movetoold;
								failcounter=0;
						}
						break;
		}
		}while(!go);
		go=false;
		printf("Direction %d\n", moveto);
		mvDir = moveto;
		return;
}

int move(char *world)
{
		state = check_target; 	//reset the state
		loop_break1 = FALSE;		//reset the loopbreaker

		while(!loop_break1)
		{
			switch(state)
			{
			case check_target: //If literally adjacent to the target
				printf("Looking for target!\n");
				if(map[Ycurrent-1][Xcurrent] == 'T')        //check north
	  				{
							printf("Target found, going North!\n");
							mvDir = north; //go to north
							target_located = TRUE;
							loop_break1 = TRUE;
							break;
						}
	  		else if(map[Ycurrent][Xcurrent+1] == 'T')   //check east
	    		  {
							printf("Target found, going East!\n");
							mvDir = east; //go to east
							target_located = TRUE;
							loop_break1 = TRUE;
							break;
						}
	  		else if(map[Ycurrent+1][Xcurrent] == 'T')	  //check south
	    		  {
							printf("Target found, going South!\n");
							mvDir = south;//go to south
							target_located = TRUE;
							loop_break1 = TRUE;
							break;
						}
	  		else if(map[Ycurrent][Xcurrent-1] == 'T')		//check west
						{
							printf("Target found, going West!\n");
							mvDir = west; //go to west
							target_located = TRUE;
							loop_break1 = TRUE;
							break;
						}
	  		else
	    		state = prioritization_method;


			case prioritization_method: 			// Quickest way to go to a blank spot
				printf("Target is not found! Trying to explore and find the target!\n");
				loop_break2 = FALSE;
	 			while (!loop_break2)
				{
					switch(direction)
		 			{
					case south:
							if(map[Ycurrent+1][Xcurrent] == 'O' && map[Ycurrent+2][Xcurrent] == '_' || map[Ycurrent+1][Xcurrent] == '~' && map[Ycurrent+2][Xcurrent] == '_') // Go south if the spot after that is unknown
		  				{
								mvDir=south;					//move direction south
		         	  Ytarget = Ycurrent + 1; //change the target coordinates for checking
								loop_break1 = TRUE; // Break the loops
								loop_break2 = TRUE;
								printf("I'm going towards (%d, %d)!\n", Xtarget, Ytarget);
							}
		  				else
		        		direction = west;
		        	break;

		    	case west:
		 				if(map[Ycurrent][Xcurrent-1] == 'O' && map[Ycurrent][Xcurrent-2] == '_' || map[Ycurrent][Xcurrent-1] == '~' && map[Ycurrent][Xcurrent-2] == '_') // Try west next
		  			{
		    				mvDir=west;
		            Xtarget = Xcurrent - 1;
								loop_break1 = TRUE; // Break the loops
								loop_break2 = TRUE;
		        }
		        else
		        	 direction = north;
		  			break;

					case north:
							if(map[Ycurrent-1][Xcurrent] == 'O' && map[Ycurrent-2][Xcurrent] == '_' || map[Ycurrent-1][Xcurrent] == '~' && map[Ycurrent-2][Xcurrent] == '_') // Then north
		  				{
								mvDir=north;
		          	Ytarget = Ycurrent - 1;
								loop_break1 = TRUE; // Break the loops
								loop_break2 = TRUE;
							}
		  				else
		        		direction = east;
		        	break;

					case east:
							if(map[Ycurrent][Xcurrent+1] == 'O' && map[Ycurrent][Xcurrent+2] == '_' || map[Ycurrent][Xcurrent+1] == '~' && map[Ycurrent][Xcurrent+2] == '_') // Lastly east
		  				{
		        		mvDir=east;
		            Xtarget = Xcurrent + 1;
								loop_break1 = TRUE; // Break the loops
								loop_break2 = TRUE;
							}
							else
								state = unstuck_method; // If none of these methods works, move to unstucking mode.
								loop_break2 = TRUE;
		        	break;
						}
				}

				case unstuck_method:	//Breaking out of a pickle
					printf("Look like I am stuck! Trying to break out!\n");
					spot_located = 0; //Ensuring that the flag is reset
					for(int a = 0; a < Ymax; a++) //Scan the created map to find a blank spot that is neighbouring to an empty spot
						{
							if (!spot_located)
							{
								for(int b = 0; b < Xmax; b++)
									{
										if(map[a][b] == '_') //Breaking it up to 4 different direction so that it is less a pain to write
											{
												if(map[a+1][b] == 'O' || map[a+1][b] == '~') //If the adjacent spot is an empty spot
												{
													Xtarget = b;
													Ytarget = a;
													spot_located = TRUE;
													printf("I've found a good spot to go to, target coordinate is (%d, %d)!\n", Xtarget, Ytarget);
													break;
												}
												else if(map[a-1][b] == 'O' || map[a-1][b] == '~')
												{
													Xtarget = b;
													Ytarget = a;
													spot_located = TRUE;
													printf("I've found a good spot to go to, target coordinate is (%d, %d)!\n", Xtarget, Ytarget);
													break;
												}
												else if(map[a][b-1] == 'O' || map[a][b-1] == '~')
												{
													Xtarget = b;
													Ytarget = a;
													spot_located = TRUE;
													printf("I've found a good spot to go to, target coordinate is (%d, %d)!\n", Xtarget, Ytarget);
													break;
												}
												else if(map[a][b+1] == 'O' || map[a][b+1] == '~')
												{
													Xtarget = b;
													Ytarget = a;
													spot_located = TRUE;
													break;
													printf("I've found a good spot to go to, target coordinate is (%d, %d)!\n", Xtarget, Ytarget);
												}
												else // If none of the condition above is true, continue the loop
													continue;
											}
									}
							}
							else
								break;
							}
						 targeted_move();	//move here
					}
		}

  if(map[Ytarget][Xtarget] == '~' && driveMode == land || map[Ytarget][Xtarget] == 'W' && driveMode == land) //If the target location is water and drive mode is land
  {
		printf("Water detected! Changing mode!\n");
  	mvDir = modeSwitch;
  }
  else if(map[Ytarget][Xtarget] == 'O' && driveMode == water || map[Ytarget][Xtarget] == 'F' && driveMode == water) //If the target location is land and drive mode is water
  {
		printf("Land detected! Changing mode!\n");
  	mvDir = modeSwitch;
  }

	if (mvDir != modeSwitch)
	{
		Xcurrent = rx;
		Ycurrent = ry;
	}
	
	return mvDir;
}
