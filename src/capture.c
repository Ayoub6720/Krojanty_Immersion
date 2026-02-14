/**
 * \file capture.c
 * \brief Gestion des règles de capture et conditions de victoire.
 * \author loic.claude@uha.fr, nolann.wickers@uha.fr, valentin.prevot@uha.fr, valentin.leray@uha.fr,
 *         ayoub.darkaoui@uha.fr, ismail.el-maallem@uha.fr, corentin.banocay@uha.fr
 * \version 0.1
 * \date 2025-09-16
 *
 * \details
 * Ce fichier implémente les règles de capture spécifiques au jeu :
 * - Capture par encadrement (règle Linca).
 * - Capture par arrière vide (règle Seltou).
 * - Règle d’auto-défaite quand un joueur ne garde qu’un pion et son roi.
 *
 * Chaque capture met à jour :
 * - Le tableau des pièces actives.
 * - La zone de contrôle (`cell_control`).
 * - Le statut de la partie (victoire, fin de jeu).
 */


#include "captures.h"
#include "game.h"
#include "status.h"


 /**
 * \fn int find_piece_at(int row, int col)
 * \brief Vérifie et applique les captures par encadrement (Linca).
 *
 * \param moved_index Indice de la pièce qui vient d’être déplacée.
 *
 * Une capture Linca se produit lorsqu’une pièce ennemie est encadrée
 * entre deux pièces alliées alignées horizontalement ou verticalement.
 * - Si un roi est capturé, la partie se termine immédiatement.
 * - Sinon, le pion est supprimé du plateau.
 */
void check_linca_capture(int moved_index) {
    Piece *moved = &pieces[moved_index];
    int r = moved->row, c = moved->col;
    char ally = moved->color;
    char enemy = (ally == 'B') ? 'R' : 'B';
    int dr[4] = {-1, 1, 0, 0};
    int dc[4] = { 0, 0,-1, 1};

    for (int d = 0; d < 4; ++d) {
        int r1 = r + dr[d], c1 = c + dc[d];
        int r2 = r + 2*dr[d], c2 = c + 2*dc[d];
        if (r2 < 0 || r2 > 8 || c2 < 0 || c2 > 8) continue;

        int idx1 = find_piece_at(r1, c1);
        int idx2 = find_piece_at(r2, c2);
        if (idx1 >= 0 && pieces[idx1].color == enemy &&
            idx2 >= 0 && pieces[idx2].color == ally) {

            // Roi capturé → fin
            if (pieces[idx1].type == 'K') {
                gboolean blue_king_captured = (pieces[idx1].color == 'B');
                set_victory_message(!blue_king_captured,
                    blue_king_captured ? "Le roi bleu a été capturé."
                                       : "Le roi rouge a été capturé.");
                // supprimer la pièce
                for (int k = idx1; k < piece_count - 1; ++k) pieces[k] = pieces[k+1];
                --piece_count;
                game_over = 1;
                return;

            }
            // pion capturé
            if (r1 >= 0 && r1 < 9 && c1 >= 0 && c1 < 9){
                cell_control[r1][c1] = 0;
                printf("Capture de %c en %c%d en Linca\n", pieces[idx1].color, 'A'+c1, 9-r1);}
            for (int k = idx1; k < piece_count - 1; ++k) pieces[k] = pieces[k+1];
            --piece_count;
            if (selected_piece > idx1) --selected_piece;
        

            // recommencer (en cas de chain), repasse dans la boucle
            d = -1;
        }
    }
}

/**
 * \fn void check_seltou_capture(int moved_index, int old_row, int old_col)
 * \brief Vérifie et applique les captures par arrière vide (Seltou).
 *
 * \param moved_index Indice de la pièce déplacée.
 * \param old_row Ligne de départ du mouvement.
 * \param old_col Colonne de départ du mouvement.
 *
 * Une capture Seltou se produit lorsqu’une pièce ennemie adjacente
 * n’est pas protégée par une autre pièce derrière elle.
 * - Si un roi est capturé, la partie se termine.
 * - Sinon, le pion est retiré et les scores mis à jour.
 */
void check_seltou_capture(int moved_index, int old_row, int old_col) {
    Piece *moved = &pieces[moved_index];
    char ally = moved->color;
    char enemy = (ally == 'B') ? 'R' : 'B';
    int r = moved->row, c = moved->col;

    int dr = 0, dc = 0;
    if (r != old_row && c != old_col) return; // Mouvement diagonal invalide
    if (r == old_row && c == old_col) return; // Pas de mouvement
    if (r != old_row) dr = (r > old_row) ? 1 : -1;
    if (c != old_col) dc = (c > old_col) ? 1 : -1;

    int enemy_row = r + dr, enemy_col = c + dc;
    if (enemy_row < 0 || enemy_row > 8 || enemy_col < 0 || enemy_col > 8) return;

    int idx_enemy = find_piece_at(enemy_row, enemy_col); //pièce ennemie capturable
    if (idx_enemy < 0 || pieces[idx_enemy].color != enemy) return;

    int back_row = enemy_row + dr, back_col = enemy_col + dc;
    if (back_row >= 0 && back_row < 9 && back_col >= 0 && back_col < 9) {
        int idx_back = find_piece_at(back_row, back_col);
        if (idx_back >= 0 && pieces[idx_back].color == enemy) return; // protégé
    }

    // capture
    if (pieces[idx_enemy].type == 'K') {
        gboolean blue_king_captured = (pieces[idx_enemy].color == 'B');
        set_victory_message(!blue_king_captured,
            blue_king_captured ? "Le roi bleu a été capturé." : "Le roi rouge a été capturé.");
        for (int k = idx_enemy; k < piece_count - 1; ++k) pieces[k] = pieces[k+1];
        --piece_count;
        game_over = 1;
        return;
    }

    cell_control[enemy_row][enemy_col] = 0;
    for (int k = idx_enemy; k < piece_count - 1; ++k) pieces[k] = pieces[k+1];
    --piece_count;
    printf("Capture de %c en %c%d en Seltou\n", pieces[idx_enemy].color, 'A'+enemy_col, 9-enemy_row);
    if (selected_piece > idx_enemy) --selected_piece;

    update_scores();
    // auto defeat peut déclarer la fin
    check_auto_defeat();
}

/**
 * \fn void check_auto_defeat(void)
 * \brief Vérifie la condition d’auto-défaite.
 *
 * Un joueur perd automatiquement s’il ne possède plus qu’un roi
 * et un seul pion. Dans ce cas, la victoire est déclarée à l’adversaire.
 */
void check_auto_defeat(void) {
    int blue_count = 0, blue_king = 0;
    int red_count  = 0, red_king  = 0;

    for (int i = 0; i < piece_count; ++i) {
        if (pieces[i].color == 'B') {
            if (pieces[i].type == 'K') blue_king++;
            else blue_count++;
        } else {
            if (pieces[i].type == 'K') red_king++;
            else red_count++;
        }
    }
    if (!game_over && blue_count == 1 && blue_king == 1) {
        set_victory_message(FALSE, "Les bleus n'ont plus qu'un pion et un roi.");
        game_over = 1;
    }
    if (!game_over && red_count == 1 && red_king == 1) {
        set_victory_message(TRUE, "Les rouges n'ont plus qu'un pion et un roi.");
        game_over = 1;
    }
}
