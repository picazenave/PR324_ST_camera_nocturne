#ifndef DETECTION_ZONE_H
#define DETECTION_ZONE_H

#include <stdint.h>
#include "custom_ranging_sensor.h"

#define ERROR -1
#define INITIALIZATION 0
#define ACQUISITION 1
#define ANIMAL 2
#define CAPTURE 3

#define NB_SEUIL_ZONE     (4U) /* Nombre de zone à dépasser pour affirmer une présence */
#define TOO_CLOSE         (300U) /* Distance minimale en mm pour ne pas avoir un cible trop proche */
#define RANGE_MAX         (2500U) /* Distance maximale en mm pour ne pas avoir un cible trop éloignée */
#define SEUIL_BRUIT       (5U) /* Le capteur détecte des distances différentes en étant immobile, on rajoute un seuil en % */
#define SEUIL_BRUIT_PLUS  (1.f + (float)SEUIL_BRUIT/100.f)
#define SEUIL_BRUIT_MOINS (1.f - (float)SEUIL_BRUIT/100.f)

// Définition des couleurs
#define BLACK      "\x1b[30m"
#define RED        "\x1b[31m"
#define GREEN      "\x1b[32m"
#define YELLOW     "\x1b[33m"
#define BLUE       "\x1b[34m"
#define MAGENTA    "\x1b[35m"
#define CYAN       "\x1b[36m"
#define WHITE      "\x1b[37m"
#define RESET      "\x1b[0m"
#define BG_BLACK   "\x1b[40m"
#define BG_RED     "\x1b[41m"
#define BG_GREEN   "\x1b[42m"
#define BG_YELLOW  "\x1b[43m"
#define BG_BLUE    "\x1b[44m"
#define BG_MAGENTA "\x1b[45m"
#define BG_CYAN    "\x1b[46m"
#define BG_WHITE   "\x1b[47m"


// Structure pour le mouvement de l'animal : Edouard
typedef struct  // ! structure de base, nom et type peut être modifié !
{
    // un vecteur mouvement entre deux cases
    int vec_movement[2];
    // une distance par rapport au centre
    int distance_centre;
    // une direction
    int angle_direction;

} Animal_t;


// Structure pour DetectionZone
typedef struct
{
    int initialization;
    int acquisition;
    int capture;
    uint32_t matrix_distance[64];
    uint32_t number_of_zones;
    uint8_t zones_per_line;
    Animal_t animal;
    uint8_t score[64];  // Edouard : pour quel besoin ?
} DetectionZone_t;


static uint8_t matrice_zones[64] = {
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 2, 2, 2, 2, 2, 2, 1,
    1, 2, 3, 3, 3, 3, 2, 1,
    1, 2, 3, 4, 4, 3, 2, 1,
    1, 2, 3, 4, 4, 3, 2, 1,
    1, 2, 3, 3, 3, 3, 2, 1,
    1, 2, 2, 2, 2, 2, 2, 1,
    1, 1, 1, 1, 1, 1, 1, 1
};

static uint32_t environment_matrix[64];


/* Initialization */
void sensor2matrix(RANGING_SENSOR_Result_t *pResult, uint8_t zones_per_line, DetectionZone_t* detect);
<<<<<<< HEAD
void print_matrix_distance(DetectionZone_t* detect);
void print_matrix(uint32_t matrix8x8[64]);
void print_2_matrix_distance(DetectionZone_t* detect1, DetectionZone_t* detect2);
=======
>>>>>>> 5ee879522c726ff21ed2d0133932ecfdc1f32c19
void matrix_pattern(DetectionZone_t* detect);
void init_environment_matrix(DetectionZone_t* detect, uint32_t environment_matix[64]);

/* Print matrix */
void print_matrix_distance(DetectionZone_t* detect);
void print_matrix(uint32_t matrix8x8[64]);
void print_2_matrix_distance(DetectionZone_t* detect1, DetectionZone_t* detect2);
void distance2evolution(DetectionZone_t* new_detect);

/* Acquisition */
int check_evolution(DetectionZone_t* detect, DetectionZone_t* detect_n);
int check(DetectionZone_t* detect, RANGING_SENSOR_Result_t *pResult, uint8_t zones_per_line);

/* Copy a struct in other struct */
void copy_detection_zone(DetectionZone_t* detect, DetectionZone_t* new_detect);

/* Not use but may be use later */
void difference_matrix(DetectionZone_t* detect, DetectionZone_t* detect_n, uint32_t matrix_difference[64]);



#endif // DETECTION_ZONE_H
