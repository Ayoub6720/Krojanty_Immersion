/**
 * \file net.c
 * \brief Gestion du réseau pour le jeu : serveur et client TCP.
 * \author valentin.leray@uha.fr
 * \version 0.1
 * \date 2025-09-17
 *
 * \details
 * Ce fichier implémente les fonctions réseau du jeu :
 * - Création et gestion des sockets TCP.
 * - Boucles serveur et client avec réception et envoi des coups.
 * - Communication avec l'interface GTK via des callbacks.
 * - Gestion des erreurs réseau et fermeture propre des sockets.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <gui.h>
#include <game.h>
#include <status.h>
#include <ctype.h>
#include <pthread.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    #define SOCKET_ERROR -1
#endif
#include "net.h"

#define BUFFER_SIZE 5               // 4 chars + \0 pour les chaînes de coups

SOCKET g_socket = INVALID_SOCKET;   // Socket globale utilisée pour la communication reseau
char my_color = '\0';               // Couleur du joueur local ('R' ou 'B')

/**
 * @brief Contexte d'exécution d'un serveur/client réseau
 * 
 * Cette structure maintient l'état d'une connexion réseau active,
 * utilisée à la fois par le serveur et le client pour gérer
 * la communication et la synchronisation.
 */
typedef struct {
    SOCKET s;           /**< Socket de communication active */
    game_mode_t mode;   /**< Mode de jeu (serveur/client) */
    int running;        /**< Flag d'exécution du thread réseau */
} server_context_t;


/**
 * \fn void cleanup_socket(SOCKET s)
 * \brief Ferme un socket si nécessaire.
 * 
 * \param s Socket à fermer.
 */
void cleanup_socket(SOCKET s) {
    if (s != INVALID_SOCKET) {
        #ifdef _WIN32
            closesocket(s);
        #else
            close(s);
        #endif
    }
}

/**
 * \fn int recv_move(SOCKET s, char *buffer)
 * \brief Lit exactement 4 caractères depuis une socket.
 * 
 * \param s Socket source.
 * \param buffer Buffer de réception (doit pouvoir contenir 5 caractères).
 * \return Nombre de caractères lus (4) ou <=0 en cas d'erreur ou connexion fermée.
 */
int recv_move(SOCKET s, char *buffer) {
    int total = 0;
    while (total < 4) {
        int r = recv(s, buffer + total, 4 - total, 0);
        if (r <= 0) {
            return r; // erreur ou connexion fermee
        }
        total += r;
    }
    buffer[4] = '\0'; // terminaison de chaîne
    return total;
}

/**
 * \fn SOCKET TCP_Create_Server(short port)
 * \brief Crée un serveur TCP.
 * 
 * \param port Port d'écoute.
 * \return Socket serveur valide ou INVALID_SOCKET en cas d'erreur.
 */
SOCKET TCP_Create_Server(short port) {
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        perror("socket");
        return INVALID_SOCKET;
    }

    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    struct sockaddr_in sin = {0};
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(port);

    if (bind(s,(struct sockaddr*)&sin,sizeof(sin))==SOCKET_ERROR) {
        perror("bind");
        cleanup_socket(s);
        return INVALID_SOCKET;
    }
    if (listen(s,1)==SOCKET_ERROR) {
        perror("listen");
        cleanup_socket(s);
        return INVALID_SOCKET;
    }
    return s;
}

/**
 * \fn SOCKET TCP_Create_Client(const char* addr, short port)
 * \brief Crée un client TCP et se connecte à un serveur.
 * 
 * \param addr Adresse IP du serveur.
 * \param port Port du serveur.
 * \return Socket client valide ou INVALID_SOCKET en cas d'erreur.
 */
SOCKET TCP_Create_Client(const char* addr, short port) {
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        perror("socket");
        return INVALID_SOCKET;
    }

    struct sockaddr_in sin = {0};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    if (inet_pton(AF_INET, addr, &sin.sin_addr)<=0) {
        fprintf(stderr,"Adresse invalide\n");
        cleanup_socket(s);
        return INVALID_SOCKET;
    }
    if (connect(s,(struct sockaddr*)&sin,sizeof(sin))==SOCKET_ERROR) {
        perror("connect");
        cleanup_socket(s);
        return INVALID_SOCKET;
    }
    return s;
}

