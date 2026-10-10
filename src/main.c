/*
 * VitaMarket - E-commerce de hardware e modding (Versao Alfa)
 * Projeto Integrador - Programacao Estruturada em C
 *
 * A navegacao e uma maquina de estados de console:
 * enum Estado + while + switch-case. Cada tela devolve o proximo estado.
 */
#include <stdio.h>
#include "tipos.h"
#include "utils.h"
#include "produtos.h"
#include "usuarios.h"
#include "pedidos.h"
#include "telas.h"
#include "admin.h"

int main(void)
{
    static Contexto ctx;            /* static: evita estourar a pilha */
    Estado estado = ESTADO_HOME;

    ctx.usuario_logado = -1;
    ctx.retorno_login = ESTADO_HOME;

    carregar_produtos(&ctx);
    carregar_usuarios(&ctx);
    carregar_pedidos(&ctx);

    while (estado != ESTADO_SAIR) {
        switch (estado) {
        case ESTADO_HOME:           estado = tela_home(&ctx);           break;
        case ESTADO_CATALOGO:       estado = tela_catalogo(&ctx);       break;
        case ESTADO_DETALHES:       estado = tela_detalhes(&ctx);       break;
        case ESTADO_CARRINHO:       estado = tela_carrinho(&ctx);       break;
        case ESTADO_AUTENTICACAO:   estado = tela_autenticacao(&ctx);   break;
        case ESTADO_ENDERECO_FRETE: estado = tela_endereco_frete(&ctx); break;
        case ESTADO_PAGAMENTO:      estado = tela_pagamento(&ctx);      break;
        case ESTADO_PROCESSANDO:    estado = tela_processando(&ctx);    break;
        case ESTADO_CONFIRMADO:     estado = tela_confirmado(&ctx);     break;
        case ESTADO_RECUSADO:       estado = tela_recusado(&ctx);       break;
        case ESTADO_MEUS_PEDIDOS:   estado = tela_meus_pedidos(&ctx);   break;
        case ESTADO_RASTREAMENTO:   estado = tela_rastreamento(&ctx);   break;
        case ESTADO_ADMIN:          estado = tela_admin(&ctx);          break;
        default:                    estado = ESTADO_HOME;               break;
        }
    }

    limpar_tela();
    printf("Obrigado por visitar a VitaMarket! Até logo.\n");
    return 0;
}
