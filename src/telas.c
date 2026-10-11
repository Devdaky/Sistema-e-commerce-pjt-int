#include <stdio.h>
#include <stdlib.h>
#include "telas.h"
#include "utils.h"
#include "produtos.h"
#include "carrinho.h"
#include "usuarios.h"
#include "pedidos.h"
#include "pagamento.h"

/* ===================== Auxiliares ===================== */

static int esta_logado(const Contexto *ctx)
{
    return ctx->usuario_logado >= 0;
}

static void exibir_status_sessao(const Contexto *ctx)
{
    if (esta_logado(ctx)) {
        printf("Usuário: %s%s", ctx->usuarios[ctx->usuario_logado].nome,
               ctx->usuarios[ctx->usuario_logado].admin ? " (Administrador)" : "");
    } else {
        printf("Usuário: Visitante");
    }
    printf("   |   Carrinho: %d item(ns)\n", ctx->qtd_carrinho);
}

static void exibir_filtros_ativos(const Filtro *f)
{
    int algum = 0;
    printf("Filtros ativos: ");
    if (f->termo[0] != '\0') {
        printf("[busca: %s] ", f->termo);
        algum = 1;
    }
    if (f->categoria[0] != '\0') {
        printf("[categoria: %s] ", f->categoria);
        algum = 1;
    }
    if (f->marca[0] != '\0') {
        printf("[marca: %s] ", f->marca);
        algum = 1;
    }
    if (f->preco_min > 0 || f->preco_max > 0) {
        printf("[preço: R$ %.2f a ", f->preco_min);
        if (f->preco_max > 0) {
            printf("R$ %.2f] ", f->preco_max);
        } else {
            printf("sem limite] ");
        }
        algum = 1;
    }
    if (!algum) {
        printf("(nenhum)");
    }
    printf("\n\n");
}

/* Lista valores distintos (categoria ou marca) e deixa o usuario escolher */
static void escolher_valor(const Contexto *ctx, int campo, const char *titulo,
                           char *dest, int tam)
{
    char valores[MAX_PRODUTOS][TAM_CATEGORIA];
    int i, n, op;

    n = listar_valores_distintos(ctx, campo, valores, MAX_PRODUTOS);
    printf("\n%s:\n", titulo);
    for (i = 0; i < n; i++) {
        printf("  %d) %s\n", i + 1, valores[i]);
    }
    printf("  0) Todas (remove este filtro)\n");
    op = ler_opcao_segura("Escolha: ", 0, n);
    if (op == 0) {
        dest[0] = '\0';
    } else {
        str_copiar(dest, valores[op - 1], tam);
    }
}

static void exibir_resumo_checkout(const Contexto *ctx)
{
    double subtotal = calcular_subtotal(ctx);
    exibir_carrinho(ctx);
    printf("  Frete            : R$ %.2f\n", ctx->checkout.frete);
    printf("  TOTAL DO PEDIDO  : R$ %.2f\n", subtotal + ctx->checkout.frete);
    printf("  Entrega em       : %s - CEP %.5s-%s\n", ctx->checkout.endereco,
           ctx->checkout.cep, ctx->checkout.cep + 5);
}

/* ===================== HOME ===================== */

