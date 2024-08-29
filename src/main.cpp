#include <map>
#include <regex>
#include <cstdlib>
#include <iostream>
#include <RtMidi.h>
#include <raylib.h>
#include <unistd.h>
#include <typeinfo>
#include "piano.cpp"
#include <functional>
std::vector<int> parseChord(std::string string){
    std::map<std::string, int> noteMap;
    noteMap["C"]  = 0;
    noteMap["C#"] = 1;
    noteMap["Db"] = 1;
    noteMap["D"]  = 2;
    noteMap["D#"] = 3;
    noteMap["Eb"] = 3;
    noteMap["E"]  = 4;
    noteMap["F"]  = 5;
    noteMap["F#"] = 6;
    noteMap["Gb"] = 6;
    noteMap["G"]  = 7;
    noteMap["G#"] = 8;
    noteMap["Ab"] = 8;
    noteMap["A"]  = 9;
    noteMap["A#"] = 10;
    noteMap["Bb"] = 10;
    noteMap["B"]  = 11;
    noteMap["Cb"] = 11;
    std::map<std::string, std::function<std::vector<int>(int)>> chordMap;
    chordMap["maj7"] = [](int x) {return std::vector<int>{x,x+4,x+7,x+11}; };
    chordMap["7"] = [](int x) {return std::vector<int>{x,x+4,x+7,x+10};};
    chordMap["dim7"] = [](int x) {return std::vector<int>{x,x+3,x+6,x+9};};
    chordMap["min7"] = [](int x) {return std::vector<int>{x,x+3,x+7,x+10};};
    chordMap["min7b5"] = [](int x) {return std::vector<int>{x,x+3,x+6,x+10};};
    chordMap["dim7"] = [](int x) {return std::vector<int>{x,x+3,x+6,x+9};};
    std::smatch match;
    std::regex pattern("(^[A-Za-z]#?b?)(.*)$");
    std::regex_match(string, match, pattern);
    std::string noteString = match[1].str();
    std::string chordType = match[2].str();
    return chordMap[chordType](noteMap[noteString]);
}
struct InputBox {
    float posX;
    float posY;
    Color color;
    int fontSize;
    std::string text;
    InputBox(int posX, int posY, int fontSize, Color color){
        this->posX = posX;
        this->posY = posY;
        this->fontSize = fontSize;
        this->color = color;
    }
    void draw(){
        DrawText(text.c_str(), posX, posY, fontSize, color);
    }
};
void handleKeyPresses(InputBox* input, int* backspaceCooldown, std::vector<std::string>* chords){
        // update input box
        int key = GetCharPressed();
        while (key > 0)
        {
            input->text += static_cast<char>(key);
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_ENTER)) {
            chords->push_back(input->text);
            input->text.clear();
        }
        else if (IsKeyDown(KEY_BACKSPACE) && *backspaceCooldown == 0) {
            if (!input->text.empty()) {
                input->text.pop_back();
            }
            *backspaceCooldown = 3;
        } else if (*backspaceCooldown > 0) {
            --*backspaceCooldown;
        }
}
int main() {
    int backspaceCooldown = 0;
    std::vector<std::string> chordStrings;
    auto *piano = new Piano(5.0f,275.0f,25.0f);
    auto *input = new InputBox(350, 200, 20, GRAY);

    piano->initMidi();
    SetTargetFPS(60);
    InitWindow(800, 450, "Piano");
    std::vector<int> chordVector;
    while (!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        handleKeyPresses(input,&backspaceCooldown,&chordStrings);
        if (!chordStrings.empty()){
            chordVector = parseChord(chordStrings.back());
        }
        std::string concatString;
        for (auto x : chordStrings){concatString+= x + ", ";};
        DrawText(concatString.c_str(), 300, 100, 20, LIGHTGRAY);
        DrawText(piano->activeNotesString().c_str(), 350, 20, 20, LIGHTGRAY);
        input->draw();
        piano->draw(chordVector);
        EndDrawing();
    }
}
