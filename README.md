# Melhor-rota-na-corrida

> **Projeto de Sistemas Evolutivos**  
> Aplicação de Algoritmos Genéticos para otimização de trajeto em corrida com visualização em tempo real via OpenGL, feito para a disciplina disciplina SSC0713 - Sistemas Evolutivos Aplicados à Robótica

Este Projeto foi feito para visualizar como diferentes parâmetros de um sistema evolutivo afetam o desempenho do melhor resultado depois de 5000 gerações de 100 indivíduos. O objetivo é encontrar a sequência ótima para percorrer 100 checkpoints sem um caminho pré-determinado em um problema do tipo **Caixeiro Viajante**.

**Exemplo**

<img width="700" height="839" alt="image" src="https://github.com/user-attachments/assets/fadef65e-5ec1-4e99-b853-10f6ff97c8cd" />


**O que a interface gráfica indica**:
1.  O mapa com melhor rota encontrada.
2.  Gráfico de evolução (tempo médio vs melhor tempo).
3.  A linha vertical amarela no gráfico de evolução marca a ocorrência de um reset populacional.
4.  As cores dos segmentos e pontos seguem a seguinte lógica:
4.1. O ponto branco indica onde a rota começa e termina;
4.2. Os segmentos indicam em qual ordem o segmento fica em relação a um gradiente (azul -> verde -> laranja/vermelho, sendo azul a cor dos segmentos de início e avermelhado a dos segmentos finais)


##  Demonstração em Vídeo



---

##  O que você pode mexer

*   **Pausa:** Você pode pausar o processo a qualquer momento apertando **Espaço** ou **P**
*   **Visualização:** É possível controlar quantas gerações são simuladas antes de cada atualização e quantos frames se espera antes de atualizar.
*   **Mecanismo de Cataclismo:** É possível experimentar com valores diferentes para quantas gerações estagnadas se espera antes de fazer o reset
*   **Propriedades do problema:** O número de checkpoints, total de indivíduos, máximo de gerações e seed usada podem ser trocados para experimentar mais combinações
*   **Mutação:** 
---
## Como o processo evoulutivo ocorre

---

## Instalação e Dependências

O projeto foi desenvolvido em **C** utilizando a biblioteca **FreeGLUT** para renderização gráfica.

####  Linux (Ubuntu/Debian/WSL)
instalar FreeGLUT:
```bash
sudo apt-get install build-essential freeglut3-dev
```
Compilar:
```bash
gcc sistevo.c -o sistevo -lGL -lGLU -lglut -lm
```
rodar:
```bash
./sistevo
