#include <stdio.h>
#include "carrinho.h"
#include "produtos.h"

int adicionar_ao_carrinho(Contexto *ctx, int id_produto, int qtd)
{
    int i, idx = indice_produto_por_id(ctx, id_produto);

    if (idx < 0) {
        return CARRINHO_SEM_PRODUTO;
    }
    for (i = 0; i < ctx->qtd_carrinho; i++) {
        if (ctx->carrinho[i].id_produto == id_produto) {
            if (ctx->carrinho[i].quantidade + qtd > ctx->produtos[idx].estoque) {
                return CARRINHO_SEM_ESTOQUE;
            }
            ctx->carrinho[i].quantidade += qtd;
            return CARRINHO_OK;
        }
    }
    if (ctx->qtd_carrinho >= MAX_ITENS_CARRINHO) {
        return CARRINHO_CHEIO;
    }
    if (qtd > ctx->produtos[idx].estoque) {
        return CARRINHO_SEM_ESTOQUE;
    }
    ctx->carrinho[ctx->qtd_carrinho].id_produto = id_produto;
    ctx->carrinho[ctx->qtd_carrinho].quantidade = qtd;
    ctx->qtd_carrinho++;
    return CARRINHO_OK;
}

int alterar_quantidade_item(Contexto *ctx, int pos, int nova_qtd)
{
    int idx;
    if (pos < 0 || pos >= ctx->qtd_carrinho) {
        return CARRINHO_SEM_PRODUTO;
    }
    idx = indice_produto_por_id(ctx, ctx->carrinho[pos].id_produto);
    if (idx < 0) {
        return CARRINHO_SEM_PRODUTO;
    }
    if (nova_qtd > ctx->produtos[idx].estoque) {
        return CARRINHO_SEM_ESTOQUE;
    }
    ctx->carrinho[pos].quantidade = nova_qtd;
    return CARRINHO_OK;
}

void remover_item_carrinho(Contexto *ctx, int pos)
{
    int i;
    if (pos < 0 || pos >= ctx->qtd_carrinho) {
        return;
    }
    for (i = pos; i < ctx->qtd_carrinho - 1; i++) {
        ctx->carrinho[i] = ctx->carrinho[i + 1];
    }
    ctx->qtd_carrinho--;
}

void remover_produto_do_carrinho(Contexto *ctx, int id_produto)
{
    int i;
    for (i = 0; i < ctx->qtd_carrinho; i++) {
        if (ctx->carrinho[i].id_produto == id_produto) {
            remover_item_carrinho(ctx, i);
            return;
        }
    }
}

void esvaziar_carrinho(Contexto *ctx)
{
    ctx->qtd_carrinho = 0;
}

double calcular_subtotal(const Contexto *ctx)
{
    int i, idx;
    double total = 0.0;
    for (i = 0; i < ctx->qtd_carrinho; i++) {
        idx = indice_produto_por_id(ctx, ctx->carrinho[i].id_produto);
        if (idx >= 0) {
            total += ctx->produtos[idx].preco * ctx->carrinho[i].quantidade;
        }
    }
    return total;
}

/* Mostra foto (placeholder), titulo, quantidade, preco e subtotal por item */
int exibir_carrinho(const Contexto *ctx)
{
    int i, idx;
    if (ctx->qtd_carrinho == 0) {
        printf("\n  Seu carrinho esta vazio.\n");
        return 0;
    }
    printf("\n");
    for (i = 0; i < ctx->qtd_carrinho; i++) {
        idx = indice_produto_por_id(ctx, ctx->carrinho[i].id_produto);
        if (idx < 0) {
            continue;
        }
        printf("  %d) [foto] %s\n", i + 1, ctx->produtos[idx].nome);
        printf("     %d x R$ %.2f = R$ %.2f\n", ctx->carrinho[i].quantidade,
               ctx->produtos[idx].preco,
               ctx->produtos[idx].preco * ctx->carrinho[i].quantidade);
    }
    printf("\n  TOTAL DOS PRODUTOS: R$ %.2f\n", calcular_subtotal(ctx));
    return ctx->qtd_carrinho;
}

int validar_estoque_carrinho(const Contexto *ctx)
{
    int i, idx, ok = 1;
    for (i = 0; i < ctx->qtd_carrinho; i++) {
        idx = indice_produto_por_id(ctx, ctx->carrinho[i].id_produto);
        if (idx < 0) {
            printf("[!] Um item do carrinho nao esta mais disponivel.\n");
            ok = 0;
        } else if (ctx->carrinho[i].quantidade > ctx->produtos[idx].estoque) {
            printf("[!] Estoque insuficiente para \"%s\" (disponivel: %d).\n",
                   ctx->produtos[idx].nome, ctx->produtos[idx].estoque);
            ok = 0;
        }
    }
    return ok;
}
