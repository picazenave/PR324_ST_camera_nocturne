#ifndef DETECTION_ZONE_H
#define DETECTION_ZONE_H

#include <stdint.h>
#include <math.h>
#include "custom_ranging_sensor.h"


#define NB_SEUIL_ZONE     (4U) /* Nombre de zone à dépasser pour affirmer une présence */
#define TOO_CLOSE         (300U) /* Distance minimale en mm pour ne pas avoir un cible trop proche */
#define RANGE_MAX         (2500U) /* Distance maximale en mm pour ne pas avoir un cible trop éloignée */
#define SEUIL_BRUIT       (5U) /* Le capteur détecte des distances différentes en étant immobile, on rajoute un seuil en % */
#define SEUIL_BRUIT_PLUS  (1.f + (float)SEUIL_BRUIT/100.f)
#define SEUIL_BRUIT_MOINS (1.f - (float)SEUIL_BRUIT/100.f)
#define N 8  // Taille de la matrice (nombre de lignes et de colonnes)

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

// Etat du statut
typedef enum {
    ERREUR = -1,
    INITIALISATION,
    ACQUISITION,
    ANIMAL,
    CAPTURE
} Etat;


// Structure pour représenter les coordonnées (x, y)
struct Coordonnees {
    int8_t x;
    int8_t y;
};


// Structure pour le mouvement de l'animal : Edouard
typedef struct  // ! structure de base, nom et type peut être modifie !
{
    // un vecteur mouvement entre deux cases
    int vec_movement[2];
    // une distance par rapport au centre
    int distance_centre;
    // une direction avec son angle
    int direction;
    int angle_direction;

} Animal_t;

// Structure pour DetectionZone
typedef struct
{
    int      initialization;
    int      acquisition;
    int      capture;
    uint32_t matrix_distance[64];
    uint32_t number_of_zones;
    uint8_t  zones_per_line;
    uint8_t  score;
    Animal_t animal;
} DetectionZone_t;


/****************
 * Initialization
 ****************/

void init_animal(Animal_t* animal);
void init_detection_zone(DetectionZone_t* detect);
void init_trigonometric_matrix(struct Coordonnees trigonometric_matrix[64]);

/**
 * @brief Place les distances mesurées dans une matrice
 * @param pResult        Ranging Sensor
 * @param zones_per_line nombre de zone par ligne
 * @param detect         Détection de zone
*/
void sensor2matrix(RANGING_SENSOR_Result_t *pResult, uint8_t zones_per_line, DetectionZone_t* detect);

/**
 * @brief Afficher une matrice 8x8
 * @param matrix8x8[64] une matrice 8x8
*/
void print_matrix(int32_t matrix8x8[64]);

/**
 * @brief Afficher deux matrices 8x8
 * @param matrix8x8_1[64] une matrice 8x8
 * @param matrix8x8_2[64] une matrice 8x8
*/
void print_2_matrix(int32_t matrix8x8_1[64], int32_t matrix8x8_2[64]);

/**
 * @brief Afficher une matrice en couleur en fonction de la distance
 * @param detect_cur une DetectionZone_t
*/
void print_matrix_color(DetectionZone_t* detect_cur);

/*************
 * Acquisition
 *************/

/**
 * @brief Check l'évolution entre deux matrices en gérant la bruit
 * @param detect matrice n-1
 * @param detect_n matrice n
 * @retval l'indice minimum ou -1 en cas d'erreur
*/
int8_t check_evolution(DetectionZone_t* detect, DetectionZone_t* detect_n);

/**
 * @brief Séquencement du capteur
 * @param detect zone de détection
 * @param pResult ranging_sensor
 * @param zones_per_line nombre de zone par ligne (ici 8)
 * @return le statut du capteur
*/
int check(DetectionZone_t* detect_pre, RANGING_SENSOR_Result_t *pResult, uint8_t zones_per_line);

/**
 * @brief Copie une matrice dans une autre
 * @param matrix8x8_dest reçoit la nouvelle matrice
 * @param matrix8x8_src matrice copiée
*/
void copy_matrix(uint32_t matrix8x8_dest[64], uint32_t matrix8x8_src[64]);

/**
 * @brief Copie de la matrice + score (N vers N-1)
 * @param detect_dest Zone de détection N-1
 * @param detect_src Zone de détection N
*/
void copy_detection_zone(DetectionZone_t* detect_dest, DetectionZone_t* detect_src);


/* Fonction pour l'animal */
void deplacement_animal(Animal_t* animal, int new_position);
void print_animal(Animal_t* animal);

#endif // DETECTION_ZONE_H