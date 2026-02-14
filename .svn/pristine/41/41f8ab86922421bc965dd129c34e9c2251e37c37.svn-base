/**
 * \file gui.c
 * \brief Gestion de l’interface graphique GTK pour le jeu Krojanty.
 * \author loic.claude@uha.fr, nolann.wickers@uha.fr, valentin.prevot@uha.fr, valentin.leray@uha.fr, alan.akgun@uha.fr
 *         ayoub.darkaoui@uha.fr, ismail.el-maallem@uha.fr, corentin.banocay@uha.fr
 * \version 0.1
 * \date 2025-09-16
 *
 * \details
 * Ce fichier implémente l’interface graphique avec GTK :
 * - Création de la fenêtre principale et de la zone de dessin.
 * - Gestion des événements (clics, boutons, fermeture).
 * - Affichage des scores, statut et messages de victoire.
 * - Intégration avec la logique du jeu (déplacements, IA, réseau).
 */



#include <stdio.h>
#include "gui.h"
#include "app.h"
#include "net.h"
#include "game.h"
#include "captures.h"
#include "drawing.h"
#include "status.h"


/** \brief Overlay GTK pour afficher le message de victoire */
static GtkWidget *victory_overlay = NULL;

/** \brief Mode de jeu courant (local, serveur, client) */
static game_mode_t current_mode;

/** \brief Zone de dessin GTK pour le plateau de jeu */
GtkWidget *g_drawing_area = NULL;

/** \brief Indique si l'interface graphique est active */
static volatile int gui_active = 0;

/** \brief Application GTK */
static GtkApplication *g_app = NULL;

/**
 * \fn static void reset_game(void)
 * \brief Réinitialise le jeu et met à jour l'affichage.
 */
static void reset_game(void) {
    restore_initial_state();
    refresh_game_status();
    if (g_drawing_area) gtk_widget_queue_draw(g_drawing_area);
}

/**
 * \fn static void on_replay_clicked(GtkButton *btn, gpointer user_data)
 * \brief Callback pour le bouton "Rejouer".
 * 
 * \param btn Bouton GTK cliqué.
 * \param user_data Données utilisateur (non utilisées).
 * 
 * Cette fonction réinitialise le jeu et masque l'overlay de victoire.
 */
static void on_replay_clicked(GtkButton *btn, gpointer user_data) {
    if (victory_overlay) {
        gtk_widget_set_visible(victory_overlay, FALSE);
    }
    reset_game();
}

/**
 * \fn static void on_window_destroy(GtkWidget *widget, gpointer user_data)
 * \brief Callback pour la fermeture de la fenêtre principale.
 * 
 * \param widget Widget GTK (la fenêtre).
 * \param user_data Données utilisateur (non utilisées).
 * 
 * Cette fonction arrête l'application GTK proprement.
 */
static void on_window_destroy(GtkWidget *widget, gpointer user_data) {
    gui_active = 0;
    
    if (g_app) {
        g_application_quit(G_APPLICATION(g_app));
    }
}


/**
 * \fn gboolean gui_move_piece_callback(gpointer data)
 * \brief Callback GTK pour traiter un coup reçu et mettre à jour l'interface.
 * 
 * \param data Chaîne de 4 caractères représentant le coup.
 * \return G_SOURCE_REMOVE après traitement.
 */
static void on_click(GtkGestureClick *gesture, int n_press, double x, double y, gpointer user_data) {
    (void)gesture; (void)n_press; (void)user_data;
    
    if (game_over) return;

    extern int ia_active; extern int ia_both_active; extern char ia_color;
    
    if (ia_both_active || (ia_active && current_turn == ia_color)) return;
    if (g_socket != INVALID_SOCKET && current_turn != my_color) return;

    int row, col;
    click_to_cell(x, y, &row, &col, g_drawing_area);

    // Vérifier si clic hors grille
    if (row < 0 || row > 8 || col < 0 || col > 8)
        return; // ignorer clic

    if (selected_piece == -1) {
        for (int i = 0; i < piece_count; ++i) {
            if (pieces[i].row == row && pieces[i].col == col && pieces[i].color == current_turn) {
                selected_piece = i;
                compute_highlight_for_selected();
                break;
            }
        }
    // Ignorer clics hors grille
        if (row == -1 || col == -1)
            return;
        refresh_game_status();
    } else {
        int switched = 0;
        for (int i = 0; i < piece_count; ++i) {
            if (pieces[i].row == row && pieces[i].col == col && pieces[i].color == current_turn) {
                selected_piece = i;
                compute_highlight_for_selected();
                switched = 1;
                break;
            }
        }
        if (!switched) {
            int old_row = pieces[selected_piece].row;
            int old_col = pieces[selected_piece].col;
            int moved = move_piece(selected_piece, row, col);
            if (moved && g_socket != INVALID_SOCKET) {
                char msg[5];
                int N = 9;
                msg[0] = 'A' + old_col;
                msg[1] = '0' + (N - old_row);
                msg[2] = 'A' + col;
                msg[3] = '0' + (N - row);
                msg[4] = '\0';
                TCP_Send_Message(g_socket, msg);
                selected_piece = -1;
            }
        }
        refresh_game_status();
    }
    gtk_widget_queue_draw(g_drawing_area);
}

