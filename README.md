# Melhor-rota-na-corrida

> **Projeto de Sistemas Evolutivos**  
> Aplicação de Algoritmos Genéticos para otimização de trajeto em corrida com visualização em tempo real via OpenGL, feito para a disciplina disciplina SSC0713 - Sistemas Evolutivos Aplicados à Robótica

Este Projeto foi feito para visualizar como diferentes parâmetros de um sistema evolutivo afetam o desempenho do melhor resultado depois de 5000 gerações de 200 indivíduos. O objetivo é encontrar a sequência ótima para percorrer 100 checkpoints sem um caminho pré-determinado em um problema do tipo **Caixeiro Viajante**.

**Exemplo**

<img width="740" height="560" alt="image" src="https://github.com/user-attachments/assets/d35e7c50-1694-41a9-b173-73effb00ddc9" />

**O que a interface gráfica indica**:
1.  O mapa com melhor rota encontrada.
2.  Gráfico de evolução (tempo médio vs melhor tempo).
3.  A linha vertical amarela no gráfico de evolução marca a ocorrência de um reset populacional.
4.  As cores dos segmentos e pontos seguem a seguinte lógica:
4.1. O ponto branco indica onde a rota começa e termina;
4.2. Os segmentos indicam em qual ordem o segmento fica em relação a um gradiente (azul -> verde -> laranja/vermelho, sendo azul a cor dos segmentos de início e avermelhado a dos segmentos finais)


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
