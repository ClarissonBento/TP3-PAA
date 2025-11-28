#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../headers/crypto.h"

int main(void) {
    srand((unsigned int) time(NULL));
    init_criptoanalise();

    printf("=== TP3 - Criptoanalise (Modular) ===\n");
    
    /* Etapa de Cifragem Opcional */
    encrypt_plaintext_step();

    char filename[256];
    printf("\nArquivo cifrado para analisar (ex: Cerydra_Cifrado.txt): ");
    read_line(filename, sizeof(filename));
    
    if (!load_cipher_text(filename)) return 1;

    while (1) {
        int opcao;
        printf("\n=== MENU ===\n");
        printf("1 - Estado atual\n2 - Analise de Frequencia\n3 - Busca Exata\n");
        printf("4 - Busca Aproximada\n5 - Alterar Chave\n6 - Exportar e Sair\nEscolha: ");
        
        if (scanf("%d", &opcao) != 1) {
            printf("Opcao invalida.\n");
            flush_input();
            continue;
        }
        flush_input();

        switch (opcao) {
            case 1: show_state(); break;
            case 2: frequency_analysis_menu(); break;
            case 3: kmp_search_interactive(); break;
            case 4: approx_search_interactive(); break;
            case 5: alter_key_interactive(); break;
            case 6: export_and_exit(); break;
            default: printf("Invalido.\n");
        }
    }
    return 0;
}