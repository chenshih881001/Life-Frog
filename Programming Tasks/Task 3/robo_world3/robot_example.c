#include <stdio.h>
#include "robot_example.h"

// IMPLEMENT THIS FUNCTION
// ALLOWED RETURN VALUES:
// 1: North, 2: East, 3: South, 4: West, 5: Toggle watern/land mode
int c = 0;

int move(char *world) {
    c = c+1;
    if(c > 3) {
        return 3;
    } else {
        return 1;
    }
}