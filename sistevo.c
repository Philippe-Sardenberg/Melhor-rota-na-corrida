
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

// --- CONFIGURAÇÕES ---
#define NUM_CHECKPOINTS 100   
#define TAM_POPULACAO 200     
#define MAX_GERACOES 5000     
#define SEED_FIXA 151
#define LIMITE_ESTAGNACAO 150 

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

// --- LÓGICA GENÉTICA E RESET ---
void aplicarResetPopulacional() {
    // Mantém o elite (índice 0) e reseta o resto
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

    qsort(populacao, TAM_POPULACAO, sizeof(Individuo), compararIndividuos);

    if (populacao[0].fitness < melhorFitnessGlobal - 0.1) {
        melhorFitnessGlobal = populacao[0].fitness;
        contadorEstagnacao = 0;
    } else {
        contadorEstagnacao++;
    }

    historicoMelhor[geracaoAtual] = populacao[0].fitness;
    
    float soma = 0;
    for(int i=0; i<TAM_POPULACAO; i++) soma += populacao[i].fitness;
    historicoMedia[geracaoAtual] = soma / TAM_POPULACAO;
    
    historicoReset[geracaoAtual] = false;

    // --- CHECK DO RESET ---
    if (contadorEstagnacao >= LIMITE_ESTAGNACAO && geracaoAtual < MAX_GERACOES - 200) {
        aplicarResetPopulacional();
        contadorEstagnacao = 0;
        qsort(populacao, TAM_POPULACAO, sizeof(Individuo), compararIndividuos);
    }

    int eliteCount = TAM_POPULACAO * 0.05;
    for(int i=0; i<eliteCount; i++) novaPopulacao[i] = populacao[i];

    for (int i = eliteCount; i < TAM_POPULACAO; i++) {
        int idx1 = rand() % (TAM_POPULACAO / 2);
        int idx2 = rand() % TAM_POPULACAO;
        Individuo filho;
        crossover(populacao[idx1], populacao[idx2], &filho);
        if((rand() % 100) < 15) mutacao(&filho);
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
    glViewport(0, 300, 800, 300);
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

    if(geracaoAtual > 0) {
        // Linha da Média (Vermelho Claro)
        glColor3f(0.8, 0.3, 0.3); 
        glBegin(GL_LINE_STRIP);
        for(int i=0; i<geracaoAtual; i++) {
            float val = historicoMedia[i];
            if(val > yMaxGraph) val = yMaxGraph; 
            glVertex2f(i, val);
        }
        glEnd();
        
        // Linha do Melhor (Verde)
        glColor3f(0.0, 1.0, 0.0);
        glLineWidth(1.5);
        glBegin(GL_LINE_STRIP);
        for(int i=0; i<geracaoAtual; i++) glVertex2f(i, historicoMelhor[i]);
        glEnd();
    }

    glColor3f(0.7, 0.7, 0.7);
    for(int i=0; i<=MAX_GERACOES; i+=1000) drawNum(i+50, yMaxGraph*0.02, (float)i);
    
    // --- DADOS (Painel Lateral Direito) ---
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
    
    // Calcula a média para exibição (pega do histórico ou calcula se for a gen 0)
    float mediaExibir = 0.0;
    if (geracaoAtual > 0) mediaExibir = historicoMedia[geracaoAtual - 1];
    else {
        float soma = 0; 
        for(int k=0; k<TAM_POPULACAO; k++) soma += populacao[k].fitness;
        mediaExibir = soma / TAM_POPULACAO;
    }
    
    glColor3f(0.8, 0.3, 0.3); // Mesma cor da linha do gráfico
    sprintf(buf, "Media: %.2f", mediaExibir);
    drawString(0.05, 0.66, GLUT_BITMAP_HELVETICA_18, buf);
    // ===================

    // Resets
    glColor3f(1.0, 1.0, 0.0);
    sprintf(buf, "Resets: %d", totalResets);
    drawString(0.05, 0.58, GLUT_BITMAP_HELVETICA_18, buf);

    // Estagnação
    glColor3f(0.8, 0.8, 0.8);
    sprintf(buf, "Estagnacao: %d / %d", contadorEstagnacao, LIMITE_ESTAGNACAO);
    drawString(0.05, 0.50, GLUT_BITMAP_HELVETICA_12, buf);

    if (simulacaoFinalizada) {
        glColor3f(0.0, 1.0, 0.0);
        drawString(0.05, 0.2, GLUT_BITMAP_TIMES_ROMAN_24, "CONCLUIDO");
    }

    glutSwapBuffers();
}

void timer(int value) {
    if (!pausado && !simulacaoFinalizada) {
        for(int k=0; k<5; k++) evoluirGeracao();
    }
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); 
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