#include <map>
#include <regex>
#include <sstream>
#include <RtMidi.h>
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "../include/raygui/raygui.h"

std::array<bool, 36> PIANONOTESACTIVE;
enum Scene { SCENE_INPUT, SCENE_PIANO };

std::vector<std::string> splitString(std::string s, std::string separator) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string token;

    while (std::getline(ss, token, separator[0])) {
        result.push_back(token);
    }
    
    return result;
}

template<typename T>
void printVector(const std::vector<T>& vec, std::string end = "\n") {
    std::cout << "{ ";
    for (const auto& elem : vec) {
        std::cout << elem << " ";
    }
    std::cout << "}" << end;
}

template<typename T>
void printVector(const std::vector<std::vector<T>>& vec, std::string end = "\n") {
    std::cout << "[";
    for (const auto& elem : vec) {
        printVector(elem, end="");
    }
    std::cout << "]" << "\n";
}

std::vector<int> parseChord(std::string string){
    enum semitones {
        minor3rd = 3,
        major3rd = 4,
        diminished5th = 6,
        perfect5th = 7,
        major6th = 9,
        minor7th = 10,
        major7th = 11
    };

    static std::map<std::string, int> noteMap = {
        {"C",   0}, 
        {"C#",  1}, {"Db",  1}, 
        {"D",   2},
        {"D#",  3}, {"Eb",  3}, 
        {"E",   4}, 
        {"F",   5},
        {"F#",  6}, {"Gb",  6}, 
        {"G",   7}, 
        {"G#",  8}, {"Ab",  8}, 
        {"A",   9}, 
        {"A#", 10}, {"Bb", 10}, 
        {"B",  11}, {"Cb", 11}
    };

    static std::map<std::string, std::function<std::vector<int>(int)>> chordMap = {
      {"maj", [](int x) { return std::vector<int>{x, x + major3rd, x + perfect5th}; }},
      {"min", [](int x) { return std::vector<int>{x, x + minor3rd, x + perfect5th}; }},
      {"7", [](int x) { return std::vector<int>{x, x + major3rd, x + perfect5th, x + minor7th}; }},
      {"maj7", [](int x) { return std::vector<int>{x, x + major3rd, x + perfect5th, x + major7th}; }},
      {"min7", [](int x) { return std::vector<int>{x, x + minor3rd, x + perfect5th, x + minor7th}; }},
      {"dim7", [](int x) { return std::vector<int>{x, x + minor3rd, x + diminished5th, x + major6th}; }},
      {"min7b5", [](int x) { return std::vector<int>{x, x + minor3rd, x + diminished5th, x + minor7th}; }}
    };
    
    static std::regex pattern("(^[A-Za-z]#?b?)(.*)$");

    std::smatch match;
    std::regex_match(string, match, pattern);

    std::string note = match[1].str();
    std::string chordType = match[2].str();

    if ((noteMap.find(note) != noteMap.end()) && (chordMap.find(chordType) != chordMap.end())) {
        return chordMap[chordType](noteMap[note]);
    } else {
        throw std::invalid_argument("Failed parsing chord for note " + note + " and chord type " + chordType + ".");
    }
}

std::vector<std::vector<int>> parseChords(std::vector<std::string> chords) {
    std::vector<std::vector<int>> allChordNotes;
    for (std::string chord : chords) {
        std::vector<int> chordNotes = parseChord(chord);
        allChordNotes.push_back(chordNotes);
    }
    return allChordNotes;
}

std::string activeNotesString() {
    std::string result;
    static std::array<std::string, 12> noteSymbols = {"c", "c#", "d", "d#", "e", "f", "f#", "g", "g#", "a", "a#", "b"};
    for (size_t i = 0; i < PIANONOTESACTIVE.size(); ++i) {
        if (PIANONOTESACTIVE[i]) {
            result += noteSymbols[i%12] + " ";
        }
    }
    return result;
}

std::vector<int> removeActiveNotes(std::vector<int>& chord, std::array<bool,36>& activeNotes) {
    std::vector<int> result;
    for (int i : chord){
        if (!activeNotes[i]){
            result.push_back(i);
        }
    }
    return result;
} 

static void midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData) {
    if (message->size() > 1) {
        int noteIndex = static_cast<int>(message->at(1)) - 48;
            PIANONOTESACTIVE[noteIndex] = !(PIANONOTESACTIVE[noteIndex]);
    }
}

RtMidiIn* initMidi(){
  try {
    RtMidiIn* midiin = nullptr;
    midiin = new RtMidiIn(RtMidi::MACOSX_CORE);
    midiin->openPort(0);
    std::cout << "Device name: " << midiin->getPortName(0) << "\n";
    midiin->setCallback(midiCallback);
    midiin->ignoreTypes(false, false, false);
    return midiin;
  } catch (RtMidiError &error) {
    error.printMessage();
  }
}

