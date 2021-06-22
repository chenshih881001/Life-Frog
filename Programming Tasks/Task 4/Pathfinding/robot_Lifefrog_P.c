#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "robot_Lifefrog_P.h"

#define check_target 0
#define prioritization_method 1
#define unstuck_method 2

#define south 3
#define west 4
#define north 1
#define east 2
#define modeSwitch 5

#define DES_N 6
#define DES_E 7
#define DES_S 8
#define DES_W 9

#define land 0
#define water 1

#define Xmax 51
#define Ymax 51

#define FALSE 0
#define TRUE 1


// IMPLEMENT THIS FUNCTION
// ALLOWED RETURN VALUES:
// 1: North, 2: East, 3: South, 4: West, 5: Toggle watern/land mode
int current_id = -1;

int mvDir = 0, mvDirOld; 						// Final return variable
int initMap = 0;

int Xorigin = Xmax/2; // The starting position of the robot
int Yorigin = Ymax/2;

char map[Ymax][Xmax]; // Initialize the robot's map

int col = 0, columns = 0, rows = 0, robotIndex=0; //For world
int originPoint=0, initRobot=0; //For Initial Robot Location

int Xcurrent = Xmax/2; //Variable for current position of the robot
int Ycurrent = Ymax/2;

int Xold, Yold;
char spot_save;

int Xtarget = Xmax/2, Ytarget = Ymax/2; //Variable for target direction
int XtargetOld = 0, YtargetOld = 0; //For Target_Move to check if it needs to clear its map from footprints

int driveMode = land, driveModeOld;
int rock_break = 0;
int target_located = 0, spot_located = 0, loop_break1 = 0, loop_break2 = 0, footprint_cleared = 0;

int state = 0;
int direction = south;

char sur[4];

// Christian's code integrated
char world2[Ymax][Xmax];
char waterland[Ymax][Xmax];
bool worldiscopied=false, stuck=false, driveLand=true;
int tx, ty, rx, ry, xx, xy, state2, xmax=0, ymax=0, moveto, movetoold, rxold = 0, ryold = 0,lastopposidemoveto=0, failcounter=0;

void InitializeMap()
{

	for(int y = 0; y < Ymax; y++)
  {
  	for(int x = 0; x < Xmax; x++)
    	map[y][x] = '_';
  }

	map[Yorigin][Xorigin] = 'X';
	initMap = 1;
  return;
}

void WorldSize(char *world)
{
  for (int i = 0; world[i]; i++)
  {
    if (world[i] != '\n')
    {
      col++;
      if (col >= columns)
        columns = col;
    }
    else if (world[i] == '\n')
    {
      rows++;
      col=0;
    }
  }
  columns++;
  return;
}

void PrintMap()
{
	for(int y = 0; y < Ymax; y++)
  {
  	for(int x = 0; x < Xmax; x++)
    {
    	printf("%c",map[y][x]);
    }
    printf("\n");
  }
	return;
}

void robotPosition(char *world)
{
  for(int i=0; i<(columns*rows); i++)
  {
    if(world[i]=='R')
      robotIndex=i;
  }
}

void FillMap(char *world){
  //Position on the map

	//Marking the trail behind of the Robot
	if(mvDir)		// The reason why the map is done like this because there are no reference map.
	{						// So if I want to see the position of R, I have to use the last drive mode to check for water or land
		if(mvDirOld == modeSwitch && mvDir != modeSwitch && driveModeOld == water)		//The first 2 ifs check to see if the drivemode changed.
		{																																							//This can cause the map to be shifted if not done
			map[Yold][Xold] = 'F';																											//If the drivemode has been changed as the last return, mark the trail as 'F' if the lastdrivemode is land
		}

		else if(mvDirOld == modeSwitch && mvDir != modeSwitch && driveModeOld == land)
		{
			map[Yold][Xold] = 'W';																											//If the drivemode has been changed as the last return, mark the trail as 'F' if the lastdrivemode is water
		}
		else if(driveMode == land && mvDirOld != modeSwitch && mvDir != modeSwitch)
		{
			map[Yold][Xold] = 'F';																											//If drive mode has not been changed, mark the trail as 'F' if the current driveMode is land
		}
		else if(driveMode == water && mvDirOld != modeSwitch && mvDir != modeSwitch)	//If the drivemode has not been changed, mark the trail as 'W' if the current driveMode is water
		{
			map[Yold][Xold] = 'W';
		}
	}

	//Marking the surroundings of the robot if there is no footprint on it already.
	if(map[Ycurrent-1][Xcurrent]!='F' && map[Ycurrent-1][Xcurrent]!='W'){
	  map[Ycurrent-1][Xcurrent] = sur[0];
	}
	if(map[Ycurrent][Xcurrent+1]!='F' && map[Ycurrent][Xcurrent+1]!='W'){
	  map[Ycurrent][Xcurrent+1] = sur[1];
	}
	if(map[Ycurrent+1][Xcurrent]!='F' && map[Ycurrent+1][Xcurrent]!='W'){
	  map[Ycurrent+1][Xcurrent] = sur[2];
	}
	if(map[Ycurrent][Xcurrent-1]!='F' && map[Ycurrent][Xcurrent-1]!='W'){
	  map[Ycurrent][Xcurrent-1] = sur[3];
	}

	map[Yorigin][Xorigin]='X';	//Make sure the origin is always X, unless R is on top of it
	map[Ycurrent][Xcurrent]='R';	//Mark the current position as R


}

