#ifndef PRODUTOS_H
#define PRODUTOS_H

#include "tipos.h"

int  carregar_produtos(Contexto *ctx);
int  salvar_produtos(const Contexto *ctx);
int  indice_produto_por_id(const Contexto *ctx, int id);
int  proximo_id_produto(const Contexto *ctx);
int  produto_corresponde(const Produto *p, const Filtro *f);
void exibir_produto_resumo(const Produto *p);
int  listar_produtos_filtrados(const Contexto *ctx);
int  listar_valores_distintos(const Contexto *ctx, int campo,
                              char valores[][TAM_CATEGORIA], int max);

#endif
