#ifndef DETECTION_ZONE_H
#define DETECTION_ZONE_H

#include <stdint.h>
#include "custom_ranging_sensor.h"

#define INITIALIZATION -1
#define STABLE 0
#define INCREASE 1
#define DECREASE 2


#define NB_SEUIL_ZONE     (4U) /* Nombre de zone à dépasser pour affirmer une présence */
#define TOO_CLOSE         (300U) /* Distance minimale en mm pour ne pas avoir un cible trop proche */
#define RANGE_MAX         (1000U) /* Distance maximale en mm pour ne pas avoir un cible trop éloignée */
#define SEUIL_BRUIT       (5U) /* Le capteur détecte des distances différentes en étant immobile, on rajoute un seuil en % */
#define SEUIL_BRUIT_PLUS  (1.f + (float)SEUIL_BRUIT/100.f)
#define SEUIL_BRUIT_MOINS (1.f - (float)SEUIL_BRUIT/100.f)


// Structure pour DetectionZone
typedef struct
{
    uint32_t matrix_distance[64];
    uint32_t number_of_zones;
    uint8_t zones_per_line;
    int counters[5];
} DetectionZone_t;

static uint32_t matrice_zones[64] = {
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 2, 2, 2, 2, 2, 2, 1,
        1, 2, 3, 3, 3, 3, 2, 1,
        1, 2, 3, 4, 4, 3, 2, 1,
        1, 2, 3, 4, 4, 3, 2, 1,
        1, 2, 3, 3, 3, 3, 2, 1,
        1, 2, 2, 2, 2, 2, 2, 1,
        1, 1, 1, 1, 1, 1, 1, 1
    };

void sensor2matrix(RANGING_SENSOR_Result_t *pResult, uint8_t zones_per_line, DetectionZone_t* detect);
void print_matrix_distance(DetectionZone_t* detect);
void print_2_matrix_distance(DetectionZone_t* detect1, DetectionZone_t* detect2);
void matrix_pattern(DetectionZone_t* detect);
void calcul_counters(DetectionZone_t* detect);
void print_counters(int counters[5]);
int compare(DetectionZone_t* detect, DetectionZone_t* new_detect, int comparaison[5]);
int check(DetectionZone_t* detect, RANGING_SENSOR_Result_t *pResult, uint8_t zones_per_line);
void copy_detection_zone(DetectionZone_t* detect, DetectionZone_t* new_detect);


#endif // DETECTION_ZONE_H