void targeted_move()
{
	bool go=false;

    			printf("\n");
			if(!target_located)
			{
				tx = Xtarget;
				ty = Ytarget;
			}
			else
			{
				tx = Xorigin;
				ty = Yorigin;
			}
			rx = Xcurrent;
			ry = Ycurrent;
			xx = Xorigin;
			xy = Yorigin;
	//if(!worldiscopied)		//Copy map into the other maps
//	{
            if(XtargetOld!=tx||YtargetOld!=ty)    //New Target? Clear map from Footprints!
            {
                printf("New Target!");
                for(int i = 0; i<Xmax; i++)
                {
                    for(int j = 0; j<Ymax; j++)
                    {
                        if(map[j][i]=='W')
                        {
                            world2[i][j] = '~';
                        }
                        else if(map[j][i]=='F')
                        {
                            world2[i][j] = 'O';
                        }
                        else
                        {
                            world2[i][j] = map[j][i];
                        }
                    }
                }
                XtargetOld=tx;
                YtargetOld=ty;
            }
            else //old target
            {
                for(int i = 0; i<Xmax; i++)
                {
                    for(int j = 0; j<Ymax; j++)
                    {
                        if(world2[i][j]=='_') //just update new stuff
                            world2[i][j] = map[j][i]; // the orientation is flipped for Christian's code lmao
                    }
                }
            }
            
            
			memcpy(waterland, world2, sizeof(world2));


		//	worldiscopied=true;
//	}
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

            if(rock_break && world2[rx][ry]!='#' && world2[rx][ry]!='F')    // I added this just so to say that if rock_break is tripped, please target *
            {
              world2[rxold][ryold]='F';
              go=true;
              stuck=false;
            }

						else if(world2[rx][ry]=='#'||world2[rx][ry]=='F'||world2[rx][ry]=='*')
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

            if(rock_break && world2[rx][ry]!='#' && world2[rx][ry]!='F')  // I added this just so to say that if rock_break is tripped, please target *
            {
              world2[rxold][ryold]='F';
              state2=0;
              go=true;
              stuck=false;
            }

						else if(world2[rx][ry]=='#'||world2[rx][ry]=='F'||world2[rx][ry]=='*')
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
						if(!rock_break && world2[rx][ry]=='*'||world2[rx][ry]=='#'||(lastopposidemoveto==movetoold&&failcounter<4)) // Saying this just to make try to see if some thing happened. only added the rock_break thing
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
		Xtarget = rx;
		Ytarget = ry;
		go=false;
		printf("Direction %d\n", moveto);
		mvDir = moveto;
		return;
}

