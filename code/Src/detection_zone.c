#include "detection_zone.h"
#include <stdio.h>
#include <stdlib.h>

// Implémentation des fonctions

// Converte the ranging sensor to a matrix 8x8
void sensor2matrix(RANGING_SENSOR_Result_t *pResult, uint8_t zones_per_line, DetectionZone_t* detect) {
    detect->zones_per_line = zones_per_line;
    detect->number_of_zones = pResult->NumberOfZones;
    for (int j = 0; j < detect->number_of_zones; j += detect->zones_per_line) {
        for (int k = (detect->zones_per_line - 1); k >= 0; k--) {
            detect->matrix_distance[j + k] = pResult->ZoneResult[j + k].Distance[0];
        }
    }
    // print_matrix_distance(detect);
}

// Print the matrix 8x8
void print_matrix_distance(DetectionZone_t* detect) {
    printf("Printing 8x8 matrix:\r\n");
    // for (int j = 0; j < detect->number_of_zones; j += detect->zones_per_line) {
    //     printf("\r\n");
    //     for (int k = (detect->zones_per_line - 1); k >= 0; k--) {
    //         printf("| %4lu ", detect->matrix_distance[j + k]);
    //     }
    //     printf("|\r\n");
    // }
    print_matrix(detect->matrix_distance);
}

void print_matrix(uint32_t matrix8x8[64]) {
    printf("Scores : \r\n");
    for (u_int8_t j = 0; j < 64; j += 8)
    {
      for (u_int8_t k = (8 - 1); k >= 0; k--)
      {
        printf("| %4ld (%d) ", matrix8x8[j+k], j+k);
      }
      printf("|\r\n");
    }
}

// Print 2 matrix 8x8
void print_2_matrix_distance(DetectionZone_t* detect1, DetectionZone_t* detect2) {
    if ( (detect1->zones_per_line == detect2->zones_per_line) &&
        (detect1->number_of_zones == detect2->zones_per_line) )
    {
        printf("Printing 8x8 matrix:\r\n");
        for (int j = 0; j < detect1->number_of_zones; j += detect1->zones_per_line) {
            for (int k = (detect1->zones_per_line - 1); k >= 0; k--) {
                printf("| %4lu ", detect1->matrix_distance[j + k]);
                printf("// %4lu | ", detect2->matrix_distance[j + k]);
            }
            printf("\r\n");
        }
    }
    else
    {
        printf("Size of the two matrix are not the same\r\n");
    }
    
    
}

// Write the pattern on the matrix 8x8
void matrix_pattern(DetectionZone_t* detect) {
    // // Fill the matrix with the specified pattern
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            int min = i < j ? i : j;
            int max = i < j ? j : i;
            detect->matrix_distance[i * 8 + j] = (min < 8 - max) ? min + 1 : 8 - max;
            // detect->matrix_distance[i * 8 + j] = 1;
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
        int value = detect->matrix_distance[i];
        int zone_value = matrice_zones[i];
        detect->counters[zone_value-1] += value;
        detect->counters[4] += value; 
    }

    // print_counters(detect->counters);
}

// Print the counter table
void print_counters(int counters[5]) {
    printf("Counters:\r\n");
    for (int i = 0; i < 5; ++i) {
        printf("Counter %d: %d\r\n", i + 1, counters[i]);
    }
    printf("\r\n");
}

// Initialization of the first matrix (the environment matrix)
void init_environment_matix(DetectionZone_t* detect, uint32_t environment_matix[64]) {
    for (int j = 0; j < detect->number_of_zones; j += detect->zones_per_line) {
        for (int k = (detect->zones_per_line - 1); k >= 0; k--) {
            environment_matix[j + k] = detect->matrix_distance[j + k];
        }
    }
    
    printf("Environment matrix:\r\n");
    for (int j = 0; j < detect->number_of_zones; j += detect->zones_per_line) {
        printf("\r\n");
        for (int k = (detect->zones_per_line - 1); k >= 0; k--) {
            printf("| %4lu ", environment_matix[j + k]);
        }
        printf("|\r\n");
    }
}

