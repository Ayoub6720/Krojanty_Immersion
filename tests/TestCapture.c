/**
 * \file TestCapture.c
 * \brief Tests unitaires des règles de capture (Linca, Seltou, auto-défaite).
 *
 * Ce fichier regroupe une série de tests unitaires validant le comportement des règles
 * spéciales du jeu concernant les captures :
 *
 * - **Linca** : capture "sandwich" (une pièce prise en étau entre deux adversaires).
 * - **Seltou** : capture "frontal" (une pièce qui saute par-dessus une autre).
 * - **Auto-défaite** : vérifie les conditions de fin de partie automatiques.
 *
 * Chaque test réinitialise l’état du jeu avant exécution via \ref reset_game_state,
 * place les pièces nécessaires dans une configuration précise, appelle la fonction
 * de capture correspondante, puis vérifie le résultat attendu avec \c assert().
 *
 * \note Les messages de succès sont affichés sur la sortie standard pour valider
 *       visuellement l’exécution.
 */

#include <assert.h>
#include <stdio.h>
#include "captures.h"
#include "game.h"
#include "status.h"

/** \fn void reset_game_state()
 * \brief Réinitialise l’état du jeu pour les tests.
 *
 * Remet à zéro le nombre de pièces, l’état de fin de partie, la pièce sélectionnée,
 * et le contrôle des cases.
 */
extern void reset_game_state() {
    piece_count = 0;
    game_over = 0;
    selected_piece = -1;
    for (int r = 0; r < 9; r++)
        for (int c = 0; c < 9; c++)
            cell_control[r][c] = 0;
}

/** \fn test_linca_simple_capture()
 * \brief Test basique de capture Linca.
 *
 * \details
 * - Place un pion bleu, un pion rouge, puis un pion bleu en ligne.  
 * - Simule le déplacement du pion bleu pour capturer le rouge.  
 * - Vérifie que le pion rouge a été retiré du jeu.  
 *
 * \note Affiche les résultats sur la sortie standard.
 */
void test_linca_simple_capture() {
    reset_game_state();

    // On place un pion bleu à (4,4)
    pieces[piece_count++] = (Piece){.row=4, .col=4, .color='B', .type='P'};
    // On place un pion rouge à (4,5)
    pieces[piece_count++] = (Piece){.row=4, .col=5, .color='R', .type='P'};
    // On place un pion bleu derrière à (4,6)
    pieces[piece_count++] = (Piece){.row=4, .col=6, .color='B', .type='P'};

    // On simule le déplacement du bleu en (4,4)
    check_linca_capture(0);

    // On vérifie que le rouge a été capturé
    assert(piece_count == 2); // il doit rester 2 pièces
    assert(find_piece_at(4,5) == -1); // plus de rouge en (4,5)

    printf("test_linca_simple_capture bien passe\n");
}

/** \fn test_seltou_simple_capture()
 * \brief Test basique de capture Seltou.
 *
 * \details
 * - Place un pion bleu, puis un pion rouge devant lui.  
 * - Simule le déplacement du pion bleu par-dessus le rouge.  
 * - Vérifie que le pion rouge a été retiré du jeu.  
 *
 * \note Affiche les résultats sur la sortie standard.
 */
void test_seltou_simple_capture() {
    reset_game_state();

    // Pion bleu
    pieces[piece_count++] = (Piece){.row=3, .col=4, .color='B', .type='P'};
    // Ennemi rouge devant
    pieces[piece_count++] = (Piece){.row=5, .col=4, .color='R', .type='P'};

    // Déplacement du bleu DE (3,4) vers (4,4)
    pieces[0].row = 4;
    pieces[0].col = 4;

    check_seltou_capture(0, 3, 4);
    assert(piece_count == 1); //capture rouge normalement
    assert(find_piece_at(5,4) == -1);

    printf("test_seltou_simple_capture bien passe\n");
}

/** \fn void test_auto_defeat()
 * \brief Test de la détection d’auto-défaite.
 *
 * \details
 * - Place les pièces minimales pour que chaque joueur ne puisse plus bouger.  
 * - Appelle la fonction de vérification d’auto-défaite.  
 * - Vérifie que la partie est bien marquée comme terminée.  
 *
 * \note Affiche les résultats sur la sortie standard.
 */
void test_auto_defeat() {
    reset_game_state();

    // Bleu : roi + 1 pion
    pieces[piece_count++] = (Piece){.row=0, .col=0, .color='B', .type='K'};
    pieces[piece_count++] = (Piece){.row=0, .col=1, .color='B', .type='P'};

    // Rouge : roi + 1 pion
    pieces[piece_count++] = (Piece){.row=8, .col=8, .color='R', .type='K'};
    pieces[piece_count++] = (Piece){.row=8, .col=7, .color='R', .type='P'};

    check_auto_defeat();
    assert(game_over == 1);

    printf("test_auto_defeat bien passe\n");
}

