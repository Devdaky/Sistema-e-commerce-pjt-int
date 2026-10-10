#include <stdio.h>
#include "pagamento.h"

int processar_pagamento(Checkout *c, double valor)
{
    switch (c->forma) {
    case FORMA_PIX:
        snprintf(c->mensagem, TAM_MENSAGEM, "PIX de R$ %.2f confirmado.", valor);
        return 1;
    case FORMA_BOLETO:
        snprintf(c->mensagem, TAM_MENSAGEM,
                 "Boleto de R$ %.2f gerado e compensacao simulada.", valor);
        return 1;
    case FORMA_CARTAO:
        /* Regra da simulacao: cartao terminado em 0 e recusado */
        if (c->final_cartao[3] == '0') {
            snprintf(c->mensagem, TAM_MENSAGEM,
                     "Cartao final %s recusado pela operadora.", c->final_cartao);
            return 0;
        }
        snprintf(c->mensagem, TAM_MENSAGEM,
                 "Cartao final %s autorizado (R$ %.2f).", c->final_cartao, valor);
        return 1;
    default:
        snprintf(c->mensagem, TAM_MENSAGEM, "Forma de pagamento invalida.");
        return 0;
    }
}
