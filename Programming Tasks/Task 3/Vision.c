// IMPLEMET THIS FUNCTION
// ALLOWED RETURN VALUES:
// 1: North, 2: East, 3: South, 4: West, 5: Toggle watern/land mode
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


char world[200] = {
        '#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','\n',
        '#','O','T','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','#','\n',
        '#','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','#','\n',
        '#','~','~','~','~','O','O','O','O','O','O','O','O','O','O','O','O','O','O','#','\n',
        '#','~','~','~','~','O','O','O','O','O','O','O','O','O','O','O','O','O','O','#','\n',
        '#','O','#','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','#','\n',
        '#','~','R','~','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','#','\n',
        '#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','\n'
    };

int main()
{
  int col = 0, xmax = 0, ymax = 0, rl=0;
  
  //Find Map Size (length and width)
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

  //Find Robot location
  for(int i=0; i<(xmax*ymax); i++){
    if(world[i]=='R')rl=i;
  }
  
  //Scan Surroundings
  char sur[4]={world[rl-xmax], world[rl+1], world[rl+xmax], world[rl-1]}; //sur={north, east, south, west}

  printf("%c, %c, %c, %c\n", sur[0], sur[1], sur[2], sur[3]);
  
  return *sur;
}
