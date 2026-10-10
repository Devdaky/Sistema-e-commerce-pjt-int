#ifndef UTILS_H
#define UTILS_H

/* Tela e buffer */
void limpar_tela(void);
void limpar_buffer(void);
void pausar(void);
void imprimir_linha(char c, int n);
void imprimir_cabecalho(const char *titulo);

/* Leitura segura de dados */
int    ler_opcao_segura(const char *prompt, int min, int max);
void   ler_string(const char *prompt, char *dest, int tam);
int    ler_string_opcional(const char *prompt, char *dest, int tam);
double ler_double_faixa(const char *prompt, double min, double max);
int    ler_double_opcional(const char *prompt, double *valor, double min, double max);
int    ler_confirmacao(const char *prompt);

/* Strings (implementadas a mao: so stdio.h e stdlib.h) */
int  str_tamanho(const char *s);
void str_copiar(char *dest, const char *src, int tam);
int  str_igual(const char *a, const char *b);
int  str_igual_ci(const char *a, const char *b);
int  str_contem_ci(const char *texto, const char *termo);

/* Validacoes */
int           extrair_digitos(const char *origem, char *destino, int tam);
int           validar_cpf(const char *digitos);
int           validar_cep(const char *digitos);
unsigned long calcular_hash(const char *s);

#endif
