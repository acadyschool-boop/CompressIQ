CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
LIB_DIR = lib
OBJ_DIR = obj
BIN_NAME = compression_tool  # On change 'test' en 'compression_tool' pour éviter les conflits

# Liste des fichiers objets pour la bibliothèque
LIB_OBJ = $(OBJ_DIR)/encodage.o $(OBJ_DIR)/LUT.o $(OBJ_DIR)/quadtree.o $(OBJ_DIR)/decodage.o
# Objet pour le programme principal
MAIN_OBJ = $(OBJ_DIR)/main.o

# Cible par défaut
all: setup $(LIB_DIR)/libcompression.a $(BIN_NAME)

# 1. Création de l'arborescence
setup:
	@mkdir -p $(OBJ_DIR) $(LIB_DIR)

# 2. Compilation des fichiers .c en .o (Règle générique)
$(OBJ_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# 3. Création de la bibliothèque statique
$(LIB_DIR)/libcompression.a: $(LIB_OBJ)
	ar rcs $@ $^

# 4. Compilation de l'exécutable final
$(BIN_NAME): $(MAIN_OBJ) $(LIB_DIR)/libcompression.a
	$(CC) $(MAIN_OBJ) -L$(LIB_DIR) -lcompression -o $(BIN_NAME)

# Nettoyage
clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_NAME)
	@echo "Nettoyage terminé."