#include "detection_zone.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t matrice_zones[64] = {
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 2, 2, 2, 2, 2, 2, 1,
    1, 2, 3, 3, 3, 3, 2, 1,
    1, 2, 3, 4, 4, 3, 2, 1,
    1, 2, 3, 4, 4, 3, 2, 1,
    1, 2, 3, 3, 3, 3, 2, 1,
    1, 2, 2, 2, 2, 2, 2, 1,
    1, 1, 1, 1, 1, 1, 1, 1
};

uint8_t matrice_centre[64] = {
    3, 3, 3, 3, 3, 3, 3, 4,
    3, 2, 2, 2, 2, 2, 3, 4,
    3, 2, 1, 1, 1, 2, 3, 4,
    3, 2, 1, 0, 1, 2, 3, 4,
    3, 2, 1, 1, 1, 2, 3, 4,
    3, 2, 2, 2, 2, 2, 3, 4,
    3, 3, 3, 3, 3, 3, 3, 4,
    4, 4, 4, 4, 4, 4, 4, 4
};

uint8_t matrice_trigo[64] = {
    1, 1, 1, 1, 2, 2, 2, 2,
    1, 1, 1, 1, 2, 2, 2, 2,
    1, 1, 1, 1, 2, 2, 2, 2,
    1, 1, 1, 0, 2, 2, 2, 2,
    3, 3, 3, 3, 4, 4, 4, 4,
    3, 3, 3, 3, 4, 4, 4, 4,
    3, 3, 3, 3, 4, 4, 4, 4,
    3, 3, 3, 3, 4, 4, 4, 4
};


uint32_t environment_matrix[64];

// Implémentation des fonctions

// Converte the ranging sensor to a matrix 8x8
void sensor2matrix(RANGING_SENSOR_Result_t *pResult, uint8_t zones_per_line, DetectionZone_t* detect) {
    detect->zones_per_line = zones_per_line;
    detect->number_of_zones = pResult->NumberOfZones;
    // printf("Nomber of zone = %d &  Zone per ligne = %d\r\n", detect->number_of_zones, detect->zones_per_line);
    for (int j = 0; j < detect->number_of_zones; j += detect->zones_per_line) {
        for (int k = (detect->zones_per_line - 1); k >= 0; k--) {
            detect->matrix_distance[j + k] = pResult->ZoneResult[j + k].Distance[0];
        }
    }
}

void print_matrix(uint32_t matrix8x8[64]) {
    for (uint8_t j = 0; j < 64; j += 8) {
        for (int8_t k = (8 - 1); k >= 0; k--) {
            printf("| %4ld ", matrix8x8[j+k]);
        }
        printf("|\r\n");
    }
}

void print_2_matrix(uint32_t matrix8x8_1[64], uint32_t matrix8x8_2[64]) {
    for (uint8_t j = 0; j < 64; j += 8) {
        for (int8_t k = (8 - 1); k >= 0; k--) {
            printf("| %4lu ", matrix8x8_1[j + k]);
            printf("- %4lu ", matrix8x8_2[j + k]);
        }
        printf("|\r\n");
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
            printf("| %4lu ", matrix_difference[j + k]);
        }
        printf("|\r\n");
    }
    printf("\r\n");
}

// Check if there is a movement in the matrix
int8_t check_evolution(DetectionZone_t* detect_pre, DetectionZone_t* detect_cur) {
    // uint32_t matrix_difference[64];

    // printf("Matrice distance (différence N-1 - N):\r\n");
    // difference_matrix(detect_pre, detect_n, matrix_difference);

    // printf("Matrice distance (différence N-1 - N):\r\n");
    // print_matrix(matrix_difference);

    print_2_matrix(detect_pre->matrix_distance, detect_cur->matrix_distance);

    uint32_t min = RANGE_MAX;
    int8_t indice_min = -1;

    for (int j = 0; j < detect_pre->number_of_zones; j += detect_pre->zones_per_line) {
        for (int k = (detect_pre->zones_per_line - 1); k >= 0; k--) {
            if (  detect_cur->matrix_distance[j + k] > TOO_CLOSE && detect_cur->matrix_distance[j + k] < RANGE_MAX &&
                ( detect_cur->matrix_distance[j + k] > SEUIL_BRUIT_PLUS  * environment_matrix[j+k] ||
                  detect_cur->matrix_distance[j + k] < SEUIL_BRUIT_MOINS * environment_matrix[j+k] ) &&
                  detect_cur->matrix_distance[j + k] < min) {
                    min = detect_cur->matrix_distance[j + k];
                    indice_min = j + k;
                    printf("indice = %d for min distance = %d\r\n", indice_min, min);
            }
        }
    }

    // Affichage du résultat
    if (indice_min == -1)
    {
        printf("No detection, find = %d\r\n", indice_min);
        return -1;
    }
    else
    {
        printf("Le minimum du tableau est : %d\r\n", min);
        printf("Son indice est : %d\r\n", indice_min);
        printf("La distance réelle : %4lu\r\n", detect_cur->matrix_distance[indice_min]);
        return indice_min;
    }
}

