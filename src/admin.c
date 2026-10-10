#include <stdio.h>
#include <stdlib.h>
#include "admin.h"
#include "utils.h"
#include "produtos.h"
#include "carrinho.h"
#include "usuarios.h"
#include "pedidos.h"

/* ===================== Produtos (CRUD) ===================== */

static void admin_listar_produtos(const Contexto *ctx)
{
    int i;
    imprimir_cabecalho("ADMIN - LISTA DE PRODUTOS");
    printf("\n");
    for (i = 0; i < ctx->qtd_produtos; i++) {
        exibir_produto_resumo(&ctx->produtos[i]);
    }
    if (ctx->qtd_produtos == 0) {
        printf("  Nenhum produto cadastrado.\n");
    }
    pausar();
}

static void admin_cadastrar_produto(Contexto *ctx)
{
    Produto *p;
    imprimir_cabecalho("ADMIN - CADASTRAR PRODUTO");
    if (ctx->qtd_produtos >= MAX_PRODUTOS) {
        printf("[!] Limite de produtos atingido.\n");
        pausar();
        return;
    }
    p = &ctx->produtos[ctx->qtd_produtos];
    p->id = proximo_id_produto(ctx);
    ler_string("Nome: ", p->nome, TAM_NOME);
    ler_string("Categoria: ", p->categoria, TAM_CATEGORIA);
    ler_string("Marca: ", p->marca, TAM_MARCA);
    ler_string("Especificações: ", p->specs, TAM_SPECS);
    p->preco = ler_double_faixa("Preço (R$): ", 0.01, 100000.0);
    p->estoque = ler_opcao_segura("Estoque inicial: ", 0, 100000);
    ctx->qtd_produtos++;
    salvar_produtos(ctx);
    printf("\nProduto cadastrado com o código %d.\n", p->id);
    pausar();
}

static void admin_alterar_produto(Contexto *ctx)
{
    int id, idx;
    char tmp[TAM_SPECS];
    double v;
    Produto *p;

    imprimir_cabecalho("ADMIN - ALTERAR PRODUTO");
    id = ler_opcao_segura("Código do produto (0 cancela): ", 0, 999999);
    if (id == 0) {
        return;
    }
    idx = indice_produto_por_id(ctx, id);
    if (idx < 0) {
        printf("[!] Produto não encontrado.\n");
        pausar();
        return;
    }
    p = &ctx->produtos[idx];
    printf("\n(ENTER mantém o valor atual)\n");

    printf("Nome atual: %s\n", p->nome);
    if (ler_string_opcional("Novo nome: ", tmp, TAM_NOME) > 0) {
        str_copiar(p->nome, tmp, TAM_NOME);
    }
    printf("Categoria atual: %s\n", p->categoria);
    if (ler_string_opcional("Nova categoria: ", tmp, TAM_CATEGORIA) > 0) {
        str_copiar(p->categoria, tmp, TAM_CATEGORIA);
    }
    printf("Marca atual: %s\n", p->marca);
    if (ler_string_opcional("Nova marca: ", tmp, TAM_MARCA) > 0) {
        str_copiar(p->marca, tmp, TAM_MARCA);
    }
    printf("Especificações atuais: %s\n", p->specs);
    if (ler_string_opcional("Novas especificações: ", tmp, TAM_SPECS) > 0) {
        str_copiar(p->specs, tmp, TAM_SPECS);
    }
    printf("Preço atual: R$ %.2f\n", p->preco);
    if (ler_double_opcional("Novo preço: ", &v, 0.01, 100000.0)) {
        p->preco = v;
    }
    salvar_produtos(ctx);
    printf("\nProduto atualizado.\n");
    pausar();
}

static void admin_remover_produto(Contexto *ctx)
{
    int id, idx, i;

    imprimir_cabecalho("ADMIN - REMOVER PRODUTO");
    id = ler_opcao_segura("Código do produto (0 cancela): ", 0, 999999);
    if (id == 0) {
        return;
    }
    idx = indice_produto_por_id(ctx, id);
    if (idx < 0) {
        printf("[!] Produto não encontrado.\n");
        pausar();
        return;
    }
    printf("Produto: %s\n", ctx->produtos[idx].nome);
    if (!ler_confirmacao("Confirma a remoção")) {
        return;
    }
    remover_produto_do_carrinho(ctx, id);
    for (i = idx; i < ctx->qtd_produtos - 1; i++) {
        ctx->produtos[i] = ctx->produtos[i + 1];
    }
    ctx->qtd_produtos--;
    salvar_produtos(ctx);
    printf("Produto removido.\n");
    pausar();
}

/* ===================== Estoque ===================== */

