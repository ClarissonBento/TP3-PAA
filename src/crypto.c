#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#include "../headers/crypto.h"

/* --- ESTADO GLOBAL (Variáveis estáticas ficam invisíveis fora deste arquivo) --- */
static char cipherText[MAX_TEXT + 1];
static char partialText[MAX_TEXT + 1];
static int cipherLen = 0;
static int cipherToPlain[26]; // Mapeamento: Indice 0='A' cifrado -> Valor='A' claro

static const char *ALL_FILES[12] = {
    "Anaxa.txt", "Cipher.txt", "Mydei.txt", "Cerydra.txt", "Aglaea.txt", 
    "Hyacine.txt", "Tribios.txt", "Dang Heng - Terravox.txt", 
    "Hysilens.txt", "Phainon.txt", "Castorice.txt", "Cyrene.txt"
};

static const char PORTUGUESE_FREQ_ORDER[26] = {
    'A','E','O','S','R','I','N','D','M','U','T','C','L','P',
    'V','G','H','Q','B','F','Z','J','X','K','W','Y'
};

/* --- IMPLEMENTAÇÃO DE UTILITÁRIOS --- */

void flush_input(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}
}

void read_line(char *buf, size_t size) {
    if (fgets(buf, (int)size, stdin) == NULL) {
        buf[0] = '\0'; return;
    }
    size_t len = strlen(buf);
    while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r')) {
        buf[--len] = '\0';
    }
}

/* --- FUNÇÕES INTERNAS (Helpers) --- */

static void update_partial_text(void) {
    for (int i = 0; i < cipherLen; i++) {
        char c = cipherText[i];
        if (c >= 'A' && c <= 'Z') {
            int idx = c - 'A';
            if (cipherToPlain[idx] >= 0) {
                partialText[i] = (char)('A' + cipherToPlain[idx]);
            } else {
                partialText[i] = c;
            }
        } else {
            partialText[i] = c;
        }
    }
    partialText[cipherLen] = '\0';
}

/* --- IMPLEMENTAÇÃO DA CRIPTOANÁLISE --- */

void init_criptoanalise(void) {
    for (int i = 0; i < 26; i++) cipherToPlain[i] = -1;
}

void encrypt_plaintext_step(void) {
    char op[8];
    printf("Deseja cifrar um texto claro agora? (s/n): ");
    read_line(op, sizeof(op));
    if (op[0] != 's' && op[0] != 'S') return;

    char inName[256], outName[256], inPath[512];
    printf("Arquivo de entrada (texto claro): ");
    read_line(inName, sizeof(inName));
    printf("Arquivo de saida: ");
    read_line(outName, sizeof(outName));

    snprintf(inPath, sizeof(inPath), "./entrada/%s", inName);
    FILE *in = fopen(inPath, "r");
    if (!in) in = fopen(inName, "r"); // Tenta local

    if (!in) { perror("Erro ao abrir entrada"); return; }
    
    // Tenta salvar na pasta entrada, se falhar, salva na raiz
    char outPath[512];
    snprintf(outPath, sizeof(outPath), "./entrada/%s", outName);
    FILE *out = fopen(outPath, "w"); 
    if (!out) out = fopen(outName, "w");

    if (!out) { perror("Erro ao criar saida"); fclose(in); return; }

    int shift = (rand() % 25) + 1;
    printf("Texto cifrado com x = %d.\n", shift);

    int ch;
    while ((ch = fgetc(in)) != EOF) {
        unsigned char uc = (unsigned char) ch;
        if (isalpha(uc)) {
            int base = islower(uc) ? 'a' : 'A';
            ch = base + ( (uc - base + shift) % 26 );
        }
        fputc(ch, out);
    }
    fclose(in); fclose(out);
    printf("Salvo.\n");
}

int load_cipher_text(const char *filename) {
    char path[512];
    snprintf(path, sizeof(path), "./entrada/%s", filename);
    FILE *f = fopen(path, "r");
    if (!f) f = fopen(filename, "r");
    
    if (!f) { perror("Erro ao abrir arquivo cifrado"); return 0; }

    cipherLen = 0;
    int ch;
    while ((ch = fgetc(f)) != EOF) {
        if (cipherLen >= MAX_TEXT) break;
        unsigned char uc = (unsigned char) ch;
        if (isalpha(uc)) cipherText[cipherLen++] = (char) toupper(uc);
        else cipherText[cipherLen++] = (char) ch;
    }
    cipherText[cipherLen] = '\0';
    fclose(f);
    update_partial_text();
    return 1;
}

