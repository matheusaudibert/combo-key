#include "game_logic.h"
#include <cstdlib>
#include <algorithm>
using namespace std;

// Retorna uma tecla aleatória diferente da passada como parâmetro
string getRandomKey(const string& currentKey) {
    static const vector<string> allowedKeys = {
        "A","B","C","D","E","F","G","H","I","J","K","L","M",
        "N","O","P","Q","R","S","T","U","V","W","X","Y","Z",
        "0","1","2","3","4","5","6","7","8","9",
        "ARROWUP", "ARROWLEFT", "ARROWDOWN", "ARROWRIGHT", "EMPTY1"
    };
    string newKey;
    do {
        newKey = allowedKeys[rand() % allowedKeys.size()];
    } while (newKey == currentKey && !allowedKeys.empty());
    return newKey;
}

// Gera uma sequência aleatória de teclas com o tamanho especificado
vector<string> GenerateRandomSequence(int length) {
    vector<string> sequence;
    for (int i = 0; i < length; i++) {
        sequence.push_back(getRandomKey(""));
    }
    return sequence;
}

// Configura os botões das teclas do jogo na tela, centralizando-os
void SetupGameKeys(vector<KeyButton>& gameKeys, const vector<string>& sequence, int screenWidth, int screenHeight) {
    gameKeys.clear();
    if (sequence.empty()) return;

    float keyScale = 3.0f;
    float keySpacing = 0.0f;
    float singleKeyFrameWidth = 26;
    float singleKeyFrameHeight = 32;
    float scaledKeyWidth = singleKeyFrameWidth * keyScale;
    float scaledKeyHeight = singleKeyFrameHeight * keyScale;
    float totalWidth = (sequence.size() * scaledKeyWidth) + (max(0, (int)sequence.size() - 1) * keySpacing);
    float currentStartX = (screenWidth - totalWidth) / 2.0f;
    float currentY = screenHeight / 2.0f - (scaledKeyHeight / 2.0f) + 15.0f;

    for (int i = 0; i < (int)sequence.size(); i++) {
        KeyButton keyButton;
        keyButton.name = sequence[i];
        keyButton.position = { currentStartX + (scaledKeyWidth + keySpacing) * i, currentY };
        keyButton.scale = keyScale;
        keyButton.Load("assets/keys/light/sheets/" + keyButton.name + ".png");
        keyButton.currentFrame = 0;
        gameKeys.push_back(keyButton);
    }
}

// Reinicia o estado do jogo, gerando nova sequência e resetando variáveis
void ResetGame(vector<string>& currentSequence, int& currentKeyIndex, bool& gameOver, bool& isAnimating, float& animationProgress, vector<KeyButton>& gameKeys, int screenWidth, int screenHeight, int initialSequenceLength, float& currentTime, float maxTime, int& comboCount) {
    currentSequence = GenerateRandomSequence(initialSequenceLength);
    currentKeyIndex = 0;
    gameOver = false;
    isAnimating = true;
    animationProgress = 0.0f;
    currentTime = maxTime;
    comboCount = 0;
    SetupGameKeys(gameKeys, currentSequence, screenWidth, screenHeight);
}
