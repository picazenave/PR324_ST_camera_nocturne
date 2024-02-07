#include "detection_zone.h"
#include <stdio.h>
#include <stdlib.h>


// Converte the ranging sensor to a matrix 8x8
static void sensor2matrix(RANGING_SENSOR_Result_t *pResult, uint32_t matrix8x8[8][8]) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            matrix8x8[i][j] = pResult->ZoneResult[i + j].Distance[0];
        }
    }
}


static void print_matrix8x8(uint32_t matrix8x8[8][8]) {
    printf("Printing 8x8 matrix:\n");
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            printf("%8lu ", matrix8x8[i][j]);
        }
        printf("\n");
    }
}


static void matrix_pattern(uint32_t matrix8x8[8][8]) {
    int value;

    // Fill the matrix with the specified pattern
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            if (row < col)
                value = row + 1;
            else
                value = col + 1;

            if (row > 7 - col)
                value = 8 - row;

            matrix8x8[row][col] = value;
        }
    }
}



// Constructor for MyClass
// DetectionZone* create(RANGING_SENSOR_Result_t *pResult) {
DetectionZone* create() {
    DetectionZone* obj = (DetectionZone*)malloc(sizeof(DetectionZone));

    if (obj != NULL) {

        matrix_pattern(obj->matrix8x8);
        // sensor2matrix(&pResult, obj->matrix8x8);
        print_matrix8x8(obj->matrix8x8);

        // obj->counters[4];

        // Initialize function pointers
        obj->sensor2matrix = sensor2matrix;
    }

}

// Destructor for MyClass
void destroy(DetectionZone* obj) {
    free(obj);
}
