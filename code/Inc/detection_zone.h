#ifndef DETECTION_ZONE_H
#define DETECTION_ZONE_H

#include <stdint.h>
#include "custom_ranging_sensor.h"

#define INITIALIZATION 0
#define INCREASE 1
#define DECREASE 2
#define STABLE 3


// Structure pour DetectionZone
typedef struct
{
    uint32_t matrix8x8[64];
    uint32_t NumberOfZones;
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
void print_matrix8x8(DetectionZone_t* detect);
void matrix_pattern(DetectionZone_t* detect);
void calcul_counters(DetectionZone_t* detect);
void print_counters(int counters[5]);
int compare(DetectionZone_t* detect, DetectionZone_t* new_detect, int comparaison[5]);
int check(DetectionZone_t* detect, RANGING_SENSOR_Result_t *pResult);



#endif // DETECTION_ZONE_H
