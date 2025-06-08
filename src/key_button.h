#pragma once
#include "raylib.h"
#include <string>
using namespace std;

// Estrutura que representa um botão de tecla na tela
struct KeyButton {
    Texture2D texture = {0};      // Textura da tecla
    Vector2 position = {0};       // Posição na tela
    int frameWidth = 0;           // Largura de um frame da textura (spritesheet)
    int frameHeight = 0;          // Altura de um frame da textura
    int currentFrame = 0;         // Frame atual (0 = normal, 1 = pressionado)
    float scale = 2.5f;           // Escala de desenho da tecla
    string name;             // Nome da tecla (ex: "A", "ENTER", etc)

    // Carrega a textura da tecla a partir de um caminho
    void Load(const string& path);
    // Recarrega a textura baseada no nome da tecla
    void ReloadTexture();
    // Atualiza o estado do botão (hover/click) com base no mouse
    bool Update(Vector2 mousePos, bool mouseDown, bool mousePressed);
    // Desenha o botão na tela
    void Draw();
    // Libera a textura da tecla
    void Unload();
};
