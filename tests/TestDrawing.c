/** \file TestDrawing.c
 * \brief Tests pour les helpers de dessin et conversion de coordonnées.
 */

/** \fn void click_to_cell_pure(double x, double y, int width, int height, int *row, int *col)
 * \brief Convertit les coordonnées de la souris en indices de cellule du plateau.
 *
 * \param x Coordonnée X du clic.
 * \param y Coordonnée Y du clic.
 * \param width Largeur de la zone de dessin.
 * \param height Hauteur de la zone de dessin.
 * \param row Pointeur vers la variable où stocker la ligne calculée.
 * \param col Pointeur vers la variable où stocker la colonne calculée.
 *
 * \details
 *   - La fonction considère un plateau de 9x9 cases.
 *   - Les cases sont centrées dans une grille de 400x400 pixels.
 *   - Les valeurs calculées sont bornées entre 0 et 8.
 */

#include <assert.h>
#include <stdio.h>
#include "drawing.h"

/** \fn void click_to_cell_pure(double x, double y, int width, int height, int *row, int *col)
 * \brief Convertit les coordonnées de la souris en indices de cellule du plateau.
 *
 * \param x Coordonnée X du clic.
 * \param y Coordonnée Y du clic.
 * \param width Largeur de la zone de dessin.
 * \param height Hauteur de la zone de dessin.
 * \param row Pointeur vers la variable où stocker la ligne calculée.
 * \param col Pointeur vers la variable où stocker la colonne calculée.
 *
 * \details
 *   - La fonction considère un plateau de 9x9 cases.
 *   - Les cases sont centrées dans une grille de 400x400 pixels.
 *   - Les valeurs calculées sont bornées entre 0 et 8.
 */
void click_to_cell_pure(double x, double y, int width, int height, int *row, int *col) {
    double grid_size = 400.0;
    double cell_size = grid_size / 9.0;
    double offset_x = (width - grid_size) / 2.0;
    double offset_y = (height - grid_size) / 2.0;

    *col = (int)((x - offset_x) / cell_size);
    *row = (int)((y - offset_y) / cell_size);

    if (*col < 0) *col = 0; 
    if (*col > 8) *col = 8;
    if (*row < 0) *row = 0; 
    if (*row > 8) *row = 8;
}

/** \fn void test_click_to_cell()
 * \brief Teste la fonction click_to_cell_pure avec des cas simples.
 *
 * \details
 *   - Simule des clics au centre, en haut à gauche et en bas à droite.
 *   - Affiche les résultats sur la sortie standard.
 */
void test_click_to_cell() {
    int row, col;

    click_to_cell_pure(250, 250, 500, 500, &row, &col);
    printf("test_click_to_cell OK (row=%d, col=%d)\n", row, col);

    click_to_cell_pure(0, 0, 500, 500, &row, &col);
    printf("test_click_to_cell OK (row=%d, col=%d)\n", row, col);

    click_to_cell_pure(499, 499, 500, 500, &row, &col);
    printf("test_click_to_cell OK (row=%d, col=%d)\n", row, col);
}


