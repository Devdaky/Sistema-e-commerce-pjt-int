#ifndef CARRINHO_H
#define CARRINHO_H

#include "tipos.h"

enum { CARRINHO_OK = 0, CARRINHO_SEM_PRODUTO, CARRINHO_SEM_ESTOQUE, CARRINHO_CHEIO };

int    adicionar_ao_carrinho(Contexto *ctx, int id_produto, int qtd);
int    alterar_quantidade_item(Contexto *ctx, int pos, int nova_qtd);
void   remover_item_carrinho(Contexto *ctx, int pos);
void   remover_produto_do_carrinho(Contexto *ctx, int id_produto);
void   esvaziar_carrinho(Contexto *ctx);
double calcular_subtotal(const Contexto *ctx);
int    exibir_carrinho(const Contexto *ctx);
int    validar_estoque_carrinho(const Contexto *ctx);

#endif