Estado tela_home(Contexto *ctx)
{
    int i, op, destaques, d;

    imprimir_cabecalho("VITAMARKET - Hardware & Modding");
    exibir_status_sessao(ctx);

    destaques = ctx->qtd_produtos < 3 ? ctx->qtd_produtos : 3;
    printf("\n--- DESTAQUES ---\n");
    for (i = 0; i < destaques; i++) {
        printf("  D%d) %s - R$ %.2f\n", i + 1, ctx->produtos[i].nome, ctx->produtos[i].preco);
    }

    printf("\n--- MENU PRINCIPAL ---\n");
    printf("  1) Pesquisar / filtrar catálogo\n");
    printf("  2) Ver um destaque\n");
    printf("  3) Carrinho de compras\n");
    printf("  4) Entrar / Cadastrar\n");
    printf("  5) Meus pedidos\n");
    printf("  6) Painel administrativo\n");
    printf("  7) Sair da conta (logout)\n");
    printf("  0) Encerrar programa\n");

    op = ler_opcao_segura("Escolha: ", 0, 7);
    switch (op) {
    case 1:
        return ESTADO_CATALOGO;
    case 2:
        if (destaques == 0) {
            printf("[!] Nenhum produto cadastrado.\n");
            pausar();
            return ESTADO_HOME;
        }
        d = ler_opcao_segura("Número do destaque: ", 1, destaques);
        ctx->produto_selecionado = ctx->produtos[d - 1].id;
        return ESTADO_DETALHES;
    case 3:
        return ESTADO_CARRINHO;
    case 4:
        if (esta_logado(ctx)) {
            printf("Você já está autenticado como %s.\n", ctx->usuarios[ctx->usuario_logado].nome);
            pausar();
            return ESTADO_HOME;
        }
        ctx->retorno_login = ESTADO_HOME;
        return ESTADO_AUTENTICACAO;
    case 5:
        return ESTADO_MEUS_PEDIDOS;
    case 6:
        if (esta_logado(ctx) && ctx->usuarios[ctx->usuario_logado].admin) {
            return ESTADO_ADMIN;
        }
        printf("[!] Acesso restrito: faça login como Administrador.\n");
        pausar();
        return ESTADO_HOME;
    case 7:
        if (esta_logado(ctx)) {
            ctx->usuario_logado = -1;
            printf("Sessão encerrada.\n");
        } else {
            printf("Você não está autenticado.\n");
        }
        pausar();
        return ESTADO_HOME;
    default:
        return ESTADO_SAIR;
    }
}

/* ===================== CATALOGO / BUSCA ===================== */

Estado tela_catalogo(Contexto *ctx)
{
    int achados, op, id;
    double v_min, v_max;

    imprimir_cabecalho("CATÁLOGO / BUSCA DE PRODUTOS");
    exibir_filtros_ativos(&ctx->filtro);
    achados = listar_produtos_filtrados(ctx);
    if (achados == 0) {
        printf("\n[!] Nenhum produto corresponde aos filtros aplicados.\n");
    }

    printf("\n  1) Buscar por palavra-chave\n");
    printf("  2) Filtrar por categoria\n");
    printf("  3) Filtrar por marca\n");
    printf("  4) Filtrar por faixa de preço\n");
    printf("  5) Limpar todos os filtros\n");
    printf("  6) Selecionar item (ver ficha técnica)\n");
    printf("  7) Ir para o carrinho\n");
    printf("  0) Voltar à página inicial\n");

    op = ler_opcao_segura("Escolha: ", 0, 7);
    switch (op) {
    case 1:
        ler_string_opcional("Palavra-chave (ENTER limpa a busca): ",
                            ctx->filtro.termo, TAM_NOME);
        return ESTADO_CATALOGO;
    case 2:
        escolher_valor(ctx, 0, "Categorias", ctx->filtro.categoria, TAM_CATEGORIA);
        return ESTADO_CATALOGO;
    case 3:
        escolher_valor(ctx, 1, "Marcas", ctx->filtro.marca, TAM_MARCA);
        return ESTADO_CATALOGO;
    case 4:
        v_min = ler_double_faixa("Preço mínimo (0 = sem mínimo): ", 0.0, 1000000.0);
        v_max = ler_double_faixa("Preço máximo (0 = sem máximo): ", 0.0, 1000000.0);
        if (v_max > 0 && v_max < v_min) {
            printf("[!] O preço máximo não pode ser menor que o mínimo.\n");
            pausar();
        } else {
            ctx->filtro.preco_min = v_min;
            ctx->filtro.preco_max = v_max;
        }
        return ESTADO_CATALOGO;
    case 5:
        ctx->filtro.termo[0] = '\0';
        ctx->filtro.categoria[0] = '\0';
        ctx->filtro.marca[0] = '\0';
        ctx->filtro.preco_min = 0.0;
        ctx->filtro.preco_max = 0.0;
        return ESTADO_CATALOGO;
    case 6:
        if (achados == 0) {
            printf("[!] Não há itens para selecionar.\n");
            pausar();
            return ESTADO_CATALOGO;
        }
        id = ler_opcao_segura("ID do produto (número entre colchetes): ", 1, 999999);
        if (indice_produto_por_id(ctx, id) < 0) {
            printf("[!] Produto não encontrado.\n");
            pausar();
            return ESTADO_CATALOGO;
        }
        ctx->produto_selecionado = id;
        return ESTADO_DETALHES;
    case 7:
        return ESTADO_CARRINHO;
    default:
        return ESTADO_HOME;
    }
}

