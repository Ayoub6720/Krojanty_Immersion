# Variables de Compilation
CC = gcc
CFLAGS = -Wall -g -Iinclude -MMD -O3 $(shell pkg-config --cflags gtk4) 				# -Iinclude pour indiquer le répertoire des fichiers .h
LIBS = $(shell pkg-config --libs gtk4)

ifeq ($(OS),Windows_NT)
  LIBS += -lws2_32
endif													# -MMD pour générer des fichiers .d de dépendance

TARGET = game
TEST_TARGET = tests_runner

# Répertoires

# Répertoire des sources
SRC_DIR   = src
# Répertoire pour les fichiers objets et dépendances	
BUILD_DIR = build
# Répertoire des tests
TEST_DIR  = tests
# Répertoire de la documentation
DOC_DIR   = docs

# Recherche des fichiers sources et génération des listes de fichiers objets et dépendances
SRCS = $(wildcard $(SRC_DIR)/*.c) # Tous les fichiers .c dans src/
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS = $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.o,$(TEST_SRCS))

# Remplacement src/ par build/ et .c par .o
DEPS  = $(OBJS:.o=.d) $(TEST_OBJS:.o=.d) # Liste des fichiers de dépendance .d


# Cible par défaut
all: $(TARGET)

# Cible pour créer l'exécutable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Cible pour créer l'exécutable de tests
$(TEST_TARGET): $(filter-out $(BUILD_DIR)/main.o,$(OBJS)) $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Inclusion des fichiers de dépendance
-include $(DEPS)

# Règle pour compiler les fichiers .c en fichiers .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Cible pour générer la documentation avec Doxygen
.PHONY: docs
docs:
	doxygen Doxyfile

# Cible pour exécuter les tests
.PHONY: test
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Cible pour nettoyer les fichiers générés
.PHONY: clean
clean:
	rm -f $(TARGET) $(TEST_TARGET) $(BUILD_DIR)/*.o $(BUILD_DIR)/*.d
	rm -rf $(DOC_DIR)/*
