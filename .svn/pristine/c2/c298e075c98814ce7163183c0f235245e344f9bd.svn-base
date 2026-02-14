/**
 * \file game.c
 * \brief Gestion principale du jeu Krojanty : état, déplacements, captures et IA.
 * \author loic.claude@uha.fr, nolann.wickers@uha.fr, valentin.prevot@uha.fr, valentin.leray@uha.fr, alan.akgun@uha.fr
 *         ayoub.darkaoui@uha.fr, ismail.el-maallem@uha.fr, corentin.banocay@uha.fr
 * \version 0.1
 * \date 2025-09-16
 *
 * \details
 * Ce fichier implémente la logique principale du jeu :
 * - Initialisation des pièces et de la grille.
 * - Gestion des tours, déplacements et captures.
 * - Calcul des scores et vérification des conditions de victoire.
 * - Intégration avec l'IA et le mode réseau.
 */

#include <string.h>
#include "game.h"
#include "captures.h"
#include "gui.h"
#include "status.h"
#include "net.h"


// ====== ÉTAT GLOBAL ======
/** Tableau des pièces du jeu */
Piece pieces[20];

/** Nombre de pièces */
int   piece_count = 0;

/** Grille de contrôle des cases : 0 = libre, 1 = bleue, 2 = rouge */
int   cell_control[9][9]    = {{0}};

/** Grille de surlignage des mouvements possibles */
int   highlight_moves[9][9] = {{0}};

/** Index de la pièce sélectionnée (-1 si aucune) */
int   selected_piece = -1;

/** Couleur du joueur courant ('B' ou 'R') */
char  current_turn   = 'B';

/** Numéro du tour courant */
int   turn_number    = 1;

/** Indique si la partie est terminée */
int   game_over      = 0;

/** Indique si l'IA est active */
int   ia_active = 0;

/** Couleur jouée par l'IA */
char  ia_color = 'R';

/** Indique si les deux IA jouent */
int   ia_both_active = 0;

/** Profondeur de recherche de l'IA */
int   ia_search_depth = 3;

/** Score des équipes */
int   score_blue = 0;
int   score_red  = 0;

/** Nombre de pièces restantes par équipe */
int   pieces_blue = 0;
int   pieces_red  = 0;

/** Nombre maximal de tours */
short max_turn = 64;

/**
 * \brief Définit la position initiale des pièces.
 * 
 * \param arr Tableau de pièces
 * \param count Nombre de pièces dans le tableau
 */
void game_set_position(const Piece *arr, int count) {
    if (!arr || count < 0 || count > 20) return;
    piece_count = count;
    for (int i = 0; i < count; ++i) pieces[i] = arr[i];
}

/**
 * \fn void game_setup_default(void)
 * \brief Initialise la position par défaut des pièces.
 */
void game_setup_default(void) {
    piece_count = 20;

    
    Piece start[] = {
    // --- BLEU ---
    {1, 1, 'B', 'K'}, 
    {2, 0, 'B', 'P'}, 
    {3, 0, 'B', 'P'}, 
    {2, 1, 'B', 'P'}, 
    {3, 1, 'B', 'P'}, 
    {0, 2, 'B', 'P'}, 
    {1, 2, 'B', 'P'}, 
    {2, 2, 'B', 'P'}, 
    {0, 3, 'B', 'P'}, 
    {1, 3, 'B', 'P'}, 

    // --- ROUGE ---
    {7, 7, 'R', 'K'}, 
    {7, 5, 'R', 'P'}, 
    {8, 5, 'R', 'P'}, 
    {6, 6, 'R', 'P'}, 
    {7, 6, 'R', 'P'}, 
    {8, 6, 'R', 'P'}, 
    {5, 7, 'R', 'P'}, 
    {6, 7, 'R', 'P'}, 
    {5, 8, 'R', 'P'}, 
    {6, 8, 'R', 'P'}, 
};

    initialize_controlled_cells();
    update_scores();
    game_set_position(start, (int)(sizeof(start)/sizeof(start[0])));
    reset_move_history(); // Réinitialiser l'historique IA
}

