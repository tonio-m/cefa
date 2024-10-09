#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "../include/raygui/raygui.h"
#include <map>
#include <regex>
#include <cstdlib>
#include <iostream>
#include <RtMidi.h>
#include <unistd.h>
#include <typeinfo>
#include "piano.cpp"
#include <functional>

std::vector<int> parseChord(std::string string){
    std::map<std::string, int> noteMap = {
        {"C",   0}, {"C#",  1}, {"Db",  1}, {"D",   2},
        {"D#",  3}, {"Eb",  3}, {"E",   4}, {"F",   5},
        {"F#",  6}, {"Gb",  6}, {"G",   7}, {"G#",  8},
        {"Ab",  8}, {"A",   9}, {"A#", 10}, {"Bb", 10},
        {"B",  11}, {"Cb", 11}
    };
    std::map<std::string, std::function<std::vector<int>(int)>> chordMap = {
      {"min", [](int x) { return std::vector<int>{x, x + 3, x + 7}; }},
      {"maj", [](int x) { return std::vector<int>{x, x + 4, x + 7}; }},
      {"maj7", [](int x) { return std::vector<int>{x, x + 4, x + 7, x + 11}; }},
      {"7", [](int x) { return std::vector<int>{x, x + 4, x + 7, x + 10}; }},
      {"dim7", [](int x) { return std::vector<int>{x, x + 3, x + 6, x + 9}; }},
      {"min7", [](int x) { return std::vector<int>{x, x + 3, x + 7, x + 10}; }},
      {"min7b5", [](int x) { return std::vector<int>{x, x + 3, x + 6, x + 10}; }}
    };
    std::smatch match;
    std::regex pattern("(^[A-Za-z]#?b?)(.*)$");
    std::regex_match(string, match, pattern);
    std::string noteString = match[1].str();
    std::string chordType = match[2].str();
    return chordMap[chordType](noteMap[noteString]);
}

int main() {
    int frameCounter;
    char input[256] = {0};
    int backspaceCooldown = 0;
    static int currentItem = 0;
    std::vector<int> chordVector;
    Vector2 windowSize = {800, 450};
    static bool dropdownEdit = false;
    Piano piano(windowSize.x * 0.01f, windowSize.y * 0.7f, 25.0f);

    SetTargetFPS(60);
    piano.initMidi();
    InitWindow(windowSize.x, windowSize.y, "Piano");

    while (!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (frameCounter == 60){
            piano.printBlueChords();
            frameCounter = 0;
        }

        if (GuiTextBox((Rectangle){windowSize.x * 0.01f, windowSize.y * 0.5f, 300, 40}, input, 256, true)) {
            if (IsKeyPressed(KEY_ENTER)) {
                chordVector = parseChord((std::string)input);
                piano.pushBackChord(chordVector);
                strcpy(input, "");
            }
        }

        piano.draw();
        DrawText(piano.activeNotesString().c_str(), windowSize.x * 0.5f, windowSize.y * 0.01f, 20, LIGHTGRAY);
        EndDrawing();
        frameCounter++;
    }
}