/**
 * \fn int TCP_Send_Message(SOCKET s, const char* move)
 * \brief Envoie un coup de 4 caractères sur une socket.
 * 
 * \param s Socket cible.
 * \param move Chaîne représentant le coup (4 caractères).
 * \return 4 si succès, -1 en cas d'erreur.
 */
int TCP_Send_Message(SOCKET s, const char* move) {
    if (!move) return -1;
    if (strlen(move) != 4) {
        fprintf(stderr, "Erreur: le coup doit avoir exactement 4 caractères\n");
        return -1;
    }

    time_t now;
    time(&now);
    char* time_str = ctime(&now);
    time_str[strlen(time_str)-1] = '\0'; // retirer le \n
    printf("[%s] ENVOI: %c%c%c%c\n", time_str, move[0], move[1], move[2], move[3]);

    int total = 0;
    while (total < 4) {
        int r = send(s, move + total, 4 - total, 0);
        if (r <= 0) {
            perror("send");
            return -1;
        }
        total += r;
    }
    return total;
}

/**
 * \fn gboolean gui_move_piece_callback(gpointer data)
 * \brief Callback GTK pour traiter un coup reçu et mettre à jour l'interface.
 * 
 * \param data Chaîne de 4 caractères représentant le coup.
 * \return G_SOURCE_REMOVE après traitement.
 */
gboolean gui_move_piece_callback(gpointer data) {
    char *msg = (char*)data;
    if (strlen(msg) != 4) {
        g_free(msg);
        return G_SOURCE_REMOVE;
    }

    int N = 9;

    int from_col = toupper(msg[0]) - 'A';
    int from_row = N - (msg[1] - '0');
    int to_col = toupper(msg[2]) - 'A';
    int to_row = N - (msg[3] - '0');

    int piece_idx = find_piece_at(from_row, from_col);
    if (piece_idx < 0) {
        printf("Coup reçu invalide: aucune pièce à (%d,%d)\n", from_row, from_col);
        goto invalid_move;
    }

    if (pieces[piece_idx].color != current_turn) {
        printf("Coup reçu invalide: pas le tour de la couleur %c\n", pieces[piece_idx].color);
        goto invalid_move;
    }

    if (!can_move(piece_idx, to_row, to_col)) {
        printf("Coup reçu invalide: déplacement interdit (%d,%d) -> (%d,%d)\n",
        from_row, from_col, to_row, to_col);
        goto invalid_move;
    }

    move_piece(piece_idx, to_row, to_col);
    gtk_widget_queue_draw(g_drawing_area);
    g_free(msg);
    return G_SOURCE_REMOVE;

    invalid_move:
    game_over = 1;
    set_victory_message(my_color == 'B', "L'adversaire a joué un coup invalide.");
    gtk_widget_queue_draw(g_drawing_area);

    if (g_socket != INVALID_SOCKET) {
        #ifdef _WIN32
            closesocket(g_socket);
        #else
            shutdown(g_socket, SHUT_RDWR);
            close(g_socket);
        #endif
        g_socket = INVALID_SOCKET;
    }

    g_free(msg);
    return G_SOURCE_REMOVE;
}

/**
 * \fn void* server_loop(void* arg)
 * \brief Boucle principale du serveur pour recevoir les coups du client.
 * 
 * \param arg Pointeur vers server_context_t.
 * \return NULL à la fin du thread.
 */
void* server_loop(void* arg) {
    server_context_t* ctx = (server_context_t*)arg;
    char buffer[BUFFER_SIZE];

    while (ctx->running) {
        int bytes_received = recv_move(ctx->s, buffer);
        if (bytes_received <= 0) {
            printf("[net] connexion fermée par le client\n");
            break;
        }

        time_t now;
        time(&now);
        char* time_str = ctime(&now);
        time_str[strlen(time_str)-1] = '\0';
        printf("[%s] CLIENT: %s\n", time_str, buffer);

        char *msg_copy = g_strdup(buffer);
        g_idle_add(gui_move_piece_callback, msg_copy);
    }

    ctx->running = 0;
    return NULL;
}

