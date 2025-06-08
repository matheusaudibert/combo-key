#include "key_button.h"
#include "raylib.h"

using namespace std;

// Carrega a textura da tecla a partir do caminho especificado
void KeyButton::Load(const string& path) {
    if (texture.id != 0) UnloadTexture(texture);
    texture = LoadTexture(path.c_str());
    if (texture.id != 0) {
        frameWidth = texture.width / 3;
        frameHeight = texture.height;
    } else {
        frameWidth = 26;
        frameHeight = 32;
    }
}

// Recarrega a textura baseada no nome da tecla
void KeyButton::ReloadTexture() {
    string path = "assets/keys/light/sheets/" + name + ".png";
    Load(path);
}

// Atualiza o estado do botão (hover/click) com base no mouse
bool KeyButton::Update(Vector2 mousePos, bool mouseDown, bool mousePressed) {
    Rectangle bounds = { position.x, position.y, frameWidth * scale, frameHeight * scale };
    bool isHovered = CheckCollisionPointRec(mousePos, bounds);
    currentFrame = (isHovered && mouseDown) ? 1 : 0;
    return (isHovered && mousePressed);
}

// Desenha o botão na tela
void KeyButton::Draw() {
    if (texture.id == 0) return;
    Rectangle src = { (float)(currentFrame * frameWidth), 0, (float)frameWidth, (float)frameHeight };
    Rectangle dest = { position.x, position.y, frameWidth * scale, frameHeight * scale };
    DrawTexturePro(texture, src, dest, {0, 0}, 0.0f, WHITE);
}

// Libera a textura da tecla
void KeyButton::Unload() {
    if (texture.id != 0) UnloadTexture(texture);
}