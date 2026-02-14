/**
 * \file TestStatus.c
 * \brief Tests unitaires du module status (mise à jour des labels et affichage GTK).
 *
 * \details
 * Ce fichier contient des tests pour le module status, qui gère :
 *   - L'enregistrement des labels GTK pour les scores et le statut de jeu.
 *   - La mise à jour des messages de victoire ou de match nul.
 *   - Le rafraîchissement de l'état du jeu sur l'interface.
 *
 * \note Les tests créent des widgets GTK fictifs pour vérifier que les fonctions
 * modifient correctement les labels sans nécessiter une interface complète.
 */

#include <assert.h>
#include <gtk/gtk.h>

// Protos du module status
void status_register_labels(GtkWidget* score_blue_label,
                            GtkWidget* score_red_label,
                            GtkWidget* game_status_label,
                            GtkWidget* replay_button);
void status_on_scores_changed(void);
void set_draw_message(const char* reason);
void set_victory_message(gboolean blue_won, const char* reason);
void refresh_game_status(void);

/** 
 * \fn static void ensure_gtk_inited(void)
 * \brief Initialise GTK si ce n'est pas déjà fait.
 *
 * \details
 * Cette fonction garantit que GTK est initialisé une seule fois avant
 * d'exécuter des tests qui manipulent des widgets GTK.
 */
static void ensure_gtk_inited(void) {
    static int done = 0;
    if (!done) { gtk_init(); done = 1; }
}

/** 
 * \fn void test_status_register_and_draw(void)
 * \brief Teste l'enregistrement des labels et la mise à jour du message de statut.
 *
 * \details
 * - Crée des labels GTK fictifs pour les scores et le statut.
 * - Enregistre ces labels via status_register_labels().
 * - Modifie le message de statut avec set_draw_message() et vérifie que le label est mis à jour.
 *
 * \note Affiche les résultats sur la sortie standard.
 */
void test_status_register_and_draw(void) {
    ensure_gtk_inited();
    GtkWidget *lB = gtk_label_new(NULL);
    GtkWidget *lR = gtk_label_new(NULL);
    GtkWidget *lS = gtk_label_new(NULL);
    GtkWidget *btn = gtk_button_new();

    status_register_labels(lB, lR, lS, btn);

    // draw message - label statut non vide
    set_draw_message("timeout");
    const char *t1 = gtk_label_get_text(GTK_LABEL(lS));
    assert(t1 && *t1);
    puts("test_status_register_and_draw passed");
}
