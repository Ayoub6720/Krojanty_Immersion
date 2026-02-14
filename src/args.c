/**
 * \file args.c
 * \brief Analyse et gestion des arguments de la ligne de commande.
 * \author loic.claude@uha.fr, nolann.wickers@uha.fr, valentin.prevot@uha.fr, valentin.leray@uha.fr,
 *         ayoub.darkaoui@uha.fr, ismail.el-maallem@uha.fr, corentin.banocay@uha.fr
 * \version 0.1
 * \date 2025-09-16
 *
 * \details
 * Ce fichier implémente les fonctions liées à la lecture et validation
 * des arguments passés au programme :
 * - Sélection du mode de jeu (local, serveur, client).
 * - Paramètres IA (zéro, une ou deux IA).
 * - Parsing des adresses et des ports.
 * - Vérification des erreurs et affichage de l’aide.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "args.h"


/**
 * \fn args_t init_args(void)
 * \brief Initialise une structure
 * 
 * \c args_t avec des valeurs par défaut.
 * \return Une structure args_t prête à l’usage.
 */
static args_t init_args(void) {
    args_t args = {
        .mode = MODE_NONE,   // Aucun mode par défaut
        .is_ia = 0,
        .is_ia_both = 0,
        .host = NULL,
        .port = 0,
        .help = 0,
        .error = 0
    };
    return args;
}

/**
 * \fn int parse_address(const char *addr_str, char **host, int *port)
 * \brief Analyse une adresse au format HOST:PORT.
 * 
 * \param addr_str Chaîne d’adresse (exemple : "127.0.0.1:5555").
 * \param host Pointeur vers une chaîne allouée dynamiquement contenant l’hôte (retour).
 * \param port Pointeur vers le port extrait (retour).
 * \return 0 si succès, -1 si erreur de parsing.
 */
static int parse_address(const char *addr_str, char **host, int *port) {
    char *colon_pos = strchr(addr_str, ':');
    if (!colon_pos) {
        return -1;
    }
    
    *port = atoi(colon_pos + 1);
    if (*port <= 0 || *port > 65535) {
        return -1;
    }
    
    int host_len = colon_pos - addr_str;
    *host = malloc(host_len + 1);
    if (!*host) {
        return -1;
    }
    
    strncpy(*host, addr_str, host_len);
    (*host)[host_len] = '\0';
    
    return 0;
}


/**
 * \fn int parse_port_token(const char* tok, int* out_port)
 * \brief Analyse un token pour extraire un port entier valide.
 * 
 * \param tok Chaîne contenant le port.
 * \param out_port Pointeur pour stocker le port converti.
 * \return 0 si succès, -1 si erreur.
 */
static int parse_port_token(const char* tok, int* out_port) {
    if (!tok || !out_port) return -1;
    char* end = NULL;
    long val = strtol(tok, &end, 10);
    if (*tok == '\0' || (end && *end != '\0')) return -1;
    if (val <= 0 || val > 65535) return -1;
    *out_port = (int)val;
    return 0;
}

/**
 * \fn args_t parse_args(int argc, char **argv)
 * \brief Analyse tous les arguments de la ligne de commande.
 * 
 * \param argc Nombre d’arguments.
 * \param argv Tableau d’arguments.
 * \return Structure args_t remplie avec les options et valeurs détectées.
 */
args_t parse_args(int argc, char **argv) {
    args_t args = init_args();

    if (argc < 2) {
        args.error = 1;  // Aucun argument fourni = erreur
        return args;
    }

    int ia_count = 0;
    for (int i = 1; i < argc; i++) {
        const char* tok = argv[i];

        // Option d'aide
        if (strcmp(tok, "-h") == 0 || strcmp(tok, "--help") == 0) {
            args.help = 1;
            return args;
        }
        // Modes
        else if (strcmp(tok, "-l") == 0 || strcmp(tok, "--local") == 0) {
            args.mode = MODE_LOCAL;
        } else if (strcmp(tok, "-s") == 0 || strcmp(tok, "--server") == 0) {
            args.mode = MODE_SERVER;
        } else if (strcmp(tok, "-c") == 0 || strcmp(tok, "--client") == 0) {
            args.mode = MODE_CLIENT;
        }
        // IA
        else if (strcmp(tok, "-ia") == 0 || strcmp(tok, "--ia") == 0) {
            ia_count++;
        }
        // Opérandes non option (port ou host:port)
        else if (tok[0] != '-') {
            if (args.mode == MODE_SERVER && args.port == 0) {
                if (parse_port_token(tok, &args.port) != 0) { args.error = 1; return args; }
            } else if (args.mode == MODE_CLIENT && args.host == NULL) {
                if (parse_address(tok, &args.host, &args.port) != 0) { args.error = 1; return args; }
            } else {
                fprintf(stderr, "Argument inattendu: %s\n", tok);
                args.error = 1;
                return args;
            }
        }
        // Inconnu
        else {
            fprintf(stderr, "Option inconnue: %s\n", tok);
            args.error = 1;
            return args;
        }
    }

    // Post-traitement IA
    args.is_ia = (ia_count >= 1);
    args.is_ia_both = (ia_count >= 2);

    // Dernières vérifs
    if (args.mode == MODE_NONE) {
        args.error = 1;
    }
    if (args.mode == MODE_SERVER && args.port == 0) {
        args.error = 1;
    }
    if (args.mode == MODE_CLIENT && (args.host == NULL || args.port == 0)) {
        args.error = 1;
    }

    return args;
}


/**
 * \fn void print_usage(const char *program_name)
 * \brief Affiche l’aide à l’utilisateur.
 * 
 * \param program_name Nom du programme (argv[0]).
 */
void print_usage(const char *program_name) {
    printf("Usage: %s [MODE] [OPTIONS]\n\n", program_name);
    printf("Modes de jeu (obligatoires):\n");
    printf("  -l, --local               #Mode local (2 joueurs sur le même exécutable)\n");
    printf("  -s, --server PORT         #Mode serveur sur le port spécifié\n");
    printf("  -c, --client HOST:PORT    #Mode client, connexion à HOST:PORT\n\n");
    printf("Options:\n");
    printf("  -ia, --ia                 #Active l'IA (1x = une IA joue votre couleur, 2x = IA vs IA en local)\n");
    printf("  -h, --help                #Affiche cette aide\n\n");
    printf("Exemples:\n");
    printf("  %s -l                     # Jeu local\n", program_name);
    printf("  %s -s 5555                # Serveur sur le port 5555\n", program_name);
    printf("  %s -c 127.0.0.1:5555      # Client vers 127.0.0.1:5555\n", program_name);
    printf("  %s -s -ia 5555            # Serveur avec IA (Rouge)\n", program_name);
    printf("  %s -c -ia 127.0.0.1:5555  # Client avec IA (Bleu)\n", program_name);
    printf("  %s -l -ia -ia             # Local IA vs IA (les deux couleurs)\n", program_name);
}


/**
 * \fn void free_args(args_t *args)
 * \brief Libère la mémoire allouée dans une structure args_t.
 * 
 * \param args Pointeur vers la structure à nettoyer.
 *
 * Libère notamment la chaîne \c host si elle a été allouée.
 */
void free_args(args_t *args) {
    if (args && args->host) {
        free(args->host);
        args->host = NULL;
    }
}
