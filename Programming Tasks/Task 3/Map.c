#define Xmax 25
#define Ymax 25

int initMap = 0;
int Xorigin = Xmax/2;
int Yorigin = Ymax/2;
char map[Ymax][Xmax];

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
