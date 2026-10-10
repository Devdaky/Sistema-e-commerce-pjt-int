#ifndef PAGAMENTO_H
#define PAGAMENTO_H

#include "tipos.h"

/* Simula o Sistema de Pagamento. Retorna 1 (aprovada) ou 0 (recusada)
   e grava a mensagem de retorno em c->mensagem. */
int processar_pagamento(Checkout *c, double valor);

#endif
