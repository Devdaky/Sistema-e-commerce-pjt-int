# Makefile - VitaMarket (Projeto Integrador)
CC       = gcc
CFLAGS   = -Wall -Wextra -g -I./src/include -MMD -MP
SRC_DIR  = src
OBJ_DIR  = obj
BIN_DIR  = bin
TARGET   = $(BIN_DIR)/vitamarket

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(OBJS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

run: all
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# remove tambem os dados salvos (produtos, usuarios e pedidos)
reset: clean
	rm -f produtos.txt usuarios.txt pedidos.txt

-include $(DEPS)

.PHONY: all run clean reset
