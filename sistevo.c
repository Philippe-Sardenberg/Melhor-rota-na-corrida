#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

// --- CONFIGURAÇÕES DE EVOLUÇÃO ---
#define NUM_CHECKPOINTS 100   
#define TAM_POPULACAO 100     
#define MAX_GERACOES 5000    
#define SEED_FIXA 151

// --- CONFIGURAÇÕES DE COMPORTAMENTO GENÉTICO ---
#define LIMITE_ESTAGNACAO 150   // Quantas gerações estagnadas se espera antes do reset populacional
#define USAR_CROSSOVER 1       // 1 = Sim, 0 = Apenas clona (respeitando seleção) e muta
#define TAXA_ELITE 0.05        // % que passa direto para a próxima geração
#define PROB_MUTACAO 25        // Chance de mutação
#define MODO_SELECAO_PAIS 1   // 0 = usa  o melhor de todos como pai1, 1 = usa um aleatório entre os N% melhores como pai1
#define TOP_N_PERCENT 50       // Se MODO for 1, define o tamanho do grupo de elite para cruza

// --- CONFIGURAÇÕES DE VISUALIZAÇÃO ---
#define GERACOES_ENTRE_ATUALIZACOES 5 // Quantas gerações calcula antes atualizar
#define FRAMES_ENTRE_ATUALIZACOES 16 // Quantos frames espera antes atualizar

// --- VARIÁVEIS GLOBAIS ---
float tempos[NUM_CHECKPOINTS][NUM_CHECKPOINTS];
float coords[NUM_CHECKPOINTS][2]; 

typedef struct {
    int genes[NUM_CHECKPOINTS];
    float fitness;
} Individuo;

Individuo populacao[TAM_POPULACAO];
Individuo novaPopulacao[TAM_POPULACAO];

int geracaoAtual = 0;
bool simulacaoFinalizada = false;
bool pausado = false; 
float melhorFitnessGlobal = 999999.0;
int contadorEstagnacao = 0;
int totalResets = 0;

// Histórico
float historicoMelhor[MAX_GERACOES];
float historicoMedia[MAX_GERACOES];
bool historicoReset[MAX_GERACOES];
float maxFitnessHistorico = 0; 

// --- FUNÇÕES AUXILIARES ---
void swap(int *a, int *b) {
    int temp = *a; *a = *b; *b = temp;
}

bool existeNoArray(int val, int *arr, int tam) {
    for (int i = 0; i < tam; i++) if (arr[i] == val) return true;
    return false;
}

void calcularFitness(Individuo *ind) {
    float tempoTotal = 0.0;
    for (int i = 0; i < NUM_CHECKPOINTS - 1; i++) {
        tempoTotal += tempos[ind->genes[i]][ind->genes[i+1]];
    }
    tempoTotal += tempos[ind->genes[NUM_CHECKPOINTS-1]][ind->genes[0]];
    ind->fitness = tempoTotal;
}

void inicializarDados() {
    srand(SEED_FIXA);
    
    // 1. Gera coordenadas
    for (int i = 0; i < NUM_CHECKPOINTS; i++) {
        coords[i][0] = (float)rand() / RAND_MAX * 0.8 + 0.1; 
        coords[i][1] = (float)rand() / RAND_MAX * 0.8 + 0.1; 
    }

    // 2. Calcula matriz de custos
    for (int i = 0; i < NUM_CHECKPOINTS; i++) {
        for (int j = 0; j < NUM_CHECKPOINTS; j++) {
            if (i == j) {
                tempos[i][j] = 0.0;
            } else {
                float dx = coords[i][0] - coords[j][0];
                float dy = coords[i][1] - coords[j][1];
                float dist = sqrt(dx*dx + dy*dy);
                tempos[i][j] = dist * 100.0; 
            }
        }
    }
}

void criarIndividuoAleatorio(Individuo *ind) {
    for (int j = 0; j < NUM_CHECKPOINTS; j++) ind->genes[j] = j;
    for (int j = 0; j < NUM_CHECKPOINTS; j++) {
        int r = rand() % NUM_CHECKPOINTS;
        swap(&ind->genes[j], &ind->genes[r]);
    }
    calcularFitness(ind);
}