static void admin_estoque(Contexto *ctx)
{
    int i, id, idx;

    imprimir_cabecalho("ADMIN - CONTROLE DE ESTOQUE");
    printf("\n");
    for (i = 0; i < ctx->qtd_produtos; i++) {
        printf("  [%3d] %s -> saldo: %d%s\n", ctx->produtos[i].id, ctx->produtos[i].nome,
               ctx->produtos[i].estoque,
               ctx->produtos[i].estoque <= 5 ? "  <-- ESTOQUE BAIXO" : "");
    }
    printf("\n");
    id = ler_opcao_segura("Código do produto a ajustar (0 cancela): ", 0, 999999);
    if (id == 0) {
        return;
    }
    idx = indice_produto_por_id(ctx, id);
    if (idx < 0) {
        printf("[!] Produto não encontrado.\n");
        pausar();
        return;
    }
    ctx->produtos[idx].estoque = ler_opcao_segura("Novo saldo de estoque: ", 0, 100000);
    salvar_produtos(ctx);
    printf("Estoque atualizado.\n");
    pausar();
}

/* ===================== Categorias ===================== */

static void admin_categorias(Contexto *ctx)
{
    char valores[MAX_PRODUTOS][TAM_CATEGORIA];
    char nova[TAM_CATEGORIA];
    int i, j, n, op, qtd;

    imprimir_cabecalho("ADMIN - CATEGORIAS");
    n = listar_valores_distintos(ctx, 0, valores, MAX_PRODUTOS);
    printf("\n");
    for (i = 0; i < n; i++) {
        qtd = 0;
        for (j = 0; j < ctx->qtd_produtos; j++) {
            if (str_igual_ci(ctx->produtos[j].categoria, valores[i])) {
                qtd++;
            }
        }
        printf("  %d) %s (%d produto(s))\n", i + 1, valores[i], qtd);
    }
    if (n == 0) {
        printf("  Nenhuma categoria.\n");
        pausar();
        return;
    }
    printf("\n(Novas categorias nascem ao cadastrar um produto.)\n");
    op = ler_opcao_segura("Número da categoria a renomear (0 cancela): ", 0, n);
    if (op == 0) {
        return;
    }
    ler_string("Novo nome da categoria: ", nova, TAM_CATEGORIA);
    for (j = 0; j < ctx->qtd_produtos; j++) {
        if (str_igual_ci(ctx->produtos[j].categoria, valores[op - 1])) {
            str_copiar(ctx->produtos[j].categoria, nova, TAM_CATEGORIA);
        }
    }
    salvar_produtos(ctx);
    printf("Categoria renomeada.\n");
    pausar();
}

/* ===================== Pedidos / Logistica ===================== */

static void admin_pedidos(Contexto *ctx)
{
    int i, op, id, idx, u;

    while (1) {
        imprimir_cabecalho("ADMIN - PEDIDOS E LOGÍSTICA");
        printf("\n");
        for (i = 0; i < ctx->qtd_pedidos; i++) {
            const Pedido *p = &ctx->pedidos[i];
            u = indice_usuario_por_id(ctx, p->id_usuario);
            printf("  #%d | %s | R$ %.2f | %s | %s\n", p->id,
                   u >= 0 ? ctx->usuarios[u].nome : "(usuário removido)",
                   p->total, nome_status(p->status),
                   p->rastreio[0] != '\0' ? p->rastreio : "sem rastreio");
        }
        if (ctx->qtd_pedidos == 0) {
            printf("  Nenhum pedido registrado.\n");
        }
        printf("\n  1) Despachar pedido (gera código de rastreio)\n");
        printf("  2) Marcar pedido como entregue\n");
        printf("  3) Ver detalhes de um pedido\n");
        printf("  0) Voltar\n");
        op = ler_opcao_segura("Escolha: ", 0, 3);
        if (op == 0) {
            return;
        }
        if (ctx->qtd_pedidos == 0) {
            printf("[!] Não há pedidos.\n");
            pausar();
            continue;
        }
        id = ler_opcao_segura("Número do pedido: ", 1, 999999);
        idx = indice_pedido_por_id(ctx, id);
        if (idx < 0) {
            printf("[!] Pedido não encontrado.\n");
            pausar();
            continue;
        }
        if (op == 1) {
            if (ctx->pedidos[idx].status != STATUS_AGUARDANDO) {
                printf("[!] Este pedido já foi despachado.\n");
            } else {
                gerar_codigo_rastreio(&ctx->pedidos[idx]);
                ctx->pedidos[idx].status = STATUS_ENVIADO;
                salvar_pedidos(ctx);
                printf("Pedido despachado. Rastreio: %s\n", ctx->pedidos[idx].rastreio);
            }
        } else if (op == 2) {
            if (ctx->pedidos[idx].status != STATUS_ENVIADO) {
                printf("[!] Só é possível entregar pedidos já enviados.\n");
            } else {
                ctx->pedidos[idx].status = STATUS_ENTREGUE;
                salvar_pedidos(ctx);
                printf("Pedido marcado como entregue.\n");
            }
        } else {
            exibir_pedido_detalhe(&ctx->pedidos[idx]);
        }
        pausar();
    }
}

