/**
 * \file drawing.c
 * \brief Fonctions d’affichage du plateau de jeu avec Cairo/GTK.
 * \author loic.claude@uha.fr, nolann.wickers@uha.fr, valentin.prevot@uha.fr, valentin.leray@uha.fr, alan.akgun@uha.fr
 *         ayoub.darkaoui@uha.fr, ismail.el-maallem@uha.fr, corentin.banocay@uha.fr
 * \version 0.1
 * \date 2025-09-16
 *
 * \details
 * Ce fichier contient les fonctions de dessin pour l’interface graphique :
 * - Conversion des coordonnées de clic en indices de cases.
 * - Dessin du plateau de 9x9 cases.
 * - Mise en évidence des cases contrôlées et des déplacements possibles.
 * - Affichage des pions, rois et bases avec leurs couleurs.
 */


#include <cairo.h>
#include <math.h>
#include "drawing.h"
#include "game.h"


/** Logo de la base bleue */
GdkPixbuf *logo_blue = NULL;

/** Logo de la base rouge */
GdkPixbuf *logo_red = NULL;

/**
 * \fn void click_to_cell(double x, double y, int *row, int *col, GtkWidget *drawing_area)
 * \brief Convertit des coordonnées de clic en indices de cellule.
 *
 * \param x Position X du clic (pixels).
 * \param y Position Y du clic (pixels).
 * \param row Pointeur vers l’indice de ligne (retour).
 * \param col Pointeur vers l’indice de colonne (retour).
 * \param drawing_area Widget GTK de la zone de dessin.
 *
 * Si le clic est en dehors de la grille, \p row et \p col valent -1.
 */
void click_to_cell(double x, double y, int *row, int *col, GtkWidget *drawing_area) {
    int width  = gtk_widget_get_width(drawing_area);
    int height = gtk_widget_get_height(drawing_area);
    double grid_size = 400.0;
    double cell_size = grid_size / 9.0;
    double offset_x = (width - grid_size) / 2.0;
    double offset_y = (height - grid_size) / 2.0;

    // Vérifier si le clic est hors de la grille
    if (x < offset_x || x > offset_x + grid_size || y < offset_y || y > offset_y + grid_size) {
        *row = -1;
        *col = -1;
        return;
    }


    *col = (int)((x - offset_x) / cell_size);
    *row = (int)((y - offset_y) / cell_size);
}

/**
 * \fn void draw_cb(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data)
 * \brief Callback de dessin GTK, affiche le plateau et les pièces.
 *
 * \param area Zone de dessin GTK.
 * \param cr Contexte Cairo.
 * \param width Largeur de la zone de dessin.
 * \param height Hauteur de la zone de dessin.
 * \param user_data Donnée utilisateur (non utilisée).
 *
 * Cette fonction gère :
 * - Le remplissage du fond.
 * - Les cases contrôlées par chaque joueur.
 * - Les coups jouables (points noirs).
 * - Les pièces avec leur couleur, sélection et type.
 * - L’affichage des bases avec logos.
 */