/**
 * \fn int find_piece_at(int row, int col)
 * \brief Déclenche l'IA pour jouer son coup.
 * 
 * \param user_data Données utilisateur (non utilisées)
 * \return G_SOURCE_REMOVE pour arrêter le timeout
 */
gboolean trigger_ia_move(gpointer user_data) {
    (void)user_data; // Supprime le warning unused parameter
    
    if (!(ia_active || ia_both_active) || game_over) return G_SOURCE_REMOVE;
    if (!ia_both_active && current_turn != ia_color) return G_SOURCE_REMOVE;
    
    printf("L'IA reflechit...\n");
    
    // Créer l'état de jeu pour l'IA
    GameState state = createGameStateFromCurrent();
    
    // Trouver le meilleur coup
    Move best_move;
    best_move.piece_index = -1; // Initialiser
    
    trouverMeilleurCoupIA(&state, &best_move, ia_search_depth);
    
    if (best_move.piece_index >= 0 && best_move.piece_index < piece_count) {
        
        
        // Éviter la récursion en désactivant temporairement l'IA
        int temp_ia = ia_active;
        ia_active = 0;
        
        // Jouer le coup
        if (move_piece(best_move.piece_index, best_move.to_row, best_move.to_col)) {
            // Si en réseau, envoyer le coup joué par l'IA
            if (g_socket != INVALID_SOCKET) {
                char msg[5];
                int N = 9;
                msg[0] = 'A' + best_move.from_col;
                msg[1] = '0' + (N - best_move.from_row);
                msg[2] = 'A' + best_move.to_col;
                msg[3] = '0' + (N - best_move.to_row);
                msg[4] = '\0';
                TCP_Send_Message(g_socket, msg);
            }
            if (g_drawing_area) gtk_widget_queue_draw(g_drawing_area);
        }
        
        // Réactiver l'IA
        ia_active = temp_ia;
    } else {
        printf("L'IA n'a pas trouvé de coup valide\n");
    }
    
    return G_SOURCE_REMOVE;
}

static Piece initial_pieces[128];
static int   initial_piece_count = 20;

/**
 * \fn void snapshot_initial_state(void)
 * \brief Sauvegarde l'état initial du plateau pour permettre un reset.
 */
void snapshot_initial_state(void) {
    initial_piece_count = piece_count;
    for (int i = 0; i < piece_count; ++i) initial_pieces[i] = pieces[i];
}

/**
 * \fn void restore_initial_state(void)
 * \brief Restaure l'état initial du plateau.
 */
void restore_initial_state(void) {
    piece_count = initial_piece_count;
    for (int i = 0; i < piece_count; ++i) pieces[i] = initial_pieces[i];

    selected_piece = -1;
    current_turn   = 'B';
    turn_number    = 1;
    game_over      = 0;

    clear_highlight();
    initialize_controlled_cells();
    update_scores();
    reset_move_history(); // Réinitialiser l'historique IA
}

/**
 * \fn void initialize_controlled_cells(void)
 * \brief Initialise les cellules contrôlées par chaque équipe.
 */
void initialize_controlled_cells(void) {
    for (int r = 0; r < 9; ++r)
        for (int c = 0; c < 9; ++c)
            cell_control[r][c] = 0;

    for (int i = 0; i < piece_count; ++i) {
        if (pieces[i].color == 'B')
            cell_control[pieces[i].row][pieces[i].col] = 1;
        else if (pieces[i].color == 'R')
            cell_control[pieces[i].row][pieces[i].col] = 2;
    }
}

/**
 * \fn void update_scores(void)
 * \brief Met à jour les scores des deux équipes.
 */
void update_scores(void) {
    pieces_blue = 0;
    pieces_red = 0;
    int controlled_blue = 0, controlled_red = 0;

    for (int i = 0; i < piece_count; ++i) {
        if (pieces[i].color == 'B') pieces_blue++;
        else if (pieces[i].color == 'R') pieces_red++;
    }
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (cell_control[r][c] == 1) controlled_blue++;
            else if (cell_control[r][c] == 2) controlled_red++;
        }
    }

    score_blue = pieces_blue + controlled_blue - 1;
    score_red  = pieces_red  + controlled_red  - 1;

    status_on_scores_changed();
}

