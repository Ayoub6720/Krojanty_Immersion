/**
 * \file status.c
 * \brief Gestion de l'affichage du statut du jeu (score, état, victoire, égalité) avec GTK.
 * \author ayoub.darkaoui@uha.fr, valentin.prevot@uha.fr, valentin.leray@uha.fr
 * \version 0.1
 * \date 2025-09-17
 *
 * \details
 * Ce fichier gère :
 * - Les labels de score pour les équipes rouge et bleue.
 * - L'affichage de l'état actuel du jeu.
 * - L'affichage des messages de victoire ou d'égalité.
 * - L'activation du bouton "Rejouer".
 */

#include "status.h"
#include "game.h"

/** \brief Label GTK pour le score de l'équipe bleue */
static GtkWidget *g_score_blue_label  = NULL;
/** \brief Label GTK pour le score de l'équipe rouge */
static GtkWidget *g_score_red_label   = NULL;
/** \brief Label GTK pour l'état du jeu (tour, sélection, etc.) */
static GtkWidget *g_game_status_label = NULL;
/** \brief Bouton GTK pour rejouer */
static GtkWidget *g_replay_button     = NULL;
/** \brief Label GTK pour le message de victoire */
static GtkWidget *g_victory_label     = NULL;
/** \brief Conteneur GTK pour le message de victoire */
static GtkWidget *g_victory_box       = NULL;

/**
 * \fn void status_register_labels(GtkWidget *score_blue_label, GtkWidget *score_red_label,
 *                          GtkWidget *game_status_label, GtkWidget *replay_button,
 *                          GtkWidget *victory_label, GtkWidget *victory_box)
 * \brief Enregistre les widgets de statut pour pouvoir les manipuler.
 * 
 * \param score_blue_label Label GTK du score bleu
 * \param score_red_label Label GTK du score rouge
 * \param game_status_label Label GTK de l'état du jeu
 * \param replay_button Bouton GTK pour rejouer
 * \param victory_label Label GTK pour la victoire
 * \param victory_box Conteneur GTK pour la victoire
 */
void status_register_labels(GtkWidget *score_blue_label, GtkWidget *score_red_label, GtkWidget *game_status_label,
                            GtkWidget *replay_button, GtkWidget *victory_label, GtkWidget *victory_box) {
    g_score_blue_label  = score_blue_label;
    g_score_red_label   = score_red_label;
    g_game_status_label = game_status_label;
    g_replay_button     = replay_button;
    g_victory_label     = victory_label;
    g_victory_box      = victory_box;
}


/**
 * \fn void status_on_scores_changed(void)
 * \brief Met à jour les labels de score avec les valeurs actuelles.
 */
void status_on_scores_changed(void) {
    if (g_score_blue_label) {
        char *txt = g_strdup_printf("• Score équipe bleue: %d\n\t%d pièces restantes", score_blue, pieces_blue);
        gtk_label_set_text(GTK_LABEL(g_score_blue_label), txt);
        g_free(txt);
    }
    if (g_score_red_label) {
        char *txt = g_strdup_printf("• Score équipe rouge: %d\n\t%d pièces restantes", score_red, pieces_red);
        gtk_label_set_text(GTK_LABEL(g_score_red_label), txt);
        g_free(txt);
    }
}

/**
 * \fn void set_draw_message(const char *reason)
 * \brief Affiche un message d'égalité avec une raison optionnelle.
 * 
 * \param reason Raison de l'égalité (peut être NULL).
 */
void set_draw_message(const char *reason) {
    if (!g_game_status_label) return;
    const char *color = "#6B7280"; 
    char *markup = g_strdup_printf(
        "<span foreground=\"%s\" weight=\"bold\" size=\"large\">Égalité</span>\n"
        "<span size=\"small\">%s</span>",
        color, reason ? reason : ""
    );
    gtk_label_set_markup(GTK_LABEL(g_game_status_label), markup);
    g_free(markup);
    if (g_replay_button) gtk_widget_set_sensitive(g_replay_button, TRUE);
}

/**
 * \fn void set_victory_message(gboolean blue_won, const char *reason)
 * \brief Affiche un message de victoire pour l'équipe gagnante avec une raison optionnelle.
 * 
 * \param blue_won TRUE si l'équipe bleue a gagné, FALSE si l'équipe rouge a gagné.
 * \param reason Raison de la victoire (peut être NULL).
 */
void set_victory_message(gboolean blue_won, const char *reason) {
    if (!g_victory_label) return;
    const char *color = blue_won ? "#b3b3f0ff" : "#FF1E1E";
    const char *team  = blue_won ? "BLEUS" : "ROUGES";
    char *markup = g_strdup_printf(
        "<span foreground=\"%s\" weight=\"bold\" size=\"30000\">Les %s ont gagné</span>\n"
        "<span foreground=\"%s\" size=\"15000\" weight=\"bold\">%s</span>",
        color, team, color, reason ? reason : ""
    );
    gtk_label_set_markup(GTK_LABEL(g_victory_label), markup);
    gtk_widget_set_visible(g_victory_label, TRUE); // rendre visible
    gtk_widget_set_visible(g_victory_box, TRUE); // rendre visible
    g_free(markup);
    if (g_replay_button) gtk_widget_set_sensitive(g_replay_button, TRUE);
}

/**
 * \fn void refresh_game_status(void)
 * \brief Met à jour le label de statut du jeu avec le tour actuel et la pièce sélectionnée.
 */
void refresh_game_status(void) {
    if (!g_game_status_label) return;
    if (game_over) return; // Ne pas écraser un message de fin de partie
    char buf[256];
    if (selected_piece >= 0) {
        snprintf(buf, sizeof(buf),
                 "<span weight='bold'>Tour %d / %d — </span>"
                 "<span foreground='%s' weight='bold'>au tour des %s.\nPièce sélectionnée : %c en %c%d </span>",
                 turn_number, max_turn,
                 (current_turn == 'B') ? "#0A84FF" : "#FF3B30",
                 (current_turn == 'B') ? "bleus" : "rouges",
                 pieces[selected_piece].color,
                 'A' + pieces[selected_piece].col,
                 9-pieces[selected_piece].row);
    } else {
        snprintf(buf, sizeof(buf),
                 "<span weight='bold'>Tour %d / %d — </span>"
                 "<span foreground='%s' weight='bold'>au tour des %s.\nCliquez sur une pièce %s.</span>",
                 turn_number,max_turn,
                 (current_turn == 'B') ? "#0A84FF" : "#FF3B30",
                 (current_turn == 'B') ? "bleus" : "rouges",
                 (current_turn == 'B') ? "bleue" : "rouge");
    }
    gtk_label_set_markup(GTK_LABEL(g_game_status_label), buf);
}
