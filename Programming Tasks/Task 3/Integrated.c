// IMPLEMET THIS FUNCTION
// ALLOWED RETURN VALUES:
// 1: North, 2: East, 3: South, 4: West, 5: Toggle water/land mode
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "robot_example.h"

#define Xmax 25
#define Ymax 25

int mvDir;
int initMap = 0;
int Xorigin = Xmax/2;
int Yorigin = Ymax/2;
char map[Ymax][Xmax];

int col = 0, xmax = 0, ymax = 0, robotIndex=0; //For world
int north, east, south, west;
int originPoint=0, initRobot=0; //For Initial Robot Location

int XCurrent = Xorigin;
int YCurrent = Yorigin;

int Xold, Yold;

void InitializeMap()
{

	for(int y = 0; y < Ymax; y++)
  {
  	for(int x = 0; x < Xmax; x++)
    	map[y][x] = '_';
  }

	map[Yorigin][Xorigin] = 'o';
  return;
}

void WorldSize(char *world)
{
  for (int i = 0; world[i]; i++)
  {
    if (world[i] != '\n')
    {
      col++;
      if (col >= xmax)
        xmax = col;
    }
    else if (world[i] == '\n')
    {
      ymax++;
      col=0;
    }
  }
  xmax++;
  return;
}

void PrintMap()
{
	for(int y = 0; y < Ymax; y++)
  {
  	for(int x = 0; x < Xmax; x++)
    {
    	printf(map[y][x]);
    }
    printf("\n");
  }
	return;
}

void robotPosition()
{
  for(int i=0; i<(xmax*ymax); i++)
  {
    if(world[i]=='R')
      robotIndex=i;
  }
}

void FillMap(){
  //Position on the map
  int xindex=Xorigin+((robotIndex%xmax)-(originPoint%xmax));
  int yindex=Yorigin+((robotIndex/xmax)-(originPoint/xmax));
  map[yindex][xindex]='F'; //F = footprint
  map[yindex-1][xindex]=(map[yindex-1][xindex]!='F') ? north : 'F'; //short if statement (if its not already F then copy from sur[])
  map[yindex][xindex+1]=(map[yindex][xindex+1]!='F') ? east : 'F';
  map[yindex+1][xindex]=(map[yindex+1][xindex]!='F') ? south : 'F';
  map[yindex][xindex-1]=(map[yindex][xindex-1]!='F') ? west : 'F';
}

int move(char *world) {
    InitializeMap();

    //Find Map Size (length and width)
    WorldSize(world);

    //Find Robot location
    robotPosition();
    
    if(!initRobot){   //Gets initial robot index
      originPoint=robotIndex;
      initRobot++;
    }

    //Scan Surroundings
    north=world[robotIndex-xmax];
    east=world[robotIndex+1];
    south=world[robotIndex+xmax];
    west=world[robotIndex-1];
    char sur[4]={north, east, south, west};

    //Fill in the map with surroundings
    FillMap();

    PrintMap();
    return mvDir;
}