/* ===================== DETALHES DO PRODUTO ===================== */

Estado tela_detalhes(Contexto *ctx)
{
    int idx, op, qtd, res;
    const Produto *p;

    idx = indice_produto_por_id(ctx, ctx->produto_selecionado);
    if (idx < 0) {
        printf("[!] Produto não encontrado.\n");
        pausar();
        return ESTADO_CATALOGO;
    }
    p = &ctx->produtos[idx];

    imprimir_cabecalho("FICHA TÉCNICA DO PRODUTO");
    printf("  [imagem do produto]\n\n");
    printf("  Código        : %d\n", p->id);
    printf("  Nome          : %s\n", p->nome);
    printf("  Categoria     : %s\n", p->categoria);
    printf("  Marca         : %s\n", p->marca);
    printf("  Especificações: %s\n", p->specs);
    printf("  Preço unitário: R$ %.2f\n", p->preco);
    if (p->estoque > 0) {
        printf("  Disponibilidade: %d unidade(s) em estoque\n", p->estoque);
    } else {
        printf("  Disponibilidade: INDISPONÍVEL\n");
    }

    printf("\n  1) Adicionar ao carrinho\n");
    printf("  2) Voltar ao catálogo\n");
    printf("  3) Ir para o carrinho\n");
    printf("  0) Página inicial\n");

    op = ler_opcao_segura("Escolha: ", 0, 3);
    switch (op) {
    case 1:
        if (p->estoque <= 0) {
            printf("[!] Produto sem estoque no momento.\n");
            pausar();
            return ESTADO_DETALHES;
        }
        qtd = ler_opcao_segura("Quantidade: ", 1, p->estoque);
        res = adicionar_ao_carrinho(ctx, p->id, qtd);
        if (res == CARRINHO_OK) {
            printf("Produto adicionado ao carrinho!\n");
            pausar();
            return ESTADO_CARRINHO;
        }
        if (res == CARRINHO_SEM_ESTOQUE) {
            printf("[!] Quantidade total no carrinho excederia o estoque (%d).\n", p->estoque);
        } else if (res == CARRINHO_CHEIO) {
            printf("[!] Carrinho cheio (máximo %d itens diferentes).\n", MAX_ITENS_CARRINHO);
        } else {
            printf("[!] Não foi possível adicionar o produto.\n");
        }
        pausar();
        return ESTADO_DETALHES;
    case 2:
        return ESTADO_CATALOGO;
    case 3:
        return ESTADO_CARRINHO;
    default:
        return ESTADO_HOME;
    }
}

/* ===================== CARRINHO ===================== */