void drawPiano(Vector2 &windowSize, std::vector<int> &blueChord){
    Color color;
    float size = 25.0f;
    int numOctaves = 3;
    float keyWidth = size;
    float keyHeight = size * 5.0f;
    float startingY = windowSize.y * 0.7f;
    float startingX = windowSize.x * 0.01f;
    static std::vector<int> blackKeys = {1, 3, 6, 8, 10, 13, 15, 18, 20, 22, 25, 27, 30, 32, 34};
    static std::vector<int> whiteKeys = {0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21, 23, 24, 26, 28, 29, 31, 33, 35};
    static std::array<Rectangle, 36> shapes = {
        Rectangle{  startingX,                      startingY,   keyWidth * 1.5f, keyHeight },    // c
        Rectangle{  startingX +      keyWidth,   startingY,   keyWidth, keyHeight * 0.7f},        // c#
        Rectangle{  startingX + 1.5f *  keyWidth,   startingY,   keyWidth * 1.5f, keyHeight },    // d
        Rectangle{  startingX + 2.5f *  keyWidth,   startingY,   keyWidth, keyHeight * 0.7f},     // d#
        Rectangle{  startingX + 3.0f *     keyWidth,   startingY,   keyWidth * 1.5f, keyHeight }, // e
        Rectangle{  startingX + 4.5f *     keyWidth,   startingY,   keyWidth * 1.5f, keyHeight }, // f
        Rectangle{  startingX + 5.5f *  keyWidth,   startingY,   keyWidth, keyHeight * 0.7f},     // f#
        Rectangle{  startingX + 6.0f *     keyWidth,   startingY,   keyWidth * 1.5f, keyHeight }, // g
        Rectangle{  startingX + 7.0f *  keyWidth,   startingY,   keyWidth, keyHeight * 0.7f},     // g#
        Rectangle{  startingX + 7.5f *     keyWidth,   startingY,   keyWidth * 1.5f, keyHeight }, // a
        Rectangle{ startingX + 8.5f * keyWidth,   startingY,   keyWidth, keyHeight * 0.7f},      // a#
        Rectangle{ startingX + 9.0f *    keyWidth,   startingY,   keyWidth * 1.5f, keyHeight },  // b

        Rectangle{ startingX + 10.5f * keyWidth,                      startingY,   keyWidth * 1.5f, keyHeight },    // c
        Rectangle{ startingX + 10.5f * keyWidth +      keyWidth,   startingY,   keyWidth, keyHeight * 0.7f},        // c#
        Rectangle{ startingX + 10.5f * keyWidth + 1.5f *  keyWidth,   startingY,   keyWidth * 1.5f, keyHeight },    // d
        Rectangle{ startingX + 10.5f * keyWidth + 2.5f *  keyWidth,   startingY,   keyWidth, keyHeight * 0.7f},     // d#
        Rectangle{ startingX + 10.5f * keyWidth + 3.0f *     keyWidth,   startingY,   keyWidth * 1.5f, keyHeight }, // e
        Rectangle{ startingX + 10.5f * keyWidth + 4.5f *     keyWidth,   startingY,   keyWidth * 1.5f, keyHeight }, // f
        Rectangle{ startingX + 10.5f * keyWidth + 5.5f *  keyWidth,   startingY,   keyWidth, keyHeight * 0.7f},     // f#
        Rectangle{ startingX + 10.5f * keyWidth + 6.0f *     keyWidth,   startingY,   keyWidth * 1.5f, keyHeight }, // g
        Rectangle{ startingX + 10.5f * keyWidth + 7.0f *  keyWidth,   startingY,   keyWidth, keyHeight * 0.7f},     // g#
        Rectangle{ startingX + 10.5f * keyWidth + 7.5f *     keyWidth,   startingY,   keyWidth * 1.5f, keyHeight }, // a
        Rectangle{ startingX + 10.5f * keyWidth + 8.5f * keyWidth,   startingY,   keyWidth, keyHeight * 0.7f},      // a#
        Rectangle{ startingX + 10.5f * keyWidth + 9.0f *    keyWidth,   startingY,   keyWidth * 1.5f, keyHeight },  // b

        Rectangle{ startingX + 21.0f * keyWidth,                      startingY,   keyWidth * 1.5f, keyHeight },    // c
        Rectangle{ startingX + 21.0f * keyWidth +      keyWidth,   startingY,   keyWidth, keyHeight * 0.7f},        // c#
        Rectangle{ startingX + 21.0f * keyWidth + 1.5f *  keyWidth,   startingY,   keyWidth * 1.5f, keyHeight },    // d
        Rectangle{ startingX + 21.0f * keyWidth + 2.5f *  keyWidth,   startingY,   keyWidth, keyHeight * 0.7f},     // d#
        Rectangle{ startingX + 21.0f * keyWidth + 3.0f *     keyWidth,   startingY,   keyWidth * 1.5f, keyHeight }, // e
        Rectangle{ startingX + 21.0f * keyWidth + 4.5f *     keyWidth,   startingY,   keyWidth * 1.5f, keyHeight }, // f
        Rectangle{ startingX + 21.0f * keyWidth + 5.5f *  keyWidth,   startingY,   keyWidth, keyHeight * 0.7f},     // f#
        Rectangle{ startingX + 21.0f * keyWidth + 6.0f *     keyWidth,   startingY,   keyWidth * 1.5f, keyHeight }, // g
        Rectangle{ startingX + 21.0f * keyWidth + 7.0f *  keyWidth,   startingY,   keyWidth, keyHeight * 0.7f},     // g#
        Rectangle{ startingX + 21.0f * keyWidth + 7.5f *     keyWidth,   startingY,   keyWidth * 1.5f, keyHeight }, // a
        Rectangle{ startingX + 21.0f * keyWidth + 8.5f * keyWidth,   startingY,   keyWidth, keyHeight * 0.7f},      // a#
        Rectangle{ startingX + 21.0f * keyWidth + 9.0f *    keyWidth,   startingY,   keyWidth * 1.5f, keyHeight }   // b
    };

    for (int i : whiteKeys) {
        color = RAYWHITE;
        if (PIANONOTESACTIVE[i]) color = RED;
        else if ((std::find(blueChord.begin(), blueChord.end(), i) != blueChord.end())) color = BLUE;
        DrawRectangleRec(shapes.at(i), color);
        DrawRectangleLinesEx(shapes.at(i), 2, BLACK);
    }

    for (int i : blackKeys) {
        color = BLACK;
        if (PIANONOTESACTIVE[i]) color = RED;
        else if ((std::find(blueChord.begin(), blueChord.end(), i) != blueChord.end())) color = BLUE;
        DrawRectangleRec(shapes.at(i), color);
        DrawRectangleLinesEx(shapes.at(i), 2, BLACK);
    }
}

