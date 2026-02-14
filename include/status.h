#ifndef STATUS_H
#define STATUS_H
#include "app.h"
 
/**
 * @file status.h
 * @brief Gestion des labels, messages de victoire et mise à jour d'état.
 *
 * Le module status expose des fonctions pour enregistrer les widgets GTK
 * utilisés pour afficher les scores, afficher les messages de fin de
 * partie et rafraîchir l'état affiché à l'utilisateur.
 */

/** Enregistre les widgets utilisés par le module status. */
void status_register_labels(GtkWidget *score_blue_label,
                            GtkWidget *score_red_label,
                            GtkWidget *game_status_label,
                            GtkWidget *replay_button,
                            GtkWidget *victory_label,
                            GtkWidget *victory_box
                            );
 
/** Affiche un message indiquant une partie nulle (ou raison). */
void set_draw_message(const char *reason);

/** Affiche le message de victoire.
 * @param blue_won vrai si l'équipe bleue a gagné
 * @param reason texte explicatif (optionnel)
 */
void set_victory_message(gboolean blue_won, const char *reason);
 
/** Met à jour le label décrivant l'état courant du jeu. */
void refresh_game_status(void);
 
/** Mise à jour des labels de scores si les widgets ont été enregistrés. */
void status_on_scores_changed(void);
 
#endif // STATUS_H