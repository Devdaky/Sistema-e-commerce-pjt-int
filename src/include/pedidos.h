#ifndef PEDIDOS_H
#define PEDIDOS_H

#include "tipos.h"

int         carregar_pedidos(Contexto *ctx);
int         salvar_pedidos(const Contexto *ctx);
int         indice_pedido_por_id(const Contexto *ctx, int id);
int         proximo_id_pedido(const Contexto *ctx);
int         criar_pedido(Contexto *ctx);
void        gerar_codigo_rastreio(Pedido *p);
const char *nome_forma_pagamento(int forma);
const char *nome_status(int status);
void        exibir_pedido_detalhe(const Pedido *p);

#endif
