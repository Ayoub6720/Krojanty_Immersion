
/**
 * \file TestAi.c
 * \brief Tests unitaires pour le module IA (minimax, évaluation).
 *
 * \details
 * Ce fichier contient plusieurs fonctions de test destinées à vérifier :
 * - L’évaluation d’un état de jeu par l’IA.
 * - Le fonctionnement de l’algorithme minimax avec différentes profondeurs.
 * - La recherche du meilleur coup possible par l’IA.
 *
 * Ces tests sont conçus pour s’exécuter dans le cadre du harness principal
 * défini dans MainTest.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include "ia.h"
#include "game.h"

/**
 * \fn void test_ai_minimax_basic()
 * \brief Test basique de l’IA avec Minimax et évaluation.
 *
 * \details
 * - Crée un GameState minimal avec deux pièces.  
 * - Vérifie l’évaluation pour les deux couleurs.  
 * - Exécute minimaxIA sur une profondeur de 1.  
 * - Teste la fonction trouverMeilleurCoupIA.  
 *
 * \note Affiche les résultats sur la sortie standard.
 */
void test_ai_minimax_basic() {
    //Création d'un GameState minimal pour test
    GameState gs = createGameStateFromCurrent();

    // Définir quelques pièces manuellement pour le test
    gs.piece_count = 2;
    gs.pieces[0] = (Piece){ .row = 0, .col = 0, .type = 'K', .color = 'R' };
    gs.pieces[1] = (Piece){ .row = 8, .col = 8, .type = 'K', .color = 'B' };
    gs.current_player = 'R';

    // Tester l'évaluation pour Red
    int eval_red = evaluation(&gs, 'R');
    int eval_blue = evaluation(&gs, 'B');
    printf("Evaluation Red: %d\n", eval_red);
    printf("Evaluation Blue: %d\n", eval_blue);

    // Tester minimaxIA sur 1 profondeur
    int score = minimaxIA(&gs, 1, 'R', -1000000, 1000000);
    printf("MinimaxIA score (Red): %d\n", score);

    // Tester trouverMeilleurCoupIA
    Move best;
    trouverMeilleurCoupIA(&gs, &best, 1);
    printf("Meilleur coup IA: piece_index=%d, from=(%d,%d), to=(%d,%d)\n",
        best.piece_index, best.from_row, best.from_col, best.to_row, best.to_col);

    printf("test_ai_minimax_basic OK \n");
}


/**
 * \fn void test_trouverMeilleurCoupIA()
 * \brief Test de la fonction trouverMeilleurCoupIA.
 *
 * \details
 * - Crée un GameState minimal avec deux pièces.  
 * - Exécute trouverMeilleurCoupIA pour une profondeur de 1.  
 * - Affiche le coup choisi par l’IA.  
 *
 * \note Affiche les résultats sur la sortie standard.
 */
void test_trouverMeilleurCoupIA() {

    // Création d'un GameState minimal
    GameState gs = createGameStateFromCurrent();
    gs.piece_count = 4;
    gs.pieces[0] = (Piece){ .row = 0, .col = 0, .type = 'K', .color = 'R' };
    gs.pieces[1] = (Piece){ .row = 1, .col = 0, .type = 'P', .color = 'R' };
    gs.pieces[2] = (Piece){ .row = 8, .col = 8, .type = 'K', .color = 'B' };
    gs.pieces[3] = (Piece){ .row = 7, .col = 8, .type = 'P', .color = 'B' };
    gs.current_player = 'R';

    // Appliquer l'IA pour 1 profondeur
    Move best;
    trouverMeilleurCoupIA(&gs, &best, 1);

    // Afficher le coup choisi
    if (best.piece_index >= 0) {
        printf("Meilleur coup choisi : piece_index=%d, from=(%d,%d), to=(%d,%d)\n",
            best.piece_index, best.from_row, best.from_col, best.to_row, best.to_col);
    } else {
        printf("Aucun coup possible pour l'IA.\n");
    }

    printf("test_trouverMeilleurCoupIA termine\n");
}


/**
 * \fn void test_minimaxIA_basic()
 * \brief Test basique de l’algorithme minimaxIA.
 *
 * \details
 * - Crée un GameState minimal avec deux pièces.  
 * - Exécute minimaxIA sur des profondeurs 1 et 2.  
 * - Affiche les scores retournés.  
 *
 * \note Affiche les résultats sur la sortie standard.
 */
void test_minimaxIA_basic() {

    // Création d'un GameState minimal
    GameState gs = createGameStateFromCurrent();
    gs.piece_count = 4;
    gs.pieces[0] = (Piece){ .row = 0, .col = 0, .type = 'K', .color = 'R' };
    gs.pieces[1] = (Piece){ .row = 1, .col = 0, .type = 'P', .color = 'R' };
    gs.pieces[2] = (Piece){ .row = 8, .col = 8, .type = 'K', .color = 'B' };
    gs.pieces[3] = (Piece){ .row = 7, .col = 8, .type = 'P', .color = 'B' };
    gs.current_player = 'R';

    // Tester minimaxIA profondeur 1
    int score = minimaxIA(&gs, 1, 'R', -100000000, 100000000);
    printf("Score minimaxIA profondeur 1: %d\n", score);

    // Tester minimaxIA profondeur 2
    score = minimaxIA(&gs, 2, 'R', -100000000, 100000000);
    printf("Score minimaxIA profondeur 2: %d\n", score);

    printf("test_minimaxIA_basic OK\n");
}