Estado tela_carrinho(Contexto *ctx)
{
    int op, n, pos, idx, estoque, nova, res;

    imprimir_cabecalho("CARRINHO DE COMPRAS");
    exibir_status_sessao(ctx);
    exibir_carrinho(ctx);

    printf("\n  1) Alterar quantidade de um item\n");
    printf("  2) Remover um item\n");
    printf("  3) Esvaziar carrinho\n");
    printf("  4) Continuar comprando\n");
    printf("  5) Finalizar compra\n");
    printf("  0) Página inicial\n");

    op = ler_opcao_segura("Escolha: ", 0, 5);
    switch (op) {
    case 1:
    case 2:
        if (ctx->qtd_carrinho == 0) {
            printf("[!] O carrinho está vazio.\n");
            pausar();
            return ESTADO_CARRINHO;
        }
        n = ler_opcao_segura("Número do item: ", 1, ctx->qtd_carrinho);
        pos = n - 1;
        if (op == 2) {
            remover_item_carrinho(ctx, pos);
            printf("Item removido.\n");
            pausar();
            return ESTADO_CARRINHO;
        }
        idx = indice_produto_por_id(ctx, ctx->carrinho[pos].id_produto);
        estoque = (idx >= 0) ? ctx->produtos[idx].estoque : 0;
        if (estoque < 1) {
            printf("[!] Este produto está sem estoque. Remova-o do carrinho.\n");
            pausar();
            return ESTADO_CARRINHO;
        }
        nova = ler_opcao_segura("Nova quantidade: ", 1, estoque);
        res = alterar_quantidade_item(ctx, pos, nova);
        if (res == CARRINHO_OK) {
            printf("Quantidade atualizada.\n");
        } else {
            printf("[!] Quantidade superior ao estoque disponível (%d).\n", estoque);
        }
        pausar();
        return ESTADO_CARRINHO;
    case 3:
        esvaziar_carrinho(ctx);
        printf("Carrinho esvaziado.\n");
        pausar();
        return ESTADO_CARRINHO;
    case 4:
        return ESTADO_CATALOGO;
    case 5:
        if (ctx->qtd_carrinho == 0) {
            printf("[!] Adicione produtos antes de finalizar a compra.\n");
            pausar();
            return ESTADO_CARRINHO;
        }
        if (!validar_estoque_carrinho(ctx)) {
            pausar();
            return ESTADO_CARRINHO;
        }
        if (esta_logado(ctx)) {
            return ESTADO_ENDERECO_FRETE;
        }
        printf("Para finalizar a compra é preciso entrar ou se cadastrar.\n");
        pausar();
        ctx->retorno_login = ESTADO_ENDERECO_FRETE;
        return ESTADO_AUTENTICACAO;
    default:
        return ESTADO_HOME;
    }
}

/* ===================== AUTENTICACAO ===================== */

static int executar_login(Contexto *ctx)
{
    char login[TAM_LOGIN], senha[30];
    int idx;

    ler_string("Usuário: ", login, (int)sizeof login);
    ler_string("Senha: ", senha, (int)sizeof senha);

    idx = indice_usuario_por_login(ctx, login);
    if (idx < 0 || ctx->usuarios[idx].senha_hash != calcular_hash(senha)) {
        printf("[!] Usuário ou senha inválidos.\n");
        return 0;
    }
    ctx->usuario_logado = idx;
    printf("Bem-vindo(a), %s!\n", ctx->usuarios[idx].nome);
    return 1;
}

/* Retorna 1 se o texto serve como nome de usuario: 3 a 19 caracteres, sem espacos */
static int login_valido(const char *login)
{
    int i, n = str_tamanho(login);
    if (n < 3 || n >= TAM_LOGIN) {
        return 0;
    }
    for (i = 0; i < n; i++) {
        if (login[i] == ' ') {
            return 0;
        }
    }
    return 1;
}

