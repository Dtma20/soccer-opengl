# Soccer

## Descrição

O **Soccer** é um jogo de futebol em 2D desenvolvido em C utilizando a biblioteca OpenGL (GLUT) para renderização gráfica. O jogo simula uma partida de futebol com dois times de 11 jogadores cada, permitindo que os usuários controlem os jogadores, movam a bola, realizem passes e chutes ao gol. A mecânica inclui colisão entre jogadores, posse de bola, troca de jogadores controlados e pontuação, com uma física simplificada para a bola e interações no campo.

![<alt-text>](<imgs/image.png>)


Este projeto foi criado como uma demonstração de conceitos de computação gráfica, breseham e simulação de jogos.

## Autores

- **Filipe Soriano**
- **Diogo Amorim**

## Funcionalidades

- **Controle de Jogadores**: Cada time é controlado por um jogador humano, que pode mover o jogador ativo, passar e chutar a bola.
  - Time 1: Usa as teclas `W`, `A`, `S`, `D` para movimento, `Q` para chute, `R` para passe e `E` para trocar de jogador.
  - Time 2: Usa as teclas `I`, `J`, `K`, `L` para movimento, `U` para chute, `P` para passe e `O` para trocar de jogador.
- **Colisão e Física**: Inclui detecção de colisão entre jogadores e a bola, com reflexão e atrito aplicados à bola.
- **Pontuação**: Gols são registrados quando a bola cruza a linha do gol dentro da área válida.
- **Campo Visual**: O campo é desenhado com texturas para jogadores, bola e goleiros, usando OpenGL.

## Requisitos

- **Sistema Operacional**: Linux, macOS ou Windows (com suporte a GLUT).
- **Bibliotecas**:
  - OpenGL
  - FreeGLUT
  - GLU
- **Compilador**: GCC ou outro compilador C compatível.
- **Arquivos de Textura**: As texturas (`goleiro.png`, `ball.png`, `player1.png`, `player2.png`) devem estar no diretório `assets/`.

- **Objetivo**: Marcar gols no time adversário controlando os jogadores e a bola.
- **Controles**:
  - **Time 1**:
    - Movimento: `W` (cima), `A` (esquerda), `S` (baixo), `D` (direita)
    - Chute: Segurar `Q` (força aumenta com o tempo)
    - Passe: Segurar `R` (força aumenta com o tempo)
    - Trocar jogador: `E`
  - **Time 2**:
    - Movimento: `I` (cima), `J` (esquerda), `K` (baixo), `L` (direita)
    - Chute: Segurar `U` (força aumenta com o tempo)
    - Passe: Segurar `P` (força aumenta com o tempo)
    - Trocar jogador: `O`
- **Mecânicas**:
  - O jogador controlado é destacado e pode interagir com a bola.
  - A bola é passada para o companheiro mais próximo ao soltar a tecla de passe.
  - O chute é direcionado ao centro do gol adversário, com força proporcional ao tempo de carrnto.
  - Colisões entre jogadores afetam suas posições e velocidades.

## Estrutura do Código

- `campo.c`: Arquivo principal contendo a lógica do jogo, incluindo inicialização, atualização, renderização e entrada do usuário.
- `draw.h`: Cabeçalho assumido que contém funções de desenho e definições de estruturas (como `Player` e `Ball`).
- **Diretório** `assets/`: Contém as texturas usadas para renderizar goleiros, bola e jogadores.

## Notas Técnicas

- O jogo usa uma resolução de janela de 1200x800 pixels.
- O campo tem dimensões virtuais de 1050x680 unidades.
- A taxa de atualização é de aproximadamente 60 FPS (controlada por um timer de 16ms).
- A física da bola inclui atrito e reflexão nas bordas do campo.
- Algumas funcionalidades, como texturas, dependem da implementação de `loadTexture` no arquivo `draw.h`.

Desenvolvido com ❤️ por **Filipe Soriano** e **Diogo Amorim**.