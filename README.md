# IMM2526-GR2 — Guide d'installation, d'utilisation et documentation

Ce dépôt contient le jeu "Krojanty" (moteur de jeu, interface GTK, IA et support réseau).
Ce README décrit l'installation, la compilation, l'exécution, la génération de la documentation
et donne une description fichier-par-fichier pour alimenter Doxygen.

---

## 1) Pré-requis (Debian/Ubuntu)

Installer les outils nécessaires :

```bash
sudo apt update
sudo apt install build-essential pkg-config libgtk-4-dev doxygen graphviz
```

Si une autre distribution est utilisée, installer `gcc`, `pkg-config`, `libgtk-4-dev`, `doxygen` et `graphviz` via le gestionnaire de paquets.

## 2) Compilation

Pour compiler le code en un fichier exécutable, lancer la commande suivante depuis la racine du projet :

```bash
make
```

## 3) Exécution

### Mode Local
Pour jouer en local (sur la même machine) :
```bash
# Deux joueurs humains
./game -l
```

### Mode Réseau
Pour jouer via le réseau, il faut un serveur et un client :
```bash
# Lancer un serveur (humain)
./game -s [port]

# Lancer un client (humain, le premier joueur)
./game -c [adresse]:[port]
```

### Mode Réseau avec IA
Il est possible d'activer une IA soit côté serveur, soit côté client :
```bash
# Serveur avec IA contre client humain
./game -s -ia [port]
./game -c [adresse]:[port]

# Serveur humain contre client IA
./game -s [port]
./game -c -ia [adresse]:[port]

# IA contre IA en réseau
./game -s -ia [port]
./game -c -ia [adresse]:[port]
```

Pour plus de détails sur les options :
```bash
./game --help
```

## 4) Tests

Depuis la racine du projet :

```
make test
```

Cela construit et exécute le binaire de tests.

## 5) Générer la documentation Doxygen

1. Installer `doxygen` et `graphviz` (si ce n'est pas déjà fait).
2. Lancer la génération depuis la racine :

```bash
make docs
```

La documentation HTML est produite dans `docs/doxygen/html/index.html`.

---

## 6) Jeu du Krojanty

### Présentation
Le Krojanty est un jeu de plateau pour 2 joueurs qui se déroule sur un damier 9x9.
Chaque joueur contrôle une armée de 10 pièces composée d'un roi et de 9 soldats.
Le but est de capturer soit le roi adverse, soit toutes ses pièces.

### Règles du jeu
- Le plateau fait 9x9 cases
- Chaque joueur a 10 pièces : 1 roi + 9 soldats 
- Les pièces sont placées sur les 2 premières rangées
- Déplacement en ligne droite (comme une tour aux échecs)
- Pas de saut par-dessus les autres pièces
- Deux types de captures : Linca (orthogonale) et Seltou (diagonale)
- Victoire par capture du roi ou de toutes les pièces adverses
- Match nul après 64 tours sans capture

### Plateau et mise en place
- Plateau de 9x9 cases avec cases jaunes pour meilleure visualisation
- 2 joueurs (Bleu commence)
- Chaque joueur dispose de :
  - 1 roi (♚ pour bleu, ♔ pour rouge)
  - 9 soldats (♜ pour bleu, ♖ pour rouge)
- Chaque camp possède une cité (case spéciale) dans son coin du plateau

### Déplacements
- Les pièces (roi et soldats) se déplacent en ligne droite (verticale ou horizontale)
- Distance de déplacement illimitée
- Interdictions :
  - Sauter par-dessus une autre pièce
  - Se poser sur une case déjà occupée
- Note : Seul le roi peut conquérir la cité adverse

### Captures
Règles générales :
- La capture n'est possible qu'après un mouvement (pas de capture passive)
- La pièce capturée disparaît et sa case devient neutre

#### Capture Linca (prise en sandwich)
- Une pièce est prise entre deux ennemis alignés
- Permet des captures doubles

#### Capture Seultou (prise en poussant)
- Une pièce est repoussée contre un bord ou une pièce ennemie
- La pièce repoussée ne doit pas avoir de soutien derrière elle
- Permet des captures multiples

*Note : Certaines situations permettent une triple capture (Linca + Seultou combinés)*

### Conditions de victoire
Un joueur gagne par :
- Conquête : amener son roi dans la cité adverse
- Extermination : capturer 8 soldats adverses
- Exécution : capturer le roi adverse

### Contrôle du plateau
- Au départ : 10 cases contrôlées par joueur
- Une troupe contrôle une case en se déplaçant dessus
- Les cases occupées restent contrôlées après départ de l'unité
- Les cités ne comptent pas dans le contrôle
- Une case libre peut être reprise par l'adversaire

### Fin de partie (limite de 64 tours)
Si la partie atteint 64 tours (32 par joueur), le score final est calculé :
- +1 point par case contrôlée
- +1 point par soldat encore en vie (roi exclu)
- Le plus haut score gagne
- En cas d'égalité parfaite : match nul



## 7) Architecture et rôle des fichiers

Voici une liste détaillée des fichiers importants et de leur rôle :

### include/

- `app.h` — Déclarations globales partagées (structure `Piece`, tableaux d'état, scores, logos).
  - Contient les variables globales définies dans `src/game.c`.

- `args.h` — Structures et prototypes pour le parsing d'arguments (`args_t`, `parse_args`, `print_usage`).

- `captures.h` — Prototypes des règles de capture : `check_linca_capture`, `check_seltou_capture`, `check_auto_defeat`.

- `drawing.h` — Callbacks et helpers de rendu (`draw_cb`, `click_to_cell`).

- `game.h` — API principale du jeu : initialisation (`game_setup_default`), mouvements (`move_piece`), gestion IA (`trigger_ia_move`), utilitaires (`find_piece_at`, `snapshot_initial_state`, ...).

- `gui.h` — Entrée GUI : `start_gui` et widgets exportés (ex: `g_drawing_area`).

- `ia.h` — Structures IA (`GameState`, `Move`) et fonctions publiques (`trouverMeilleurCoupIA`, `minimaxIA`, `evaluation`).

- `net.h` — API réseau : `run_server`, `run_client`, `TCP_Send_Message`, socket global `g_socket`.

- `status.h` — Gestion des messages / labels : `status_register_labels`, `set_victory_message`, `refresh_game_status`.

### src/

- `main.c` — Point d'entrée : parse les arguments, choisit le mode (local, server, client), configure l'IA et démarre la GUI ou le réseau.

- `args.c` — Implémentation du parsing d'arguments (option `--help`, host:port parsing, validations).

- `game.c` — Moteur de jeu : initialisation des pièces, logique de tours, application des règles, gestion des scores, snapshot/restore.

- `ai.c` — IA : minimax avec alpha-bêta, génération de coups, évaluations (documenter complexité et paramètres comme `ia_search_depth`).

- `gui.c` — GTK : création de la fenêtre, gestion des événements (clics), integration avec `game` et `status`.

- `drawing.c` — Dessin du plateau (Cairo) : dessin des cases, pions, surbrillance et conversion clic→case.

- `capture.c` — Logique des captures (Linca, Seltou) et effets sur l'état et le score.

- `net.c` — Implémentation serveur/client (sockets), lecture et envoi de messages, thread réseau si nécessaire.

- `status.c` — Mise à jour des labels GTK, messages formatés pour victoire/nul et rafraîchissement.

### tests/

- `Test*.c` — tests unitaires par module : vérifier la logique IA, règles de capture, parsing des arg, etc.

---