/* ===================== Relatorios ===================== */

typedef struct {
    int    id;
    char   nome[TAM_NOME];
    int    qtd;
    double valor;
} Vendido;

static void admin_relatorios(const Contexto *ctx)
{
    Vendido vendidos[MAX_PRODUTOS];
    Vendido tmp;
    int n = 0, i, j, k, achou, topo;
    double faturamento = 0.0, por_forma[4] = {0.0, 0.0, 0.0, 0.0};
    int qtd_forma[4] = {0, 0, 0, 0};

    imprimir_cabecalho("ADMIN - RELATÓRIOS GERENCIAIS");

    for (i = 0; i < ctx->qtd_pedidos; i++) {
        const Pedido *p = &ctx->pedidos[i];
        faturamento += p->total;
        if (p->forma_pagamento >= 1 && p->forma_pagamento <= 3) {
            por_forma[p->forma_pagamento] += p->total;
            qtd_forma[p->forma_pagamento]++;
        }
        for (j = 0; j < p->qtd_itens; j++) {
            achou = 0;
            for (k = 0; k < n; k++) {
                if (vendidos[k].id == p->itens[j].id_produto) {
                    vendidos[k].qtd += p->itens[j].quantidade;
                    vendidos[k].valor += p->itens[j].quantidade * p->itens[j].preco_unit;
                    achou = 1;
                    break;
                }
            }
            if (!achou && n < MAX_PRODUTOS) {
                vendidos[n].id = p->itens[j].id_produto;
                str_copiar(vendidos[n].nome, p->itens[j].nome, TAM_NOME);
                vendidos[n].qtd = p->itens[j].quantidade;
                vendidos[n].valor = p->itens[j].quantidade * p->itens[j].preco_unit;
                n++;
            }
        }
    }

    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - 1 - i; j++) {
            if (vendidos[j].qtd < vendidos[j + 1].qtd) {
                tmp = vendidos[j];
                vendidos[j] = vendidos[j + 1];
                vendidos[j + 1] = tmp;
            }
        }
    }

    printf("\n  Pedidos realizados : %d\n", ctx->qtd_pedidos);
    printf("  Faturamento total  : R$ %.2f\n", faturamento);
    printf("\n  Vendas por forma de pagamento:\n");
    printf("    PIX               : %d pedido(s) | R$ %.2f\n", qtd_forma[FORMA_PIX], por_forma[FORMA_PIX]);
    printf("    Cartão de Crédito : %d pedido(s) | R$ %.2f\n", qtd_forma[FORMA_CARTAO], por_forma[FORMA_CARTAO]);
    printf("    Boleto            : %d pedido(s) | R$ %.2f\n", qtd_forma[FORMA_BOLETO], por_forma[FORMA_BOLETO]);

    printf("\n  Itens mais vendidos:\n");
    topo = n < 5 ? n : 5;
    for (i = 0; i < topo; i++) {
        printf("    %d) %s - %d un. (R$ %.2f)\n", i + 1, vendidos[i].nome,
               vendidos[i].qtd, vendidos[i].valor);
    }
    if (n == 0) {
        printf("    Ainda não há vendas.\n");
    }
    pausar();
}

/* ===================== Menu do painel ===================== */

Estado tela_admin(Contexto *ctx)
{
    int op;

    if (ctx->usuario_logado < 0 || !ctx->usuarios[ctx->usuario_logado].admin) {
        printf("[!] Acesso restrito ao Administrador.\n");
        pausar();
        return ESTADO_HOME;
    }

    imprimir_cabecalho("PAINEL ADMINISTRATIVO");
    printf("  1) Listar produtos\n");
    printf("  2) Cadastrar produto\n");
    printf("  3) Alterar produto\n");
    printf("  4) Remover produto\n");
    printf("  5) Controle de estoque\n");
    printf("  6) Categorias\n");
    printf("  7) Pedidos e logística (despacho / rastreio)\n");
    printf("  8) Relatórios gerenciais\n");
    printf("  0) Voltar à página inicial\n");

    op = ler_opcao_segura("Escolha: ", 0, 8);
    switch (op) {
    case 1: admin_listar_produtos(ctx);   return ESTADO_ADMIN;
    case 2: admin_cadastrar_produto(ctx); return ESTADO_ADMIN;
    case 3: admin_alterar_produto(ctx);   return ESTADO_ADMIN;
    case 4: admin_remover_produto(ctx);   return ESTADO_ADMIN;
    case 5: admin_estoque(ctx);           return ESTADO_ADMIN;
    case 6: admin_categorias(ctx);        return ESTADO_ADMIN;
    case 7: admin_pedidos(ctx);           return ESTADO_ADMIN;
    case 8: admin_relatorios(ctx);        return ESTADO_ADMIN;
    default:                              return ESTADO_HOME;
    }
}
