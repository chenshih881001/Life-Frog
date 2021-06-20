#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>

#define max_row 8
#define max_col 20

struct pair //creating a struct for pair of coordinates for ease of usage
{
  int row;
  int col;
};

struct pPair  //creating a struct for a pair of total cost coordinates for the open list
{
  double f;
  struct pair coord;
};

struct nodes			//Creating struct for the nodes;
{
	int parent_row, parent_col;	//Row and column position of the parents
	double f, g, h;			//f = sum of cost, g = movement cost from starting point to given cell, h = estimated movement cost from current cell to destination
  char travel_dir;
};



double heuristic_calc(int row, int col, struct pair target)		//Calculate the approximate Euclidean Distance
{
  double h = sqrt(pow((row - target.row), 2.0) + pow((col - target.col), 2.0)); //Euclidean Distance formula
  return h;
}

bool isValid(int row, int col)  //Check to see if cell position is valid
{
  if (row >= 0 && row < max_row && col >= 0 && row < max_col)
    return true;
  else
    return false;
}

bool isDestination(int row, int col, struct pair target)  //check to see if it is the destination
{
  if (row == target.row && col == target.col)
    return true;
  else
    return false;
}

bool isOpen(char map[][max_col], int row, int col)  //check to see if the node is an obstacle or not
{
  if(map[row][col]=='O')
    return true;
  else
    return false;
}

void sort(struct pPair *openList, int list_count)
{
  int index_tmp;
  bool smaller_found = false;
  struct pPair tmp_1, tmp_2;                // creating tmp structs
  for(int x = 0; x < list_count; x++)       // first loop is for index
  {
    tmp_1 = openList[x];                    // assign the tmp variable as the current open list member for comparison later
    for (int y = x; y < list_count; y++)    // 2nd for loop for comparison of all element
    {
      if(openList[x].f > openList[y].f && tmp_1.f > openList[y].f)  // If a smaller f is found
      {
        tmp_1 = openList[y];                                        // assign the tmp as that member
        index_tmp = y;                                              // save the index for later
        smaller_found = true;                                       // trip the flag
      }
    }
    if(smaller_found) //Swap the values if a smaller f found
    {
      tmp_2 = openList[x];    //save the current in tmp_2
      openList[x] = tmp_1;    //assign the current as the smaller value
      openList[index_tmp] = tmp_2;  //assign the other index that the current value
      smaller_found = false;
    }
  }
/*  printf("The values of the open list so far after sorting: ");
  for (int a = 0; a < list_count; a++)
  {
    printf("(f: %.3f, r: %d, c: %d), ", openList[a].f, openList[a].coord.row, openList[a].coord.col);
  }
  printf("\n"); */
}

void shift_forward(struct pPair *openList, int list_count)  //For shifting the stack forward and removing the first part of the stack
{
  for(int x = 0; x < list_count; x++)
  {
    if(x != list_count - 1) //if not the last element
      openList[x] = openList[x+1];  //copy the next value into it
    else  //if last value just assign an invalid value so that it can be ignored
    {
        openList[x].f = FLT_MAX;
        openList[x].coord.row = -1;
        openList[x].coord.col = -1;
    }
  }
  /* printf("The values of the open list so far after shifting forward: ");
  for (int a = 0; a < list_count; a++)
  {
    printf("(f: %.3f, r: %d, c: %d), ", openList[a].f, openList[a].coord.row, openList[a].coord.col);
  }
  printf("\n"); */
  return;
}

void shift_backward(struct pPair *openList, int list_count) //For shifting the whole stack backward and adding a new part to the stack
{
  struct pPair tmp_1, tmp_2;                //creating tmp structs
  tmp_1.f = 0.0;        //assign tmp_1 as a blank canvas
  tmp_1.coord.row = 0;
  tmp_1.coord.col = 0;
  for(int x = 0; x < list_count; x++)
  {
    tmp_2 = openList[x];  //Usual switcharoo
    openList[x] = tmp_1;
    tmp_1 = tmp_2;
  }
  /* printf("The values of the open list so far after shifting backward: ");
  for (int a = 0; a < list_count; a++)
  {
    printf("(f: %.3f, r: %d, c: %d), ", openList[a].f, openList[a].coord.row, openList[a].coord.col);
  }
  printf("\n"); */
  return;
}