/** \fn void test_seltou_out_of_bounds()
 * \brief Test de la capture Seltou avec des pièces hors limites.
 *
 * \details
 * - Place un pion bleu sur le bord du plateau.  
 * - Place un pion rouge "hors tableau" (coordonnées négatives).  
 * - Simule le déplacement du pion bleu.  
 * - Vérifie que la fonction ne tente pas de capturer hors limites et ne plante pas.  
 *
 * \note Affiche les résultats sur la sortie standard.
 */
void test_seltou_out_of_bounds() {
    reset_game_state();

    // Pion bleu sur le bord supérieur
    pieces[piece_count++] = (Piece){.row=0, .col=0, .color='B', .type='P'};
    // Ennemi rouge "hors tableau" 
    pieces[piece_count++] = (Piece){.row=-1, .col=0, .color='R', .type='P'}; // hors limites

    // Déplacement du bleu vers le coin (toujours dans le tableau)
    pieces[0].row = 0;
    pieces[0].col = 0;

    // Appel de la fonction : ça ne doit rien faire ni crasher
    check_seltou_capture(0, 0, 0);

    // Vérifie que le nombre de pièces n'a pas changé (pas de capture)
    assert(piece_count == 2);

    printf("test_seltou_out_of_bounds bien passe\n");
}

/** \fn void test_seltou_out_of_bounds2()
 * \brief Test de la capture Seltou avec des pièces hors limites.
 *
 * \details
 * - Place un pion bleu sur le bord du plateau.  
 * - Place un pion rouge "hors tableau" (coordonnées négatives).  
 * - Simule le déplacement du pion bleu.  
 * - Vérifie que la fonction ne tente pas de capturer hors limites et ne plante pas.  
 *
 * \note Affiche les résultats sur la sortie standard.
 */
void test_seltou_out_of_bounds2() {
    reset_game_state();

    // Pion bleu sur le bord supérieur
    pieces[piece_count++] = (Piece){.row=0, .col=0, .color='B', .type='P'};
    // Ennemi rouge "hors tableau" 
    pieces[piece_count++] = (Piece){.row=-1, .col=0, .color='R', .type='P'}; // hors limites

    // Déplacement du bleu vers le coin (toujours dans le tableau)
    pieces[0].row = -3;
    pieces[0].col = -6;

    // Appel de la fonction : ça ne doit rien faire ni crasher
    check_seltou_capture(0, 0, -3);

    // Vérifie que le nombre de pièces n'a pas changé (pas de capture)
    assert(piece_count == 2);

    printf("test_seltou_out_of_bounds2 bien passe\n");
}


/** \fn void test_seltou_out_of_bounds3()
 * \brief Test de la capture Seltou avec des pièces hors limites.
 *
 * \details
 * - Place un pion bleu sur le bord du plateau.  
 * - Place un pion rouge "hors tableau" (coordonnées négatives).  
 * - Simule le déplacement du pion bleu.  
 * - Vérifie que la fonction ne tente pas de capturer hors limites et ne plante pas.  
 *
 * \note Affiche les résultats sur la sortie standard.
 */
void test_seltou_out_of_bounds3() {
    reset_game_state();

    // Pion bleu sur le bord supérieur
    pieces[piece_count++] = (Piece){.row=0, .col=0, .color='B', .type='P'};
    // Ennemi rouge "hors tableau" 
    pieces[piece_count++] = (Piece){.row=-1, .col=0, .color='R', .type='P'}; // hors limites

    // Déplacement du bleu vers le coin (toujours dans le tableau)
    pieces[0].row = 2456000000;
    pieces[0].col = 4300000;

    // Appel de la fonction : ça ne doit rien faire ni crasher
    check_seltou_capture(0, 0, 2456000000);

    // Vérifie que le nombre de pièces n'a pas changé (pas de capture)
    assert(piece_count == 2);

    printf("test_seltou_out_of_bounds3 bien passe\n");
}

/** \fn void test_double_capture_seltou_linca()
 * \brief Test capture combinée Seltou + Linca.
 *
 * \details
 *   - Place un pion bleu et deux pions rouges avec un pion bleu derrière pour Linca.  
 *   - Déplace le pion bleu pour déclencher Seltou puis Linca.  
 *   - Vérifie que les deux pions rouges sont correctement capturés.
 *
 * \note Affiche le résultat du test sur la sortie standard.
 */
void test_double_capture_seltou_linca() {
    reset_game_state();

    // Bleu : pièce qui bouge
    pieces[piece_count++] = (Piece){.row=3, .col=4, .color='B', .type='P'};

    // Rouge 1 : devant (pour Seltou)
    pieces[piece_count++] = (Piece){.row=5, .col=4, .color='R', .type='P'};

    // Rouge 2 : sur la droite (pour Linca après déplacement)
    pieces[piece_count++] = (Piece){.row=4, .col=5, .color='R', .type='P'};
    // Bleu derrière Rouge 2 pour sandwich
    pieces[piece_count++] = (Piece){.row=4, .col=6, .color='B', .type='P'};

    // Déplacement de la pièce bleue de (3,4) à (4,4) => déclenche Seltou
    pieces[0].row = 4;
    pieces[0].col = 4;

    // On simule Seltou
    check_seltou_capture(0, 3, 4);

    // Ensuite, on simule Linca pour vérifier si la chaîne se déclenche
    check_linca_capture(0);

    // Vérification
    assert(find_piece_at(5,4) == -1); // Rouge 1 capturé
    assert(find_piece_at(6,4) == -1); // Rouge 2 capturé
    assert(piece_count == 2);         // il reste seulement les pièces bleues

    printf("test_double_capture_seltou_linca bien passe\n");
}


