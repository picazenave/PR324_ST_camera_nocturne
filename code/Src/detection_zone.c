#include "detection_zone.h"
#include <stdio.h>
#include <stdlib.h>

// Implémentation des fonctions

// Converte the ranging sensor to a matrix 8x8
static void sensor2matrix(RANGING_SENSOR_Result_t *pResult, uint32_t matrix8x8[8][8]) {

    int k = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            matrix8x8[i][j] = pResult->ZoneResult[k++].Distance[0];
        }
    }


}

// Print the matrix 8<8
static void print_matrix8x8(uint32_t matrix8x8[8][8]) {
    printf("Printing 8x8 matrix:\r\n");
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            printf("%8u ", matrix8x8[i][j]);
        }
        printf("\r\n");
    }
}

// Write the pattern on the matrix 8x8
static void matrix_pattern(DetectionZone* obj) {
    // Fill the matrix with the specified pattern
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            int min = row < col ? row : col;
            int max = row < col ? col : row;
            obj->matrix8x8[row][col] = (min < 8 - max) ? min + 1 : 8 - max;
            // obj->matrix8x8[row][col] = 1;
        }
    }
}

// Calcul the detection for each zone
static void calcul_counters(DetectionZone* obj) {
    // Réinitialiser les compteurs
    for (int i = 0; i < 5; ++i) {
        obj->counters[i] = 0;
    }

    // Compter les valeurs de la matrice
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            int value = obj->matrix8x8[row][col];
            obj->counters[value - 1]++;
            obj->counters[4]++;
        }
    }
}

// Print the counter table
static void print_counters(DetectionZone* obj) {
    printf("Counters:\r\n");
    for (int i = 0; i < 5; ++i) {
        printf("Counter %d: %d\r\n", i + 1, obj->counters[i]);
    }
    printf("\r\n");
}

// Compare 2 matrix with the counters table of 2 Detection Zone (n-1 and n)
int compare(DetectionZone* obj, DetectionZone* obj_new, int comparaison[5]){
    calcul_counters(obj);
    calcul_counters(obj_new);
    
    for (int i = 0; i < 5; ++i) {
        int value = obj_new->counters[i] - obj->counters[i];
        switch (value) {
        case 0:
            printf("You entered zero.\r\n");
            comparaison[i] = STABLE;
            break;
        case 1:
            printf("You entered a positive number.\r\n");
            comparaison[i] = INCREASE;
            break;
        default:
            printf("You entered a negative number.\r\n");
            comparaison[i] = DECREASE;
        }      
    }
    
    for (int i = 3; i > 0; --i) {
        switch (comparaison[i]) {
        case INCREASE:
            return INCREASE;
        case DECREASE:
            return DECREASE;
        default:
            break;
        }    
    }
    
}

// Check the comparaison and return a status
int check(DetectionZone* obj, RANGING_SENSOR_Result_t *pResult){
    if (obj->counters[4] == 0)
    {
        // Initialization, so no check and just register

        printf("Initialization\r\n");

        uint32_t new_matrix8x8[8][8];
        obj->sensor2matrix(pResult, new_matrix8x8);
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                obj->matrix8x8[i][j] = new_matrix8x8[i][j];
            }
        }

        print_matrix8x8(obj->matrix8x8);

        return INITIALIZATION;
    }
    else
    {
        // Check the evolution of the matrix

        printf("Check the evolution\r\n");

        uint32_t new_matrix8x8[8][8];
        obj->sensor2matrix(pResult, new_matrix8x8);

        DetectionZone* obj_new = create();
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                obj->matrix8x8[i][j] = new_matrix8x8[i][j];
            }
        }

        int comparaison[5];
        int status = compare(obj, obj_new, comparaison);

        if (status != DECREASE)
        {
            obj = obj_new;
        }
        
        return status;
    }
}

DetectionZone* create() {
    DetectionZone* obj = (DetectionZone*)malloc(sizeof(DetectionZone));

    if (obj != NULL) {
        for (int i = 0; i < 5; ++i) {
            obj->counters[i] = 0;
        }
        // matrix_pattern(obj);
        obj->print_matrix8x8 = print_matrix8x8;
        obj->calcul_counters = calcul_counters;
        obj->print_counters = print_counters;
        obj->check = check;
        obj->sensor2matrix = sensor2matrix;
    }

    return obj;
}

void destroy(DetectionZone* obj) {
    free(obj);
}

// int main() {
//     // Create an instance of DetectionZone
//     DetectionZone* obj = create();

//     // Call methods on the instance
//     obj->print_matrix8x8(obj);
//     obj->calcul_counters(obj);
//     obj->print_counters(obj);

//     // Destroy the instance
//     destroy(obj);

//     return 0;
// }
