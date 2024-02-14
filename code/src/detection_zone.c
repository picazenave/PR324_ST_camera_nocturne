#include "detection_zone.h"
#include <stdio.h>
#include <stdlib.h>

// Matrice d'éloignement
uint8_t distance_centre[TAILLE_MATRICE] = {
    6, 5, 4, 3, 3, 4, 5, 6,
    5, 4, 3, 2, 2, 3, 4, 5,
    4, 3, 2, 1, 1, 2, 3, 4,
    3, 2, 1, 0, 0, 1, 2, 3,
    3, 2, 1, 0, 0, 1, 2, 3,
    4, 3, 2, 1, 1, 2, 3, 4,
    5, 4, 3, 2, 2, 3, 4, 5,
    6, 5, 4, 3, 3, 4, 5, 6
};

// Contiendra la toute 1ère matrice, celle de l'environnement face au capteur
uint32_t environment_matrix[TAILLE_MATRICE];

Coordonnees_t trigonometric_matrix[TAILLE_MATRICE];

/* Implémentation des fonctions */

void init_animal(Animal_t* animal) {
    animal->vec_movement[0] = -1;
    animal->vec_movement[1] = -1;
    animal->distance_centre = -1;
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

void init_trigonometric_matrix(Coordonnees_t trigonometric_matrix[TAILLE_MATRICE]) {
    // Initialisation de la matrice avec les coordonnées
    for (int i = 0; i < LARGEUR_MATRICE; i++) {
        for (int j = 0; j < LARGEUR_MATRICE; j++) {
            trigonometric_matrix[i * LARGEUR_MATRICE + j].x = i - LARGEUR_MATRICE / 2 + 1;
            trigonometric_matrix[i * LARGEUR_MATRICE + j].y = j - LARGEUR_MATRICE / 2 + 1;
        }
    }
    
    int indiceCercle = 2;  // Indice du cercle trigonométrique

    for (int i = 0; i < LARGEUR_MATRICE * LARGEUR_MATRICE; i++) {
        // Calcul des nouvelles coordonnées en fonction du cercle trigonométrique
        double angle = (double)indiceCercle * 2 * M_PI / LARGEUR_MATRICE;  // Conversion de l'indice en angle
        double new_x = trigonometric_matrix[i].x * cos(angle) + trigonometric_matrix[i].y * sin(angle);
        double new_y = trigonometric_matrix[i].y * cos(angle) - trigonometric_matrix[i].x * sin(angle);

        // Arrondir les coordonnées pour obtenir des valeurs entières
        trigonometric_matrix[i].x = round(new_x);
        trigonometric_matrix[i].y = round(new_y);
    }

    print_trigonometric_matrix(trigonometric_matrix);
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

void print_matrix(int32_t matrix8x8[TAILLE_MATRICE]) {
    for (uint8_t j = 0; j < TAILLE_MATRICE; j += 8) {
        for (int8_t k = (8 - 1); k >= 0; k--) {
            printf("| %4ld ", matrix8x8[j+k]);
        }
        printf("|\r\n");
    }
}

void print_2_matrix(int32_t matrix8x8_1[TAILLE_MATRICE], int32_t matrix8x8_2[TAILLE_MATRICE]) {
    for (uint8_t j = 0; j < TAILLE_MATRICE; j += 8) {
        for (int8_t k = (8 - 1); k >= 0; k--) {
            printf("| %4ld ", matrix8x8_1[j + k]);
            printf("- %4ld ", matrix8x8_2[j + k]);
        }
        printf("|\r\n");
    }
}

void print_trigonometric_matrix(Coordonnees_t trigonometric_matrix[64]) {
    for (uint8_t j = 0; j < 64; j += 8) {
        for (int8_t k = (8 - 1); k >= 0; k--) {
            printf("(%2d, %2d) ", trigonometric_matrix[j + k].x, trigonometric_matrix[j + k].y);
        }
        printf("\r\n");
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
    printf("Check evolution : find = %d (%4ld)\r\n", indice_min, min);
    return indice_min;
}

int check(DetectionZone_t* detect_pre, RANGING_SENSOR_Result_t *pResult, uint8_t zones_per_line){
    int8_t find = -1;
    int8_t status_animal = -1;
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
            status_animal = deplacement_animal(&detect_pre->animal, find);

            // Mise à jour de la matrice N-1 par N
            copy_detection_zone(detect_pre, &detect_cur);

            return status_animal;

            // // si l'animal s'éloigne du centre et sa direction aussi alors on continue le capture
            // if (distance_centre_pre < detect_pre->animal.distance_centre)
            // {
            //     return CAPTURE;
            // }
            // // si l'animal se rapproche du centre et sa direction est OK alors on continue de la suivre
            // else
            // {
            //     return ANIMAL;
            // }
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

        return ERREUR;
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

void copy_matrix(uint32_t matrix8x8_dest[TAILLE_MATRICE], uint32_t matrix8x8_src[TAILLE_MATRICE]) {
    for (uint8_t j = 0; j < TAILLE_MATRICE; j += 8) {
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

int8_t calcul_distance_centre(Animal_t* animal) {
    int8_t x = trigonometric_matrix[animal->vec_movement[1]].x;
    int8_t y = trigonometric_matrix[animal->vec_movement[1]].y;
    
    if (abs(x) >= abs(y))
        return abs(x);
    return abs(y);
}

float calcul_angle(int8_t x_a, int8_t y_a, int8_t x_b, int8_t y_b) {
    int8_t y_diff = y_b - y_a;
    int8_t x_diff = x_b - x_a;

    if (x_diff != 0)
    {
        float coeff_directeur = (float) y_diff / (float) x_diff;
        float angle_direction = atan(coeff_directeur);

        float angle_direction_degres = angle_direction * (180.0 / M_PI);

        return angle_direction_degres;
    }
    else
    {
        return 90;
    }
}   

int8_t calcul_zone(int8_t x_b, int8_t y_b) {
    if (x_b < 1 && y_b > -1)
    {
        printf("Zone 1\r\n");
        return 1;
    }
    else if (x_b > -1 && y_b > -1)
    {
        printf("Zone 2\r\n");
        return 2;
    }
    else if (x_b > -1 && y_b < 0)
    {
        printf("Zone 3\r\n");
        return 3;
    }
    else if (x_b < 1 && y_b < 0)
    {
        printf("Zone 4\r\n");
        return 4;
    }
    else
    {
        return -1;
    }
}

int8_t check_angle_degre(int8_t zone, float angle_direction_degres) {
    switch (zone)
    {
    case 1:
        if ((ANGLE_MIN <= (180 + angle_direction_degres)) && ((180 + angle_direction_degres) <= ANGLE_MAX))
        {
            return CAPTURE;
        }
        else
        {
            return ANIMAL;
        }
        break;
    case 2:
        if ((ANGLE_MIN <= angle_direction_degres) && (angle_direction_degres <= ANGLE_MAX))
        {
            return CAPTURE;
        }
        else
        {
            return ANIMAL;
        }
        break;
    case 3:
        if ((ANGLE_MIN <= (180 + angle_direction_degres)) && ((180 + angle_direction_degres) <= ANGLE_MAX))
        {
            return CAPTURE;
        }
        else
        {
            return ANIMAL;
        }
        break;
    case 4:
        if ((ANGLE_MIN <= angle_direction_degres) && (angle_direction_degres <= ANGLE_MAX))
        {
            return CAPTURE;
        }
        else
        {
            return ANIMAL;
        }
        break;
    default:
        return ERREUR;
        break;
    }
}

int8_t deplacement_animal(Animal_t* animal, int new_position) {    
    // Mise à jour de la position N-1 et N
    animal->vec_movement[0] = animal->vec_movement[1];
    animal->vec_movement[1] = new_position;

    // Calcul de la distance par rapport au centre
    animal->distance_centre = calcul_distance_centre(animal);

    // Calcul de son angle de direction
    int8_t x_a = trigonometric_matrix[animal->vec_movement[0]].x;
    int8_t y_a = trigonometric_matrix[animal->vec_movement[0]].y;
    int8_t x_b = trigonometric_matrix[animal->vec_movement[1]].x;
    int8_t y_b = trigonometric_matrix[animal->vec_movement[1]].y;

    animal->angle_direction = calcul_angle(x_a, y_a, x_b, y_b);

    int8_t zone = calcul_zone(x_b, y_b);

    int8_t status = check_angle_degre(zone, animal->angle_direction);

    print_animal(animal);

    // Affichage du résultat
    printf("A (%2d, %2d), B (%2d, %2d)\r\n", x_a, y_a, x_b, y_b);
    printf("Angle de direction (degrés) : %lf\r\n", animal->angle_direction);
    printf("Zone : %d\r\n", zone);
    printf("Status : %d\r\n", status);

    return status;
}

void print_animal(Animal_t* animal) {
    printf("Animal :\r\n");
    printf("Position (N-1) : @%d  =>  Position (N) : @%d\r\n", animal->vec_movement[0], animal->vec_movement[1]);
    printf("Distance au centre : %d\r\n", animal->distance_centre);
    printf("Angle de direction : %f\r\n", animal->angle_direction);
}