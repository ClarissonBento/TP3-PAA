#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdio.h>

/* Configurações */
#define MAX_TEXT 200000

/* === Utilitários === */
void flush_input(void);
void read_line(char *buf, size_t size);

/* === Criptoanálise (Estado e Arquivos) === */
void init_criptoanalise(void);
void encrypt_plaintext_step(void);
int load_cipher_text(const char *filename);
void show_state(void);
void alter_key_interactive(void);
void export_and_exit(void);

/* === Funcionalidades === */
void frequency_analysis_menu(void);
void kmp_search_interactive(void);
void approx_search_interactive(void);

#endif