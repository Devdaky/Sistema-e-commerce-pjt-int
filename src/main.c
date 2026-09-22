#include <stdio.h>

int main(void)
{
    int opcao;

    do
    {
        printf("\n");
        printf("=================================\n");
        printf("          VITAMARKET\n");
        printf("=================================\n");
        printf("1 - Produtos\n");
        printf("2 - Carrinho\n");
        printf("3 - Pedidos\n");
        printf("4 - Login\n");
        printf("5 - Administracao\n");
        printf("0 - Sair\n");
        printf("=================================\n");
        printf("Escolha uma opcao: ");

        scanf("%d", &opcao);

        switch (opcao)
        {
            case 1:
                printf("\nModulo de produtos.\n");
                break;

            case 2:
                printf("\nModulo de carrinho.\n");
                break;

            case 3:
                printf("\nModulo de pedidos.\n");
                break;

            case 4:
                printf("\nModulo de login.\n");
                break;

            case 5:
                printf("\nModulo administrativo.\n");
                break;

            case 0:
                printf("\nEncerrando VitaMarket...\n");
                break;

            default:
                printf("\nOpcao invalida.\n");
        }

    } while (opcao != 0);

    return 0;
}

#include <stdio.h>
#include "produtos.h"

int main(void)
{
    listar_produtos();

    return 0;
}