static int executar_cadastro(Contexto *ctx)
{
    char nome[TAM_NOME], login[TAM_LOGIN], entrada[40], cpf[16], senha[30], conf[30];
    int idx;

    printf("\nLGPD: guardamos apenas seu nome, o nome de usuário e o hash da senha.\n");
    printf("O CPF é usado somente para validar o cadastro e NÃO é armazenado.\n");
    if (!ler_confirmacao("Você concorda com o tratamento dos dados?")) {
        printf("Cadastro cancelado.\n");
        return 0;
    }
    ler_string("Nome completo: ", nome, (int)sizeof nome);

    while (1) {
        if (ler_string_opcional("Nome de usuário (3 a 19 caracteres, sem espaços, ENTER cancela): ",
                                login, (int)sizeof login) == 0) {
            printf("Cadastro cancelado.\n");
            return 0;
        }
        if (!login_valido(login)) {
            printf("[!] Use de 3 a 19 caracteres, sem espaços.\n");
            continue;
        }
        if (indice_usuario_por_login(ctx, login) >= 0) {
            printf("[!] Este nome de usuário já existe. Escolha outro.\n");
            continue;
        }
        break;
    }

    while (1) {
        if (ler_string_opcional("CPF para validação (somente números, ENTER cancela): ",
                                entrada, (int)sizeof entrada) == 0) {
            printf("Cadastro cancelado.\n");
            return 0;
        }
        extrair_digitos(entrada, cpf, (int)sizeof cpf);
        if (validar_cpf(cpf)) {
            break;
        }
        printf("[!] CPF inválido. Confira os 11 dígitos.\n");
    }

    while (1) {
        ler_string("Senha (mínimo 6 caracteres): ", senha, (int)sizeof senha);
        if (str_tamanho(senha) >= 6) {
            break;
        }
        printf("[!] Senha muito curta.\n");
    }
    ler_string("Confirme a senha: ", conf, (int)sizeof conf);
    if (!str_igual(senha, conf)) {
        printf("[!] As senhas não conferem. Cadastro cancelado.\n");
        return 0;
    }

    idx = adicionar_usuario(ctx, nome, login, senha, 0);
    if (idx < 0) {
        printf("[!] Limite de usuários atingido.\n");
        return 0;
    }
    salvar_usuarios(ctx);
    ctx->usuario_logado = idx;
    printf("Cadastro concluído! Bem-vindo(a), %s!\n", nome);
    return 1;
}

Estado tela_autenticacao(Contexto *ctx)
{
    int op, sucesso;
    Estado destino;

    imprimir_cabecalho("AUTENTICAÇÃO (LOGIN / CADASTRO)");
    printf("  1) Entrar\n");
    printf("  2) Criar cadastro\n");
    printf("  0) Voltar\n");
    printf("\n  (Teste: administrador usuário admin / senha admin123)\n\n");

    op = ler_opcao_segura("Escolha: ", 0, 2);
    if (op == 0) {
        destino = (ctx->retorno_login == ESTADO_ENDERECO_FRETE) ? ESTADO_CARRINHO : ESTADO_HOME;
        ctx->retorno_login = ESTADO_HOME;
        return destino;
    }
    sucesso = (op == 1) ? executar_login(ctx) : executar_cadastro(ctx);
    pausar();
    if (!sucesso) {
        return ESTADO_AUTENTICACAO;
    }
    destino = ctx->retorno_login;
    ctx->retorno_login = ESTADO_HOME;
    return destino;
}

/* ===================== ENDERECO E FRETE ===================== */

Estado tela_endereco_frete(Contexto *ctx)
{
    char entrada[40];
    double subtotal;
    int op;

    if (ctx->qtd_carrinho == 0) {
        return ESTADO_CARRINHO;
    }
    if (!esta_logado(ctx)) {
        ctx->retorno_login = ESTADO_ENDERECO_FRETE;
        return ESTADO_AUTENTICACAO;
    }
    subtotal = calcular_subtotal(ctx);

    imprimir_cabecalho("IDENTIFICAÇÃO DO ENDEREÇO E FRETE");
    printf("  Total dos produtos: R$ %.2f\n\n", subtotal);

    ler_string("Endereço (rua, número, bairro, cidade/UF): ",
               ctx->checkout.endereco, TAM_ENDERECO);
    while (1) {
        ler_string("CEP (8 dígitos): ", entrada, (int)sizeof entrada);
        extrair_digitos(entrada, ctx->checkout.cep, (int)sizeof ctx->checkout.cep);
        if (validar_cep(ctx->checkout.cep)) {
            break;
        }
        printf("[!] CEP inválido.\n");
    }

    printf("\nOpções de frete:\n");
    if (subtotal >= FRETE_GRATIS_ACIMA) {
        printf("  1) PAC   - GRÁTIS (compras acima de R$ %.2f)\n", FRETE_GRATIS_ACIMA);
    } else {
        printf("  1) PAC   - R$ %.2f (grátis acima de R$ %.2f)\n", FRETE_PAC, FRETE_GRATIS_ACIMA);
    }
    printf("  2) SEDEX - R$ %.2f (entrega expressa)\n", FRETE_SEDEX);
    printf("  0) Cancelar e voltar ao carrinho\n");

    op = ler_opcao_segura("Escolha: ", 0, 2);
    if (op == 0) {
        return ESTADO_CARRINHO;
    }
    if (op == 1) {
        ctx->checkout.frete = (subtotal >= FRETE_GRATIS_ACIMA) ? 0.0 : FRETE_PAC;
    } else {
        ctx->checkout.frete = FRETE_SEDEX;
    }
    return ESTADO_PAGAMENTO;
}

