#ifndef USUARIOS_H
#define USUARIOS_H

#include "tipos.h"

int carregar_usuarios(Contexto *ctx);
int salvar_usuarios(const Contexto *ctx);
int indice_usuario_por_cpf(const Contexto *ctx, const char *cpf);
int indice_usuario_por_id(const Contexto *ctx, int id);
int adicionar_usuario(Contexto *ctx, const char *nome, const char *cpf,
                      const char *senha, int admin);

#endif