/**
 * \fn int run_server(short port, game_mode_t mode)
 * \brief Lance le serveur TCP et l'interface GUI.
 * 
 * \param port Port d'écoute.
 * \param mode Mode de jeu.
 * \return 0 si succès, 1 en cas d'erreur.
 */
int run_server(short port, game_mode_t mode) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
    fprintf(stderr, "WSAStartup failed\n");
    return 1;
    }
#endif

    printf("Mode serveur, port %d\n", port);
    SOCKET master = TCP_Create_Server(port);
    if (master == INVALID_SOCKET) return 1;

    printf("En attente d'une connexion...\n");
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    SOCKET s = accept(master, (struct sockaddr*)&client_addr, &client_len);
    cleanup_socket(master);

    if (s == INVALID_SOCKET) {
        fprintf(stderr, "Erreur accept\n");
        return 1;
    }

    printf("Client connecté depuis %s:%d\n",
           inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));
    g_socket = s;
    my_color = 'R';

    server_context_t ctx;
    ctx.s = s;
    ctx.mode = mode;
    ctx.running = 1;

    pthread_t net_thread;
    pthread_create(&net_thread, NULL, server_loop, &ctx);

    int gui_argc = 1;
    char *gui_argv[] = { "server_gui", NULL };
    start_gui(gui_argc, gui_argv, mode);

    ctx.running = 0;

#ifdef _WIN32
    closesocket(s);
#else
    shutdown(s, SHUT_RDWR);
#endif

    pthread_join(net_thread, NULL);

    cleanup_socket(s);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}

/**
 * \fn void* client_loop(void* arg)
 * \brief Boucle principale du client pour recevoir les coups du serveur.
 * 
 * \param arg Pointeur vers server_context_t.
 * \return NULL à la fin du thread.
 */
void* client_loop(void* arg) {
    server_context_t* ctx = (server_context_t*)arg;
    char buffer[BUFFER_SIZE];

    while (ctx->running) {
        int bytes_received = recv_move(ctx->s, buffer);
        if (bytes_received <= 0) {
            printf("[net] connexion fermée par le serveur\n");
            break;
        }

        time_t now;
        time(&now);
        char* time_str = ctime(&now);
        time_str[strlen(time_str)-1] = '\0';
        printf("[%s] SERVEUR: %s\n", time_str, buffer);

        char *msg_copy = g_strdup(buffer);
        g_idle_add(gui_move_piece_callback, msg_copy);
    }

    ctx->running = 0;
    return NULL;
}

/**
 * \fn int run_client(const char* addr, short port, game_mode_t mode)
 * \brief Lance le client TCP et l'interface GUI.
 * 
 * \param addr Adresse IP du serveur.
 * \param port Port du serveur.
 * \param mode Mode de jeu.
 * \return 0 si succès, 1 en cas d'erreur.
 */
int run_client(const char* addr, short port, game_mode_t mode) {
    #ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
            fprintf(stderr, "WSAStartup failed\n");
            return 1;
        }
    #endif

    printf("Mode client, connexion à %s:%d\n", addr, port);
    SOCKET s = TCP_Create_Client(addr, port);
    if (s == INVALID_SOCKET) {
        #ifdef _WIN32
            WSACleanup();
        #endif
        return 1;
    }
    printf("Connecté au serveur\n");

    g_socket = s;
    my_color = 'B';

    server_context_t ctx;
    ctx.s = s;
    ctx.mode = mode;
    ctx.running = 1;

    pthread_t net_thread;
    if (pthread_create(&net_thread, NULL, client_loop, &ctx) != 0) {
        perror("pthread_create");
        ctx.running = 0;
        cleanup_socket(s);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    int gui_argc = 1;
    char *gui_argv[] = { "client_gui", NULL };
    start_gui(gui_argc, gui_argv, mode);

    ctx.running = 0;

#ifdef _WIN32
    closesocket(s);
#else
    shutdown(s, SHUT_RDWR);
    close(s);
#endif
    g_socket = INVALID_SOCKET;

    pthread_join(net_thread, NULL);

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
