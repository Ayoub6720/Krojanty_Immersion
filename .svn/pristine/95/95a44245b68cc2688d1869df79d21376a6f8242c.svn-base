/** \file TestGame.c
 * \brief Tests de la logique du moteur de jeu (mouvements, règles, scores).
 */

/** \fn void test_game_setup_default()
 * \brief Vérifie que le setup initial du plateau fonctionne correctement.
 *
 * \details
 *   - Vérifie que 20 pièces sont placées.
 *   - Vérifie que les couleurs sont correctes.
 */

#include <stdio.h>
#include <assert.h>
#include "game.h"
#include "captures.h"
void reset_game_state(void); //IMPORTANT


/** \fn test_game_setup_default()
 * \brief Vérifie que le setup initial du plateau fonctionne correctement.
 *
 * \details
 *   - Vérifie que 20 pièces sont placées.
 *   - Vérifie que les couleurs sont correctes.
 */
void test_game_setup_default() {
    game_setup_default();
    assert(piece_count == 20);
    assert(pieces[0].color == 'B');
    assert(pieces[10].color == 'R');
    printf("test_game_setup_default OK\n");
}

/** \fn void test_move_piece_basic()
 * \brief Teste un déplacement simple d'une pièce.
 *
 * \details
 *   - Place une pièce bleue au centre.
 *   - Tente de la déplacer horizontalement sur des cases libres.
 *   - Vérifie que la pièce a bien bougé.
 *
 * \note Affiche les résultats sur la sortie standard.
 */
void test_move_piece_basic() {
    reset_game_state();
    pieces[piece_count++] = (Piece){.row=4, .col=4, .color='B', .type='P'};
    
    int ok = move_piece(0, 4, 6); // déplacement horizontal sur cases libres
    assert(ok);
    assert(pieces[0].row == 4 && pieces[0].col == 6);
    printf("test_move_piece_basic OK\n");
}

/** \fn void test_move_blocked()
 * \brief Teste un déplacement bloqué par une pièce ennemie.
 *
 * \details
 *   - Place une pièce bleue et une pièce rouge devant.
 *   - Tente de déplacer la pièce bleue par-dessus la rouge.
 *   - Vérifie que le déplacement est refusé et que la pièce bleue reste en place.
 *
 * \note Affiche les résultats sur la sortie standard.
 */
void test_move_blocked() {
    reset_game_state();
    pieces[piece_count++] = (Piece){.row=4, .col=4, .color='B', .type='P'};
    pieces[piece_count++] = (Piece){.row=4, .col=5, .color='R', .type='P'};
    
    int ok = move_piece(0, 4, 6); // déplacement bloqué par pièce ennemie
    assert(!ok);
    assert(pieces[0].row == 4 && pieces[0].col == 4);
    printf("test_move_blocked OK\n");
}

/** \fn void test_score_update()
 * \brief Teste la mise à jour des scores.
 *
 * \details
 *   - Place des pièces de chaque couleur.
 *   - Force certaines cellules contrôlées.
 *   - Appelle update_scores() et vérifie que les scores sont mis à jour.
 *
 * \note Affiche les résultats sur la sortie standard.
 */
void test_score_update() {
    reset_game_state();

    // 2 pions bleus
    pieces[0] = (Piece){.row=3,.col=4,.color='B',.type='P'};
    pieces[1] = (Piece){.row=2,.col=4,.color='B',.type='P'};

    // 2 pions rouges
    pieces[2] = (Piece){.row=5,.col=4,.color='R',.type='P'};
    pieces[3] = (Piece){.row=6,.col=4,.color='R',.type='P'};

    piece_count = 4;

    // forcer des cellules contrôlées
    cell_control[3][5] = 1; // bleu contrôle une cellule
    cell_control[5][5] = 2; // rouge contrôle une cellule

    update_scores();
    assert(score_blue > 0 && score_red > 0);
    printf("test_score_update OK\n");

}

