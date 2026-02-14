/**
 * \file MainTest.c
 * \brief Harness de test principal : exécute la suite de tests unitaires.
 *
 * \details
 * Ce fichier centralise et exécute toutes les fonctions de tests unitaires 
 * réparties dans les différents modules du projet :
 * - Capture.c : tests des règles de capture Linca et Seltou.
 * - Drawing.c : tests des fonctions graphiques (conversion clic → case).
 * - Game.c : tests de la logique de jeu (déplacement, score, blocage, auto-défaite).
 * - Args.c : tests du parsing des arguments en ligne de commande.
 * - Net.c : tests des fonctions réseau (envoi de messages).
 * - Status.c : tests de l’affichage et de l’enregistrement des états.
 * - IA : tests de l’algorithme Minimax et de la recherche de coups.
 *
 * \authors loic.claude@uha.fr, corentin.banocay@uha.fr
 *
 *
 * \date 2025-09-18
 * \version 0.1
 */

#include <stdio.h>
#include <gtk/gtk.h>

// Déclarations des tests Capture.c
void test_linca_simple_capture();
void test_seltou_simple_capture();
void test_auto_defeat();
void test_seltou_out_of_bounds();   //pas de capture hors case
void test_seltou_out_of_bounds2(); 
void test_seltou_out_of_bounds3();
void test_double_capture_seltou_linca();
void test_triple_capture();
void test_linca_deux_pieces();
void test_seltou_deux_pieces();

// Déclarations des tests Drawing.c
void test_click_to_cell();

// Déclarations des tests Game.c
void test_game_setup_default();
void test_move_piece_basic();
void test_move_blocked();
void test_score_update();
void Test_Auto_Defaite();
void test_move_blocked_by_ally();
int test_find_piece_at();
void test_auto_defeat_trigger();

// Déclarations des tests Args.c
void test_parse_args_local();
void test_parse_args_server();
void test_parse_args_client();
void test_parse_args_ia();
void test_parse_args_invalid();

// Déclarations des tests Net.c
void test_net_send_message_invalid();
void test_net_send_message_null();
void test_net_send_message_sequence();


// Déclarations des tests status.c
void test_status_register_and_draw(void);


// Déclarations des tests IA
void test_ai_minimax_basic();
void test_trouverMeilleurCoupIA();
void test_minimaxIA_basic();



/**
 * \fn int main()
 * \brief Point d’entrée du programme de tests.
 *
 * \details Initialise GTK, exécute tous les tests unitaires et affiche le résultat.
 *
 * \return 0 si tous les tests passent avec succès, code d’erreur sinon.
 */
int main() {
    gtk_init();

    printf("\n  === Debut des tests ===\n");

     printf("\n  === Lancement des tests Capture.c ===\n");
     test_linca_simple_capture();
     test_seltou_simple_capture();
     test_auto_defeat();
     test_seltou_out_of_bounds();
     test_seltou_out_of_bounds2();
     test_seltou_out_of_bounds3();
     test_double_capture_seltou_linca();
     test_triple_capture();
     test_linca_deux_pieces();
     test_seltou_deux_pieces();
     printf("Tous les tests Capture.c sont passes avec succes\n");
    

    printf("\n  === Lancement des tests drawing.c ===\n");
    test_click_to_cell();
    printf("Tous les tests drawing.c sont passes avec succes\n");


    printf("\n  === Lancement des tests game.c ===\n");
    test_game_setup_default();
    test_move_piece_basic();
    test_move_blocked();
    test_score_update();
    Test_Auto_Defaite();
    test_move_blocked_by_ally();
    test_find_piece_at();
    test_auto_defeat_trigger();
    printf("Tous les tests game.c sont passes avec succes\n");


    printf("\n === Lancement des tests args.c ===\n");
    test_parse_args_local();
    test_parse_args_server();
    test_parse_args_client();
    test_parse_args_ia();
    test_parse_args_invalid();
    printf("Tous les tests args.c sont passes avec succes\n");

    printf("\n === Lancement des tests net.c ===\n");
    test_net_send_message_invalid();
    test_net_send_message_null();
    test_net_send_message_sequence();
    printf("Tous les tests net.c sont passes avec succes\n");

    printf("\n === Lancement des tests status.c ===\n");
    test_status_register_and_draw();
    printf("Tous les tests status.c sont passes avec succes\n");

    printf("\n=== Lancement des tests Ai.c ===\n");
    test_ai_minimax_basic();
    test_trouverMeilleurCoupIA();
    test_minimaxIA_basic();
    printf("Tous les tests IA sont passes avec succes\n");


    printf("\n  === Tous les tests sont passes avec succes ===\n");
    
    return 0;
}
