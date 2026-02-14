#ifndef GUI_H
#define GUI_H
#include <args.h>
#include <gtk/gtk.h>

/**
 * @file gui.h
 * @brief Points d'entrée et widgets exportés pour l'interface GTK.
 *
 * Fournit `start_gui` pour initialiser et lancer l'interface graphique
 * et expose quelques widgets globaux utilisés par d'autres modules.
 */


/**
 * @brief Lance l'interface GTK et démarre la boucle principale.
 * @param argc nombre d'arguments
 * @param argv tableau d'arguments
 * @param mode mode de jeu choisi (local/server/client)
 */
void start_gui(int argc, char **argv, game_mode_t mode);

extern GtkWidget *g_drawing_area; /**< Zone de dessin principale */

#endif // GUI_H
