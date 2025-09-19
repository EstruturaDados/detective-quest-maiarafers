#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_HASH 26

// --------------------------
// Estrutura da Árvore de Salas
typedef struct Sala {
    char nome[50];
    struct Sala* esquerda;
    struct Sala* direita;
} Sala;

Sala* criarSala(const char* nome) {
    Sala* nova = (Sala*)malloc(sizeof(Sala));
    strcpy(nova->nome, nome);
    nova->esquerda = nova->direita = NULL;
    return nova;
}

// --------------------------
// Estrutura da BST de Pistas
typedef struct Pista {
    char texto[100];
    struct Pista* esquerda;
    struct Pista* direita;
} Pista;

Pista* inserirPista(Pista* raiz, const char* texto) {
    if (raiz == NULL) {
        Pista* nova = (Pista*)malloc(sizeof(Pista));
        strcpy(nova->texto, texto);
        nova->esquerda = nova->direita = NULL;
        return nova;
    }

    if (strcmp(texto, raiz->texto) < 0)
        raiz->esquerda = inserirPista(raiz->esquerda, texto);
    else if (strcmp(texto, raiz->texto) > 0)
        raiz->direita = inserirPista(raiz->direita, texto);

    return raiz;
}

void listarPistasEmOrdem(Pista* raiz) {
    if (raiz != NULL) {
        listarPistasEmOrdem(raiz->esquerda);
        printf("- %s\n", raiz->texto);
        listarPistasEmOrdem(raiz->direita);
    }
}

// --------------------------
// Estrutura da Tabela Hash de Suspeitos
typedef struct ListaPistas {
    char pista[100];
    struct ListaPistas* prox;
} ListaPistas;

typedef struct Suspeito {
    char nome[50];
    int contador;
    ListaPistas* pistas;
    struct Suspeito* prox;
} Suspeito;

Suspeito* tabelaHash[TAM_HASH];

void inicializarHash() {
    for (int i = 0; i < TAM_HASH; i++)
        tabelaHash[i] = NULL;
}

int hash(const char* nome) {
    return (nome[0] - 'A') % TAM_HASH;
}

void inserirHash(const char* nomeSuspeito, const char* pista) {
    int indice = hash(nomeSuspeito);
    Suspeito* atual = tabelaHash[indice];

    while (atual != NULL) {
        if (strcmp(atual->nome, nomeSuspeito) == 0)
            break;
        atual = atual->prox;
    }

    if (atual == NULL) {
        atual = (Suspeito*)malloc(sizeof(Suspeito));
        strcpy(atual->nome, nomeSuspeito);
        atual->contador = 0;
        atual->pistas = NULL;
        atual->prox = tabelaHash[indice];
        tabelaHash[indice] = atual;
    }

    ListaPistas* novaPista = (ListaPistas*)malloc(sizeof(ListaPistas));
    strcpy(novaPista->pista, pista);
    novaPista->prox = atual->pistas;
    atual->pistas = novaPista;
    atual->contador++;
}

void listarAssociacoes() {
    printf("\n📋 Suspeitos e suas Pistas:\n");
    for (int i = 0; i < TAM_HASH; i++) {
        Suspeito* atual = tabelaHash[i];
        while (atual != NULL) {
            printf("- %s (%d pistas):\n", atual->nome, atual->contador);
            ListaPistas* lp = atual->pistas;
            while (lp != NULL) {
                printf("  • %s\n", lp->pista);
                lp = lp->prox;
            }
            atual = atual->prox;
        }
    }
}

void suspeitoMaisProvavel() {
    Suspeito* maisCitado = NULL;
    for (int i = 0; i < TAM_HASH; i++) {
        Suspeito* atual = tabelaHash[i];
        while (atual != NULL) {
            if (maisCitado == NULL || atual->contador > maisCitado->contador)
                maisCitado = atual;
            atual = atual->prox;
        }
    }

    if (maisCitado != NULL) {
        printf("\n🔎 Suspeito mais provável: %s (%d pistas)\n", maisCitado->nome, maisCitado->contador);
    }
}

// --------------------------
// Exploração das Salas
void explorarSala(Sala* salaAtual, Pista** raizPistas) {
    char opcao;
    while (salaAtual != NULL) {
        printf("\nVocê está na sala: %s\n", salaAtual->nome);

        // Inserir pista se for uma sala relevante
        if (strcmp(salaAtual->nome, "Biblioteca") == 0) {
            *raizPistas = inserirPista(*raizPistas, "Livro rasgado encontrado");
            inserirHash("Sr. Mustache", "Livro rasgado encontrado");
        } else if (strcmp(salaAtual->nome, "Cozinha") == 0) {
            *raizPistas = inserirPista(*raizPistas, "Faca faltando no suporte");
            inserirHash("Chef Gourmet", "Faca faltando no suporte");
        } else if (strcmp(salaAtual->nome, "Sótão") == 0) {
            *raizPistas = inserirPista(*raizPistas, "Pegadas suspeitas no pó");
            inserirHash("Srta. Sombria", "Pegadas suspeitas no sótão");
        }

        printf("Digite [e]squerda, [d]ireita, [s]air, [p]istas ou [h]ash: ");
        scanf(" %c", &opcao);

        if (opcao == 'e') {
            salaAtual = salaAtual->esquerda;
        } else if (opcao == 'd') {
            salaAtual = salaAtual->direita;
        } else if (opcao == 'p') {
            printf("\n📁 Pistas coletadas (em ordem alfabética):\n");
            listarPistasEmOrdem(*raizPistas);
        } else if (opcao == 'h') {
            listarAssociacoes();
        } else if (opcao == 's') {
            break;
        }
    }
}

// --------------------------
// Função Principal
int main() {
    // Criando as salas fixas da mansão
    Sala* hall = criarSala("Hall de Entrada");
    Sala* biblioteca = criarSala("Biblioteca");
    Sala* cozinha = criarSala("Cozinha");
    Sala* sotao = criarSala("Sótão");

    // Conectando salas (árvore binária fixa)
    hall->esquerda = biblioteca;
    hall->direita = cozinha;
    cozinha->direita = sotao;

    Pista* raizPistas = NULL;
    inicializarHash();

    printf("🎩 Bem-vindo ao Detective Quest!\n");
    explorarSala(hall, &raizPistas);

    suspeitoMaisProvavel();

    return 0;
}