void show_state(void) {
    update_partial_text();
    printf("\n=== Texto criptografado ===\n%s\n", cipherText);
    printf("=== Chave ===\nCIFRA: ABCDEFGHIJKLMNOPQRSTUVWXYZ\nCLARO: ");
    for (int i = 0; i < 26; i++) {
        if (cipherToPlain[i] >= 0) putchar('A' + cipherToPlain[i]);
        else putchar('.');
    }
    printf("\n\n=== Texto parcialmente decifrado ===\n%s\n", partialText);
}

void alter_key_interactive(void) {
    char line[64];
    printf("\n[5] Alterar chave\nInforme CIFRADA seguida da CLARA (ex: H E): ");
    read_line(line, sizeof(line));
    
    char cCipher = 0, cPlain = 0;
    int i = 0;
    while (line[i] && isspace((unsigned char)line[i])) i++;
    if (line[i]) cCipher = toupper((unsigned char)line[i++]);
    while (line[i] && isspace((unsigned char)line[i])) i++;
    if (line[i]) cPlain = toupper((unsigned char)line[i]);

    if (isalpha(cCipher) && isalpha(cPlain)) {
        int idxC = cCipher - 'A';
        int idxP = cPlain - 'A';
        
        // Remove conflitos anteriores
        for(int c=0; c<26; c++) {
            if (cipherToPlain[c] == idxP && c != idxC) cipherToPlain[c] = -1;
        }
        cipherToPlain[idxC] = idxP;
        printf("Registrado: %c -> %c\n", cCipher, cPlain);
        update_partial_text();
    } else {
        printf("Entrada invalida.\n");
    }
}

void export_and_exit(void) {
    char filename[256];
    printf("\n[6] Exportar resultado\nNome do arquivo: ");
    read_line(filename, sizeof(filename));
    
    if (filename[0] != '\0') {
        FILE *f = fopen(filename, "w");
        if (f) {
            update_partial_text();
            fprintf(f, "Chave CIFRA: ABCDEFGHIJKLMNOPQRSTUVWXYZ\n      CLARO: ");
            for (int i = 0; i < 26; i++) 
                fputc(cipherToPlain[i] >= 0 ? 'A' + cipherToPlain[i] : '.', f);
            fprintf(f, "\n\n%s", partialText);
            fclose(f);
            printf("Salvo em '%s'.\n", filename);
        }
    }
    exit(0);
}

/* --- IMPLEMENTAÇÃO DE FREQUÊNCIA --- */

static void print_freq_helper(const long counts[26], long total, const char *ctx) {
    int order[26];
    for(int i=0; i<26; i++) order[i] = i;

    // Sort simples
    for(int i=0; i<25; i++) {
        for(int j=i+1; j<26; j++) {
            if(counts[order[j]] > counts[order[i]]) {
                int tmp = order[i]; order[i] = order[j]; order[j] = tmp;
            }
        }
    }

    printf("\nFrequencias (%s):\n", ctx);
    for(int i=0; i<26; i++) {
        if(counts[order[i]] > 0)
            printf("  %c: %6.2f%%\n", 'A'+order[i], 100.0*counts[order[i]]/total);
    }

    printf("\nChute Automatico:\n");
    int usedPlain[26] = {0};
    for(int i=0; i<26; i++) if(cipherToPlain[i] >= 0) usedPlain[cipherToPlain[i]] = 1;

    int plainPos = 0;
    for(int r=0; r<26; r++) {
        int cIdx = order[r];
        if (counts[cIdx] == 0) break;
        if (cipherToPlain[cIdx] != -1) continue; 

        while (plainPos < 26 && usedPlain[PORTUGUESE_FREQ_ORDER[plainPos]-'A']) 
            plainPos++;
        
        if (plainPos < 26) {
            int pIdx = PORTUGUESE_FREQ_ORDER[plainPos] - 'A';
            cipherToPlain[cIdx] = pIdx;
            usedPlain[pIdx] = 1;
            printf("  %c -> %c\n", 'A'+cIdx, 'A'+pIdx);
        }
    }
    update_partial_text();
}