/** \fn void test_triple_capture()
 * \brief Test capture combinée Seltou + double Linca.
 *
 * \details
 *   - Place un pion bleu et trois pions rouges avec pions bleus pour sandwich.  
 *   - Déplace le pion bleu pour déclencher toutes les captures.  
 *   - Vérifie que les trois pions rouges sont correctement capturés.
 *
 * \note Affiche le résultat du test sur la sortie standard.
 */
void test_triple_capture() {
    reset_game_state();

    // Bleu : pièce qui bouge
    pieces[piece_count++] = (Piece){.row=1, .col=4, .color='B', .type='P'};

    // Rouge 1 : devant (pour Seltou)
    pieces[piece_count++] = (Piece){.row=3, .col=4, .color='R', .type='P'};

    // Rouge 2 : sur la droite (pour Linca après déplacement)
    pieces[piece_count++] = (Piece){.row=2, .col=5, .color='R', .type='P'};
    // Bleu derrière Rouge 2 pour sandwich
    pieces[piece_count++] = (Piece){.row=2, .col=6, .color='B', .type='P'};

    // Rouge 3 : sur la gauche (pour Linca après la deuxième capture)
    pieces[piece_count++] = (Piece){.row=2, .col=3, .color='R', .type='P'};
    // Bleu derrière Rouge 3 pour sandwich
    pieces[piece_count++] = (Piece){.row=2, .col=2, .color='B', .type='P'};

    // Déplacement de la pièce bleue de (1,4) à (2,4) => déclenche LES CAPTURES
    pieces[0].row = 2;
    pieces[0].col = 4;

    // Appel des captures
    check_seltou_capture(0, 1, 4);
    check_linca_capture(0);

    // Vérification
    assert(find_piece_at(3,4) == -1); // Rouge 1 capturé (Seltou)
    assert(find_piece_at(4,5) == -1); // Rouge 2 capturé (Linca)
    assert(find_piece_at(4,3) == -1); // Rouge 3 capturé (Linca)
    assert(piece_count == 3);         // il reste uniquement les pièces bleues

    printf("test_triple_capture bien passe\n");
}


/** \fn void test_linca_deux_pieces()
 * \brief Test Linca avec deux pions rouges adjacents.
 *
 * \details
 *   - Place un pion bleu, deux pions rouges et un pion bleu derrière.  
 *   - Déplace le pion bleu pour déclencher Linca.  
 *   - Vérifie qu’aucune capture ne se produit.
 *
 * \note Affiche le résultat du test sur la sortie standard.
 */
void test_linca_deux_pieces() {
    reset_game_state();

    // On place un pion bleu à (4,3)
    pieces[piece_count++] = (Piece){.row=4, .col=3, .color='B', .type='P'};
    // On place un pion rouge à (4,5)
    pieces[piece_count++] = (Piece){.row=4, .col=5, .color='R', .type='P'};
        // On place un pion rouge à (4,6)
    pieces[piece_count++] = (Piece){.row=4, .col=6, .color='R', .type='P'};
    // On place un pion bleu derrière à (4,7)
    pieces[piece_count++] = (Piece){.row=4, .col=7, .color='B', .type='P'};

    // On simule le déplacement du bleu en (4,4)
    check_linca_capture(0);

    // On vérifie Qu'IL N'Y A PAS DE CAPTURE
    assert(piece_count == 4); // il doit rester 4 pièces

    printf("test_linca_deux_pieces bien passe\n");
}


/** \fn void test_seltou_deux_pieces()
 * \brief Test Seltou avec deux pions ennemis devant.
 *
 * \details
 *   - Place un pion bleu et deux pions rouges devant.  
 *   - Déplace le pion bleu pour déclencher Seltou.  
 *   - Vérifie que seule la première pièce rouge est capturée.
 *
 * \note Affiche le résultat du test sur la sortie standard.
 */
void test_seltou_deux_pieces() {
    reset_game_state();

    // Pion bleu
    pieces[piece_count++] = (Piece){.row=3, .col=4, .color='B', .type='P'};
    // Ennemi rouge devant
    pieces[piece_count++] = (Piece){.row=5, .col=4, .color='R', .type='P'};
        // Ennemi rouge devant
    pieces[piece_count++] = (Piece){.row=6, .col=4, .color='R', .type='P'};

    // Déplacement du bleu DE (3,4) vers (4,4)
    pieces[0].row = 4;
    pieces[0].col = 4;

    check_seltou_capture(0, 3, 4);
    assert(piece_count == 3); //capture rouge normalement

    printf("test_seltou_deux_pieces bien passe\n");
}
