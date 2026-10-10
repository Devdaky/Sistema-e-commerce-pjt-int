#include <stdio.h>
#include "pedidos.h"
#include "produtos.h"
#include "carrinho.h"
#include "utils.h"

const char *nome_forma_pagamento(int forma)
{
    switch (forma) {
    case FORMA_PIX:    return "PIX";
    case FORMA_CARTAO: return "Cartão de Crédito";
    case FORMA_BOLETO: return "Boleto";
    default:           return "-";
    }
}

const char *nome_status(int status)
{
    switch (status) {
    case STATUS_AGUARDANDO: return "Aguardando despacho";
    case STATUS_ENVIADO:    return "Enviado";
    case STATUS_ENTREGUE:   return "Entregue";
    default:                return "Desconhecido";
    }
}

int salvar_pedidos(const Contexto *ctx)
{
    int i, j;
    FILE *f = fopen(ARQ_PEDIDOS, "w");
    if (f == NULL) {
        printf("[!] Nao foi possivel gravar %s.\n", ARQ_PEDIDOS);
        return 0;
    }
    for (i = 0; i < ctx->qtd_pedidos; i++) {
        const Pedido *p = &ctx->pedidos[i];
        fprintf(f, "%d|%d|%.2f|%.2f|%.2f|%d|%d|%s|%s|%d\n",
                p->id, p->id_usuario, p->subtotal, p->frete, p->total,
                p->forma_pagamento, p->status,
                p->rastreio[0] != '\0' ? p->rastreio : "-",
                p->endereco, p->qtd_itens);
        for (j = 0; j < p->qtd_itens; j++) {
            fprintf(f, "%d|%s|%d|%.2f\n", p->itens[j].id_produto, p->itens[j].nome,
                    p->itens[j].quantidade, p->itens[j].preco_unit);
        }
    }
    fclose(f);
    return 1;
}

int carregar_pedidos(Contexto *ctx)
{
    char linha[400];
    char rastreio[TAM_RASTREIO];
    Pedido ped;
    int j, ok;
    FILE *f = fopen(ARQ_PEDIDOS, "r");

    ctx->qtd_pedidos = 0;
    if (f == NULL) {
        return 0;
    }
    while (fgets(linha, (int)sizeof linha, f) != NULL) {
        if (sscanf(linha, "%d|%d|%lf|%lf|%lf|%d|%d|%19[^|]|%119[^|]|%d",
                   &ped.id, &ped.id_usuario, &ped.subtotal, &ped.frete, &ped.total,
                   &ped.forma_pagamento, &ped.status, rastreio, ped.endereco,
                   &ped.qtd_itens) != 10) {
            continue;
        }
        if (ped.qtd_itens < 0 || ped.qtd_itens > MAX_ITENS_CARRINHO) {
            continue;
        }
        str_copiar(ped.rastreio, str_igual(rastreio, "-") ? "" : rastreio, TAM_RASTREIO);
        ok = 1;
        for (j = 0; j < ped.qtd_itens; j++) {
            if (fgets(linha, (int)sizeof linha, f) == NULL ||
                sscanf(linha, "%d|%49[^|]|%d|%lf", &ped.itens[j].id_produto,
                       ped.itens[j].nome, &ped.itens[j].quantidade,
                       &ped.itens[j].preco_unit) != 4) {
                ok = 0;
                break;
            }
        }
        if (ok && ctx->qtd_pedidos < MAX_PEDIDOS) {
            ctx->pedidos[ctx->qtd_pedidos++] = ped;
        }
    }
    fclose(f);
    return ctx->qtd_pedidos;
}

int indice_pedido_por_id(const Contexto *ctx, int id)
{
    int i;
    for (i = 0; i < ctx->qtd_pedidos; i++) {
        if (ctx->pedidos[i].id == id) {
            return i;
        }
    }
    return -1;
}

int proximo_id_pedido(const Contexto *ctx)
{
    int i, maior = 0;
    for (i = 0; i < ctx->qtd_pedidos; i++) {
        if (ctx->pedidos[i].id > maior) {
            maior = ctx->pedidos[i].id;
        }
    }
    return maior + 1;
}

void gerar_codigo_rastreio(Pedido *p)
{
    snprintf(p->rastreio, TAM_RASTREIO, "VM%06dBR", p->id);
}

/* Transforma o carrinho em pedido, da baixa no estoque (RF-06) e grava tudo.
   Retorna o id do pedido ou -1 em caso de falha. */
int criar_pedido(Contexto *ctx)
{
    int i, idx;
    Pedido *p;

    if (ctx->qtd_pedidos >= MAX_PEDIDOS || ctx->qtd_carrinho == 0 ||
        ctx->usuario_logado < 0) {
        return -1;
    }
    p = &ctx->pedidos[ctx->qtd_pedidos];
    p->id = proximo_id_pedido(ctx);
    p->id_usuario = ctx->usuarios[ctx->usuario_logado].id;
    p->qtd_itens = 0;
    p->subtotal = 0.0;

    for (i = 0; i < ctx->qtd_carrinho; i++) {
        idx = indice_produto_por_id(ctx, ctx->carrinho[i].id_produto);
        if (idx < 0) {
            continue;
        }
        p->itens[p->qtd_itens].id_produto = ctx->produtos[idx].id;
        str_copiar(p->itens[p->qtd_itens].nome, ctx->produtos[idx].nome, TAM_NOME);
        p->itens[p->qtd_itens].quantidade = ctx->carrinho[i].quantidade;
        p->itens[p->qtd_itens].preco_unit = ctx->produtos[idx].preco;
        p->subtotal += ctx->produtos[idx].preco * ctx->carrinho[i].quantidade;
        ctx->produtos[idx].estoque -= ctx->carrinho[i].quantidade;
        p->qtd_itens++;
    }
    p->frete = ctx->checkout.frete;
    p->total = p->subtotal + p->frete;
    p->forma_pagamento = ctx->checkout.forma;
    p->status = STATUS_AGUARDANDO;
    p->rastreio[0] = '\0';
    snprintf(p->endereco, TAM_ENDERECO_PEDIDO, "%s - CEP %.5s-%s",
             ctx->checkout.endereco, ctx->checkout.cep, ctx->checkout.cep + 5);

    ctx->qtd_pedidos++;
    esvaziar_carrinho(ctx);
    salvar_produtos(ctx);
    salvar_pedidos(ctx);
    return p->id;
}

void exibir_pedido_detalhe(const Pedido *p)
{
    int i;
    printf("\n  Pedido #%d\n", p->id);
    imprimir_linha('-', 60);
    for (i = 0; i < p->qtd_itens; i++) {
        printf("  %dx %s  (R$ %.2f cada)\n", p->itens[i].quantidade,
               p->itens[i].nome, p->itens[i].preco_unit);
    }
    imprimir_linha('-', 60);
    printf("  Subtotal : R$ %.2f\n", p->subtotal);
    printf("  Frete    : R$ %.2f\n", p->frete);
    printf("  TOTAL    : R$ %.2f\n", p->total);
    printf("  Pagamento: %s\n", nome_forma_pagamento(p->forma_pagamento));
    printf("  Entrega  : %s\n", p->endereco);
    printf("  Status   : %s\n", nome_status(p->status));
    if (p->rastreio[0] != '\0') {
        printf("  Rastreio : %s\n", p->rastreio);
    } else {
        printf("  Rastreio : aguardando postagem pela transportadora\n");
    }
}
