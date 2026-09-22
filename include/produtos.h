#ifndef PRODUTOS_H
#define PRODUTOS_H

typedef struct
{
    int id;
    char nome[100];
    float preco;
    int estoque;
} Produto;

void cadastrar_produto(void);
void listar_produtos(void);
void buscar_produto(void);

#endif