void frequency_analysis_menu(void) {
    long counts[26] = {0};
    long total = 0;
    int opcao;

    printf("\n[2] Analise de Frequencia\n1 - Arquivo atual\n2 - Todos os arquivos\nOpcao: ");
    if (scanf("%d", &opcao) != 1) { flush_input(); return; }
    flush_input();

    if (opcao == 1) {
        for(int i=0; i<cipherLen; i++) {
            if(isalpha(cipherText[i])) { counts[toupper(cipherText[i])-'A']++; total++; }
        }
        if(total > 0) print_freq_helper(counts, total, "Atual");
    } else if (opcao == 2) {
        for(int i=0; i<12; i++) {
            char path[512];
            snprintf(path, sizeof(path), "./entrada/%s", ALL_FILES[i]);
            FILE *f = fopen(path, "r");
            if(!f) continue;
            int ch;
            while((ch = fgetc(f)) != EOF) {
                if(isalpha(ch)) { counts[toupper(ch)-'A']++; total++; }
            }
            fclose(f);
        }
        if(total > 0) print_freq_helper(counts, total, "Todos");
    }
}

/* --- IMPLEMENTAÇÃO DE BUSCA (KMP & SHIFT-AND) --- */

static void build_lps(const char *pat, int m, int *lps) {
    int len = 0, i = 1;
    lps[0] = 0;
    while (i < m) {
        if (pat[i] == pat[len]) lps[i++] = ++len;
        else {
            if (len != 0) len = lps[len - 1];
            else lps[i++] = 0;
        }
    }
}

void kmp_search_interactive(void) {
    char pattern[256];
    printf("\n[3] Busca Exata (KMP)\nPadrao: ");
    read_line(pattern, sizeof(pattern));
    int m = strlen(pattern);
    if(m==0) return;
    for(int i=0; i<m; i++) pattern[i] = toupper(pattern[i]);

    int *lps = malloc(m * sizeof(int));
    build_lps(pattern, m, lps);

    int i=0, j=0, count=0;
    printf("Posicoes: ");
    while (i < cipherLen) {
        if (pattern[j] == cipherText[i]) { i++; j++; }
        if (j == m) {
            printf("%d ", i - j); count++; j = lps[j - 1];
        } else if (i < cipherLen && pattern[j] != cipherText[i]) {
            if (j != 0) j = lps[j - 1]; else i++;
        }
    }
    printf("\nTotal: %d\n", count);
    free(lps);
}

void approx_search_interactive(void) {
    typedef unsigned long long Word;
    char pattern[128];
    int k;
    
    update_partial_text();
    printf("\n[4] Busca Shift-And\nPadrao: ");
    read_line(pattern, sizeof(pattern));
    int m = strlen(pattern);
    if (m == 0 || m > 63) { printf("Erro: Padrao invalido.\n"); return; }
    for(int i=0; i<m; i++) pattern[i] = toupper(pattern[i]);

    printf("Tolerancia: ");
    if(scanf("%d", &k) != 1) k=0;
    flush_input();

    Word *R = malloc((k + 1) * sizeof(Word));
    Word mask[256];
    for(int i=0; i<=k; i++) R[i] = ~1ULL;
    for(int i=0; i<256; i++) mask[i] = ~0ULL;
    for(int i=0; i<m; i++) mask[(unsigned char)pattern[i]] &= ~(1ULL << i);

    int count = 0;
    printf("Ocorrencias:\n");
    for (int i = 0; i < cipherLen; i++) {
        Word prev = R[0];
        unsigned char c = (unsigned char)partialText[i];
        R[0] |= mask[c];
        R[0] <<= 1;

        for (int e = 1; e <= k; e++) {
            Word tmp = R[e];
            R[e] = (prev & (R[e] | mask[c])) << 1;
            prev = tmp;
        }

        if (0 == (R[k] & (1ULL << m))) {
            int start = i - m + 1;
            char occ[128];
            int len_print = (m > 120) ? 120 : m;
            strncpy(occ, partialText + start, len_print);
            occ[len_print] = '\0';
            printf("  Pos %d: %s\n", start, occ);
            count++;
        }
    }
    printf("Total: %d\n", count);
    free(R);
}