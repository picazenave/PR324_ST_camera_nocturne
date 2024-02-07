#ifndef DETECTION_ZONE_H
#define DETECTION_ZONE_H

#include <stdint.h>
#include "custom_ranging_sensor.h"

#define INITIALIZATION 0
#define INCREASE 1
#define DECREASE 2
#define STABLE 3


// Structure pour DetectionZone
typedef struct DetectionZone DetectionZone;

struct DetectionZone {
    uint32_t matrix8x8[8][8];
    int counters[5];

    // Pointeurs de fonction
    void (*sensor2matrix)(RANGING_SENSOR_Result_t *pResult, uint32_t matrix8x8[8][8]);
    void (*print_matrix8x8)(uint32_t matrix8x8[8][8]);
    void (*matrix_pattern)(DetectionZone* obj);
    void (*calcul_counters)(DetectionZone* obj);
    void (*print_counters)(DetectionZone* obj);
    int (*compare)(DetectionZone* obj, DetectionZone* obj_new, int comparaison[5]);
    int (*check)(DetectionZone* obj, RANGING_SENSOR_Result_t *pResult);
    
};

// Prototype des fonctions
DetectionZone* create();
void destroy(DetectionZone*);

#endif // DETECTION_ZONE_H
