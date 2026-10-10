#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

/* ===================== Tela e buffer ===================== */

void limpar_tela(void)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* Descarta tudo o que sobrou na linha de entrada (no lugar de fflush(stdin)) */
void limpar_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

void pausar(void)
{
    printf("\nPressione ENTER para continuar...");
    limpar_buffer();
}

void imprimir_linha(char c, int n)
{
    int i;
    for (i = 0; i < n; i++) {
        putchar(c);
    }
    putchar('\n');
}

void imprimir_cabecalho(const char *titulo)
{
    limpar_tela();
    imprimir_linha('=', 60);
    printf("  %s\n", titulo);
    imprimir_linha('=', 60);
}

/* ===================== Strings ===================== */

int str_tamanho(const char *s)
{
    int n = 0;
    while (s[n] != '\0') {
        n++;
    }
    return n;
}

void str_copiar(char *dest, const char *src, int tam)
{
    int i = 0;
    while (i < tam - 1 && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

int str_igual(const char *a, const char *b)
{
    int i = 0;
    while (a[i] != '\0' && a[i] == b[i]) {
        i++;
    }
    return a[i] == b[i];
}

static char minusculo(char c)
{
    if (c >= 'A' && c <= 'Z') {
        return (char)(c - 'A' + 'a');
    }
    return c;
}

int str_igual_ci(const char *a, const char *b)
{
    int i = 0;
    while (a[i] != '\0' && minusculo(a[i]) == minusculo(b[i])) {
        i++;
    }
    return minusculo(a[i]) == minusculo(b[i]);
}

int str_contem_ci(const char *texto, const char *termo)
{
    int i, j;
    if (termo[0] == '\0') {
        return 1;
    }
    for (i = 0; texto[i] != '\0'; i++) {
        j = 0;
        while (termo[j] != '\0' && texto[i + j] != '\0' &&
               minusculo(texto[i + j]) == minusculo(termo[j])) {
            j++;
        }
        if (termo[j] == '\0') {
            return 1;
        }
    }
    return 0;
}

/* ===================== Leitura segura ===================== */

static void aparar(char *s)
{
    int i = 0, ini = 0, fim;
    while (s[ini] == ' ' || s[ini] == '\t') {
        ini++;
    }
    while (s[ini + i] != '\0') {
        s[i] = s[ini + i];
        i++;
    }
    s[i] = '\0';
    fim = i - 1;
    while (fim >= 0 && (s[fim] == ' ' || s[fim] == '\t')) {
        s[fim] = '\0';
        fim--;
    }
}

/* Le uma linha inteira com fgets. Se a linha for maior que o buffer,
   descarta o excesso com limpar_buffer() (evita "lixo" na proxima leitura). */
static void ler_linha(char *buf, int tam)
{
    int i = 0;
    if (fgets(buf, tam, stdin) == NULL) {
        printf("\nEntrada encerrada. Ate logo!\n");
        exit(0);
    }
    while (buf[i] != '\0' && buf[i] != '\n') {
        i++;
    }
    if (buf[i] == '\n') {
        buf[i] = '\0';
        if (i > 0 && buf[i - 1] == '\r') {
            buf[i - 1] = '\0';
        }
    } else {
        limpar_buffer();
    }
    aparar(buf);
}

/* O caractere '|' e o separador dos arquivos: troca por '/' */
static void higienizar(char *s)
{
    int i;
    for (i = 0; s[i] != '\0'; i++) {
        if (s[i] == '|') {
            s[i] = '/';
        }
    }
}

int ler_opcao_segura(const char *prompt, int min, int max)
{
    char buf[32];
    char *fim;
    long v;

    while (1) {
        printf("%s", prompt);
        ler_linha(buf, (int)sizeof buf);
        if (buf[0] == '\0') {
            printf("[!] Digite um numero entre %d e %d.\n", min, max);
            continue;
        }
        v = strtol(buf, &fim, 10);
        if (*fim != '\0' || v < min || v > max) {
            printf("[!] Opcao invalida. Digite um numero entre %d e %d.\n", min, max);
            continue;
        }
        return (int)v;
    }
}

void ler_string(const char *prompt, char *dest, int tam)
{
    while (1) {
        printf("%s", prompt);
        ler_linha(dest, tam);
        higienizar(dest);
        if (dest[0] == '\0') {
            printf("[!] Campo obrigatorio.\n");
            continue;
        }
        return;
    }
}

int ler_string_opcional(const char *prompt, char *dest, int tam)
{
    printf("%s", prompt);
    ler_linha(dest, tam);
    higienizar(dest);
    return str_tamanho(dest);
}

static int converter_double(const char *s, double *saida)
{
    char tmp[40];
    char *fim;
    int i = 0;
    while (s[i] != '\0' && i < 39) {
        tmp[i] = (s[i] == ',') ? '.' : s[i];
        i++;
    }
    tmp[i] = '\0';
    if (i == 0) {
        return 0;
    }
    *saida = strtod(tmp, &fim);
    return *fim == '\0';
}

double ler_double_faixa(const char *prompt, double min, double max)
{
    char buf[40];
    double v;
    while (1) {
        printf("%s", prompt);
        ler_linha(buf, (int)sizeof buf);
        if (!converter_double(buf, &v) || v < min || v > max) {
            printf("[!] Valor invalido. Digite um numero entre %.2f e %.2f.\n", min, max);
            continue;
        }
        return v;
    }
}

int ler_double_opcional(const char *prompt, double *valor, double min, double max)
{
    char buf[40];
    double v;
    while (1) {
        printf("%s", prompt);
        ler_linha(buf, (int)sizeof buf);
        if (buf[0] == '\0') {
            return 0;
        }
        if (!converter_double(buf, &v) || v < min || v > max) {
            printf("[!] Valor invalido. Digite um numero entre %.2f e %.2f (ou ENTER).\n", min, max);
            continue;
        }
        *valor = v;
        return 1;
    }
}

int ler_confirmacao(const char *prompt)
{
    char buf[16];
    while (1) {
        printf("%s (S/N): ", prompt);
        ler_linha(buf, (int)sizeof buf);
        if (str_igual_ci(buf, "s") || str_igual_ci(buf, "sim")) {
            return 1;
        }
        if (str_igual_ci(buf, "n") || str_igual_ci(buf, "nao")) {
            return 0;
        }
        printf("[!] Responda com S ou N.\n");
    }
}

/* ===================== Validacoes ===================== */

/* Copia apenas os digitos de 'origem'. Retorna quantos digitos existiam. */
int extrair_digitos(const char *origem, char *destino, int tam)
{
    int i, n = 0;
    for (i = 0; origem[i] != '\0'; i++) {
        if (origem[i] >= '0' && origem[i] <= '9') {
            if (n < tam - 1) {
                destino[n] = origem[i];
            }
            n++;
        }
    }
    destino[(n < tam - 1) ? n : tam - 1] = '\0';
    return n;
}

int validar_cpf(const char *d)
{
    int i, j, soma, dv, iguais = 1;

    if (str_tamanho(d) != 11) {
        return 0;
    }
    for (i = 0; i < 11; i++) {
        if (d[i] < '0' || d[i] > '9') {
            return 0;
        }
        if (d[i] != d[0]) {
            iguais = 0;
        }
    }
    if (iguais) {
        return 0;
    }
    for (j = 9; j <= 10; j++) {
        soma = 0;
        for (i = 0; i < j; i++) {
            soma += (d[i] - '0') * (j + 1 - i);
        }
        dv = (soma * 10) % 11;
        if (dv == 10) {
            dv = 0;
        }
        if (dv != d[j] - '0') {
            return 0;
        }
    }
    return 1;
}

int validar_cep(const char *d)
{
    int i;
    if (str_tamanho(d) != 8) {
        return 0;
    }
    for (i = 0; i < 8; i++) {
        if (d[i] < '0' || d[i] > '9') {
            return 0;
        }
    }
    return 1;
}

/* Hash djb2 (simples, adequado para a versao alfa) */
unsigned long calcular_hash(const char *s)
{
    unsigned long h = 5381UL;
    int i;
    for (i = 0; s[i] != '\0'; i++) {
        h = h * 33UL + (unsigned char)s[i];
    }
    return h;
}
