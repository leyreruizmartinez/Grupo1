#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int x;
    int y;
} Point;

int main() {
    int i, j;
    
    Point ***points = (Point ***)malloc(5 * sizeof(Point **));
    if (points == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }
    
    for (i = 0; i < 5; i++) {
        points[i] = (Point **)malloc(4 * sizeof(Point *));
        if (points[i] == NULL) {
            printf("Memory allocation failed\n");
            return 1;
        }
        
        for (j = 0; j < 4; j++) {
            points[i][j] = (Point *)malloc(3 * sizeof(Point));
            if (points[i][j] == NULL) {
                printf("Memory allocation failed\n");
                return 1;
            }
        }
    }
    
    points[2][3][1].x = 3;
    points[2][3][1].y = 4;
    
    printf("Point at [2][3][1]: (%d, %d)\n", points[2][3][1].x, points[2][3][1].y);

    for (i = 0; i < 5; i++) {
        for (j = 0; j < 4; j++) {
            free(points[i][j]);
        }
        free(points[i]);
    }
    free(points);
    
    return 0;
}