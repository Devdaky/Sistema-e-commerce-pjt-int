#ifndef TELAS_H
#define TELAS_H

#include "tipos.h"

/* Cada tela desenha o menu, trata a escolha e RETORNA o proximo estado. */
Estado tela_home(Contexto *ctx);
Estado tela_catalogo(Contexto *ctx);
Estado tela_detalhes(Contexto *ctx);
Estado tela_carrinho(Contexto *ctx);
Estado tela_autenticacao(Contexto *ctx);
Estado tela_endereco_frete(Contexto *ctx);
Estado tela_pagamento(Contexto *ctx);
Estado tela_processando(Contexto *ctx);
Estado tela_confirmado(Contexto *ctx);
Estado tela_recusado(Contexto *ctx);
Estado tela_meus_pedidos(Contexto *ctx);
Estado tela_rastreamento(Contexto *ctx);

#endif