/* ===================== PAGAMENTO ===================== */

Estado tela_pagamento(Contexto *ctx)
{
    char entrada[40], digitos[24];
    int op, n;
    double total;

    if (ctx->qtd_carrinho == 0) {
        return ESTADO_CARRINHO;
    }
    total = calcular_subtotal(ctx) + ctx->checkout.frete;

    imprimir_cabecalho("SELEÇÃO DO PAGAMENTO");
    exibir_resumo_checkout(ctx);

    printf("\n  1) PIX\n");
    printf("  2) Cartão de crédito\n");
    printf("  3) Boleto\n");
    printf("  0) Voltar (alterar endereço/frete)\n");

    op = ler_opcao_segura("Forma de pagamento: ", 0, 3);
    if (op == 0) {
        return ESTADO_ENDERECO_FRETE;
    }
    ctx->checkout.forma = op;
    str_copiar(ctx->checkout.final_cartao, "----", 5);

    if (op == FORMA_CARTAO) {
        printf("(Simulação: cartões terminados em 0 são recusados.)\n");
        while (1) {
            if (ler_string_opcional("Número do cartão (16 dígitos, ENTER cancela): ",
                                    entrada, (int)sizeof entrada) == 0) {
                return ESTADO_PAGAMENTO;
            }
            n = extrair_digitos(entrada, digitos, (int)sizeof digitos);
            if (n == 16) {
                break;
            }
            printf("[!] O cartão deve ter 16 dígitos.\n");
        }
        ctx->checkout.final_cartao[0] = digitos[12];
        ctx->checkout.final_cartao[1] = digitos[13];
        ctx->checkout.final_cartao[2] = digitos[14];
        ctx->checkout.final_cartao[3] = digitos[15];
        ctx->checkout.final_cartao[4] = '\0';
    }

    printf("\nForma escolhida: %s\n", nome_forma_pagamento(ctx->checkout.forma));
    printf("Valor a pagar  : R$ %.2f\n", total);
    if (!ler_confirmacao("Confirmar dados financeiros e pagar")) {
        return ESTADO_PAGAMENTO;
    }
    return ESTADO_PROCESSANDO;
}

/* ===================== PROCESSANDO ===================== */

Estado tela_processando(Contexto *ctx)
{
    double total;
    int aprovado, id;

    imprimir_cabecalho("PROCESSANDO PAGAMENTO");
    printf("  Validando transação com o Sistema de Pagamento...\n\n");

    if (!validar_estoque_carrinho(ctx)) {
        printf("Revise o carrinho antes de continuar.\n");
        pausar();
        return ESTADO_CARRINHO;
    }
    if (ctx->qtd_pedidos >= MAX_PEDIDOS) {
        printf("[!] Limite de pedidos do sistema atingido.\n");
        pausar();
        return ESTADO_HOME;
    }

    total = calcular_subtotal(ctx) + ctx->checkout.frete;
    aprovado = processar_pagamento(&ctx->checkout, total);
    printf("  Retorno: %s\n", ctx->checkout.mensagem);

    if (!aprovado) {
        pausar();
        return ESTADO_RECUSADO;
    }
    id = criar_pedido(ctx);
    if (id < 0) {
        printf("[!] Falha ao registrar o pedido.\n");
        pausar();
        return ESTADO_CARRINHO;
    }
    ctx->pedido_selecionado = id;
    printf("  Estoque atualizado automaticamente.\n");
    pausar();
    return ESTADO_CONFIRMADO;
}

