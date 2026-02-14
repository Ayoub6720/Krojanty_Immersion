#ifndef DRAWING_H
#define DRAWING_H
#include "app.h"

/**
 * @file drawing.h
 * @brief Fonctions de rendu et utilitaires de conversion de coordonnées.
 *
 * Contient la callback de dessin utilisée par GTK et des helpers pour
 * convertir un clic en indices de cellule (row/col).
 */

/**
 * @brief Callback de rendu pour la zone de dessin.
 *
 * Dessine la grille, les pions, les surbrillances et les éléments UI
 * liés au plateau.
 *
 * @param area la zone de dessin
 * @param cr le contexte Cairo
 * @param width largeur en pixels disponible pour le dessin
 * @param height hauteur en pixels disponible pour le dessin
 * @param user_data pointeur optionnel passé par GTK
 */
void draw_cb(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data);

/**
 * @brief Convertit des coordonnées x,y (pixels) en indices de cellule.
 *
 * Si le clic est hors de la grille, `row` et `col` sont mis à -1.
 *
 * @param x position X du clic en pixels
 * @param y position Y du clic en pixels
 * @param row pointeur pour obtenir l'indice de ligne (retour)
 * @param col pointeur pour obtenir l'indice de colonne (retour)
 * @param drawing_area le widget GTK utilisé pour déterminer offsets/scale
 */
void click_to_cell(double x, double y, int *row, int *col, GtkWidget *drawing_area);

#endif // DRAWING_H
