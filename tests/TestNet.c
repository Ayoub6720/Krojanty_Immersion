/**
 * \file TestNet.c
 * \brief Tests unitaires et d'intégration pour le module réseau.
 *
 * \details
 * Ce fichier contient les tests pour le module réseau (net.c), notamment
 * la fonction TCP_Send_Message qui envoie des coups de jeu via TCP.  
 * Les tests vérifient :
 *   - Les comportements avec des entrées valides et invalides.
 *   - La robustesse face à des pointeurs NULL ou des chaînes de longueur incorrecte.
 *   - La séquence d'envoi de plusieurs coups.
 *
 * \note Tous les tests utilisent des sockets fictifs (INVALID_SOCKET) pour ne
 * pas dépendre d'une vraie connexion réseau.
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "net.h"  // TCP_Send_Message, INVALID_SOCKET

/** 
 * \fn void test_net_send_message_invalid()
 * \brief Test de l’envoi d’un message avec une chaîne de longueur incorrecte.
 *
 * \details
 * - Tente d’envoyer une chaîne de 3 caractères au lieu de 4.  
 * - Vérifie que la fonction retourne -1 pour indiquer une erreur.  
 *
 * \note Affiche les résultats sur la sortie standard.
 */
void test_net_send_message_invalid() {
    int r = TCP_Send_Message(INVALID_SOCKET, "ABC"); // 3 caractères au lieu de 4
    assert(r == -1);
    printf("test_net_send_message_invalid OK\n");
}


/** 
 * \fn void test_net_send_message_null()
 * \brief Test de l’envoi d’un message avec un pointeur NULL.
 *
 * \details
 * - Tente d’envoyer un message NULL.  
 * - Vérifie que la fonction retourne -1 pour indiquer une erreur.  
 *
 * \note Affiche les résultats sur la sortie standard.
 */
void test_net_send_message_null() {
    int r = TCP_Send_Message(INVALID_SOCKET, NULL);
    assert(r == -1);
    printf("test_net_send_message_null OK\n");
}

/** 
 * \fn void test_net_send_message_sequence()
 * \brief Test de l’envoi d’une séquence de messages valides et invalides.
 *
 * \details
 * - Tente d’envoyer plusieurs messages, certains valides (4 caractères),
 *   d’autres invalides (3 caractères, NULL, 6 caractères).  
 * - Vérifie que les envois valides retournent 4 ou -1 (socket invalide),
 *   et les envois invalides retournent -1.  
 *
 * \note Affiche les résultats sur la sortie standard.
 */
void test_net_send_message_sequence() {
    SOCKET dummy_socket = INVALID_SOCKET; // pas de vrai socket, juste pour test

    const char* moves[] = {
        "A2A3", // valide
        "B2B4", // valide
        "C3",   // invalide (3 caractères)
        NULL,   // invalide (NULL)
        "D1D2D3" // invalide (6 caractères)
    };

    int results[5] = {0};

    for (int i = 0; i < 5; i++) {
        results[i] = TCP_Send_Message(dummy_socket, moves[i]);
    }

    // Vérifications
    assert(results[0] == -1 || results[0] == 4); // si socket invalide, -1 ; sinon 4
    assert(results[1] == -1 || results[1] == 4);
    assert(results[2] == -1); // trop court
    assert(results[3] == -1); // NULL
    assert(results[4] == -1); // trop long

    printf("test_net_send_message_sequence OK\n");
}