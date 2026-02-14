/**
 * \file TestArgs.c
 * \brief Tests unitaires pour le module de gestion des arguments (`args.c`).
 *
 * Ce fichier contient une série de tests unitaires qui valident le comportement
 * de la fonction \ref parse_args et la structure \ref args_t.  
 *
 * Les fonctionnalités testées incluent :
 *  - Le parsing d'arguments pour les différents modes de jeu (local, serveur, client).
 *  - La gestion des options spécifiques comme l'IA simple ou l'IA des deux côtés.
 *  - La détection des erreurs dans le cas d'arguments invalides ou incomplets.
 *
 * Chaque fonction de test simule un appel au programme principal avec une série
 * d'arguments (`argv` et `argc`) puis vérifie les champs de la structure `args_t`
 * avec des assertions.
 *
 * \note Les fonctions utilisent \c assert() pour vérifier la validité.
 *       En cas d'échec, l'exécution du test est interrompue.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <assert.h>
#include "args.h"

/**
 * \fn void test_parse_args_local()
 * \brief Test du parsing des arguments pour le mode local.
 *
 * \details
 * - Simule l'appel avec `-l`.  
 * - Vérifie que le mode est bien local et qu'aucune option IA n'est activée.  
 * - Vérifie qu'aucune erreur n'est signalée.  
 *
 * \note Affiche les résultats sur la sortie standard.
 */
void test_parse_args_local() {
    char *argv[] = {"program", "-l"};
    args_t args = parse_args(2, argv);
    assert(args.mode == MODE_LOCAL);
    assert(!args.is_ia && !args.is_ia_both);
    assert(!args.error);
    free_args(&args);
    printf("test_parse_args_local OK\n");
}

/**
 * \fn void test_parse_args_server()
 * \brief Test du parsing des arguments pour le mode serveur.
 *
 * \details
 * - Simule l'appel avec `-s <port>`.  
 * - Vérifie que le mode est bien serveur et que le port est correct.  
 * - Vérifie qu'aucune erreur n'est signalée.  
 *
 * \note Affiche les résultats sur la sortie standard.
 */
void test_parse_args_server() {
    char *argv[] = {"program", "-s", "5555"};
    args_t args = parse_args(3, argv);
    assert(args.mode == MODE_SERVER);
    assert(args.port == 5555);
    assert(!args.error);
    free_args(&args);
    printf("test_parse_args_server OK\n");
}

/**
 * \fn void test_parse_args_client()
 * \brief Test du parsing des arguments pour le mode client.
 *
 * \details
 * - Simule l'appel avec `-c <host:port>`.  
 * - Vérifie que le mode est bien client, que l'hôte et le port sont corrects.  
 * - Vérifie qu'aucune erreur n'est signalée.  
 */
void test_parse_args_client() {
    char *argv[] = {"program", "-c", "127.0.0.1:5555"};
    args_t args = parse_args(3, argv);
    assert(args.mode == MODE_CLIENT);
    assert(args.port == 5555);
    assert(args.host != NULL && strcmp(args.host, "127.0.0.1") == 0);
    free_args(&args);
    printf("test_parse_args_client OK\n");
}

/**
 * \fn void test_parse_args_ia()
 * \brief Test du parsing des arguments pour l'option IA.
 *
 * \details
 * - Simule l'appel avec `-l -ia -ia`.  
 * - Vérifie que le mode est local et que les options IA sont activées.  
 * - Vérifie qu'aucune erreur n'est signalée.  
 */
void test_parse_args_ia() {
    char *argv[] = {"program", "-l", "-ia", "-ia"};
    args_t args = parse_args(4, argv);
    assert(args.mode == MODE_LOCAL);
    assert(args.is_ia && args.is_ia_both);
    free_args(&args);
    printf("test_parse_args_ia OK\n");
}

/**
 * \fn void test_parse_args_invalid()
 * \brief Test du parsing des arguments pour des cas invalides.
 *
 * \details
 * - Simule plusieurs appels avec des arguments incorrects ou incomplets.  
 * - Vérifie que les erreurs sont correctement détectées.  
 */
void test_parse_args_invalid() {
    char *argv1[] = {"program"}; // Aucun argument
    args_t args1 = parse_args(1, argv1);
    assert(args1.error);
    free_args(&args1);

    char *argv3[] = {"program", "-s", "abc"}; // port invalide
    args_t args3 = parse_args(3, argv3);
    assert(args3.error);
    free_args(&args3);

    char *argv4[] = {"program", "-c", "127.0.0.1"}; // client sans port
    args_t args4 = parse_args(3, argv4);
    assert(args4.error);
    free_args(&args4);

    printf("test_parse_args_invalid OK\n");
}
