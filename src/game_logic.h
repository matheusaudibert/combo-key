#pragma once
#include <vector>
#include <string>
#include "key_button.h"
using namespace std;

// Retorna uma tecla aleatória diferente da passada como parâmetro
string getRandomKey(const string& currentKey);

// Gera uma sequência aleatória de teclas com o tamanho especificado
vector<string> GenerateRandomSequence(int length);

// Configura os botões das teclas do jogo na tela, centralizando-os
void SetupGameKeys(vector<KeyButton>& gameKeys, const vector<string>& sequence, int screenWidth, int screenHeight);

// Reinicia o estado do jogo, gerando nova sequência e resetando variáveis
void ResetGame(vector<string>& currentSequence, int& currentKeyIndex, bool& gameOver, bool& isAnimating, float& animationProgress, vector<KeyButton>& gameKeys, int screenWidth, int screenHeight, int initialSequenceLength, float& currentTime, float maxTime, int& comboCount);
