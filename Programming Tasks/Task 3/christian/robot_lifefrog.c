#include "robot_lifefrog.h"

// IMPLEMET THIS FUNCTION
// ALLOWED RETURN VALUES:
// 1: North, 2: East, 3: South, 4: West, 5: Toggle watern/land mode
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char world2[21][9];
char waterland[21][9];
bool worldiscopied=false, stuck=false, driveLand=true;
int tx, ty, rx, ry, xx, xy, state, xmax=0, ymax=0, moveto, movetoold, rxold = 0, ryold = 0,lastopposidemoveto=0, failcounter=0;

int move(char *world) {
    bool go=false;
    if(!worldiscopied)
    {
        int counter = 0, x = 0, y = 0;
        do
        {
            if(world[counter]!='\n')
            {
                world2[x][y]=world[counter];
                waterland[x][y]=world[counter];
                if(x>xmax){xmax=x;}
                x++;
            }
            else
            {
                x=0;
                if(y>ymax){ymax=y;}
                y++;
            }
            counter++;
        }while(counter<200);
        //printf("copy world with dimensions of x=%i and y=%i\n",xmax,ymax);
        /*for(int i = 0; i<=8; i++)
        {
            for(int j = 0; j<=20; j++)
            {
                printf("%c", world2[j][i]);
            }
            printf("\n");
        }*/
        printf("\n");
        worldiscopied=true;
        for(int i = 0; i<=ymax; i++)
        {
            for(int j = 0; j<=xmax; j++)
            {
                if(world2[j][i]=='T')
                {
                    tx=j;
                    ty=i;
                }
                if(world2[j][i]=='R')
                {
                    rx=j;
                    ry=i;
                    xx=j;
                    xy=i;
                }
            }
        }
        printf("Robot: x=%i, y=%i;      Target: x=%i, y=%i;\n",rx,ry,tx,ty);
    }
    
    if(waterland[rx][ry]=='T') //Are we finally there? I'm hungry and I need to pee!
    {
        tx=xx; //The new target is our home!
        ty=xy;
        printf("Mission complete! Heading back home now x=%i y=%i\n",tx,ty);
        for(int i = 0; i<=8; i++)
        {
            for(int j = 0; j<=20; j++)
            {
                world2[j][i]=waterland[j][i]; //Clear map from Footprints
            }
        }
    }
    
    rxold=rx;
    ryold=ry;
    do{
    switch(state)
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
                state=1;
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
                state=2;
                rx=rxold;
                ry=ryold;
            }
            else
            {
                world2[rxold][ryold]='F';
                state=0;
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
                state=3;
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
                state=0;
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
                state=0;
                go=true;
                moveto=movetoold;
                failcounter=0;
            }
            break;
    }
    }while(!go);
    if(driveLand==true&&waterland[rx][ry]=='~')
    {
        driveLand=false;
        moveto=5;
        rx=rxold;
        ry=ryold;
    }
    else if(driveLand==false&&waterland[rx][ry]!='~'&&waterland[rx][ry]!='#'&&world2[rx][ry]!='*')
    {
        driveLand=true;
        moveto=5;
        rx=rxold;
        ry=ryold;
    }
    go=false;
    printf("Direction %i\n", moveto);
    return moveto;
}
