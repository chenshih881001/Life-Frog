#include <stdio.h>
#include <stdlib.h>

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
int tx, ty, rx, ry, state, xmax=0, ymax=0, moveto, movetoold, rxold = 0, ryold = 0,lastopposidemoveto=0, failcounter=0;

int move(char *world)
{
		state = check_target; 	//reset the state

		Switch(state)
		{
		case check_target:
			if(map[Ycurrent-1][Xcurrent] == 'T')        //check north
  				{
						mvDir = north; //go to north
						target_located = TRUE;
						break;
					}
  		else if(map[Ycurrent][Xcurrent+1] == 'T')   //check east
    		  {
						mvDir = east; //go to east
						target_located = TRUE;
						break;
					}
  		else if(map[Ycurrent+1][Xcurrent] == 'T')	  //check south
    		  {
						mvDir = south;//go to south
						target_located = TRUE;
						break;
					}
  		else if(map[Ycurrent][Xcurrent-1] == 'T')		//check west
					{
						mvDir = west; //go to west
						target_located = TRUE;
						break;
					}
  		else
    		state = prioritization_method;


		case prioritization_method: 			// Quickest way to go to a blank spot
 			switch(direction)
 			{
			case south:
					if(map[Ycurrent+1][Xcurrent] == 'O' && map[Ycurrent+2][Xcurrent] == '_' || map[Ycurrent+1][Xcurrent] == '~' && map[Ycurrent+2][Xcurrent] == '_') // Go south if the spot after that is unknown
  				{
						mvDir=south;					//move direction south
         	  Ytarget = Ycurrent + 1; //change the target coordinates for checking
					}
  				else
        		direction = west;
        	break;

    	case west:
 				if(map[Ycurrent][Xcurrent-1] == 'O' && map[Ycurrent][Xcurrent-2] == '_' || map[Ycurrent][Xcurrent-1] == '~' && map[Ycurrent][Xcurrent-2] == '_') // Try west next
  			{
    				mvDir=west;
            Xtarget = Xcurrent - 1;
        }
        else
        	 direction = north;
  			break;

			case north:
					if(map[Ycurrent-1][Xcurrent] == 'O' && map[Ycurrent-2][Xcurrent] == '_' || map[Ycurrent-1][Xcurrent] == '~' && map[Ycurrent-2][Xcurrent] == '_') // Then north
  				{
						mvDir=north;
          	Ytarget = Ycurrent - 1;
					}
  				else
        		direction = east;
        	break;

			case east:
					if(map[Ycurrent][Xcurrent+1] == 'O' && map[Ycurrent][Xcurrent+2] == '_' || map[Ycurrent][Xcurrent+1] == '~' && map[Ycurrent][Xcurrent+2] == '_') // Lastly east
  				{
        		mvDir=east;
            Xtarget = Xcurrent + 1;
					}
					else
						state = unstuck_method; // If none of these methods works, move to unstucking mode.
        	break;
				}

			case unstuck_method:
				spot_located = 0; //Ensuring that the flag is reset
				for(int a = 0; a < Ymax; a++) //Scan the created map to find a blank spot that is neighbouring to an empty spot
					{
						if (!spot_located)
						{
							for(int b = 0; b < Xmax; b++)
								{
									if(map[a][b] == '_') //Breaking it up to 4 different direction so that it is less a pain to write
										{
											if(map[a+1][b] == 'O' || map[a+1][b] == '~')
											{
												Xtarget = b;
												Ytarget = a;
												spot_located = TRUE;
												break;
											}
											else if(map[a-1][b] == 'O' || map[a-1][b] == '~')
											{
												Xtarget = b;
												Ytarget = a;
												spot_located = TRUE;
												break;
											}
											else if(map[a][b-1] == 'O' || map[a][b-1] == '~')
											{
												Xtarget = b;
												Ytarget = a;
												spot_located = TRUE;
												break;
											}
											else if(map[a][b+1] == 'O' || map[a][b+1] == '~')
											{
												Xtarget = b;
												Ytarget = a;
												spot_located = TRUE;
												break;
											}
											else // If none of the condition above is true, continue the loop
												continue;
										}
								}
						}
						else
							break;
						}
					// Move towards that blank spot

		}

  if(map[Ytarget][Xtarget] == '~' && driveMode == land || map[Ytarget][Xtarget] == 'W' && driveMode == land) //If the target location is water and drive mode is land
  {
  	mvDir = modeSwitch;
  }
  else if(map[Ytarget][Xtarget] == 'O' && driveMode == water || map[Ytarget][Xtarget] == 'F' && driveMode == water) //If the target location is land and drive mode is water
  {
  	mvDir = modeSwitch;
  }
	return mvDir;
}