void draw_path(struct nodes node[][max_col], struct pair target)
{
  printf("\nThe Path is ");
  int row = target.row;
  int col = target.col;
  int count = 0;
  struct pair path[200];
  while(!(node[row][col].parent_row == row && node[row][col].parent_col == col))
  {
    path[count].row = row;
    path[count].col = col;
    count++;
    int temp_row = node[row][col].parent_row;
    int temp_col = node[row][col].parent_col;
    row = temp_row;
    col = temp_col;
  }
  path[count].row = row;
  path[count].col = col;
  for(int i = count; i >= 0; i--)
  {
    printf("-> (%d,%d) ", path[i].row, path[i].col);
  }
  return;
}

void a_star(char map[][max_col], struct pair origin, struct pair target)
{
  bool closedList[max_row][max_col];  //Creating a closed list for all nodes that has been visited
  memset(closedList, false, sizeof(closedList));  //Initially set all of the nodes as not visited

  struct nodes node[max_row][max_col]; //Initiallize the nodes with in the map

  int turn_count = 1; // Turn counter (For debugging)
  int i, j; //Variable for ease of typing i = row, j = column

  for (i = 0; i<max_row; i++) // Assign all of the node
  {
    for (j = 0; j<max_col; j++) //Basically just to Initiallize them
    {
      node[i][j].f = FLT_MAX; //inf f
      node[i][j].g = FLT_MAX; //inf g
      node[i][j].h = FLT_MAX; //inf h
      node[i][j].parent_row = -1; //invalid parent row
      node[i][j].parent_col = -1; //invalid parent column
      node[i][j].travel_dir = 'U';//unknown travel direction
    }
  }

  i = origin.row, j = origin.col; //Make i and j the origin

  node[i][j].f = 0.0;//reset the origin basically
  node[i][j].g = 0.0;
  node[i][j].h = 0.0;
  node[i][j].parent_row = i;
  node[i][j].parent_col = j;
  node[i][j].travel_dir = 'O';

  int list_count = 0; //Initiallize a counter for the open nodes that need to be tested
  struct pPair openList[max_col * max_row]; //Initiallize an list of open nodes
  //Make the first in the list the origin node
  openList[list_count].f = node[i][j].f;
  openList[list_count].coord.row = origin.row;
  openList[list_count].coord.col = origin.col;
  list_count++;

  bool target_located = false; // variable to check if target is located

  while(list_count) //while there are still nodes that need to be check
  {
    /*printf("___________________________________________________________________________________________________");
    printf("\nTurn %d, nodes to be checked %d: \n", turn_count, list_count);


    printf("The values of the open list so far at start of turn: ");
    for (int a = 0; a < list_count; a++)
    {
      printf("(f: %.3f, r: %d, c: %d), ", openList[a].f, openList[a].coord.row, openList[a].coord.col);
    }
    printf("\n");*/


    struct pPair tmp = openList[0]; //copy the 1st index of the openList to tmp
    shift_forward(openList, list_count);  //use up the first element of the list
    list_count--; //subtract the count by 1
    i = tmp.coord.row;  //assign the i and j as the node that need to be tested
    j = tmp.coord.col;

    //printf("Testing row: %d, col: %d.\n", i,j);

/*
         Generating all the 4 successor of this cell

                   N
                   |
               W--Cell--E
                   |
                   S

         Cell-->Current Cell (i, j)
         N -->  North       (i-1, j)
         S -->  South       (i+1, j)
         E -->  East        (i, j+1)
         W -->  West           (i, j-1)
*/

        // To store the 'g', 'h' and 'f' of the 4 neighbours
        double gNew, hNew, fNew;

        //----------- Northern neighbour ------------

        // Only process this cell if this is a valid one
        if (isValid(i - 1, j))
        {
            // If the destination cell is the same as the
            // this neighbouring cell
            if (isDestination(i - 1, j, target))
            {
                // Set the Parent of the destination cell
                node[i - 1][j].parent_row = i;
                node[i - 1][j].parent_col = j;
                printf("The destination cell is found\n");
                draw_path(node, target);
                target_located = true;
                return;
            }
            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            else if (!closedList[i - 1][j] && isOpen(map, i - 1, j))
            {
                gNew = node[i][j].g + 1.0;
                hNew = heuristic_calc(i - 1, j, target);
                fNew = gNew + hNew;

                // If it isn’t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is
                // better, using 'f' cost as the measure.
                if (node[i - 1][j].f == FLT_MAX || node[i - 1][j].f > fNew)
                {
                //    printf("///Neighbour found in the north direction!(%d, %d)\n", i-1, j);
                    list_count++;
                    shift_backward(openList, list_count);
                    openList[0].f = fNew;
                    openList[0].coord.row = i - 1;
                    openList[0].coord.col = j;

                  /*  printf("The values of the open list so far after adding a new neighbour: ");
                    for (int a = 0; a < list_count; a++)
                    {
                      printf("(f: %.3f, r: %d, c: %d), ", openList[a].f, openList[a].coord.row, openList[a].coord.col);
                    }
                    printf("\n"); */

                    // Update the details of this cell
                    node[i - 1][j].f = fNew;
                    node[i - 1][j].g = gNew;
                    node[i - 1][j].h = hNew;
                    node[i - 1][j].parent_row = i;
                    node[i - 1][j].parent_col = j;
                }
            }
        }

        //----------- Southern neighbour ------------

        // Only process this cell if this is a valid one
        if (isValid(i + 1, j))
        {
            // If the destination cell is the same as the
            // this neighbouring cell
            if (isDestination(i + 1, j, target))
            {
                // Set the Parent of the destination cell
                node[i + 1][j].parent_row = i;
                node[i + 1][j].parent_col = j;
                printf("The destination cell is found\n");
                draw_path(node, target);
                target_located = true;
                return;
            }
            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            else if (!closedList[i + 1][j] && isOpen(map, i + 1, j))
            {
                gNew = node[i][j].g + 1.0;
                hNew = heuristic_calc(i + 1, j, target);
                fNew = gNew + hNew;

                // If it isn’t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is
                // better, using 'f' cost as the measure.
                if (node[i + 1][j].f == FLT_MAX || node[i + 1][j].f > fNew)
                {
                    //printf("///Neighbour found in the south direction!(%d, %d)\n", i+1, j);
                    list_count++;
                    shift_backward(openList, list_count);
                    openList[0].f = fNew;
                    openList[0].coord.row = i + 1;
                    openList[0].coord.col = j;

                    /*printf("The values of the open list so far after adding a new neighbour: ");
                    for (int a = 0; a < list_count; a++)
                    {
                      printf("(f: %.3f, r: %d, c: %d), ", openList[a].f, openList[a].coord.row, openList[a].coord.col);
                    }
                    printf("\n");*/

                    // Update the details of this cell
                    node[i + 1][j].f = fNew;
                    node[i + 1][j].g = gNew;
                    node[i + 1][j].h = hNew;
                    node[i + 1][j].parent_row = i;
                    node[i + 1][j].parent_col = j;
                }
            }
        }

        //----------- Eastern neighbour ------------

        // Only process this cell if this is a valid one
        if (isValid(i, j + 1))
        {
            // If the destination cell is the same as the
            // this neighbouring cell
            if (isDestination(i, j + 1, target))
            {
                // Set the Parent of the destination cell
                node[i][j + 1].parent_row = i;
                node[i][j + 1].parent_col = j;
                printf("The destination cell is found\n");
                draw_path(node, target);
                target_located = true;
                return;
            }
            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            else if (!closedList[i][j + 1] && isOpen(map, i, j + 1))
            {
                gNew = node[i][j].g + 1.0;
                hNew = heuristic_calc(i, j + 1, target);
                fNew = gNew + hNew;

                // If it isn’t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is
                // better, using 'f' cost as the measure.
                if (node[i][j + 1].f == FLT_MAX || node[i][j + 1].f > fNew)
                {
              //      printf("///Neighbour found in the east direction!(%d, %d)\n", i, j+1);
                    list_count++;
                    shift_backward(openList, list_count);
                    openList[0].f = fNew;
                    openList[0].coord.row = i;
                    openList[0].coord.col = j + 1;

            /*        printf("The values of the open list so far after adding a new neighbour: ");
                    for (int a = 0; a < list_count; a++)
                    {
                      printf("(f: %.3f, r: %d, c: %d), ", openList[a].f, openList[a].coord.row, openList[a].coord.col);
                    }
                    printf("\n"); */

                    // Update the details of this cell
                    node[i][j + 1].f = fNew;
                    node[i][j + 1].g = gNew;
                    node[i][j + 1].h = hNew;
                    node[i][j + 1].parent_row = i;
                    node[i][j + 1].parent_col = j;
                }
            }
        }

        //----------- Western neighbour ------------

        // Only process this cell if this is a valid one
        if (isValid(i, j - 1))
        {
            // If the destination cell is the same as the
            // this neighbouring cell
            if (isDestination(i, j - 1, target))
            {
                // Set the Parent of the destination cell
                node[i][j - 1].parent_row = i;
                node[i][j - 1].parent_col = j;
                printf("The destination cell is found\n");
                draw_path(node, target);
                target_located = true;
                return;
            }
            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            else if (!closedList[i][j - 1] && isOpen(map, i, j - 1))
            {
                gNew = node[i][j].g + 1.0;
                hNew = heuristic_calc(i, j - 1, target);
                fNew = gNew + hNew;

                // If it isn’t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is
                // better, using 'f' cost as the measure.
                if (node[i][j - 1].f == FLT_MAX || node[i][j - 1].f > fNew)
                {
        //            printf("///Neighbour found in the west direction!(%d, %d)\n", i, j-1);
                    list_count++;
                    shift_backward(openList, list_count);
                    openList[0].f = fNew;
                    openList[0].coord.row = i;
                    openList[0].coord.col = j - 1;

        /*            printf("The values of the open list so far after adding a new neighbour: ");
                    for (int a = 0; a < list_count; a++)
                    {
                      printf("(f: %.3f, r: %d, c: %d), ", openList[a].f, openList[a].coord.row, openList[a].coord.col);
                    }
                    printf("\n"); */

                    // Update the details of this cell
                    node[i][j - 1].f = fNew;
                    node[i][j - 1].g = gNew;
                    node[i][j - 1].h = hNew;
                    node[i][j - 1].parent_row = i;
                    node[i][j - 1].parent_col = j;
                }
            }
        }
    //Sort the open list here
/*    printf("The values of the open list so far before sorting: ");
    for (int a = 0; a < list_count; a++)
    {
      printf("(f: %.3f, r: %d, c: %d), ", openList[a].f, openList[a].coord.row, openList[a].coord.col);
    }
    printf("\n"); */

    sort(openList, list_count);
    turn_count++;
  }
  if (target_located == false)
        printf("Failed to find the Destination Cell\n");

    return;
}


// Driver program to test above function
int main()
{
    char map[max_row][max_col]
        = { {'#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#'},
            {'#','O','T','O','O','O','O','O','O','O','O','O','#','O','O','O','O','O','O','#'},
            {'#','#','#','#','#','#','#','#','#','#','#','O','#','O','O','O','O','O','O','#'},
            {'#','O','O','O','#','O','O','O','O','O','O','O','#','O','O','O','O','O','O','#'},
            {'#','#','O','O','O','O','#','O','#','#','#','#','#','O','O','O','O','O','O','#'},
            {'#','#','#','#','#','#','#','O','#','O','O','O','O','O','O','O','O','O','O','#'},
            {'#','O','O','R','O','O','O','O','#','O','O','O','O','O','O','O','O','O','O','#'},
            {'#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#'},
          };

    // Source is the left-most bottom-most corner
    struct pair origin, target;
    origin.row = 6;
    origin.col = 3;
    // Destination is the left-most top-most corner
    target.row = 1;
    target.col = 2;

    a_star(map, origin, target);

    return (0);
}
