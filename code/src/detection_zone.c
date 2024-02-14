#include "detection_zone.h"
#include <stdio.h>
#include <stdlib.h>


// Contiendra la toute 1ère matrice, celle de l'environnement face au capteur
uint32_t environment_matrix[64];

struct Coordonnees trigonometric_matrix[64];

/* Implémentation des fonctions */

void init_animal(Animal_t* animal) {
    animal->vec_movement[0] = -1;
    animal->vec_movement[1] = -1;
    animal->distance_centre = -1;
    animal->direction = -1;
    animal->angle_direction = -1;
}

void init_detection_zone(DetectionZone_t* detect) {
    detect->initialization = 0;
    detect->acquisition = 0;
    detect->capture = 0;
    // detect->matrix_distance = {0};
    detect->number_of_zones = 0;
    detect->zones_per_line = 0;
    detect->score = 0;
    init_animal(&detect->animal);
}

void init_trigonometric_matrix(struct Coordonnees trigonometric_matrix[64]) {
    // Initialisation de la matrice avec les coordonnées
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            trigonometric_matrix[i * N + j].x = i - N / 2 + 1;
            trigonometric_matrix[i * N + j].y = j - N / 2 + 1;
        }
    }
    
    int indiceCercle = 2;  // Indice du cercle trigonométrique

    for (int i = 0; i < N * N; i++) {
        // Calcul des nouvelles coordonnées en fonction du cercle trigonométrique
        double angle = (double)indiceCercle * 2 * M_PI / N;  // Conversion de l'indice en angle
        double new_x = trigonometric_matrix[i].x * cos(angle) + trigonometric_matrix[i].y * sin(angle);
        double new_y = trigonometric_matrix[i].y * cos(angle) - trigonometric_matrix[i].x * sin(angle);

        // Arrondir les coordonnées pour obtenir des valeurs entières
        trigonometric_matrix[i].x = round(new_x);
        trigonometric_matrix[i].y = round(new_y);
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("(%2d, %2d) ", trigonometric_matrix[i * N + j].x, trigonometric_matrix[i * N + j].y);
        }
        printf("\n");
    }
}

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

void print_matrix(int32_t matrix8x8[64]) {
    for (uint8_t j = 0; j < 64; j += 8) {
        for (int8_t k = (8 - 1); k >= 0; k--) {
            printf("| %4ld ", matrix8x8[j+k]);
        }
        printf("|\r\n");
    }
}

void print_2_matrix(int32_t matrix8x8_1[64], int32_t matrix8x8_2[64]) {
    for (uint8_t j = 0; j < 64; j += 8) {
        for (int8_t k = (8 - 1); k >= 0; k--) {
            printf("| %4ld ", matrix8x8_1[j + k]);
            printf("- %4ld ", matrix8x8_2[j + k]);
        }
        printf("|\r\n");
    }
}

int8_t check_evolution(DetectionZone_t* detect_pre, DetectionZone_t* detect_cur) {
    detect_cur->score = 0;
    uint32_t min = RANGE_MAX;
    int8_t indice_min = -1;

    for (int j = 0; j < detect_pre->number_of_zones; j += detect_pre->zones_per_line) {
        for (int k = (detect_pre->zones_per_line - 1); k >= 0; k--) {
            if (  detect_cur->matrix_distance[j + k] > TOO_CLOSE && detect_cur->matrix_distance[j + k] < RANGE_MAX &&
                ( detect_cur->matrix_distance[j + k] > SEUIL_BRUIT_PLUS  * environment_matrix[j+k] ||
                  detect_cur->matrix_distance[j + k] < SEUIL_BRUIT_MOINS * environment_matrix[j+k] )) {
                    detect_cur->score++;
                    if (detect_cur->matrix_distance[j + k] < min)
                    {
                        min = detect_cur->matrix_distance[j + k];
                        indice_min = j + k;
                    }
            }
        }
    }
    printf("Check evolution : find = %d (%4d)\r\n", indice_min, min);
    return indice_min;
}

