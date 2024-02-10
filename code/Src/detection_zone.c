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
    for (int j = 0; j < detect->number_of_zones; j += detect->zones_per_line) {
        printf("\r\n");
        for (int k = (detect->zones_per_line - 1); k >= 0; k--) {
            printf("| %4lu ", detect->matrix_distance[j + k]);
        }
        printf("|\r\n");
    }
    // print_matrix(detect->matrix_distance); // Not working
    printf("\r\n");
}

void print_matrix(uint32_t matrix8x8[]) {
    for (uint8_t j = 0; j < 64; j += 8)
    {
        for (uint8_t k = (8 - 1); k >= 0; k--)
        {
        printf("| %5ld ", matrix8x8[j+k]);
        // printf("| %5ld ", j+k);
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
    detect->zones_per_line = 8;
    detect->number_of_zones = 64;

    // // Fill the matrix with the specified pattern
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            int min = i < j ? i : j;
            int max = i < j ? j : i;
            // detect->matrix_distance[i * 8 + j] = (min < 8 - max) ? min + 1 : 8 - max;
            detect->matrix_distance[i * 8 + j] = 2600;
        }
    }
}

// Initialization of the first matrix (the environment matrix)
void init_environment_matrix(DetectionZone_t* detect, uint32_t environment_matrix[64]) {
    for (int j = 0; j < detect->number_of_zones; j += detect->zones_per_line) {
        for (int k = (detect->zones_per_line - 1); k >= 0; k--) {
            environment_matrix[j + k] = detect->matrix_distance[j + k];
        }
    }
    
    printf("Environment matrix:\r\n");
    for (int j = 0; j < detect->number_of_zones; j += detect->zones_per_line) {
        printf("\r\n");
        for (int k = (detect->zones_per_line - 1); k >= 0; k--) {
            printf("| %4lu ", environment_matrix[j + k]);
        }
        printf("|\r\n");
    }
}

// Do the a subtraction : matrix_difference = detect->matrix_distance - detect_n->matrix_distance
void difference_matrix(DetectionZone_t* detect, DetectionZone_t* detect_n, uint32_t matrix_difference[64]) {
    for (int j = 0; j < detect->number_of_zones; j += detect->zones_per_line) {
        for (int k = (detect->zones_per_line - 1); k >= 0; k--) {
            matrix_difference[j + k] = detect->matrix_distance[j + k] - detect_n->matrix_distance[j + k];
        }
    }
}

<<<<<<< HEAD
    // print_2_matrix_distance(detect, new_detect);
    
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
=======
// Check if there is a movement in the matrix
int check_evolution(DetectionZone_t* detect, DetectionZone_t* detect_n) {
    // uint32_t matrix_difference[64];
    // difference_matrix(detect, detect_n, matrix_difference);

    // printf("Matrice distance:\r\n");
    // print_matrix(matrix_difference);

    int min = detect_n->matrix_distance[0];
    int indice_min = 0;

    for (int j = 0; j < detect->number_of_zones; j += detect->zones_per_line) {
        for (int k = (detect->zones_per_line - 1); k >= 0; k--) {
            if ( detect_n->matrix_distance[j + k] > TOO_CLOSE && detect_n->matrix_distance[j + k] < RANGE_MAX
                 && ( detect_n->matrix_distance[j + k] > SEUIL_BRUIT_PLUS  * environment_matrix[j+k]
                 || detect_n->matrix_distance[j + k] < SEUIL_BRUIT_MOINS * environment_matrix[j+k] )
                 &&  detect_n->matrix_distance[j + k] < min) {
                    min = detect_n->matrix_distance[j + k];
                    indice_min = j + k;
            }
        }
>>>>>>> 5ee879522c726ff21ed2d0133932ecfdc1f32c19
    }

    // Affichage du résultat
    if (indice_min == 0)
    {
        printf("No detection\r\n");
        return -1;
    }
    else
    {
        printf("Le maximum du tableau est : %d\r\n", min);
        printf("Son indice est : %d\r\n", indice_min);
        printf("La distance réelle : %4lu\r\n", detect_n->matrix_distance[indice_min]);
        return indice_min;
    }
}

