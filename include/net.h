#ifndef NET_H
#define NET_H
#include <args.h>

/**
 * @file net.h
 * @brief Module réseau permettant de jouer au Krojanty en ligne via TCP/IP
 * @author Ayoub DARKAOUI (ayoub.darkaoui@uha.fr)
 * @date 2025-09-18
 * @version 1.0
 *
 * Ce module implémente toute la couche réseau du jeu, permettant à deux joueurs
 * de s'affronter à distance. Il gère :
 * - La création de serveur et connexion des clients
 * - La synchronisation de l'état du jeu entre les joueurs
 * - Le protocole de communication basé sur des messages texte
 * - La gestion des déconnexions et des erreurs réseau
 * 
 * Le protocole est conçu pour être simple et robuste, avec une validation
 * des mouvements côté serveur pour garantir l'intégrité du jeu.
 *
 * @struct server_context_t
 * @brief Contexte d'un serveur de jeu
 *
 * Cette structure maintient l'état d'un serveur de jeu Krojanty,
 * incluant la socket d'écoute, les connexions clients et l'état
 * du jeu en cours.
 *
 * @var server_context_t::listen_sock
 * Socket d'écoute pour les nouvelles connexions
 *
 * @var server_context_t::client_sock
 * Socket de communication avec le client connecté
 *
 * @var server_context_t::client_addr
 * Adresse du client connecté
 *
 * @var server_context_t::client_len
 * Longueur de l'adresse client
 *
 * @var server_context_t::client_color
 * Couleur assignée au client ('R' ou 'B')
 *
 * @var server_context_t::server_color
 * Couleur du serveur ('R' ou 'B')
 *
 * @var server_context_t::game_started
 * Flag indiquant si une partie est en cours
 *
 * \section net_usage Utilisation du Mode Réseau
 * 
 * \subsection server_setup Configuration du Serveur
 * 1. Lancer le serveur :
 *    ```bash
 *    ./game --server 12345
 *    ```
 * 2. Le serveur attend une connexion sur le port spécifié
 * 3. Une fois le client connecté, la partie démarre automatiquement
 *
 * \subsection server_ia Serveur avec IA
 * Pour utiliser l'IA comme adversaire sur le serveur :
 * ```bash
 * ./game --server 12345 --ia
 * ```
 * L'IA jouera automatiquement ses coups contre le client connecté.
 *
 * \subsection client_setup Configuration du Client
 * 1. Connexion à un serveur local :
 *    ```bash
 *    ./game --client localhost:12345
 *    ```
 * 2. Connexion à un serveur distant :
 *    ```bash
 *    ./game --client 192.168.1.10:12345
 *    ```
 * 
 * \subsection net_notes Notes Importantes
 * - Le serveur doit être lancé avant le client
 * - Le port doit être le même des deux côtés
 * - En cas de déconnexion, la partie est sauvegardée
 * - Le serveur valide tous les mouvements
 * - Les messages d'erreur réseau sont affichés dans la GUI
 * 
 * @see args.h pour les options de ligne de commande
 * @see gui.h pour l'interface graphique
 *
 * \section net_overview Vue d'ensemble
 * Le module réseau permet de jouer au Krojanty en réseau via TCP/IP.
 * Il implémente un protocole simple basé sur des messages textuels.
 *
 * \section protocol Protocole de Communication
 *
 * \subsection conn_protocol Protocole de Connexion
 * 1. Client -> Serveur : "HELLO"
 * 2. Serveur -> Client : "COLOR <R|B>"
 * 3. Client -> Serveur : "READY"
 *
 * \subsection game_protocol Protocole de Jeu
 * Messages durant la partie :
 * - MOVE <from_x> <from_y> <to_x> <to_y>
 * - CAPTURE <x> <y>
 * - RESIGN
 * - DRAW_OFFER
 * - DRAW_ACCEPT
 * - DRAW_DECLINE
 *
 * \subsection sync_protocol Synchronisation
 * - SYNC_REQUEST
 * - SYNC_STATE <état_complet>
 * - PING
 * - PONG
 *
 * \section error_handling Gestion des Erreurs
 * - Timeout de connexion : 30 secondes
 * - Retry sur perte de connexion : 3 tentatives
 * - Validation des messages
 * - Détection de déconnexion
 *
 * \section implementation Implémentation
 * Le module définit les fonctions pour :
 * - Démarrer un serveur TCP
 * - Se connecter en client
 * - Envoyer des messages via sockets
 * - Gérer le protocole de jeu
 */

#ifdef _WIN32
#include <winsock2.h>
typedef int socklen_t;
#else

#include <sys/socket.h>
typedef int SOCKET;
#define INVALID_SOCKET -1
#endif

/**
 * @brief Socket global pour la communication réseau
 * 
 * Socket principal utilisé pour la communication client/serveur.
 * Initialisé lors de la connexion et utilisé pour envoyer/recevoir
 * les messages du protocole.
 */
extern SOCKET g_socket;

/**
 * @brief Couleur assignée au joueur local
 * 
 * Identifie la couleur du joueur sur cette instance :
 * - 'R' : Rouge (serveur)
 * - 'B' : Bleu (client)
 * Utilisé pour valider les mouvements et synchroniser l'état.
 */
extern char my_color;

/** Démarre un serveur TCP pour une partie réseau.
 * @param port port à écouter
 * @param mode mode (utilisé pour ajustements spécifiques)
 * @return 0 si succès, négatif sinon
 */
int run_server(short port, game_mode_t mode);

/** Se connecte à un serveur distant.
 * @param addr adresse du serveur (IP ou nom)
 * @param port port du serveur
 * @param mode mode de jeu
 * @return 0 si succès, négatif sinon
 */
int run_client(const char* addr, short port, game_mode_t mode);

/** Envoi d'un message via socket (wrapper simple). */
int TCP_Send_Message(SOCKET s, const char* message);

#endif