int check(DetectionZone_t* detect_pre, RANGING_SENSOR_Result_t *pResult, uint8_t zones_per_line){
    int8_t find = -1;
    DetectionZone_t detect_cur;
    int distance_centre_pre;
    
    // Initialization, so no check and just register
    if (detect_pre->initialization != 1)
    {
        printf("Initialization\r\n");

        sensor2matrix(pResult, zones_per_line, detect_pre);
        print_matrix(detect_pre->matrix_distance);

        copy_matrix(environment_matrix, detect_pre->matrix_distance);
        init_trigonometric_matrix(trigonometric_matrix);

        detect_pre->initialization = 1;

        return INITIALISATION;
    }
    // Check the presence of an animal
    else if (detect_pre->initialization == 1 && detect_pre->acquisition == 0)
    {
        printf("Acquisition\r\n");

        sensor2matrix(pResult, zones_per_line, &detect_cur);

        find = check_evolution(detect_pre, &detect_cur);

        // An animal is detected
        if (find != -1)
        {
            printf("An animal is detected (@%d)\r\n", find);

            detect_pre->acquisition = 1;

            detect_pre->animal.vec_movement[1] = find;

            // Mise à jour de la matrice N-1 par N
            copy_detection_zone(detect_pre, &detect_cur);

            return ANIMAL;
        }

        // Mise à jour de la matrice N-1 par N
        copy_detection_zone(detect_pre, &detect_cur);

        return ACQUISITION;
    }
    // Following the movement of the animal
    else if (detect_pre->initialization == 1 && detect_pre->acquisition == 1)
    {
        printf("Following an animal\r\n");

        sensor2matrix(pResult, zones_per_line, &detect_cur);

        find = check_evolution(detect_pre, &detect_cur);

        // The animal is in movement
        if (find != -1)
        {
            printf("An animal is in movement (@%d)\r\n", find);

            distance_centre_pre = detect_pre->animal.distance_centre;

            // mettre a jour la structure de l'animal : Edouard
            deplacement_animal(&detect_pre->animal, find);

            // Mise à jour de la matrice N-1 par N
            copy_detection_zone(detect_pre, &detect_cur);

            // si l'animal s'éloigne du centre et sa direction aussi alors on continue le capture
            if (distance_centre_pre < detect_pre->animal.distance_centre)
            {
                return CAPTURE;
            }
            // si l'animal se rapproche du centre et sa direction est OK alors on continue de la suivre
            else
            {
                return ANIMAL;
            }
            
        }
        // The animal disappears
        else
        {
            detect_pre->acquisition = 0;
            detect_pre->capture = 0;

            // Mise à jour de la matrice N-1 par N
            copy_detection_zone(detect_pre, &detect_cur);
            
            return CAPTURE;
        }

    }
    // Reset and comeback in the acquisition process
    else if (detect_pre->initialization == 1 && detect_pre->acquisition == 1 && detect_pre->capture == 1)
    {
        // Peut-être que cette étape n'est pas nécessaire
        // doit être fait dans le main qui appelle cette fonction quand celle-ci retourne CAPTURE
        detect_pre->acquisition = 0;
        detect_pre->capture = 0;

        return ACQUISITION;
    }
    // Error
    else
    {
        printf("Error\r\n");

        return ERROR;
    }
    
}

void print_matrix_color(DetectionZone_t* detect_cur) {
    for (int j = 0; j < detect_cur->number_of_zones; j += detect_cur->zones_per_line) {
        for (int k = (detect_cur->zones_per_line - 1); k >= 0; k--) {


            //  Trop proche
          if (  detect_cur->matrix_distance[j + k] < TOO_CLOSE &&
              ( detect_cur->matrix_distance[j + k] > SEUIL_BRUIT_PLUS  * environment_matrix[j+k] ||
                detect_cur->matrix_distance[j + k] < SEUIL_BRUIT_MOINS * environment_matrix[j+k] ))
          {
            // nombre_zone++;
            printf("| " RED "%5ld" RESET " ", (long)detect_cur->matrix_distance[j + k]);
          }
          // bonne zone (donc regarder l'évolution)
          else if (  detect_cur->matrix_distance[j + k] < RANGE_MAX &&
                   ( detect_cur->matrix_distance[j + k] > SEUIL_BRUIT_PLUS  * environment_matrix[j+k] ||
                     detect_cur->matrix_distance[j + k] < SEUIL_BRUIT_MOINS * environment_matrix[j+k] ))
          {
            // -------- nombre_zone++;
            printf("| " GREEN "%5ld" RESET " ", (long)detect_cur->matrix_distance[j + k]);
          }
          // Trop loin
          else if ( detect_cur->matrix_distance[j + k] > SEUIL_BRUIT_PLUS  * environment_matrix[j+k] ||
                    detect_cur->matrix_distance[j + k] < SEUIL_BRUIT_MOINS * environment_matrix[j+k] )
          {
            printf("| " YELLOW "%5ld" RESET " ", (long)detect_cur->matrix_distance[j + k]);
          }
          // Stable
          else
          {
            printf("| " WHITE "%5ld" RESET " ", (long)detect_cur->matrix_distance[j + k]);
          }
        }
    }
}

void copy_matrix(uint32_t matrix8x8_dest[64], uint32_t matrix8x8_src[64]) {
    for (uint8_t j = 0; j < 64; j += 8) {
        for (int8_t k = (8 - 1); k >= 0; k--) {
            matrix8x8_dest[j + k] = matrix8x8_src[j + k];
        }
    }
}

void copy_detection_zone(DetectionZone_t* detect_dest, DetectionZone_t* detect_src) {
    detect_dest->score = detect_src->score;
    copy_matrix(detect_dest->matrix_distance, detect_src->matrix_distance);
}

/*********************** Fonction pour l'animal ***********************/

void deplacement_animal(Animal_t* animal, int new_position) {    
    // Mise à jour de la position N-1 et N
    animal->vec_movement[0] = animal->vec_movement[1];
    animal->vec_movement[1] = new_position;

    // Calcul de la distance par rapport au centre
    // animal->distance_centre = matrice_centre[animal->vec_movement[1]]; // TODO avec une fonction

    // Calcul de son angle de direction
    // ! Attention l'angle de direction est liée à la position !
    animal->direction = 0; // TODO avec une fonction
    animal->angle_direction = 0; // TODO avec une fonction

    print_animal(animal);
}

void print_animal(Animal_t* animal) {
    printf("Animal :\r\n");
    printf("Position (N-1) : @%d  =>  Position (N) : @%d\r\n", animal->vec_movement[0], animal->vec_movement[1]);
    printf("Distance au centre : %d\r\n", animal->distance_centre);
    printf("Diretion : %d\r\n", animal->direction);
    printf("Angle de direction : %d\r\n", animal->angle_direction);
}