void inicializarPopulacao() {
    maxFitnessHistorico = 0;
    for (int i = 0; i < TAM_POPULACAO; i++) {
        criarIndividuoAleatorio(&populacao[i]);
        if(populacao[i].fitness > maxFitnessHistorico) 
            maxFitnessHistorico = populacao[i].fitness;
    }
}

// --- RESET POPULACIONAL ---
void aplicarResetPopulacional() {
    // Mantém o melhor de todos (índice 0) e reseta o resto
    for (int i = 1; i < TAM_POPULACAO; i++) {
        criarIndividuoAleatorio(&populacao[i]);
    }
    totalResets++;
    historicoReset[geracaoAtual] = true;
    printf(">> RESET POPULACIONAL APLICADO na Geracao %d\n", geracaoAtual);
}

void crossover(Individuo pai1, Individuo pai2, Individuo *filho) {
    int pontoCorte = NUM_CHECKPOINTS / 2;
    for (int i = 0; i < pontoCorte; i++) filho->genes[i] = pai1.genes[i];
    int k = pontoCorte;
    for (int i = 0; i < NUM_CHECKPOINTS; i++) {
        int gene = pai2.genes[i];
        if (!existeNoArray(gene, filho->genes, k)) {
            filho->genes[k] = gene;
            k++;
        }
    }
}

void mutacao(Individuo *ind) {
    for(int m=0; m<2; m++) {
        int i = rand() % NUM_CHECKPOINTS;
        int j = (i + 1) % NUM_CHECKPOINTS;
        if(rand()%2 == 0) j = rand() % NUM_CHECKPOINTS;
        swap(&ind->genes[i], &ind->genes[j]);
    }
}

int compararIndividuos(const void *a, const void *b) {
    float diff = ((Individuo*)a)->fitness - ((Individuo*)b)->fitness;
    if (diff < 0) return -1;
    if (diff > 0) return 1;
    return 0;
}

void evoluirGeracao() {
    if (geracaoAtual >= MAX_GERACOES) {
        simulacaoFinalizada = true;
        return;
    }

    // 1. Ordena (Melhores no início do array)
    qsort(populacao, TAM_POPULACAO, sizeof(Individuo), compararIndividuos);

    // 2. Verifica Estagnação
    if (populacao[0].fitness < melhorFitnessGlobal - 0.1) {
        melhorFitnessGlobal = populacao[0].fitness;
        contadorEstagnacao = 0;
    } else {
        contadorEstagnacao++;
    }

    // 3. Atualiza Histórico
    historicoMelhor[geracaoAtual] = populacao[0].fitness;
    float soma = 0;
    for(int i=0; i<TAM_POPULACAO; i++) soma += populacao[i].fitness;
    historicoMedia[geracaoAtual] = soma / TAM_POPULACAO;
    historicoReset[geracaoAtual] = false;

    // 4. Checa se precisa Resetar
    if (contadorEstagnacao >= LIMITE_ESTAGNACAO && geracaoAtual < MAX_GERACOES - 200) {
        aplicarResetPopulacional();
        contadorEstagnacao = 0;
        qsort(populacao, TAM_POPULACAO, sizeof(Individuo), compararIndividuos);
    }

    // 5. Elitismo (Preserva os melhores intactos)
    int eliteCount = (int)(TAM_POPULACAO * TAXA_ELITE);
    if (TAXA_ELITE > 0 && eliteCount == 0) eliteCount = 1; // Garante ao menos 1
    for(int i=0; i<eliteCount; i++) novaPopulacao[i] = populacao[i];

    // 6. Cruzamento e Mutação (Para o resto da população)
    
    // Calcula quantos compõem o grupo "Top N%" para seleção de pais
    int limitePais = (int)(TAM_POPULACAO * (TOP_N_PERCENT / 100.0));
    if (limitePais < 1) limitePais = 1; 

    for (int i = eliteCount; i < TAM_POPULACAO; i++) {
        int idx1;
        
        // --- LÓGICA DE SELEÇÃO DO PAI 1 ---
        if (MODO_SELECAO_PAIS == 0) {
            // Modo: Melhor cruza com todos
            idx1 = 0; 
        } else {
            // Modo: Top N% cruza com todos
            // Escolhe aleatoriamente alguém dentro dos melhores N%
            idx1 = rand() % limitePais;
        }

        // Pai 2 continua sendo aleatório da população inteira para manter diversidade
        int idx2 = rand() % TAM_POPULACAO;

        Individuo filho;

        if (USAR_CROSSOVER) {
            crossover(populacao[idx1], populacao[idx2], &filho);
        } else {
            // Se sem crossover, clona o Pai 2 (para variar mais, já que Pai 1 é muito elitista)
            filho = populacao[idx2];
        }

        if((rand() % 100) < PROB_MUTACAO) mutacao(&filho);
        
        calcularFitness(&filho);
        novaPopulacao[i] = filho;
    }

    for(int i=0; i<TAM_POPULACAO; i++) populacao[i] = novaPopulacao[i];
    geracaoAtual++;
}