// Compare 2 matrix with the counters table of 2 Detection Zone (n-1 and n)
int compare(DetectionZone_t* detect, DetectionZone_t* new_detect, int comparaison[5]){
    calcul_counters(detect);
    calcul_counters(new_detect);

    print_2_matrix_distance(detect, new_detect);
    
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
int check(DetectionZone_t* detect, RANGING_SENSOR_Result_t *pResult, uint8_t zones_per_line){
    if (detect->counters[4] == 0)
    {
        // Initialization, so no check and just register

        printf("Initialization\r\n");

        sensor2matrix(pResult, zones_per_line, detect);
        print_matrix_distance(detect);
        calcul_counters(detect);

        init_environment_matix(detect, environment_matix);

        return INITIALIZATION;
    }
    else
    {
        // Check the evolution of the matrix

        printf("Check the evolution\r\n");

        DetectionZone_t new_detect;

        sensor2matrix(pResult, zones_per_line, &new_detect);

        int comparaison[5];
        int status = compare(detect, &new_detect, comparaison);

        if (status != DECREASE)
        {
            copy_detection_zone(detect, &new_detect);
        }
        
        return status;
    }
}

// Convert a distance matrix a evolution 
void distance2evolution(DetectionZone_t* detect, DetectionZone_t* new_detect) {
    for (int j = 0; j < detect->number_of_zones; j += detect->zones_per_line) {
        for (int k = (detect->zones_per_line - 1); k >= 0; k--) {


            //  Trop proche
          if (  new_detect->matrix_distance[j + k] < TOO_CLOSE &&
              ( new_detect->matrix_distance[j + k] > SEUIL_BRUIT_PLUS  * environment_matix[j+k] ||
                new_detect->matrix_distance[j + k] < SEUIL_BRUIT_MOINS * environment_matix[j+k] ))
          {
            // nombre_zone++;
            printf("| " RED "%5ld" RESET " ", (long)new_detect->matrix_distance[j + k]);
          }
          // bonne zone (donc regarder l'évolution)
          else if (  new_detect->matrix_distance[j + k] < RANGE_MAX &&
                   ( new_detect->matrix_distance[j + k] > SEUIL_BRUIT_PLUS  * environment_matix[j+k] ||
                     new_detect->matrix_distance[j + k] < SEUIL_BRUIT_MOINS * environment_matix[j+k] ))
          {
            // -------- nombre_zone++;
            printf("| " GREEN "%5ld" RESET " ", (long)new_detect->matrix_distance[j + k]);
          }
          // Trop loin
          else if ( new_detect->matrix_distance[j + k] > SEUIL_BRUIT_PLUS  * environment_matix[j+k] ||
                    new_detect->matrix_distance[j + k] < SEUIL_BRUIT_MOINS * environment_matix[j+k] )
          {
            printf("| " YELLOW "%5ld" RESET " ", (long)new_detect->matrix_distance[j + k]);
          }
          // Stable
          else
          {
            printf("| " WHITE "%5ld" RESET " ", (long)new_detect->matrix_distance[j + k]);
          }
        }
    }
}


// Copy the DetectionZone_t structure "new_detect" in the DetectionZone_t structure "detect"
void copy_detection_zone(DetectionZone_t* detect, DetectionZone_t* new_detect) {
    detect->zones_per_line = new_detect->zones_per_line;
    detect->number_of_zones = new_detect->zones_per_line;

    for (int j = 0; j < detect->number_of_zones; j += detect->zones_per_line) {
        for (int k = (detect->zones_per_line - 1); k >= 0; k--) {
            detect->matrix_distance[j + k] = new_detect->matrix_distance[j + k];
        }
    }

    calcul_counters(detect);

    printf("New calcul counters\r\n");
    print_counters(detect);

}


// int main() {
//     DetectionZone_t detect;

//     matrix_pattern(&detect);

//     print_matrix8x8(&detect);

//     calcul_counters(&detect);

//     return 0;

// }