/* ===================== PEDIDO CONFIRMADO / RECUSADO ===================== */

Estado tela_confirmado(Contexto *ctx)
{
    int idx, op;

    imprimir_cabecalho("PEDIDO CONFIRMADO!");
    idx = indice_pedido_por_id(ctx, ctx->pedido_selecionado);
    if (idx >= 0) {
        exibir_pedido_detalhe(&ctx->pedidos[idx]);
    }
    printf("\n  Obrigado pela compra na VitaMarket!\n");
    printf("\n  1) Consultar histórico de pedidos\n");
    printf("  2) Continuar comprando (página inicial)\n");
    printf("  0) Encerrar sessão e sair\n");

    op = ler_opcao_segura("Escolha: ", 0, 2);
    if (op == 1) {
        return ESTADO_MEUS_PEDIDOS;
    }
    if (op == 2) {
        return ESTADO_HOME;
    }
    ctx->usuario_logado = -1;
    return ESTADO_SAIR;
}

Estado tela_recusado(Contexto *ctx)
{
    int op;

    imprimir_cabecalho("PAGAMENTO RECUSADO");
    printf("  %s\n", ctx->checkout.mensagem);
    printf("  Seu carrinho foi mantido e nenhum valor foi cobrado.\n");
    printf("\n  1) Alterar forma de pagamento\n");
    printf("  0) Cancelar e voltar ao carrinho\n");

    op = ler_opcao_segura("Escolha: ", 0, 1);
    return (op == 1) ? ESTADO_PAGAMENTO : ESTADO_CARRINHO;
}

/* ===================== MEUS PEDIDOS / RASTREAMENTO ===================== */

Estado tela_meus_pedidos(Contexto *ctx)
{
    int i, achou = 0, uid, op, id, idx;

    if (!esta_logado(ctx)) {
        printf("Faça login para consultar seus pedidos.\n");
        pausar();
        ctx->retorno_login = ESTADO_MEUS_PEDIDOS;
        return ESTADO_AUTENTICACAO;
    }
    uid = ctx->usuarios[ctx->usuario_logado].id;

    imprimir_cabecalho("PAINEL DE MEUS PEDIDOS");
    printf("\n");
    for (i = 0; i < ctx->qtd_pedidos; i++) {
        if (ctx->pedidos[i].id_usuario == uid) {
            printf("  Pedido #%d | R$ %.2f | %s | %s\n", ctx->pedidos[i].id,
                   ctx->pedidos[i].total,
                   nome_forma_pagamento(ctx->pedidos[i].forma_pagamento),
                   nome_status(ctx->pedidos[i].status));
            achou++;
        }
    }
    if (!achou) {
        printf("  Você ainda não possui pedidos.\n");
    }
    printf("\n  1) Visualizar rastreamento logístico\n");
    printf("  0) Página inicial\n");

    op = ler_opcao_segura("Escolha: ", 0, 1);
    if (op == 0) {
        return ESTADO_HOME;
    }
    if (!achou) {
        printf("[!] Não há pedidos para rastrear.\n");
        pausar();
        return ESTADO_MEUS_PEDIDOS;
    }
    id = ler_opcao_segura("Número do pedido: ", 1, 999999);
    idx = indice_pedido_por_id(ctx, id);
    if (idx < 0 || ctx->pedidos[idx].id_usuario != uid) {
        printf("[!] Pedido não encontrado.\n");
        pausar();
        return ESTADO_MEUS_PEDIDOS;
    }
    ctx->pedido_selecionado = id;
    return ESTADO_RASTREAMENTO;
}

Estado tela_rastreamento(Contexto *ctx)
{
    int idx = indice_pedido_por_id(ctx, ctx->pedido_selecionado);

    imprimir_cabecalho("RASTREAMENTO DO ENVIO");
    if (idx < 0) {
        printf("[!] Pedido não encontrado.\n");
    } else {
        exibir_pedido_detalhe(&ctx->pedidos[idx]);
    }
    pausar();
    return ESTADO_MEUS_PEDIDOS;
}
