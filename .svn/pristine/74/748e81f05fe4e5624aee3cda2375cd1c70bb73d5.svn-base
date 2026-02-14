/** 
* \file main.c 
* \brief Point d'entrée pour lancer le jeu 
* \author loic.claude@uha.fr,nolann.wickers@uha.fr,valentin.prevot@uha.fr,valentin.leray@uha.fr,ayoub.darkaoui@uha.fr,
*         ismail.el-maallem@uha.fr,corentin.banocay@uha.fr, alan.akgun@uha.fr
* \version 0.1 
* \date 2025-09-16 
* \details Ce fichier contient la fonction principale du programme. 
* Il gère l'analyse des arguments de la ligne de commande et lance le jeu 
* en mode local, serveur ou client selon les options fournies. 
*/

#include <stdio.h>
#include <stdlib.h>
#include "../include/args.h"
#include "../include/app.h"
#include "../include/gui.h"
#include "../include/net.h"


/**
 * \fn int main(int argc, char **argv)
 * \brief Fonction principale du programme.
 *
 * \param argc Nombre d’arguments passés en ligne de commande.
 * \param argv Tableau contenant les arguments.
 * \return Code de retour du programme (0 si succès, 1 si erreur).
 *
 * \details
 * Cette fonction :
 * - Vérifie et interprète les arguments via \c parse_args().
 * - Affiche l’aide si demandé.
 * - Gère les erreurs de syntaxe des arguments.
 * - Lance le jeu dans le mode correspondant :
 *   - **Serveur** : démarre un serveur réseau (\c run_server).
 *   - **Client** : se connecte à un serveur (\c run_client).
 *   - **Local** : démarre une partie à 2 joueurs sur la même machine (\c start_gui).
 *   - **IA locale** : démarre une partie où une ou deux IA jouent en local.
 * - Configure l’IA selon le mode et les options (\c ia_active, \c ia_color, \c ia_both_active).
 * - Libère la mémoire associée aux arguments (\c free_args).
 */

int main(int argc, char **argv) {
    args_t args = parse_args(argc, argv);

    // gestion aide
    if (args.help) {
        print_usage(argv[0]);
        free_args(&args);
        return 0;
    }

    // gestion erreur args
    if (args.error) {
        fprintf(stderr, "Erreur: Arguments invalides.\n");
        fprintf(stderr, "Utilisez '%s -h' ou '%s --help' pour plus d'informations.\n",
                argv[0], argv[0]);
        free_args(&args);
        return 1;
    }


    // verif fonctionnalites
    if (args.mode == MODE_SERVER) {
        // Config IA pour le serveur: couleur rouge
        if (args.is_ia) {
            extern int ia_active;
            extern char ia_color;
            ia_active = 1;
            ia_color = 'R';
        }

        printf("Demarrage du jeu en mode serveur sur le port %d...\n", args.port);
        int res = run_server(args.port, args.mode);
        free_args(&args);
        return res;
    }

    // mode client
    if (args.mode == MODE_CLIENT) {
        // Config IA pour le client: couleur bleue
        if (args.is_ia) {
            extern int ia_active;
            extern char ia_color;
            ia_active = 1;
            ia_color = 'B';
        }

        printf("Demarrage du jeu en mode client, connexion à %s:%d...\n", args.host ? args.host : "127.0.0.1", args.port);
        int res = run_client(args.host ? args.host : "127.0.0.1", args.port, args.mode);
        free_args(&args);
        return res;
    }

    // IA locale (mode standalone)
    if (args.is_ia && args.mode == MODE_NONE) {
        fprintf(stderr, "Demarrage du jeu en mode IA locale\n");
        extern int ia_active; extern int ia_both_active; extern char ia_color;
        ia_active = 1;
        if (args.is_ia_both) {
            ia_both_active = 1;
        } else {
            ia_color = 'R';
        }
        int gui_argc = 1; char *gui_argv[] = { argv[0], NULL };
        start_gui(gui_argc, gui_argv, MODE_LOCAL);
        free_args(&args);
        return 0;
    }

    // mode local
    if (args.mode == MODE_LOCAL) {
        printf("Demarrage du jeu en mode local...\n");

        // Config IA locale
        extern int ia_active; extern int ia_both_active; extern char ia_color;
        if (args.is_ia_both) {
            ia_active = 1;
            ia_both_active = 1;
        } else if (args.is_ia) {
            ia_active = 1;
            ia_color = 'R';
        }

        int gui_argc = 1;
        char *gui_argv[] = { argv[0], NULL };
        start_gui(gui_argc, gui_argv, args.mode);

        free_args(&args);
        return 0;
    }

    // ne devrait jamais arriver
    fprintf(stderr, "Erreur: Mode de jeu non reconnu\n");
    free_args(&args);
    return 1;
}
