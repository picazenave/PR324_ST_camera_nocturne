#ifndef DETECTION_ZONE_H
#define DETECTION_ZONE_H

#include <stdint.h>
#include "custom_ranging_sensor.h"


// Define the structure for the "class"
typedef struct {
    uint32_t matrix8x8[8][8];

    // Function pointers for methods
    void (*sensor2matrix)(RANGING_SENSOR_Result_t *pResult, uint32_t matrix8x8[8][8]);
    void (*print_matrix8x8)(uint32_t matrix8x8[8][8]);
    void (*matrix_pattern)(uint32_t matrix8x8[8][8]);
} DetectionZone;

// Constructor and Destructor functions
DetectionZone* create();
void destroy(DetectionZone* obj);

#endif  // DETECTION_ZONE_H