// --- CONTROLE E VISUALIZAÇÃO ---
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) exit(0);
    if (key == 'p' || key == 'P' || key == ' ') pausado = !pausado;
}

void drawString(float x, float y, void *font, char *string) {
    glRasterPos2f(x, y);
    for (char *c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}

void drawNum(float x, float y, float num) {
    char buf[20];
    sprintf(buf, "%.0f", num);
    drawString(x, y, GLUT_BITMAP_HELVETICA_10, buf);
}

void setRouteColor(float t) {
    if (t < 0.5) {
        float localT = t * 2.0;
        glColor3f(0.0, localT, 1.0 - localT); 
    } else {
        float localT = (t - 0.5) * 2.0;
        glColor3f(localT, 1.0 - (localT * 0.5), 0.0);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (pausado) {
        glClearColor(0.2, 0.2, 0.25, 1.0); 
    } else {
        glClearColor(0.1, 0.1, 0.1, 1.0);
    }

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // --- MAPA (Visualização da Rota) ---
    glViewport(150, 300, 600, 600);
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluOrtho2D(0, 1, 0, 1);

    glLineWidth(1.5);
    glBegin(GL_LINE_STRIP); 
    for(int i=0; i<NUM_CHECKPOINTS; i++) {
        float t = (float)i / (float)(NUM_CHECKPOINTS - 1);
        setRouteColor(t);
        int geneAtual = populacao[0].genes[i];
        glVertex2f(coords[geneAtual][0], coords[geneAtual][1]);
    }
    setRouteColor(1.0);
    glVertex2f(coords[populacao[0].genes[0]][0], coords[populacao[0].genes[0]][1]);
    glEnd();

    glPointSize(5.0);
    glBegin(GL_POINTS);
    for(int i=0; i<NUM_CHECKPOINTS; i++) {
        if(i == populacao[0].genes[0]) glColor3f(1.0, 1.0, 1.0); 
        else glColor3f(0.4, 0.4, 0.4);
        glVertex2f(coords[i][0], coords[i][1]);
    }
    glEnd();
    
    if(pausado) {
        glColor3f(1.0, 1.0, 0.0);
        drawString(0.45, 0.5, GLUT_BITMAP_TIMES_ROMAN_24, "PAUSADO");
    }

    glDisable(GL_LINE_SMOOTH); 

    // --- GRÁFICOS (Evolução Fitness) ---
    glViewport(40, 30, 460, 250);
    glLoadIdentity();
    float yMaxGraph = maxFitnessHistorico > 0 ? maxFitnessHistorico * 1.05 : 100.0;
    gluOrtho2D(0.0, MAX_GERACOES, 0.0, yMaxGraph); 

    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(1.0);
    glBegin(GL_LINES);
    for(int i=0; i<=MAX_GERACOES; i+=500) { glVertex2f(i, 0); glVertex2f(i, yMaxGraph); }
    glEnd();

    // Linhas verticais amarelas indicando Resets no gráfico
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_LINES);
    for(int i=0; i<geracaoAtual; i++) {
        if(historicoReset[i]) { glVertex2f(i, 0); glVertex2f(i, yMaxGraph); }
    }
    glEnd();
    // Linhas das distâncias encontradas
    if(geracaoAtual > 0) {
        glColor3f(0.8, 0.3, 0.3); 
        glBegin(GL_LINE_STRIP);
        for(int i=0; i<geracaoAtual; i++) {
            float val = historicoMedia[i];
            if(val > yMaxGraph) val = yMaxGraph; 
            glVertex2f(i, val);
        }
        glEnd();
        
        glColor3f(0.0, 1.0, 0.0);
        glLineWidth(1.5);
        glBegin(GL_LINE_STRIP);
        for(int i=0; i<geracaoAtual; i++) glVertex2f(i, historicoMelhor[i]);
        glEnd();
    }

    glColor3f(0.7, 0.7, 0.7);
    for(int i=0; i<=MAX_GERACOES; i+=1000) drawNum(i+50, yMaxGraph*0.02, (float)i);
    
    // --- DADOS (Painel Canto Direito) ---
    glViewport(500, 0, 300, 300);
    glLoadIdentity(); gluOrtho2D(0, 1, 0, 1);

    char buf[100];
    glColor3f(0.6, 0.6, 1.0);
    drawString(0.05, 0.9, GLUT_BITMAP_TIMES_ROMAN_24, "ESTATISTICAS");
    
    // Geração
    glColor3f(1.0, 1.0, 1.0);
    sprintf(buf, "Geracao: %d / %d", geracaoAtual, MAX_GERACOES);
    drawString(0.05, 0.82, GLUT_BITMAP_HELVETICA_18, buf);
    
    // Melhor Fitness
    glColor3f(0.0, 1.0, 0.0);
    sprintf(buf, "Melhor: %.2f", populacao[0].fitness);
    drawString(0.05, 0.74, GLUT_BITMAP_HELVETICA_18, buf);
    
    // Média
    float mediaExibir = 0.0;
    if (geracaoAtual > 0) mediaExibir = historicoMedia[geracaoAtual - 1];
    else {
        float soma = 0; 
        for(int k=0; k<TAM_POPULACAO; k++) soma += populacao[k].fitness;
        mediaExibir = soma / TAM_POPULACAO;
    }
    
    glColor3f(0.8, 0.3, 0.3); 
    sprintf(buf, "Media: %.2f", mediaExibir);
    drawString(0.05, 0.66, GLUT_BITMAP_HELVETICA_18, buf);

    // Resets e Estagnação
    glColor3f(1.0, 1.0, 0.0);
    sprintf(buf, "Resets: %d", totalResets);
    drawString(0.05, 0.58, GLUT_BITMAP_HELVETICA_18, buf);

    glColor3f(0.8, 0.8, 0.8);
    sprintf(buf, "Estagnacao: %d / %d", contadorEstagnacao, LIMITE_ESTAGNACAO);
    drawString(0.05, 0.50, GLUT_BITMAP_HELVETICA_12, buf);

    // Exibe Configuração Atual (melhor de todos ou N% melhores)
    glColor3f(0.5, 0.5, 0.5);
    if(MODO_SELECAO_PAIS == 0) sprintf(buf, "Modo: Best x All");
    else sprintf(buf, "Modo: Top %d%% x All", TOP_N_PERCENT);
    drawString(0.05, 0.42, GLUT_BITMAP_HELVETICA_10, buf);

    if (simulacaoFinalizada) {
        glColor3f(0.0, 1.0, 0.0);
        drawString(0.05, 0.2, GLUT_BITMAP_TIMES_ROMAN_24, "CONCLUIDO");
    }

    glutSwapBuffers();
}

// --- TIMER E VISUALIZAÇÃO ---
void timer(int value) {
    if (!pausado && !simulacaoFinalizada) {
        for(int k=0; k<GERACOES_ENTRE_ATUALIZACOES; k++) evoluirGeracao();
    }
    glutPostRedisplay();
    glutTimerFunc(FRAMES_ENTRE_ATUALIZACOES, timer, 0); 
}

int main(int argc, char** argv) {
    inicializarDados();
    inicializarPopulacao();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(900, 600);
    glutCreateWindow("TSP Otimizado");
    
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, timer, 0);
    
    glutMainLoop();
    return 0;
}
