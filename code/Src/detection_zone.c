#include "detection_zone.h"
#include <stdio.h>
#include <stdlib.h>

// Implémentation des fonctions

// Converte the ranging sensor to a matrix 8x8
void sensor2matrix(RANGING_SENSOR_Result_t *pResult, uint8_t zones_per_line, DetectionZone_t* detect) {
    detect->zones_per_line = zones_per_line;
    printf("zone per line %8u \r\n", zones_per_line);
    for (int j = 0; j < pResult->NumberOfZones; j += 8) {
        for (int k = (8 - 1); k >= 0; k--) {
            detect->matrix8x8[j + k] = pResult->ZoneResult[j + k].Distance[0];
            printf("%8lu ", detect->matrix8x8[j + k]);
        }
        printf("\r\n");
    }

    print_matrix8x8(detect);
}

// Print the matrix 8<8
void print_matrix8x8(DetectionZone_t* detect) {
    printf("Printing 8x8 matrix:\r\n");
    for (int j = 0; j < 64; j += 8) {
        for (int k = (8 - 1); k >= 0; k--) {
            printf("%8lu ", detect->matrix8x8[j + k]);
        }
        printf("\r\n");
    }
}

// Write the pattern on the matrix 8x8
void matrix_pattern(DetectionZone_t* detect) {
    // // Fill the matrix with the specified pattern
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            int min = i < j ? i : j;
            int max = i < j ? j : i;
            // detect->matrix8x8[i * 8 + j] = (min < 8 - max) ? min + 1 : 8 - max;
            detect->matrix8x8[i * 8 + j] = 1;
        }
    }
}

// Calcul the detection for each zone
void calcul_counters(DetectionZone_t* detect) {
    // Réinitialiser les compteurs
    for (int i = 0; i < 5; ++i) {
        detect->counters[i] = 0;
    }

    // Compter les valeurs de la matrice
    for (int i = 0; i < 64; i++)
    {
        int value = detect->matrix8x8[i];
        int zone_value = matrice_zones[i];
        detect->counters[zone_value-1] += value;
        detect->counters[4] += value; 
    }

    print_counters(detect->counters);
}

// Print the counter table
void print_counters(int counters[5]) {
    printf("Counters:\r\n");
    for (int i = 0; i < 5; ++i) {
        printf("Counter %d: %d\r\n", i + 1, counters[i]);
    }
    printf("\r\n");
}

// Compare 2 matrix with the counters table of 2 Detection Zone (n-1 and n)
int compare(DetectionZone_t* detect, DetectionZone_t* new_detect, int comparaison[5]){
    calcul_counters(detect);
    calcul_counters(new_detect);
    
    for (int i = 0; i < 5; ++i) {
        int value = new_detect->counters[i] - detect->counters[i];
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
int check(DetectionZone_t* detect, RANGING_SENSOR_Result_t *pResult){
    if (detect->counters[4] == 0)
    {
        // Initialization, so no check and just register

        printf("Initialization\r\n");

        // sensor2matrix(pResult, detect);
        // print_matrix8x8(detect->matrix8x8);

        return INITIALIZATION;
    }
    else
    {
        // Check the evolution of the matrix

        printf("Check the evolution\r\n");

        DetectionZone_t* new_detect;

        // sensor2matrix(pResult, new_detect);

        int comparaison[5];
        int status = compare(detect, new_detect, comparaison);

        if (status != DECREASE)
        {
            detect = new_detect;
        }
        
        return status;
    }
}





// int main() {
//     DetectionZone_t detect;

//     matrix_pattern(&detect);

//     print_matrix8x8(&detect);

//     calcul_counters(&detect);

//     return 0;

// }
