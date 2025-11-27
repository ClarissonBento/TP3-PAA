#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

// Função para realizar a cifra de deslocamento
void preparar_dados_grupo4() {
    // Definição dos arquivos
    // Grupo 4 usa o arquivo Cerydra.txt
    const char *NOME_ARQUIVO_ENTRADA = "entradas/Cerydra.txt";
    const char *NOME_ARQUIVO_SAIDA = "saidas/texto_criptografado.txt";
    
    FILE *entrada, *saida;
    int ch;
    int shift;

    // Inicializa o gerador de números aleatórios
    srand(time(NULL));

    // Gera um shift aleatório entre 1 e 25 (evita 0 pois não criptografaria nada)
    // Fonte 32: "usando um número aleatório x"
    shift = (rand() % 25) + 1; 

    // Abre o arquivo de entrada
    entrada = fopen(NOME_ARQUIVO_ENTRADA, "r");
    if (entrada == NULL) {
        printf("Erro: Nao foi possivel abrir o arquivo '%s'.\n", NOME_ARQUIVO_ENTRADA);
        exit(1);
    }

    // Abre o arquivo de saída
    saida = fopen(NOME_ARQUIVO_SAIDA, "w");
    if (saida == NULL) {
        printf("Erro: Nao foi possivel criar o arquivo de saida.\n");
        fclose(entrada);
        exit(1);
    }

    printf("=== Preparacao de Dados (Grupo 4) ===\n");
    printf("Lendo: %s\n", NOME_ARQUIVO_ENTRADA);
    printf("Shift aleatorio gerado: %d\n", shift);

    // Loop de leitura e criptografia
    while ((ch = fgetc(entrada)) != EOF) {
        
        // Verifica se é uma letra
        if (isalpha(ch)) {
            // Normaliza para maiúscula para facilitar a criptoanálise posterior
            char base = isupper(ch) ? 'A' : 'a';
            
            // Fórmula da Cifra de César: (Letra + Shift) % 26
            char cifrado = ((ch - base + shift) % 26) + base;
            
            // Converte tudo para maiúscula no arquivo final (padrão em criptoanálise clássica)
            fputc(toupper(cifrado), saida);
        } else {
            fputc(ch, saida);
        }
    }

    printf("Sucesso! Arquivo '%s' gerado.\n", NOME_ARQUIVO_SAIDA);
    printf("Voce deve usar este arquivo gerado para testar as funcoes de quebra de senha.\n");

    // Fecha os arquivos
    fclose(entrada);
    fclose(saida);
}

int main() {
    preparar_dados_grupo4();
    return 0;
}