/**
 * \fn void mark_cell_controlled(int row, int col, char color)
 * \brief Marque une cellule comme contrôlée par une équipe.
 * 
 * \param row Ligne de la cellule (0-8)
 * \param col Colonne de la cellule (0-8)
 * \param color Couleur de l'équipe ('B' ou 'R')
 */
void mark_cell_controlled(int row, int col, char color) {
    if (row < 0 || row > 8 || col < 0 || col > 8) return;
    if ((row == 0 && col == 0) || (row == 8 && col == 8)) return;
    cell_control[row][col] = (color == 'B') ? 1 : 2;
}

/**
 * \fn int is_cell_free(int row, int col)
 * \brief Vérifie si une cellule est libre (non occupée par une pièce).
 * 
 * \param row Ligne de la cellule (0-8)
 * \param col Colonne de la cellule (0-8)
 * \return 1 si la cellule est libre, 0 sinon
 */
int is_cell_free(int row, int col) {
    for (int i = 0; i < piece_count; ++i)
        if (pieces[i].row == row && pieces[i].col == col)
            return 0;
    return 1;
}

/**
 * \fn int can_move(int index, int dest_row, int dest_col)
 * \brief Vérifie si une pièce peut se déplacer vers une cellule donnée.
 * 
 * \param index Index de la pièce dans le tableau `pieces`
 * \param dest_row Ligne de destination (0-8)
 * \param dest_col Colonne de destination (0-8)
 * \return 1 si le déplacement est possible, 0 sinon
 */
int can_move(int index, int dest_row, int dest_col) {
    int sr = pieces[index].row;
    int sc = pieces[index].col;

    if (sr != dest_row && sc != dest_col) return 0; // pas diagonal

    if (sr == dest_row) {
        int dir = (dest_col > sc) ? 1 : -1;
        for (int c = sc + dir; c != dest_col; c += dir)
            if (!is_cell_free(sr, c)) return 0;
    } else {
        int dir = (dest_row > sr) ? 1 : -1;
        for (int r = sr + dir; r != dest_row; r += dir)
            if (!is_cell_free(r, sc)) return 0;
    }
    return is_cell_free(dest_row, dest_col);
}

/**
 * \fn void clear_highlight(void)
 * \brief Efface le surlignage des mouvements possibles.
 */
void clear_highlight(void) {
    for (int r = 0; r < 9; ++r)
        for (int c = 0; c < 9; ++c)
            highlight_moves[r][c] = 0;
}

/**
 * \fn void compute_highlight_for_selected(void)
 * \brief Calcule les mouvements possibles pour la pièce sélectionnée et met à jour le surlignage.
 */
void compute_highlight_for_selected(void) {
    clear_highlight();
    if (selected_piece < 0) return;

    int r = pieces[selected_piece].row;
    int c = pieces[selected_piece].col;

    for (int cc = c + 1; cc < 9; ++cc) { if (!is_cell_free(r, cc)) break; highlight_moves[r][cc] = 1; }
    for (int cc = c - 1; cc >= 0; --cc){ if (!is_cell_free(r, cc)) break; highlight_moves[r][cc] = 1; }
    for (int rr = r + 1; rr < 9; ++rr) { if (!is_cell_free(rr, c)) break; highlight_moves[rr][c] = 1; }
    for (int rr = r - 1; rr >= 0; --rr){ if (!is_cell_free(rr, c)) break; highlight_moves[rr][c] = 1; }
}


/**
 * \brief Retourne l'index de la pièce à une position donnée.
 * \param row Ligne
 * \param col Colonne
 * \return Index de la pièce ou -1 si aucune
 */
int find_piece_at(int row, int col) {
    for (int i = 0; i < piece_count; ++i)
        if (pieces[i].row == row && pieces[i].col == col)
            return i;
    return -1;
}

/**
 * \fn void check_turn_limit(void)
 * \brief Vérifie si la limite de tours est atteinte et termine la partie si nécessaire.
 */
