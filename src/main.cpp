#include "raylib.h"
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <map>
#include "key_button.h"
#include "game_logic.h"

using namespace std;

int main(void)
{
    // Inicializa o gerador de números aleatórios
    srand((unsigned int)time(NULL));

    // Define as dimensões da janela do jogo
    const int screenWidth = 756;
    const int screenHeight = 420;
    const int baseInitialGameSequenceLength = 7; // Não é mais usado diretamente, mas mantido para referência

    // Variáveis da barra de tempo
    float maxTime = 4.0f;    // Tempo máximo para completar a sequência (ajustado pela dificuldade)
    float currentTime = maxTime; // Tempo atual restante
    const float timeBonus = 1.0f;  // Bônus de tempo ao acertar uma tecla

    // Variáveis de combo e animação de "+1"
    int comboCount = 0;
    Texture2D plusTexture = {0}; // Textura para o "+1" (não está sendo usada visualmente)
    bool showPlusAnimation = false;
    float plusAnimationTimer = 0.0f;

    // Inicializa a janela e o áudio
    InitWindow(screenWidth, screenHeight, "Combo Key");
    SetExitKey(KEY_NULL); // Impede que ESC feche a janela por padrão
    InitAudioDevice();

    // Carrega e reproduz o som do tema
    Sound themeSound = LoadSound("assets/sounds/theme.wav");
    PlaySound(themeSound);

    // Define o ícone da janela
    Image icon = LoadImage("assets/keys/dark/keys/C.png");
    SetWindowIcon(icon);
    UnloadImage(icon);

    // Carrega a imagem de fundo e aplica filtro bilinear para melhor qualidade ao redimensionar
    Texture2D background = LoadTexture("assets/others/bg.png");
    SetTextureFilter(background, TEXTURE_FILTER_BILINEAR);

    // Carrega os sons do jogo
    Sound keySound1 = LoadSound("assets/sounds/key_1.wav");
    Sound keySound2 = LoadSound("assets/sounds/key_2.wav");
    Sound keySound3 = LoadSound("assets/sounds/key_3.wav");
    Sound comboSound = LoadSound("assets/sounds/combo.wav");
    Sound startSound = LoadSound("assets/sounds/start.wav");
    Sound failSound = LoadSound("assets/sounds/fail.wav");

    // Carrega a textura do botão "+"
    plusTexture = LoadTexture("assets/keys/light/keys/PLUS.png");

    // Define o FPS alvo do jogo
    SetTargetFPS(60);

    // Função lambda para tocar um som aleatório de tecla
    auto playRandomKeySound = [&]() {
        int r = rand() % 3;
        if (r == 0) PlaySound(keySound1);
        else if (r == 1) PlaySound(keySound2);
        else PlaySound(keySound3);
    };

    // Parâmetros de layout do menu
    float menuStartX = 153.5f;
    float menuTitleY = 80.0f;
    float menuSpacing = 52.0f;
    float menuKeyScale = 2.5f;
    float menuButtonScale = 4.0f;

    // Cria os botões das letras "COMBO" no menu
    vector<string> comboKeyNames = { "C", "O", "M", "B", "O" };
    vector<KeyButton> comboButtons;
    for (int i = 0; i < (int)comboKeyNames.size(); i++) {
        KeyButton key;
        key.name = comboKeyNames[i];
        key.position = { menuStartX + menuSpacing * i, menuTitleY + 10 };
        key.scale = menuKeyScale;
        key.Load("assets/keys/light/sheets/" + key.name + ".png");
        comboButtons.push_back(key);
    }

    // Cria os botões das letras "KEY" no menu
    vector<string> keyKeyNames = { "K", "E", "Y" };
    vector<KeyButton> keyButtons_menu;
    for (int i = 0; i < (int)keyKeyNames.size(); i++) {
        KeyButton key;
        key.name = keyKeyNames[i];
        key.position = { menuStartX + menuSpacing * (i + 5) + 40.0f, menuTitleY + 10 };
        key.scale = menuKeyScale;
        key.Load("assets/keys/light/sheets/" + key.name + ".png");
        keyButtons_menu.push_back(key);
    }

    // Botão "+" para selecionar dificuldade
    KeyButton keyPlus;
    keyPlus.name = "PLUS";
    keyPlus.position = { menuStartX , menuTitleY + 199 };
    keyPlus.scale = menuKeyScale;
    keyPlus.Load("assets/keys/light/sheets/PLUS.png");

    // Botão "ENTER" para iniciar o jogo
    KeyButton keyEnter;
    keyEnter.name = "ENTER";
    keyEnter.position = { menuStartX + menuSpacing * 5 + 16.0f, menuTitleY + 88 };
    keyEnter.scale = menuButtonScale + 0.5f;
    keyEnter.Load("assets/keys/light/sheets/ENTER.png");

    // Botão "BACKSPACE" para sair do jogo
    KeyButton keyBackspace;
    keyBackspace.name = "BACKSPACE";
    keyBackspace.position = { menuStartX, menuTitleY + 88 };
    keyBackspace.scale = menuButtonScale;
    keyBackspace.Load("assets/keys/light/sheets/BACKSPACE.png");

    // Vetor para botões "+" adicionais (dificuldade)
    vector<KeyButton> plusDifficultyButtons;
    int difficultyLevel = 0; // 0: fácil, 1: médio, 2: difícil

    // Variáveis de transição de tela (fade in/out)
    float fade = 0.0f;
    bool isFadingOut = false;
    bool isFadingIn = false;
    bool isGameStarted = false;

    // Variáveis do jogo em si
    vector<string> currentSequence; // Sequência de teclas a ser pressionada
    vector<KeyButton> gameKeys;     // Botões visuais das teclas da sequência
    int currentKeyIndex = 0;        // Índice da próxima tecla a ser pressionada
    bool gameOver = false;          // Indica se o jogo acabou
    bool isAnimating = false;       // Indica se está animando a entrada da sequência
    float animationProgress = 0.0f; // Progresso da animação de entrada

    // Função lambda para lógica do botão "+" (dificuldade)
    auto processPlusClickLogic = [&]() {
        playRandomKeySound();
        if (difficultyLevel == 0) {
            // Adiciona um botão "+" para dificuldade média
            KeyButton newPlus;
            newPlus.name = "PLUS";
            newPlus.scale = keyPlus.scale;
            float spacing = 4.0f;
            newPlus.position = { keyPlus.position.x + (keyPlus.frameWidth * keyPlus.scale) + spacing, keyPlus.position.y };
            newPlus.Load("assets/keys/light/sheets/PLUS.png");
            if (newPlus.position.x + (newPlus.frameWidth * newPlus.scale) < screenWidth) {
                plusDifficultyButtons.push_back(newPlus);
                difficultyLevel = 1;
            } else {
                newPlus.Unload();
            }
        } else if (difficultyLevel == 1) {
            // Adiciona mais um "+" para dificuldade difícil
            if (plusDifficultyButtons.empty() || plusDifficultyButtons.size() > 1) {
                for (auto& btn : plusDifficultyButtons) btn.Unload();
                plusDifficultyButtons.clear();
                difficultyLevel = 0; 
                return;
            }
            KeyButton& firstAddedPlus = plusDifficultyButtons.back();
            KeyButton newPlus;
            newPlus.name = "PLUS";
            newPlus.scale = keyPlus.scale;
            float spacing = 4.0f;
            newPlus.position = { firstAddedPlus.position.x + (firstAddedPlus.frameWidth * firstAddedPlus.scale) + spacing, firstAddedPlus.position.y };
            newPlus.Load("assets/keys/light/sheets/PLUS.png");
            if (newPlus.position.x + (newPlus.frameWidth * newPlus.scale) < screenWidth) {
                plusDifficultyButtons.push_back(newPlus);
                difficultyLevel = 2;
            } else {
                newPlus.Unload();
            }
        } else { // difficultyLevel == 2
            // Remove todos os "+" e volta para fácil
            for (auto& btn : plusDifficultyButtons) {
                btn.Unload();
            }
            plusDifficultyButtons.clear();
            difficultyLevel = 0;
        }
    };

    // Loop principal do jogo
    while (!WindowShouldClose())
    {
        // Captura informações do mouse e tempo de frame
        Vector2 mouse = GetMousePosition();
        bool mouseDown = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
        bool mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        float frameTime = GetFrameTime();

        // Lógica quando o jogo está em estado de "game over"
        if (gameOver) {
            if (IsKeyPressed(KEY_ENTER)) {
                // Ao pressionar ENTER, reinicia o jogo com sequência aleatória de 1 a 8 teclas
                int currentInitialGameSequenceLength = 1 + rand() % 8;
                if (difficultyLevel == 0) maxTime = 4.0f;      // Fácil
                else if (difficultyLevel == 1) maxTime = 2.5f; // Médio
                else maxTime = 1.0f;                           // Difícil
                ResetGame(currentSequence, currentKeyIndex, gameOver, isAnimating, animationProgress, gameKeys, screenWidth, screenHeight, currentInitialGameSequenceLength, currentTime, maxTime, comboCount);
                isFadingIn = true; 
                fade = 1.0f; 
            } else if (IsKeyPressed(KEY_ESCAPE)) {
                // Volta para o menu principal
                isGameStarted = false;
                gameOver = false;
                isFadingIn = true;
                fade = 1.0f;
                for (auto& key : gameKeys) key.Unload();
                gameKeys.clear();
            } else if (IsKeyPressed(KEY_BACKSPACE)) {
                // Sai do jogo
                break; 
            }
        } else if (!isGameStarted) {
            // Lógica do menu principal (troca letras, seleciona dificuldade, inicia ou sai)
            for (auto& keyBtn : comboButtons) {
                if (keyBtn.Update(mouse, mouseDown, mousePressed)) {
                    string newKeyName = getRandomKey(keyBtn.name);
                    keyBtn.name = newKeyName;
                    keyBtn.ReloadTexture();
                    playRandomKeySound();
                }
            }
            for (auto& keyBtn : keyButtons_menu) {
                if (keyBtn.Update(mouse, mouseDown, mousePressed)) {
                    string newKeyName = getRandomKey(keyBtn.name);
                    keyBtn.name = newKeyName;
                    keyBtn.ReloadTexture();
                    playRandomKeySound();
                }
            }
            if (keyEnter.Update(mouse, mouseDown, mousePressed) && !isFadingOut && !isFadingIn) {
                PlaySound(startSound);
                isFadingOut = true;
            }
            if (keyBackspace.Update(mouse, mouseDown, mousePressed)) {
                break;
            }
            if (keyPlus.Update(mouse, mouseDown, mousePressed)) {
                processPlusClickLogic();
            }
            for (auto& addedBtn : plusDifficultyButtons) {
                if (addedBtn.Update(mouse, mouseDown, mousePressed)) {
                    processPlusClickLogic();
                }
            }
        }

        // Lógica de transição de fade out (menu -> jogo)
        if (isFadingOut) {
            fade += 0.02f;
            if (fade >= 1.0f) {
                fade = 1.0f;
                isFadingOut = false;
                isGameStarted = true;
                isFadingIn = true;
                // Ao iniciar, sequência aleatória de 1 a 8 teclas
                int currentInitialGameSequenceLength = 1 + rand() % 8;
                if (difficultyLevel == 0) maxTime = 4.0f;
                else if (difficultyLevel == 1) maxTime = 2.5f;
                else maxTime = 1.0f;
                ResetGame(currentSequence, currentKeyIndex, gameOver, isAnimating, animationProgress, gameKeys, screenWidth, screenHeight, currentInitialGameSequenceLength, currentTime, maxTime, comboCount);
            }
        } else if (isFadingIn) {
            // Lógica de fade in (transição para menu ou jogo)
            fade -= 0.02f;
            if (fade <= 0.0f) {
                fade = 0.0f;
                isFadingIn = false;
            }
        }

        // Lógica principal do jogo (quando está rodando e não está animando entrada)
        if (isGameStarted && !gameOver && !isAnimating) {
            // Atualiza o tempo restante
            currentTime -= frameTime;
            if (currentTime <= 0) {
                currentTime = 0;
                gameOver = true;
                PlaySound(failSound);
            }

            // Captura qual tecla foi pressionada
            int keyPressed = 0;
            string pressedKeyName = "";

            // Letras A-Z
            for (int k_code = KEY_A; k_code <= KEY_Z; k_code++) {
                if (IsKeyPressed(k_code)) {
                    keyPressed = k_code;
                    pressedKeyName = string(1, (char)k_code);
                    break;
                }
            }
            // Números 0-9
            if (!keyPressed) {
                for (int k_code = KEY_ZERO; k_code <= KEY_NINE; k_code++) {
                    if (IsKeyPressed(k_code)) {
                        keyPressed = k_code;
                        pressedKeyName = string(1, (char)k_code);
                        break;
                    }
                }
            }
            // Setas direcionais
            if (!keyPressed) {
                map<int, string> arrow_map = {{KEY_UP, "ARROWUP"}, {KEY_DOWN, "ARROWDOWN"}, {KEY_LEFT, "ARROWLEFT"}, {KEY_RIGHT, "ARROWRIGHT"}};
                for (const auto& pair : arrow_map) {
                    if (IsKeyPressed(pair.first)) {
                        keyPressed = pair.first;
                        pressedKeyName = pair.second;
                        break;
                    }
                }
            }

            // Verifica se a tecla pressionada está correta
            // Se a tecla esperada for "EMPTY1", qualquer tecla é aceita
            if (keyPressed && !currentSequence.empty() && currentKeyIndex < (int)currentSequence.size()) {
                if (
                    (currentSequence[currentKeyIndex] == "EMPTY1" && keyPressed) ||
                    (pressedKeyName == currentSequence[currentKeyIndex])
                ) {
                    // Marca a tecla como pressionada corretamente
                    if(currentKeyIndex < (int)gameKeys.size()) gameKeys[currentKeyIndex].currentFrame = 1;
                    currentKeyIndex++;
                    playRandomKeySound();
                    
                    // Dá bônus de tempo ao acertar
                    currentTime += timeBonus;
                    if (currentTime > maxTime) {
                        currentTime = maxTime;
                    }

                    // Se completou a sequência, gera uma nova sequência maior (até 8)
                    if (currentKeyIndex >= (int)currentSequence.size()) {
                        PlaySound(comboSound);
                        comboCount++; // Incrementa o combo
                        currentSequence = GenerateRandomSequence(min((int)currentSequence.size() + 1, 8));
                        currentKeyIndex = 0;
                        isAnimating = true;
                        animationProgress = 0.0f;
                        SetupGameKeys(gameKeys, currentSequence, screenWidth, screenHeight);
                    }
                } else {
                    // Se errou, game over
                    PlaySound(failSound);
                    gameOver = true;
                }
            }
        }
        
        // Animação de entrada das teclas da sequência
        if (isGameStarted && !gameOver && isAnimating) {
             animationProgress += 0.05f;
             if (animationProgress >= 1.0f) {
                 animationProgress = 1.0f;
                 isAnimating = false;
                 // Reseta os frames das teclas para o estado normal após a animação
                 for(auto& gk : gameKeys) gk.currentFrame = 0;
             }
        }

        // Desenho da tela
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Desenha o fundo redimensionado
        DrawTextureEx(background, {0, 0}, 0.0f, (float)screenWidth / background.width, WHITE);

        if (!isGameStarted) {
            // Desenha os botões do menu
            for (auto& keyBtn : comboButtons) keyBtn.Draw();
            for (auto& keyBtn : keyButtons_menu) keyBtn.Draw();
            keyEnter.Draw();
            keyBackspace.Draw();
            if (keyPlus.texture.id != 0) keyPlus.Draw();
            for (auto& addedBtn : plusDifficultyButtons) addedBtn.Draw();
        } else if (gameOver) {
            // Tela de game over
            DrawText("GAME OVER", screenWidth / 2 - MeasureText("GAME OVER", 40) / 2, screenHeight / 2 - 60, 40, RED);
            DrawText("Pressione ENTER para jogar novamente", screenWidth / 2 - MeasureText("Pressione ENTER para jogar novamente", 20) / 2, screenHeight / 2 + 0, 20, WHITE);
            DrawText("Pressione ESC para voltar ao menu", screenWidth / 2 - MeasureText("Pressione ESC para voltar ao menu", 20) / 2, screenHeight / 2 + 30, 20, WHITE);
            DrawText("Pressione BACKSPACE para sair", screenWidth / 2 - MeasureText("Pressione BACKSPACE para sair", 20) / 2, screenHeight / 2 + 60, 20, WHITE);
        } else {
            // Jogo em andamento

            // Desenha a barra de tempo
            float timeBarWidth = screenWidth * 0.6f;
            float timeBarHeight = 20.0f;
            float timeBarX = (screenWidth - timeBarWidth) / 2.0f;
            float timeBarY = 20.0f;
            DrawRectangleRec({timeBarX, timeBarY, timeBarWidth, timeBarHeight}, LIGHTGRAY);
            float currentTimeBarWidth = timeBarWidth * (currentTime / maxTime);
            Color timeBarColor = GREEN;
            if (currentTime / maxTime < 0.25f) timeBarColor = RED;
            else if (currentTime / maxTime < 0.5f) timeBarColor = ORANGE;
            DrawRectangleRec({timeBarX, timeBarY, currentTimeBarWidth, timeBarHeight}, timeBarColor);
            DrawRectangleLinesEx({timeBarX, timeBarY, timeBarWidth, timeBarHeight}, 2, DARKGRAY);

            // Desenha o contador de combos
            string comboText = "Combo: " + to_string(comboCount);
            int comboTextWidth = MeasureText(comboText.c_str(), 20);
            DrawText(comboText.c_str(), screenWidth / 2 - comboTextWidth / 2, timeBarY + timeBarHeight + 5, 20, WHITE);

            // Desenha as teclas da sequência
            for (int i = 0; i < (int)gameKeys.size(); i++) {
                float alpha = (isAnimating && i >= currentKeyIndex) ? animationProgress : 1.0f;
                int frameToDraw = (i < currentKeyIndex) ? 1 : 0;
                if (isAnimating && i >= currentKeyIndex) {
                     frameToDraw = 0;
                } else if (i < currentKeyIndex) {
                    frameToDraw = 1;
                } else {
                    frameToDraw = gameKeys[i].currentFrame;
                }
                if (gameKeys[i].texture.id != 0) {
                     Rectangle srcRect = { (float)(frameToDraw * gameKeys[i].frameWidth), 0, (float)gameKeys[i].frameWidth, (float)gameKeys[i].frameHeight };
                     Rectangle destRect = { gameKeys[i].position.x, gameKeys[i].position.y, gameKeys[i].frameWidth * gameKeys[i].scale, gameKeys[i].frameHeight * gameKeys[i].scale };
                     DrawTexturePro(gameKeys[i].texture, srcRect, destRect, {0,0}, 0.0f, Fade(WHITE, alpha));
                }
            }
        }

        // Desenha o efeito de fade (transição)
        if (fade > 0.0f) {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, fade));
        }

        EndDrawing();
    }

    // Libera recursos (texturas e sons)
    UnloadTexture(background);
    for (auto& key : comboButtons) key.Unload();
    for (auto& key : keyButtons_menu) key.Unload();
    for (auto& key : gameKeys) key.Unload();
    keyEnter.Unload();
    keyBackspace.Unload();
    if (keyPlus.texture.id != 0) keyPlus.Unload();
    for (auto& addedBtn : plusDifficultyButtons) addedBtn.Unload();
    if (plusTexture.id != 0) UnloadTexture(plusTexture);

    UnloadSound(keySound1);
    UnloadSound(keySound2);
    UnloadSound(keySound3);
    UnloadSound(comboSound);
    UnloadSound(startSound);
    UnloadSound(failSound);
    UnloadSound(themeSound); // Descarrega o som do tema

    CloseAudioDevice();
    CloseWindow();
    return 0;
}
