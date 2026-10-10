#include <stdio.h>
#include <stdlib.h>
#include "produtos.h"
#include "utils.h"

typedef struct {
    const char *nome;
    const char *categoria;
    const char *marca;
    double      preco;
    int         estoque;
    const char *specs;
} Semente;

static const Semente SEMENTES[] = {
    {"Cabo Sleeved ATX 24 pinos",     "Cabos Customizados",     "ModMaster", 189.90, 15, "Sleeve PET preto/vermelho, 24 pinos, 30 cm"},
    {"Cabo Extensor GPU 8 pinos",     "Cabos Customizados",     "ModMaster", 129.90, 20, "Sleeve PET, 2x 8 pinos, 30 cm"},
    {"Pente de Cabos 24 pinos",       "Cabos Customizados",     "ModMaster",  39.90, 35, "Cable comb acrilico, organiza ate 24 vias"},
    {"Fita LED ARGB 5V 1m",           "Iluminacao",             "LumiTech",   59.90, 40, "30 LEDs/m, conector 3 pinos 5V, compativel Aura/Mystic"},
    {"Kit 3 Fans 120mm ARGB",         "Cooling",                "CoolFlow",  249.90, 12, "1800 RPM, PWM 4 pinos, ARGB, controladora inclusa"},
    {"Water Cooler 240mm",            "Cooling",                "CoolFlow",  479.90,  8, "Radiador 240mm, soquetes AM4/AM5/LGA1700"},
    {"Pasta Termica 4g",              "Cooling",                "ThermoPro",  34.90, 60, "Condutividade 8.5 W/mK, seringa 4g"},
    {"Teclado Mecanico 65% Hot-swap", "Perifericos Modificados","KeyCraft",  399.90, 10, "Switch linear lubrificado, hot-swap, keycaps PBT"},
    {"Keycaps PBT 130 teclas",        "Perifericos Modificados","KeyCraft",  159.90, 25, "Perfil Cherry, dye-sub, layout ABNT2"},
    {"Mousepad Speed XL",             "Perifericos Modificados","PadWorks",   79.90, 30, "900x400 mm, base antiderrapante, borda costurada"},
    {"Gabinete Mid Tower Vidro",      "Gabinetes",              "CaseLab",   549.90,  6, "Vidro temperado, suporte radiador 360mm, 4 fans"},
    {"Suporte Vertical de GPU",       "Acessorios Modding",     "CaseLab",    89.90, 18, "Aluminio escovado, altura ajustavel, cabo riser nao incluso"}
};

#define QTD_SEMENTES ((int)(sizeof SEMENTES / sizeof SEMENTES[0]))

static void semear_produtos(Contexto *ctx)
{
    int i;
    ctx->qtd_produtos = 0;
    for (i = 0; i < QTD_SEMENTES && i < MAX_PRODUTOS; i++) {
        Produto *p = &ctx->produtos[ctx->qtd_produtos];
        p->id = i + 1;
        str_copiar(p->nome, SEMENTES[i].nome, TAM_NOME);
        str_copiar(p->categoria, SEMENTES[i].categoria, TAM_CATEGORIA);
        str_copiar(p->marca, SEMENTES[i].marca, TAM_MARCA);
        p->preco = SEMENTES[i].preco;
        p->estoque = SEMENTES[i].estoque;
        str_copiar(p->specs, SEMENTES[i].specs, TAM_SPECS);
        ctx->qtd_produtos++;
    }
}

int salvar_produtos(const Contexto *ctx)
{
    int i;
    FILE *f = fopen(ARQ_PRODUTOS, "w");
    if (f == NULL) {
        printf("[!] Nao foi possivel gravar %s.\n", ARQ_PRODUTOS);
        return 0;
    }
    for (i = 0; i < ctx->qtd_produtos; i++) {
        const Produto *p = &ctx->produtos[i];
        fprintf(f, "%d|%s|%s|%s|%.2f|%d|%s\n",
                p->id, p->nome, p->categoria, p->marca, p->preco, p->estoque, p->specs);
    }
    fclose(f);
    return 1;
}