// Check the comparaison and return a status
int check(DetectionZone_t* detect, RANGING_SENSOR_Result_t *pResult, uint8_t zones_per_line){
    if (detect->initialization != 1)
    {
        // Initialization, so no check and just register

        printf("Initialization\r\n");

        sensor2matrix(pResult, zones_per_line, detect);
        print_matrix_distance(detect);

        init_environment_matrix(detect, environment_matrix);
<<<<<<< HEAD
=======

        detect->initialization = 1;
>>>>>>> 5ee879522c726ff21ed2d0133932ecfdc1f32c19

        return INITIALIZATION;
    }
    else if (detect->initialization == 1 && detect->acquisition == 0)
    {
        // Check the presence of an animal

        printf("Acquisition\r\n");

        DetectionZone_t detect_n;
        sensor2matrix(pResult, zones_per_line, &detect_n);

        // Mise à jour de la matrice N-1 par N
        copy_detection_zone(detect, &detect_n);

        int find = check_evolution(detect, &detect_n);

        if (find != -1)
        {
            // An animal is detected

            detect->acquisition = 1;

            // voir la définition de la structure : Edouard

            Animal_t animal_find;

            animal_find.vec_movement[0] = -1; // pas de mouvement/case au temps N-1
            animal_find.vec_movement[1] = find; // case de détection au temps N
            animal_find.distance_centre = 0; // TODO avec une fonction
            animal_find.angle_direction = 0; // TODO avec une fonction

            detect->animal = animal_find;

            return ANIMAL;
        }

        return ACQUISITION;
    }
    else if (detect->initialization == 1 && detect->acquisition == 1)
    {
        // Here we need to follow the movement of the animal : Edouard

        printf("Following an animal\r\n");

        DetectionZone_t detect_n;
        sensor2matrix(pResult, zones_per_line, &detect_n);

        // Mise à jour de la matrice N-1 par N
        copy_detection_zone(detect, &detect_n);

        int find = check_evolution(detect, &detect_n);

        if (find != -1)
        {
            // The animal is in movement

            // mettre a jour la structure de l'animal : Edouard
            // detect->animal->

            // si l'animal se rapproche du centre et sa direction est OK alors on continue de la suivre
            return ANIMAL;

            // si l'animal s'éloigne du centre et sa direction aussi alors on continue le capture
            return CAPTURE;
        }
        else
        {
            // The animal disappears

            detect->capture = 1;

            return CAPTURE;
        }

    }
    else if (detect->initialization == 1 && detect->acquisition == 1 && detect->capture == 1)
    {
        // Reset to comeback in the acquisition process :  Lilian
        
        // Peut-être que cette étape n'est pas nécessaire
        // doit être fait dans le main qui appelle cette fonction quand celle-ci retourne CAPTURE

        return ACQUISITION;
    }
    else
    {
        printf("Error\r\n");

<<<<<<< HEAD
        printf("Check the evolution\r\n");

        DetectionZone_t detect_n;
        sensor2matrix(pResult, zones_per_line, &detect_n);
        distance2evolution(detect, &detect_n);
        copy_detection_zone(detect, &detect_n);

        // DetectionZone_t new_detect;

        // sensor2matrix(pResult, zones_per_line, &new_detect);

        // int comparaison[5];
        // int status = compare(detect, &new_detect, comparaison);

        // if (status != DECREASE)
        // {
        //     copy_detection_zone(detect, &new_detect);
        // }
        
        return 1;
=======
        return ERROR;
>>>>>>> 5ee879522c726ff21ed2d0133932ecfdc1f32c19
    }
    
}

// Convert a distance matrix a evolution 
void distance2evolution(DetectionZone_t* new_detect) {
    for (int j = 0; j < new_detect->number_of_zones; j += new_detect->zones_per_line) {
        for (int k = (new_detect->zones_per_line - 1); k >= 0; k--) {


            //  Trop proche
          if (  new_detect->matrix_distance[j + k] < TOO_CLOSE &&
              ( new_detect->matrix_distance[j + k] > SEUIL_BRUIT_PLUS  * environment_matrix[j+k] ||
                new_detect->matrix_distance[j + k] < SEUIL_BRUIT_MOINS * environment_matrix[j+k] ))
          {
            // nombre_zone++;
            printf("| " RED "%5ld" RESET " ", (long)new_detect->matrix_distance[j + k]);
          }
          // bonne zone (donc regarder l'évolution)
          else if (  new_detect->matrix_distance[j + k] < RANGE_MAX &&
                   ( new_detect->matrix_distance[j + k] > SEUIL_BRUIT_PLUS  * environment_matrix[j+k] ||
                     new_detect->matrix_distance[j + k] < SEUIL_BRUIT_MOINS * environment_matrix[j+k] ))
          {
            // -------- nombre_zone++;
            printf("| " GREEN "%5ld" RESET " ", (long)new_detect->matrix_distance[j + k]);
          }
          // Trop loin
          else if ( new_detect->matrix_distance[j + k] > SEUIL_BRUIT_PLUS  * environment_matrix[j+k] ||
                    new_detect->matrix_distance[j + k] < SEUIL_BRUIT_MOINS * environment_matrix[j+k] )
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
}


// int main() {
//     DetectionZone_t detect_n_1;
//     matrix_pattern(&detect_n_1);

//     DetectionZone_t detect_n;
//     matrix_pattern(&detect_n);
//     detect_n.matrix_distance[12] = 1000;
//     detect_n.matrix_distance[22] = 500;
//     detect_n.matrix_distance[32] = 299;

//     print_matrix_distance(&detect_n_1);
//     print_matrix_distance(&detect_n);

//     int find = check_evolution(&detect_n_1, &detect_n);

//     if (find != -1)
//     {
//         printf("Animal detected\r\n");
//     }


//     return 0;

// }
