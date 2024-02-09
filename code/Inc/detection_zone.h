#ifndef DETECTION_ZONE_H
#define DETECTION_ZONE_H

#include <stdint.h>
#include "custom_ranging_sensor.h"

#define INITIALIZATION -1
#define STABLE 0
#define DECREASE 1
#define INCREASE 2

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

// Structure pour DetectionZone
typedef struct
{
    uint32_t matrix_distance[64];
    uint32_t number_of_zones;
    uint8_t zones_per_line;
    int counters[5];
    uint8_t score[64];
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

void sensor2matrix(RANGING_SENSOR_Result_t *pResult, uint8_t zones_per_line, DetectionZone_t* detect);
void print_matrix_distance(DetectionZone_t* detect);
void print_2_matrix_distance(DetectionZone_t* detect1, DetectionZone_t* detect2);
void matrix_pattern(DetectionZone_t* detect);
void calcul_counters(DetectionZone_t* detect);
void print_counters(int counters[5]);
void init_environment_matrix(DetectionZone_t* detect, uint32_t environment_matix[64]);
int compare(DetectionZone_t* detect, DetectionZone_t* new_detect, int comparaison[5]);
int check(DetectionZone_t* detect, RANGING_SENSOR_Result_t *pResult, uint8_t zones_per_line);
void copy_detection_zone(DetectionZone_t* detect, DetectionZone_t* new_detect);
void distance2evolution(DetectionZone_t* new_detect);


#endif // DETECTION_ZONE_H