int carregar_produtos(Contexto *ctx)
{
    char linha[400];
    Produto p;
    FILE *f = fopen(ARQ_PRODUTOS, "r");

    ctx->qtd_produtos = 0;
    if (f != NULL) {
        while (fgets(linha, (int)sizeof linha, f) != NULL && ctx->qtd_produtos < MAX_PRODUTOS) {
            if (sscanf(linha, "%d|%49[^|]|%29[^|]|%29[^|]|%lf|%d|%99[^\n]",
                       &p.id, p.nome, p.categoria, p.marca, &p.preco, &p.estoque, p.specs) == 7) {
                ctx->produtos[ctx->qtd_produtos++] = p;
            }
        }
        fclose(f);
    }
    if (ctx->qtd_produtos == 0) {
        semear_produtos(ctx);
        salvar_produtos(ctx);
    }
    return ctx->qtd_produtos;
}

int indice_produto_por_id(const Contexto *ctx, int id)
{
    int i;
    for (i = 0; i < ctx->qtd_produtos; i++) {
        if (ctx->produtos[i].id == id) {
            return i;
        }
    }
    return -1;
}

int proximo_id_produto(const Contexto *ctx)
{
    int i, maior = 0;
    for (i = 0; i < ctx->qtd_produtos; i++) {
        if (ctx->produtos[i].id > maior) {
            maior = ctx->produtos[i].id;
        }
    }
    return maior + 1;
}

int produto_corresponde(const Produto *p, const Filtro *f)
{
    if (f->termo[0] != '\0' &&
        !str_contem_ci(p->nome, f->termo) &&
        !str_contem_ci(p->categoria, f->termo) &&
        !str_contem_ci(p->marca, f->termo) &&
        !str_contem_ci(p->specs, f->termo)) {
        return 0;
    }
    if (f->categoria[0] != '\0' && !str_igual_ci(p->categoria, f->categoria)) {
        return 0;
    }
    if (f->marca[0] != '\0' && !str_igual_ci(p->marca, f->marca)) {
        return 0;
    }
    if (f->preco_min > 0 && p->preco < f->preco_min) {
        return 0;
    }
    if (f->preco_max > 0 && p->preco > f->preco_max) {
        return 0;
    }
    return 1;
}

void exibir_produto_resumo(const Produto *p)
{
    printf(" [%3d] %s\n", p->id, p->nome);
    printf("       %s | %s | R$ %.2f | ", p->categoria, p->marca, p->preco);
    if (p->estoque > 0) {
        printf("Estoque: %d\n", p->estoque);
    } else {
        printf("INDISPONÍVEL\n");
    }
}

int listar_produtos_filtrados(const Contexto *ctx)
{
    int i, achados = 0;
    for (i = 0; i < ctx->qtd_produtos; i++) {
        if (produto_corresponde(&ctx->produtos[i], &ctx->filtro)) {
            exibir_produto_resumo(&ctx->produtos[i]);
            achados++;
        }
    }
    return achados;
}

/* campo 0 = categorias, campo 1 = marcas. Retorna quantos valores distintos. */
int listar_valores_distintos(const Contexto *ctx, int campo,
                             char valores[][TAM_CATEGORIA], int max)
{
    int i, j, n = 0, repetido;
    for (i = 0; i < ctx->qtd_produtos; i++) {
        const char *v = (campo == 0) ? ctx->produtos[i].categoria : ctx->produtos[i].marca;
        repetido = 0;
        for (j = 0; j < n; j++) {
            if (str_igual_ci(valores[j], v)) {
                repetido = 1;
                break;
            }
        }
        if (!repetido && n < max) {
            str_copiar(valores[n], v, TAM_CATEGORIA);
            n++;
        }
    }
    return n;
}
