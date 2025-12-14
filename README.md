# Melhor-rota-na-corrida

> **Projeto de Sistemas Evolutivos**  
> Aplicação de Algoritmos Genéticos para otimização de trajeto em corrida com visualização em tempo real via OpenGL, feito para a disciplina disciplina SSC0713 - Sistemas Evolutivos Aplicados à Robótica

Este Projeto foi feito para visualizar como diferentes parâmetros de um sistema evolutivo afetam o desempenho do melhor resultado depois de 5000 gerações de 100 indivíduos. O objetivo é encontrar a sequência ótima para percorrer 100 checkpoints sem um caminho pré-determinado em um problema do tipo **Caixeiro Viajante**.

**Exemplo**

<img width="700" height="839" alt="image" src="https://github.com/user-attachments/assets/a0b4009e-51d6-4bc1-b5b4-3e71f7987946" />



**O que a interface gráfica indica**:
1.  O mapa com melhor rota encontrada.
2.  Gráfico de evolução (tempo médio vs melhor tempo).
3.  A linha vertical amarela no gráfico de evolução marca a ocorrência de um reset populacional.
4.  As cores dos segmentos e pontos seguem a seguinte lógica:
4.1. O ponto branco indica onde a rota começa e termina;
4.2. Os segmentos indicam em qual ordem o segmento fica em relação a um gradiente (azul -> verde -> laranja/vermelho, sendo azul a cor dos segmentos de início e avermelhado a dos segmentos finais)


##  Demonstração em Vídeo

https://drive.google.com/file/d/1nQv0KNohOFpBERNqWrQfLPW8AGAOudZJ/view?usp=sharing
(Obs: Em uma parte eu troco usar_crossover com modo_selecao_pais.)

---

##  O que você pode mexer

*   **Pausa:** Você pode pausar o processo a qualquer momento apertando **Espaço** ou **P**
*   **Visualização:** É possível controlar quantas gerações são simuladas antes de cada atualização e quantos frames se espera antes de atualizar.
*   **Mecanismo de Cataclismo:** É possível experimentar com valores diferentes para quantas gerações estagnadas se espera antes de fazer o reset
*   **Propriedades do problema:** O número de checkpoints, total de indivíduos, máximo de gerações e seed usada podem ser trocados para experimentar mais combinações
*   **Mutação:** A probabilidade de ocorrer a mutação (troca de um checkpoint com outro)
*   **Crossover:** Se ocorre ou não crossover e como ele ocorre (melhor de todos com qualquer um ou N% dos melhores com qualquer um)
*   **Taxa na elite:** Controla qual % dos melhores indivíduos vão direto para a próxima geração (útil para preservar diversidade) 
---
## Como o processo evolutivo ocorre

1. Representação e Inicialização

    Genoma: Cada indivíduo possui um array de inteiros representando a ordem de visitação dos checkpoints (uma permutação de 0 a 99).

    População: O sistema inicia com 100 indivíduos gerados aleatoriamente (respeitando a seed fixa para reprodutibilidade).

    Custo: É calculada uma matriz de distâncias Euclidianas entre todos os pontos no início da execução para otimizar o cálculo do fitness.
   
3. Seleção e Reprodução

A cada geração, uma nova população é criada seguindo estas regras:

    Elitismo: Uma porcentagem definida (TAXA_ELITE) dos melhores indivíduos é copiada integralmente para a próxima geração.

    Seleção de Pais:

        Pai 1: Selecionado do topo da elite (pode ser o Melhor Absoluto ou um sorteado entre os Top N%, configurável via código).

        Pai 2: Escolhido aleatoriamente de toda a população.

3. Operadores Genéticos

    Crossover (Cruzamento): Ocorre se ativado.

        Copia a primeira metade dos genes do Pai 1 para o filho.

        Preenche o restante do genoma do filho com os genes do Pai 2, na ordem em que aparecem, ignorando os que já foram copiados do Pai 1.

    Mutação (Swap): Ocorre com base na probabilidade (PROB_MUTACAO). Se ocorrer, o algoritmo escolhe dois genes aleatórios no caminho e troca suas posições.

5. Mecanismo de Cataclismo (Reset Populacional)

    O sistema monitora a estagnação. Se o melhor fitness não melhorar após LIMITE_ESTAGNACAO gerações, um Reset é acionado.

    O que acontece: O melhor indivíduo é mantido, mas todos os outros 99 indivíduos são mortos e substituídos por novos indivíduos totalmente aleatórios.

    Isso é visível no gráfico como uma linha vertical amarela, geralmente seguida de salto na média da população (linha vermelha), forçando o algoritmo a explorar novas áreas do espaço de busca.
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