// Check the comparaison and return a status
int check(DetectionZone_t* detect, RANGING_SENSOR_Result_t *pResult, uint8_t zones_per_line){
    int8_t find = -1;

    if (detect->initialization != 1)
    {
        // Initialization, so no check and just register

        printf("Initialization\r\n");

        sensor2matrix(pResult, zones_per_line, detect);
        print_matrix(detect->matrix_distance);

        init_environment_matrix(detect, environment_matrix);

        detect->initialization = 1;

        return INITIALISATION;
    }
    else if (detect->initialization == 1 && detect->acquisition == 0)
    {
        // Check the presence of an animal

        printf("Acquisition\r\n");

        DetectionZone_t detect_cur;
        sensor2matrix(pResult, zones_per_line, &detect_cur);

        find = check_evolution(detect, &detect_cur);
        printf("find = %d\r\n", find);

        if (find != -1)
        {
            // An animal is detected

            printf("An animal is detected (@%d)\r\n", find);

            detect->acquisition = 1;

            // voir la définition de la structure : Edouard

            Animal_t animal_find;

            deplacement_animal(&animal_find, find);

            detect->animal = animal_find;

            // Mise à jour de la matrice N-1 par N
            copy_detection_zone(detect, &detect_cur);

            return ANIMAL;
        }

        // Mise à jour de la matrice N-1 par N
        copy_detection_zone(detect, &detect_cur);

        return ACQUISITION;
    }
    else if (detect->initialization == 1 && detect->acquisition == 1)
    {
        // Here we need to follow the movement of the animal : Edouard

        printf("Following an animal\r\n");

        DetectionZone_t detect_cur;
        sensor2matrix(pResult, zones_per_line, &detect_cur);

        find = check_evolution(detect, &detect_cur);
        printf("find = %d\r\n", find);

        if (find != -1)
        {
            // The animal is in movement

            printf("An animal is in movement (@%d)\r\n", find);

            // mettre a jour la structure de l'animal : Edouard
            // detect->animal->

            // Mise à jour de la matrice N-1 par N
            copy_detection_zone(detect, &detect_cur);

            // si l'animal se rapproche du centre et sa direction est OK alors on continue de la suivre
            return ANIMAL;

            // si l'animal s'éloigne du centre et sa direction aussi alors on continue le capture
            return CAPTURE;
        }
        else
        {
            // The animal disappears

            detect->capture = 1;

            // Mise à jour de la matrice N-1 par N
            copy_detection_zone(detect, &detect_cur);
            
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

        return ERROR;
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
    // detect->zones_per_line = new_detect->zones_per_line;
    // detect->number_of_zones = new_detect->zones_per_line;

    for (int j = 0; j < detect->number_of_zones; j += detect->zones_per_line) {
        for (int k = (detect->zones_per_line - 1); k >= 0; k--) {
            detect->matrix_distance[j + k] = new_detect->matrix_distance[j + k];
        }
    }
}

/* Fonction pou l'animal */

// Calcul de la distance au centre
int distance_centre(Animal_t* animal) {

}

// Mise à jour du déplacement de l'animal
void deplacement_animal(Animal_t* animal, int new_position) {

    // Mise à jour de la position N-1 et N
    animal->vec_movement[0] = animal->vec_movement[1];
    animal->vec_movement[1] = new_position;

    // Calcul de la distance par rapport au centre
    animal->distance_centre = matrice_centre[animal->vec_movement[1]]; // TODO avec une fonction

    // Calcul de son angle de direction
    // ! Attention l'angle de direction est liée à la position !
    animal->direction = 0; // TODO avec une fonction
    animal->angle_direction = 0; // TODO avec une fonction
}
