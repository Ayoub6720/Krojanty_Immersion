#ifndef ARGS_H
#define ARGS_H

/**
 * @file args.h
 * @brief Parsing et structure des arguments de la ligne de commande.
 * @author Groupe 2
 * @version 1.0
 * @date 2025-09-18
 *
 * Ce module gère la configuration du jeu via les options de ligne de commande.
 * Les différents modes de lancement sont :
 *
 * Mode local :
 * ```bash
 * ./game              # 2 joueurs sur la même machine
 * ./game --ia         # Joueur contre IA
 * ./game --ia-both    # IA contre IA
 * ```
 *
 * Mode réseau serveur :
 * ```bash
 * ./game --server 12345       # Serveur sur port 12345
 * ./game --server 12345 --ia  # Serveur avec IA comme adversaire
 * ```
 *
 * Mode réseau client :
 * ```bash
 * ./game --client localhost:12345     # Connexion à un serveur local
 * ./game --client 192.168.1.10:12345  # Connexion à une IP spécifique
 * ```
 *
 * Aide et diagnostic :
 * ```bash
 * ./game --help     # Affiche l'aide
 * ```
 *
 * @see app.h pour la description générale des modes de jeu
 * @see net.h pour les détails du mode réseau
 * @see ia.h pour les options de l'IA
 */

/**
 * @brief Mode de jeu sélectionné via les arguments
 * 
 * Cette énumération définit les différents modes de jeu possibles,
 * déterminant comment le jeu va s'initialiser et fonctionner.
 * 
 * @var game_mode_t::MODE_LOCAL
 * Mode de jeu local (sur la même machine)
 * - Deux joueurs sur le même ordinateur
 * - Un joueur contre l'IA
 * - Deux IA l'une contre l'autre
 * 
 * @var game_mode_t::MODE_SERVER
 * Mode serveur pour le jeu en réseau
 * - Attend la connexion d'un client
 * - Gère la synchronisation du jeu
 * - Valide les mouvements
 * 
 * @var game_mode_t::MODE_CLIENT
 * Mode client pour le jeu en réseau
 * - Se connecte à un serveur existant
 * - Envoie les mouvements au serveur
 * - Reçoit les mises à jour de l'état
 * 
 * @var game_mode_t::MODE_NONE
 * Aucun mode sélectionné (état par défaut)
 */
typedef enum {
    MODE_LOCAL,  /**< Jeu en local */
    MODE_SERVER, /**< Mode serveur réseau */
    MODE_CLIENT, /**< Mode client réseau */
    MODE_NONE    /**< Mode non défini */
} game_mode_t;

/**
 * @brief Structure de configuration du jeu Krojanty
 * 
 * Structure qui encapsule tous les paramètres de configuration du jeu,
 * qu'ils soient passés en ligne de commande ou définis par défaut.
 * Elle détermine comment le jeu va s'initialiser et fonctionner,
 * notamment le mode de jeu (local/réseau), les options réseau, et
 * les paramètres de l'IA.
 *
 * Exemple d'utilisation :
 * @code
 * args_t args;
 * if (parse_args(argc, argv, &args) != 0) {
 *     print_usage(argv[0]);
 *     return 1;
 * }
 * // Utiliser args.mode, args.port, etc.
 * @endcode
 *
 * @var args_t::mode
 * Mode de jeu choisi (local, serveur, client)
 *
 * @var args_t::port
 * Port réseau pour les modes serveur/client (défaut: 12345)
 *
 * @var args_t::address
 * Adresse IP du serveur en mode client
 * 
 * @var args_t::ai_enabled
 * Active/désactive l'IA en mode local
 *
 * @var args_t::ai_difficulty
 * Niveau de difficulté de l'IA (1-5)
 *
 * @see game_mode_t
 * @see parse_args()
 * @see print_usage()
 * les options IA, et les paramètres réseau.
 *
 * Exemple d'utilisation :
 * @code
 * int main(int argc, char **argv) {
 *     args_t args = parse_args(argc, argv);
 *     if (args.error) {
 *         // Gestion d'erreur
 *     }
 *     // Configuration du jeu selon args
 *     free_args(&args);
 * }
 * @endcode
 *
 * @var args_t::mode
 * Mode de jeu principal :
 * - MODE_LOCAL : Jeu sur la même machine
 * - MODE_SERVER : Mode serveur pour jeu en réseau
 * - MODE_CLIENT : Mode client pour jeu en réseau
 * - MODE_NONE : Mode non défini (erreur)
 *
 * @var args_t::is_ia
 * Configuration de l'IA :
 * - 0 : Mode joueur contre joueur
 * - 1 : Mode joueur contre IA
 * 
 * @var args_t::is_ia_both
 * Mode double IA :
 * - 0 : Au moins un joueur humain
 * - 1 : Deux IA s'affrontent
 * 
 * @var args_t::host
 * Configuration réseau client :
 * - Chaîne : Adresse du serveur (ex: "localhost")
 * - NULL : Mode local ou serveur
 * @note Alloué dynamiquement, libérer avec free_args()
 * 
 * @var args_t::port
 * Port pour connexion réseau :
 * - 0 : Port non spécifié
 * - 1024-65535 : Port valide pour client/serveur
 * 
 * @var args_t::help
 * Demande d'aide :
 * - 0 : Exécution normale
 * - 1 : Afficher l'aide et quitter
 * 
 * @var args_t::error
 * État de l'analyse :
 * - 0 : Arguments valides
 * - 1 : Erreur dans les arguments
 * 
 * @see game_mode_t
 * @see parse_args()
 * @see free_args()
 * @warning Le champ host doit être libéré via free_args()
 */
/**
 * @struct args_t
 * @brief Structure principale de configuration du jeu
 *
 * Cette structure encapsule toute la configuration du jeu Krojanty,
 * qu'elle soit fournie par ligne de commande ou par défaut. Elle définit :
 * - Le mode de jeu (local, serveur, client)
 * - Les paramètres réseau (hôte, port)
 * - Les options de jeu (IA, difficulté)
 * - Les drapeaux de contrôle (erreur, aide)
 *
 * C'est la structure fondamentale qui détermine comment le jeu va
 * s'initialiser et fonctionner. Elle est créée par parse_args() et
 * doit être libérée avec free_args().
 */
typedef struct {
    game_mode_t mode; /**< Mode de jeu (local/serveur/client) */
    int is_ia;        /**< Active le mode contre IA */
    int is_ia_both;   /**< Active le mode IA vs IA */
    char *host;       /**< Adresse du serveur pour client */
    int port;         /**< Port pour connexion réseau */
    int help;         /**< Demande d'affichage de l'aide */
    int error;        /**< Indicateur d'erreur de parsing */
} args_t;

/**
 * @brief Analyse les arguments de la ligne de commande.
 * @param argc nombre d'arguments
 * @param argv tableau d'arguments
 * @return structure `args_t` remplie. `free_args` doit être appelée
 *         pour libérer les champs alloués (ex: `host`).
 */
args_t parse_args(int argc, char **argv);

/**
 * @brief Affiche l'usage et les options disponibles sur stdout.
 * @param program_name nom du programme (argv[0])
 */
void print_usage(const char *program_name);

/**
 * @brief Libère les champs alloués d'une structure args_t.
 * @param args pointeur vers la structure à libérer/initialiser.
 */
void free_args(args_t *args);

#endif