static void check_turn_limit(void) {
    if (game_over) return;
    if (turn_number >= max_turn) {
        update_scores();
        if (score_blue > score_red) {
            char buf[128];
            snprintf(buf, sizeof(buf), "Limite de %d tours atteinte — score des bleus supérieur (%d vs %d).",max_turn, score_blue, score_red);
            set_victory_message(TRUE, buf);
        } else if (score_red > score_blue) {
            char buf[128];
            snprintf(buf, sizeof(buf), "Limite de %d tours atteinte — score des rouges supérieur (%d vs %d).",max_turn, score_red, score_blue);
            set_victory_message(FALSE, buf);
        } else {
            char buf[128];
            snprintf(buf, sizeof(buf), "Limite de 64 tours atteinte — scores égaux (%d chacun).", score_blue);
            set_draw_message(buf);
        }
        game_over = 1;
        clear_highlight();
        selected_piece = -1;
        if (g_drawing_area) gtk_widget_queue_draw(g_drawing_area);
    }
}

/**
 * \fn int move_piece(int piece_idx, int to_row, int to_col)
 * \brief Déplace une pièce si le mouvement est valide, gère les captures et met à jour l'état du jeu.
 * 
 * \param piece_idx Index de la pièce à déplacer.
 * \param to_row Ligne de destination.
 * \param to_col Colonne de destination.
 * \return 1 si le déplacement a réussi, 0 sinon.
 */
int move_piece(int piece_idx, int to_row, int to_col) {
    if (piece_idx < 0 || piece_idx >= piece_count) return 0;

    Piece *p = &pieces[piece_idx];

    // Vérifie si le déplacement est autorisé
    if (!can_move(piece_idx, to_row, to_col)) return 0;

    // Marque l'ancienne case
    mark_cell_controlled(p->row, p->col, p->color);

    int old_row = p->row;
    int old_col = p->col;

    // Déplace la pièce
    p->row = to_row;
    p->col = to_col;

    printf("Tour %d: Deplacement de %c en %c%d\n",turn_number, p->color, 'A'+to_col, 9-to_row);

    mark_cell_controlled(to_row, to_col, p->color);

    // Vérifie captures (Linca peut supprimer une pièce avant l'index de la pièce déplacée,
    // ce qui décale le tableau et invalide l'index et le pointeur 'p').
    check_linca_capture(piece_idx);
    if (game_over) { gtk_widget_queue_draw(g_drawing_area); return 1; }

    // Ré-identifie la pièce déplacée après d'éventuels décalages du tableau
    int new_idx = find_piece_at(to_row, to_col);
    if (new_idx >= 0) {
        piece_idx = new_idx;
        p = &pieces[piece_idx];
    }

    check_seltou_capture(piece_idx, old_row, old_col);
    if (game_over) { gtk_widget_queue_draw(g_drawing_area); return 1; }

    // Vérifie conditions de victoire spéciale
    if ((p->type == 'K' || p->type == 'R') &&
        ((p->color == 'R' && p->row == 0 && p->col == 0) ||
         (p->color == 'B' && p->row == 8 && p->col == 8))) {

        set_victory_message(p->color == 'B',
            (p->color == 'R')
            ? "Le roi rouge a atteint la base bleue."
            : "Le roi bleu a atteint la base rouge.");
        update_scores();
        clear_highlight();
        selected_piece = -1;
        game_over = 1;
        gtk_widget_queue_draw(g_drawing_area);
        return 1;
    }

    update_scores();
    check_auto_defeat();
    if (game_over) { 
        if (g_drawing_area) gtk_widget_queue_draw(g_drawing_area); 
        return 1; 
    }

    clear_highlight();
    current_turn = (current_turn == 'B') ? 'R' : 'B';
    selected_piece = -1;
    ++turn_number;

    check_turn_limit();
    if (game_over) return 1;

    if (g_drawing_area) gtk_widget_queue_draw(g_drawing_area);
    refresh_game_status();
    
    // Déclencher l'IA si c'est son tour (ou si IA vs IA active)
    if (!game_over && (ia_both_active || (ia_active && current_turn == ia_color))) {
        g_timeout_add(800, trigger_ia_move, NULL); // Délai de 800ms
    }
    
    return 1;
}