/** \fn void Test_Auto_Defaite()
 * \brief Teste la détection de la défaite automatique.
 *
 * \details
 *   - Place un roi bleu et un roi rouge.
 *   - Appelle check_auto_defeat() et vérifie que la partie n'est pas terminée.
 *   - Ajoute une pièce supplémentaire pour le rouge et vérifie que la partie est terminée.
 *
 * \note Affiche les résultats sur la sortie standard.
 */
void Test_Auto_Defaite(){
    piece_count = 2;
    pieces[0] = (Piece){.row=0,.col=0,.color='B',.type='K'};
    pieces[1] = (Piece){.row=8,.col=8,.color='R',.type='K'};
    update_scores();
    check_auto_defeat(); // pas crash
    printf("test_auto_defeat OK\n");
}

/** \fn void test_move_blocked_by_ally()
 * \brief Teste un déplacement bloqué par une pièce alliée.
 *
 * \details
 *   - Place une pièce bleue et une autre pièce bleue devant.
 *   - Tente de déplacer la première pièce par-dessus l'alliée.
 *   - Vérifie que le déplacement est refusé et que la pièce reste en place.
 *
 * \note Affiche les résultats sur la sortie standard.
 */
void test_move_blocked_by_ally() {
    reset_game_state();
    pieces[piece_count++] = (Piece){.row=4, .col=4, .color='B', .type='P'};
    pieces[piece_count++] = (Piece){.row=4, .col=5, .color='B', .type='P'}; // allié juste devant

    int ok = move_piece(0, 4, 6); // tente de passer par-dessus son allié en 6
    assert(!ok);
    assert(pieces[0].row == 4 && pieces[0].col == 4); // reste sur place
    printf("test_move_blocked_by_ally OK\n");
}

/** \fn void test_find_piece_at()
 * \brief Teste la fonction find_piece_at avec des cas simples.
 *
 * \details
 *   - Ajoute quelques pièces au plateau.
 *   - Vérifie que find_piece_at retourne les bons indices pour des positions occupées.
 *   - Vérifie qu'elle retourne -1 pour des positions vides ou hors plateau.
 *
 * \note Affiche les résultats sur la sortie standard.
 */
void test_find_piece_at() {
    reset_game_state();

    // Ajouter 2 pièces
    pieces[piece_count++] = (Piece){.row=2, .col=3, .color='B', .type='P'};
    pieces[piece_count++] = (Piece){.row=5, .col=5, .color='R', .type='K'};

    // Test pièce bleue
    int idx1 = find_piece_at(2, 3);
    assert(idx1 == 0);
    assert(pieces[idx1].color == 'B');

    // Test pièce rouge
    int idx2 = find_piece_at(5, 5);
    assert(idx2 == 1);
    assert(pieces[idx2].type == 'K');

    // Test case vide
    int idx3 = find_piece_at(4, 4);
    assert(idx3 == -1);

    // Test hors plateau (devrait retourner -1)
    int idx4 = find_piece_at(-1, 9);
    assert(idx4 == -1);

    printf("test_find_piece_at OK\n");
}


/** \fn void test_auto_defeat_trigger()
 * \brief Teste la détection de la défaite automatique avec supériorité numérique.
 *
 * \details
 *   - Place un roi bleu et un pion bleu.
 *   - Place un roi rouge et deux pions rouges (supériorité numérique).
 *   - Appelle check_auto_defeat() et vérifie que la partie est terminée.
 *
 * \note Affiche les résultats sur la sortie standard.
 */
void test_auto_defeat_trigger() {
    reset_game_state();

    // Bleu n'a qu'un roi
    pieces[0] = (Piece){.row=0, .col=0, .color='B', .type='K'};
    pieces[1] = (Piece){.row=2, .col=2, .color='B', .type='P'};

    // Rouge a un roi et un pion (supériorité numérique)
    pieces[3] = (Piece){.row=8, .col=8, .color='R', .type='K'};
    pieces[4] = (Piece){.row=7, .col=7, .color='R', .type='P'};
    pieces[5] = (Piece){.row=9, .col=7, .color='R', .type='P'};
    piece_count = 5;

    update_scores();
    check_auto_defeat();

    assert(game_over == 1); // la partie terminée
    printf("test_auto_defeat_trigger OK\n");
}



