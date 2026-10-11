#include <stdio.h>
#include <stdlib.h>
#include "usuarios.h"
#include "utils.h"

#define ADMIN_NOME  "Administrador"
#define ADMIN_LOGIN "admin"
#define ADMIN_SENHA "admin123"

int salvar_usuarios(const Contexto *ctx)
{
    int i;
    FILE *f = fopen(ARQ_USUARIOS, "w");
    if (f == NULL) {
        printf("[!] Nao foi possivel gravar %s.\n", ARQ_USUARIOS);
        return 0;
    }
    for (i = 0; i < ctx->qtd_usuarios; i++) {
        const Usuario *u = &ctx->usuarios[i];
        fprintf(f, "%d|%s|%s|%lu|%d\n", u->id, u->nome, u->login, u->senha_hash, u->admin);
    }
    fclose(f);
    return 1;
}

int carregar_usuarios(Contexto *ctx)
{
    char linha[200];
    Usuario u;
    FILE *f = fopen(ARQ_USUARIOS, "r");

    ctx->qtd_usuarios = 0;
    if (f != NULL) {
        while (fgets(linha, (int)sizeof linha, f) != NULL && ctx->qtd_usuarios < MAX_USUARIOS) {
            if (sscanf(linha, "%d|%49[^|]|%19[^|]|%lu|%d",
                       &u.id, u.nome, u.login, &u.senha_hash, &u.admin) == 5) {
                ctx->usuarios[ctx->qtd_usuarios++] = u;
            }
        }
        fclose(f);
    }
    if (ctx->qtd_usuarios == 0) {
        adicionar_usuario(ctx, ADMIN_NOME, ADMIN_LOGIN, ADMIN_SENHA, 1);
        salvar_usuarios(ctx);
    }
    return ctx->qtd_usuarios;
}

int indice_usuario_por_login(const Contexto *ctx, const char *login)
{
    int i;
    for (i = 0; i < ctx->qtd_usuarios; i++) {
        if (str_igual_ci(ctx->usuarios[i].login, login)) {
            return i;
        }
    }
    return -1;
}

int indice_usuario_por_id(const Contexto *ctx, int id)
{
    int i;
    for (i = 0; i < ctx->qtd_usuarios; i++) {
        if (ctx->usuarios[i].id == id) {
            return i;
        }
    }
    return -1;
}

int adicionar_usuario(Contexto *ctx, const char *nome, const char *login,
                      const char *senha, int admin)
{
    int i, maior = 0;
    Usuario *u;

    if (ctx->qtd_usuarios >= MAX_USUARIOS) {
        return -1;
    }
    for (i = 0; i < ctx->qtd_usuarios; i++) {
        if (ctx->usuarios[i].id > maior) {
            maior = ctx->usuarios[i].id;
        }
    }
    u = &ctx->usuarios[ctx->qtd_usuarios];
    u->id = maior + 1;
    str_copiar(u->nome, nome, TAM_NOME);
    str_copiar(u->login, login, TAM_LOGIN);
    u->senha_hash = calcular_hash(senha);
    u->admin = admin;
    ctx->qtd_usuarios++;
    return ctx->qtd_usuarios - 1;
}