void renderInputScene(Vector2 &windowSize, char (&input)[256], bool &textBoxEditable, std::vector<std::vector<int>> &parsedChordsResult, Scene &currentScene){
    char text[] = "Enter a list of chords:";
    DrawText(text, windowSize.x * 0.2f, windowSize.y * 0.4f, 20, LIGHTGRAY);
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) 
        && CheckCollisionPointRec(GetMousePosition(), (Rectangle){windowSize.x * 0.3f, windowSize.y * 0.5f, 300, 40})){
        textBoxEditable = !textBoxEditable;
    }
    
    GuiTextBox((Rectangle){windowSize.x * 0.3f, windowSize.y * 0.5f, 300, 40}, input, 256, textBoxEditable);
    
    if (IsKeyDown(KEY_ENTER)) {
        currentScene = SCENE_PIANO;

        std::vector<std::string> chords = splitString((std::string)input, ",");
        parsedChordsResult = parseChords(chords);
    }
}

void renderPianoScene(Vector2 &windowSize, char (&input)[256], std::vector<std::vector<int>> &parsedChords, Scene &currentScene){
    std::vector<int>& chord = parsedChords.front();
    std::cout << "These are the parsedChords\n";
    printVector(parsedChords);
    std::cout << "This is the current chord\n";
    printVector(chord);

    drawPiano(windowSize,chord);
    DrawText(activeNotesString().c_str(), windowSize.x * 0.2f, windowSize.y * 0.4f, 20, LIGHTGRAY);

    chord = removeActiveNotes(chord,PIANONOTESACTIVE);

    if (chord.empty() && !parsedChords.empty()){
        parsedChords.erase(parsedChords.begin());
        if (!parsedChords.empty()){
            chord = parsedChords.front();
        } else {
            std::cout << "Progression finished, please provide more chords\n";
            currentScene = SCENE_INPUT;
        }
    }
}

int main() {
    bool textBoxEditable;
    RtMidiIn* midiin = initMidi();
    Vector2 windowSize = {800, 450};
    Scene currentScene = SCENE_INPUT;
    char input[256] = "Dmin7,Cmaj7,Fmaj7";
    std::vector<std::vector<int>> parsedChords;

    SetTargetFPS(60);
    InitWindow(windowSize.x, windowSize.y, "Piano");
    

    while (!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (currentScene){
            case SCENE_INPUT:
                renderInputScene(windowSize,input,textBoxEditable,parsedChords,currentScene);
                break;
            case SCENE_PIANO:
                renderPianoScene(windowSize,input,parsedChords,currentScene);
                break;
        }
        EndDrawing();
    }
}