int move(char *world, int map_id)
{
    if(map_id != current_id)  // check the map ids
    { //resetting all flags_tbh I have idea what I have to reset here for everyone's code to work to I reset just about everything
      initMap = false;
      target_located = false;
      loop_break1 = false;
      loop_break2 = false;
      footprint_cleared = false;
      driveMode = land;
      direction = south;
      Xcurrent = Xorigin;
      Ycurrent = Yorigin;
      Xold = Xorigin;
      Yold = Yorigin;
      mvDir = 0;
      worldiscopied = false;
      stuck = false;
      driveLand = true;
      spot_located = 0;
      xmax = 0;
      ymax = 0;
      lastopposidemoveto=0;
      failcounter=0;
      sur[0] = '\0';
      sur[1] = '\0';
      sur[2] = '\0';
      sur[3] = '\0';
      current_id = map_id;
      columns = 0;
      rows = 0;
      XtargetOld = 0; 
      YtargetOld = 0;
    }

		if (!initMap)
		{
			InitializeMap();
			//Find world Size (length and width)
	    WorldSize(world);
		}



    //Find Robot location
    robotPosition(world);


		sur[0] = world[robotIndex - columns]; //copy north
		sur[1] = world[robotIndex + 1];			//copy east
		sur[2] = world[robotIndex + columns];	//copy south
		sur[3] = world[robotIndex - 1];			//copy west

    //Fill in the map with surroundings
    FillMap(world);

		PrintMap();

		mvDirOld = mvDir;

		if(!target_located)
		{
			// start of the actual movement code
			state = check_target; 	//reset the state
			loop_break1 = FALSE;		//reset the loopbreaker

			while(!loop_break1)
			{
				switch(state)
				{
				case check_target: //If literally adjacent to the target
					printf("Looking for target!\n");
					if(map[Ycurrent-1][Xcurrent] == 'T' || map[Ycurrent-1][Xcurrent] == 't')        //check north
		  				{
								printf("Target found, going North!\n");
								mvDir = north; //go to north
								target_located = TRUE;
								loop_break1 = TRUE;
								Ytarget = Ycurrent - 1;
								break;
							}
		  		else if(map[Ycurrent][Xcurrent+1] == 'T' || map[Ycurrent][Xcurrent+1] == 't')   //check east
		    		  {
								printf("Target found, going East!\n");
								mvDir = east; //go to east
								target_located = TRUE;
								loop_break1 = TRUE;
								Xtarget = Xcurrent + 1;
								break;
							}
		  		else if(map[Ycurrent+1][Xcurrent] == 'T' || map[Ycurrent+1][Xcurrent] == 't')	  //check south
		    		  {
								printf("Target found, going South!\n");
								mvDir = south;//go to south
								target_located = TRUE;
								loop_break1 = TRUE;
								Ytarget = Ycurrent + 1;
								break;
							}
		  		else if(map[Ycurrent][Xcurrent-1] == 'T' || map[Ycurrent][Xcurrent-1] == 't')		//check west
							{
								printf("Target found, going West!\n");
								mvDir = west; //go to west
								target_located = TRUE;
								loop_break1 = TRUE;
								Xtarget = Xcurrent - 1;
								break;
							}
		  		else
		    		state = prioritization_method;


				case prioritization_method: 			// Quickest way to go to a blank spot
					printf("Target is not found! Trying to explore and find the target!\n");
					loop_break2 = FALSE;
					direction = south;
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
									printf("I'm going towards (%d, %d)!\n", Xtarget, Ytarget);
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
									printf("I'm going towards (%d, %d)!\n", Xtarget, Ytarget);
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
									printf("I'm going towards (%d, %d)!\n", Xtarget, Ytarget);
								}
								else
									state = unstuck_method; // If none of these methods works, move to unstucking mode.
									loop_break2 = TRUE;
			        	break;
							}
					}
					break;

					case unstuck_method:	//Breaking out of a pickle
						printf("Look like I am stuck! Trying to break out!\n");
						for(int a = 0; a < Ymax; a++) //Scan the created map to find a blank spot that is neighbouring to an empty spot
							{
								if (!spot_located)
								{
									for(int b = 0; b < Xmax; b++)
										{
											if(map[a][b] == '_') //Breaking it up to 4 different direction so that it is less a pain to write
												{
													if(map[a+1][b] == 'O' || map[a+1][b] == '~' || map[a+1][b] == '*') //If the adjacent spot is an empty spot
													{
														Xtarget = b;
														Ytarget = a;
														spot_located = 1;
                            if(map[a+1][b] == '*')
                            {
                              printf("Looks like I've got to smash some rocks (%d, %d)!\n", Xtarget, Ytarget);
                              rock_break = 1;
                            }
                            else
														  printf("I've found a good spot to go to, target coordinate is (%d, %d)!\n", Xtarget, Ytarget);
														break;
													}
													else if(map[a-1][b] == 'O' || map[a-1][b] == '~' || map[a-1][b] == '*')
													{
														Xtarget = b;
														Ytarget = a;
														spot_located = 1;
                            if(map[a-1][b] == '*')
                            {
                              printf("Looks like I've got to smash some rocks (%d, %d)!\n", Xtarget, Ytarget);
                              rock_break = 1;
                            }
                            else
														  printf("I've found a good spot to go to, target coordinate is (%d, %d)!\n", Xtarget, Ytarget);
														break;
													}
													else if(map[a][b-1] == 'O' || map[a][b-1] == '~' || map[a][b-1] == '*')
													{
														Xtarget = b;
														Ytarget = a;
														spot_located = 1;
                            if(map[a][b-1] == '*')
                            {
                              printf("Looks like I've got to smash some rocks (%d, %d)!\n", Xtarget, Ytarget);
                              rock_break = 1;
                            }
                            else
														  printf("I've found a good spot to go to, target coordinate is (%d, %d)!\n", Xtarget, Ytarget);
														break;
													}
													else if(map[a][b+1] == 'O' || map[a][b+1] == '~' || map[a][b+1] == '*')
													{
														Xtarget = b;
														Ytarget = a;
														spot_located = 1;
                            if(map[a][b+1] == '*')
                            {
                              printf("Looks like I've got to smash some rocks (%d, %d)!\n", Xtarget, Ytarget);
                              rock_break = 1;
                            }
                            else
														  printf("I've found a good spot to go to, target coordinate is (%d, %d)!\n", Xtarget, Ytarget);
														break;
													}
                          /*else if(map[a-1][b] == '*' || map[a+1][b] == '*' || map[a][b+1] == '*' || map[a][b-1] == '*') //Priotize empty spot, if there is no possible empty spot, smash some rocks
													{
														Xtarget = b;
														Ytarget = a;
														spot_located = 1;
														printf("Looks like I've got to smash some rocks (%d, %d)!\n", Xtarget, Ytarget);
                            rock_break = 1;
														break;
													}*/
												}
										}
								}
								else
								{
									printf("Good spot located!\n", Xtarget, Ytarget);
									break;
								}
								}
							 targeted_move();	//move here
							 loop_break1 = TRUE; // Break the loops
							 break;
						}
			}
		}

		else
		{

			if(!footprint_cleared)
			{
				//Clearing the map of footprint
				for(int i = 0; i<Ymax; i++)
				{
					for(int j = 0; j<Xmax; j++)
					{
						if(map[i][j]=='F')
							map[i][j] = 'O';
						else if(map[i][j]=='W')
							map[i][j] = '~';
					}
				}
				footprint_cleared = TRUE;
			}


			printf("I'm returning to base!\n");
			if(map[Ycurrent-1][Xcurrent] == 'X')        //check north
					{
						printf("Base found, going North!\n");
						mvDir = north; //go to north
					}
			else if(map[Ycurrent][Xcurrent+1] == 'X')   //check east
					{
						printf("Base found, going East!\n");
						mvDir = east; //go to east
					}
			else if(map[Ycurrent+1][Xcurrent] == 'X')	  //check south
					{
						printf("Base found, going South!\n");
						mvDir = south;//go to south
					}
			else if(map[Ycurrent][Xcurrent-1] == 'X')		//check west
					{
						printf("Base found, going West!\n");
						mvDir = west; //go to west
					}
			else
			targeted_move();
		}

	driveModeOld = driveMode;
  if(map[Ytarget][Xtarget] == '~' && driveMode == land || map[Ytarget][Xtarget] == 'W' && driveMode == land) //If the target location is water and drive mode is land
  {
		printf("Water detected! Changing mode!\n");
  	mvDir = modeSwitch;
		driveMode = water;
  }
  else if(map[Ytarget][Xtarget] == 'O' && driveMode == water || map[Ytarget][Xtarget] == 'F' && driveMode == water) //If the target location is land and drive mode is water
  {
		printf("Land detected! Changing mode!\n");
  	mvDir = modeSwitch;
		driveMode = land;
  }
  else if(map[Ytarget][Xtarget] == '*') //If the target location an obstacle
  {
		printf("Smashing some rocks!\n");
  	if(mvDir == north)
      mvDir = DES_N;
    else if(mvDir == south)
      mvDir = DES_S;
    else if(mvDir == east)
      mvDir = DES_E;
    else if(mvDir == west)
      mvDir = DES_W;
    rock_break = 0;
  }

	if (mvDir != modeSwitch)
	{
		Xold = Xcurrent;
		Yold = Ycurrent;
		Xcurrent = Xtarget;
		Ycurrent = Ytarget;
	}
		spot_located = 0; //Ensuring that the flag is reset
    return mvDir;
}