/**
 * \fn static void on_activate(GtkApplication *app, gpointer user_data)
 * \brief Callback pour l'activation de l'application GTK.
 * 
 * \param app Instance de l'application GTK.
 * \param user_data Données utilisateur (non utilisées).
 * 
 * Cette fonction crée la fenêtre principale, configure les widgets et initialise le jeu.
 */
static void on_activate(GtkApplication *app, gpointer user_data) {
    (void)user_data;
    
    g_app = app;
    
    GtkWidget *window = gtk_application_window_new(app);
    if (!window) {
        printf("ERREUR: impossible de créer la fenêtre\n");
        return;
    }
    
    gtk_window_set_title(GTK_WINDOW(window), "Krojanty");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 500);
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);


    // --- HBOX principal ---
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_start(hbox, 10);
    gtk_widget_set_margin_end(hbox, 10);
    gtk_widget_set_margin_top(hbox, 10);
    gtk_widget_set_margin_bottom(hbox, 10);

    // --- Colonne gauche ---
    GtkWidget *text_frame = gtk_frame_new("");
    gtk_widget_set_size_request(text_frame, 250, -1);
    GtkWidget *text_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_margin_start(text_box, 10);
    gtk_widget_set_margin_end(text_box, 10);
    gtk_widget_set_margin_top(text_box, 10);
    gtk_widget_set_margin_bottom(text_box, 10);

    // Logos
    logo_blue = gdk_pixbuf_new_from_file("logo/logo-blue.png", NULL);
    logo_red  = gdk_pixbuf_new_from_file("logo/logo-red.png",  NULL);

    // Poser les pièces par défaut
    game_setup_default();
    initialize_controlled_cells();

    // Labels et info
    GtkWidget *title_label = gtk_label_new("Jeu de Krojanty");
    gtk_widget_add_css_class(title_label, "title-1");
    GtkWidget *rules_label = gtk_label_new("Score du jeu:");
    gtk_widget_add_css_class(rules_label, "heading");
    GtkWidget *score_blue_label = gtk_label_new("• Score équipe bleue: 0");
    gtk_label_set_xalign(GTK_LABEL(score_blue_label), 0.0);
    GtkWidget *score_red_label  = gtk_label_new("• Score équipe rouge: 0");
    gtk_label_set_xalign(GTK_LABEL(score_red_label), 0.0);
    GtkWidget *scoring_info = gtk_label_new("Règles de score:\n• 1 point par pièce\n• 1 point par case contrôlée");
    gtk_label_set_xalign(GTK_LABEL(scoring_info), 0.0);
    gtk_widget_add_css_class(scoring_info, "dim-label");
    GtkWidget *status_label = gtk_label_new("État du jeu:");
    gtk_widget_add_css_class(status_label, "heading");
    GtkWidget *game_status_label = gtk_label_new("");
    gtk_label_set_xalign(GTK_LABEL(game_status_label), 0.0);

    // --- Zone de dessin ---
    g_drawing_area = gtk_drawing_area_new();
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(g_drawing_area), draw_cb, NULL, NULL);
    gtk_widget_set_hexpand(g_drawing_area, TRUE);
    gtk_widget_set_vexpand(g_drawing_area, TRUE);
    GtkGestureClick *click = GTK_GESTURE_CLICK(gtk_gesture_click_new());
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(click), GDK_BUTTON_PRIMARY);
    g_signal_connect(click, "pressed", G_CALLBACK(on_click), g_drawing_area);
    gtk_widget_add_controller(g_drawing_area, GTK_EVENT_CONTROLLER(click));

    // --- Ajouter tous les widgets dans le text_box ---
    gtk_box_append(GTK_BOX(text_box), title_label);
    gtk_box_append(GTK_BOX(text_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
    gtk_box_append(GTK_BOX(text_box), rules_label);
    gtk_box_append(GTK_BOX(text_box), score_blue_label);
    gtk_box_append(GTK_BOX(text_box), score_red_label);
    gtk_box_append(GTK_BOX(text_box), scoring_info);
    gtk_box_append(GTK_BOX(text_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
    gtk_box_append(GTK_BOX(text_box), status_label);
    gtk_box_append(GTK_BOX(text_box), game_status_label);

    gtk_frame_set_child(GTK_FRAME(text_frame), text_box);

    // --- Assembler hbox ---
    gtk_box_append(GTK_BOX(hbox), text_frame);
    gtk_box_append(GTK_BOX(hbox), g_drawing_area);

    // --- Bouton Rejouer et label victoire ---
    GtkWidget *victory_label = gtk_label_new(NULL);
    gtk_label_set_xalign(GTK_LABEL(victory_label), 0.5);
    gtk_label_set_yalign(GTK_LABEL(victory_label), 0.5);
    gtk_label_set_justify(GTK_LABEL(victory_label), GTK_JUSTIFY_CENTER);

    GtkWidget *replay_button = gtk_button_new_with_label("Rejouer");
    g_signal_connect(replay_button, "clicked", G_CALLBACK(on_replay_clicked), NULL);
    gtk_widget_add_css_class(replay_button, "suggested-action");
    gtk_widget_set_sensitive(replay_button, FALSE);

    // --- Victory overlay full screen (dimming background) ---
    victory_overlay = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(victory_overlay, "victory_overlay");
    gtk_widget_set_hexpand(victory_overlay, TRUE);
    gtk_widget_set_vexpand(victory_overlay, TRUE);
    gtk_widget_set_halign(victory_overlay, GTK_ALIGN_FILL);
    gtk_widget_set_valign(victory_overlay, GTK_ALIGN_FILL);
    gtk_widget_set_visible(victory_overlay, FALSE);

    // Conteneur centré pour le message + bouton
    GtkWidget *victory_center_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_halign(victory_center_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(victory_center_box, GTK_ALIGN_CENTER);

    // Centrer horizontalement le texte et le bouton
    gtk_widget_set_halign(victory_label, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(replay_button, GTK_ALIGN_CENTER);

    gtk_box_append(GTK_BOX(victory_center_box), victory_label);
    gtk_box_append(GTK_BOX(victory_center_box), replay_button);
    gtk_box_append(GTK_BOX(victory_overlay), victory_center_box);

    // --- Créer overlay principal et ajouter hbox + victory_overlay ---
    GtkWidget *overlay = gtk_overlay_new();
    gtk_overlay_set_child(GTK_OVERLAY(overlay), hbox);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), victory_overlay);
    gtk_window_set_child(GTK_WINDOW(window), overlay);


    // --- CSS pour victory_overlay ---
    GtkCssProvider *css = gtk_css_provider_new();
    gtk_css_provider_load_from_string(css,
        ".victory_overlay { "
        "background-color: rgba(0,0,0,0.7); "
        "border-radius: 10px; "
        "padding: 40px; "
        "}");

    // Appliquer globalement à l'application
    GdkDisplay *display = gdk_display_get_default();
    gtk_style_context_add_provider_for_display(display,
        GTK_STYLE_PROVIDER(css),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    g_object_unref(css);

    // Ajouter la classe CSS au widget
    gtk_widget_add_css_class(victory_overlay, "victory_overlay");

    // --- Enregistrer les labels pour status ---
    status_register_labels(score_blue_label, score_red_label, game_status_label,
                           replay_button, victory_label, victory_overlay);

    // --- Snapshot initial et scores ---
    snapshot_initial_state();
    update_scores();
    refresh_game_status();

    // --- Déclencher IA si nécessaire ---
    extern int ia_active; extern int ia_both_active; extern char ia_color;
    if (!game_over && (ia_both_active || (ia_active && current_turn == ia_color))) {
        g_timeout_add(500, trigger_ia_move, NULL);
    }

    gtk_window_present(GTK_WINDOW(window));
}


/**
 * \fn void start_gui(int argc, char **argv, game_mode_t mode)
 * \brief Démarre l'application GTK et lance la boucle principale.
 * 
 * \param argc Nombre d'arguments de la ligne de commande.
 * \param argv Tableau des arguments de la ligne de commande.
 * \param mode Mode de jeu (local, serveur, client).
 * 
 * Cette fonction initialise GTK, crée la fenêtre principale et entre dans la boucle d'événements.
 */
void start_gui(int argc, char **argv, game_mode_t mode) {
    current_mode = mode;
    
    char app_id[256];
    if (my_color == 'R') {
        snprintf(app_id, sizeof(app_id), "org.example.krojanty.server");
    } else {
        snprintf(app_id, sizeof(app_id), "org.example.krojanty.client");
    }
    
    GtkApplication *app = gtk_application_new(app_id, G_APPLICATION_NON_UNIQUE);
    if (!app) {
        printf("ERREUR: impossible de créer l'application GTK\n");
        return;
    }
    
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
}
