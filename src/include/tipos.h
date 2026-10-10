#ifndef TIPOS_H
#define TIPOS_H

/* ---------- Limites do sistema ---------- */
#define MAX_PRODUTOS        200
#define MAX_USUARIOS        100
#define MAX_PEDIDOS         300
#define MAX_ITENS_CARRINHO  30

#define TAM_NOME            50
#define TAM_CATEGORIA       30
#define TAM_MARCA           30
#define TAM_SPECS           100
#define TAM_ENDERECO        80
#define TAM_ENDERECO_PEDIDO 120
#define TAM_RASTREIO        20
#define TAM_MENSAGEM        100

/* ---------- Arquivos de persistencia local ---------- */
#define ARQ_PRODUTOS "produtos.txt"
#define ARQ_USUARIOS "usuarios.txt"
#define ARQ_PEDIDOS  "pedidos.txt"

/* ---------- Regras de frete ---------- */
#define FRETE_PAC           19.90
#define FRETE_SEDEX         39.90
#define FRETE_GRATIS_ACIMA  500.00

/* ---------- Formas de pagamento e status de pedido ---------- */
enum { FORMA_PIX = 1, FORMA_CARTAO = 2, FORMA_BOLETO = 3 };
enum { STATUS_AGUARDANDO = 0, STATUS_ENVIADO = 1, STATUS_ENTREGUE = 2 };

/* ---------- Estados da maquina de navegacao (diagrama 6.2) ---------- */
typedef enum {
    ESTADO_HOME,
    ESTADO_CATALOGO,
    ESTADO_DETALHES,
    ESTADO_CARRINHO,
    ESTADO_AUTENTICACAO,
    ESTADO_ENDERECO_FRETE,
    ESTADO_PAGAMENTO,
    ESTADO_PROCESSANDO,
    ESTADO_CONFIRMADO,
    ESTADO_RECUSADO,
    ESTADO_MEUS_PEDIDOS,
    ESTADO_RASTREAMENTO,
    ESTADO_ADMIN,
    ESTADO_SAIR
} Estado;

/* ---------- Estruturas de dados ---------- */
typedef struct {
    int    id;
    char   nome[TAM_NOME];
    char   categoria[TAM_CATEGORIA];
    char   marca[TAM_MARCA];
    double preco;
    int    estoque;
    char   specs[TAM_SPECS];
} Produto;

typedef struct {
    int           id;
    char          nome[TAM_NOME];
    char          cpf[12];
    unsigned long senha_hash;
    int           admin;
} Usuario;

typedef struct {
    int id_produto;
    int quantidade;
} ItemCarrinho;

typedef struct {
    int    id_produto;
    char   nome[TAM_NOME];
    int    quantidade;
    double preco_unit;
} ItemPedido;

typedef struct {
    int        id;
    int        id_usuario;
    ItemPedido itens[MAX_ITENS_CARRINHO];
    int        qtd_itens;
    double     subtotal;
    double     frete;
    double     total;
    int        forma_pagamento;
    int        status;
    char       rastreio[TAM_RASTREIO];
    char       endereco[TAM_ENDERECO_PEDIDO];
} Pedido;

typedef struct {
    char   termo[TAM_NOME];
    char   categoria[TAM_CATEGORIA];
    char   marca[TAM_MARCA];
    double preco_min;   /* 0 = sem minimo */
    double preco_max;   /* 0 = sem maximo */
} Filtro;

typedef struct {
    char   endereco[TAM_ENDERECO];
    char   cep[9];
    double frete;
    int    forma;
    char   final_cartao[5];
    char   mensagem[TAM_MENSAGEM];
} Checkout;

/* Contexto: agrupa todos os dados do programa (evita variaveis globais) */
typedef struct {
    Produto      produtos[MAX_PRODUTOS];
    int          qtd_produtos;
    Usuario      usuarios[MAX_USUARIOS];
    int          qtd_usuarios;
    Pedido       pedidos[MAX_PEDIDOS];
    int          qtd_pedidos;
    ItemCarrinho carrinho[MAX_ITENS_CARRINHO];
    int          qtd_carrinho;

    int          usuario_logado;      /* indice em usuarios[] ou -1 */
    int          produto_selecionado; /* id do produto em detalhes */
    int          pedido_selecionado;  /* id do pedido em foco */
    Estado       retorno_login;       /* para onde ir apos autenticar */
    Filtro       filtro;
    Checkout     checkout;
} Contexto;

#endif
