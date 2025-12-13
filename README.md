# Melhor-rota-na-corrida

> **Projeto de Sistemas Evolutivos**  
> Aplicação de Algoritmos Genéticos para otimização de trajeto em corrida com visualização em tempo real via OpenGL, feito para a disciplina disciplina SSC0713 - Sistemas Evolutivos Aplicados à Robótica
> # Sobre o Projeto

Este software resolve uma variação do **Problema do Caixeiro Viajante (TSP)** aplicada a um contexto de corrida. O objetivo é encontrar a sequência ótima para percorrer 10 checkpoints num terreno sem caminho definido, minimizando o tempo total.

Diferente de abordagens padrão, este projeto conta com uma **Interface Gráfica em OpenGL** que exibe:
1.  O mapa e a melhor rota em tempo real.
2.  Gráficos de evolução (Fitness Médio vs Melhor Fitness).
3.  Dados estatísticos com precisão decimal.
4.  Visualização de mecanismos avançados como **Reset Populacional**.

##  Demonstração em Vídeo



---

##  Como funciona

*   **Visualização:** O tempo do melhor de todos e o tempo médio da geração são atualizados a cada geração.
*   **Mecanismo de Cataclismo:** Para explorar mais possibilidades, o sistema detecta estagnação e reseta a população, mantendo apenas o melhor indivíduo.
*   **Elitismo:** O melhor da última geração realiza crossover com cada indivíduo da última geração para produzir o próximo.
*   **Mutação:** Cada indivíduo geradado irá alterar trocar a ordem de checkpoints.

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