void draw_cb(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {
    (void)area; (void)user_data;
    int rows = 9, cols = 9;
    double grid_size = 400.0;
    double cell_size = grid_size / 9.0;
    double offset_x = (width - grid_size) / 2.0;
    double offset_y = (height - grid_size) / 2.0;

    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    // cases contrôlées
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (cell_control[r][c] == 1) {
                cairo_set_source_rgb(cr, 0.8, 0.8, 1);
                cairo_rectangle(cr, offset_x + c*cell_size, offset_y + r*cell_size, cell_size, cell_size);
                cairo_fill(cr);
            } else if (cell_control[r][c] == 2) {
                cairo_set_source_rgb(cr, 1, 0.8, 0.8);
                cairo_rectangle(cr, offset_x + c*cell_size, offset_y + r*cell_size, cell_size, cell_size);
                cairo_fill(cr);
            }
        }
    }

    // points jouables
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (highlight_moves[r][c]) {
                double cx = offset_x + c*cell_size + cell_size/2.0;
                double cy = offset_y + r*cell_size + cell_size/2.0;
                cairo_set_source_rgb(cr, 0, 0, 0);
                cairo_arc(cr, cx, cy, cell_size * 0.1, 0, 2*M_PI);
                cairo_fill(cr);
            }
        }
    }

    // teintes sous pièces (présence)
    for (int i = 0; i < piece_count; ++i) {
        Piece p = pieces[i];
        if ((p.row == 0 && p.col == 0) || (p.row == 8 && p.col == 8)) continue;
        if (p.color == 'R') cairo_set_source_rgb(cr, 1, 0.8, 0.8);
        else                cairo_set_source_rgb(cr, 0.8, 0.8, 1);
        cairo_rectangle(cr, offset_x + p.col*cell_size, offset_y + p.row*cell_size, cell_size, cell_size);
        cairo_fill(cr);
    }

    // grille
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 1);
    for (int c = 0; c <= cols; ++c) {
        double x = offset_x + c*cell_size;
        cairo_move_to(cr, x, offset_y);
        cairo_line_to(cr, x, offset_y + grid_size);
    }
    for (int r = 0; r <= rows; ++r) {
        double y = offset_y + r*cell_size;
        cairo_move_to(cr, offset_x, y);
        cairo_line_to(cr, offset_x + grid_size, y);
    }
    cairo_stroke(cr);

    // labels colonnes
    for (int c = 0; c < cols; ++c) {
        char label[2]; label[0] = 'A' + c; label[1] = '\0';
        cairo_move_to(cr, offset_x + (c + 0.4)*cell_size, offset_y - 5);
        cairo_show_text(cr, label);
    }
    // labels lignes
    for (int r = 0; r < rows; ++r) {
        char label[3]; 
        snprintf(label, sizeof(label), "%d", rows - r); 
        cairo_move_to(cr, offset_x - 15, offset_y + (r + 0.6)*cell_size);
        cairo_show_text(cr, label);
    }


    // bases
    if (logo_blue) {
        GdkPixbuf *scaled = gdk_pixbuf_scale_simple(logo_blue, (int)cell_size, (int)cell_size, GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, scaled, offset_x + 0*cell_size, offset_y + 0*cell_size);
        cairo_paint(cr);
        g_object_unref(scaled);
    }
    if (logo_red) {
        GdkPixbuf *scaled = gdk_pixbuf_scale_simple(logo_red, (int)cell_size, (int)cell_size, GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, scaled, offset_x + 8*cell_size, offset_y + 8*cell_size);
        cairo_paint(cr);
        g_object_unref(scaled);
    }

    // pièces
    for (int i = 0; i < piece_count; ++i) {
        Piece p = pieces[i];
        double x = offset_x + (p.col + 0.5)*cell_size;
        double y = offset_y + (p.row + 0.5)*cell_size;

        // sélection
        if (i == selected_piece) {
            cairo_set_source_rgb(cr, 1, 1, 0);
            cairo_arc(cr, x, y, cell_size * 0.4, 0, 2*M_PI);
            cairo_fill(cr);
        }

        double radius = cell_size * 0.3;

        // ombre
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_arc(cr, x, y, radius * 1.1, 0, 2*M_PI);
        cairo_fill(cr);

        // pion
        if (p.color == 'R') cairo_set_source_rgb(cr, 1, 0, 0);
        else                cairo_set_source_rgb(cr, 0, 0, 1);
        cairo_arc(cr, x, y, radius, 0, 2*M_PI);
        cairo_fill(cr);

        // signe roi (pastille)
        if (p.type == 'R' || p.type == 'K' || p.type == 'r' || p.type == 'k') {
            cairo_save(cr);
            cairo_new_path(cr);
            double dot = radius * 0.5;
            cairo_set_source_rgb(cr, 1, 1, 0);
            cairo_arc(cr, x, y, dot, 0, 2*M_PI);
            cairo_fill_preserve(cr);
            cairo_set_line_width(cr, 1.5);
            cairo_set_source_rgb(cr, 0, 0, 0);
            cairo_stroke(cr);
            cairo_restore(cr);
        }
    }
}
