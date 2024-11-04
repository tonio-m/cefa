#include <map>
#include <regex>
#include <sstream>
#include <RtMidi.h>
#include <raylib.h>
#include <algorithm>
#define RAYGUI_IMPLEMENTATION
#include "../include/raygui/raygui.h"


typedef std::vector<int> Chord;
std::array<bool, 36> PIANONOTESACTIVE;
enum Scene { SCENE_INPUT, SCENE_PIANO };
static std::array<std::string, 12> NOTE_SYMBOLS = {"c", "c#", "d", "d#", "e", "f", "f#", "g", "g#", "a", "a#", "b"};
std::map<std::string, std::array<char, 256>> OPTIONS = {
    {"Cmaj/Amin ii-V-I",  {"Dmin7,G7,Cmaj7,Fmaj7,Bmin7b5,E7,Amin7"}},
    {"Fmaj/Dmin ii-V-I",  {"Gmin7,C7,Fmaj7,Bbmaj7,Emin7b5,A7,Dmin7"}},
    {"Bbmaj/Gmin ii-V-I", {"Cmin7,F7,Bbmaj7,Ebmaj7,Amin7b5,D7,Gmin7"}},
    {"Ebmaj/Cmin ii-V-I", {"Fmin7,Bb7,Ebmaj7,Abmaj7,Dmin7b5,G7,Cmin7"}},
    {"Abmaj/Fmin ii-V-I", {"Bbmin7,Eb7,Abmaj7,Dbmaj7,Gmin7b5,C7,Fmin7"}},
    {"Dbmaj/Bbmin ii-V-I",{"Ebmin7,Ab7,Dbmaj7,Gbmaj7,Cmin7b5,F7,Bbmin7"}},
    {"Gbmaj/Ebmin ii-V-I",{"Abmin7,Db7,Gbmaj7,Cbmaj7,Fmin7b5,Bb7,Ebmin7"}},
    {"Bmaj/Abmin ii-V-I", {"C#min7,F#7,Bmaj7,Emaj7,Bbmin7b5,Eb7,Abmin7"}},
    {"Emaj/C#min ii-V-I", {"F#min7,B7,Emaj7,Amaj7,D#min7b5,G#7,C#min7"}},
    {"Amaj/F#min ii-V-I", {"Bmin7,E7,Amaj7,Dmaj7,G#min7b5,C#7,F#min7"}},
    {"Dmaj/Bmin ii-V-I",  {"Emin7,A7,Dmaj7,Gmaj7,C#min7b5,F#7,Bmin7"}},
    {"Gmaj/Emin ii-V-I",  {"Amin7,D7,Gmaj7,Cmaj7,F#min7b5,B7,Emin7"}}
};

template<typename K, typename V>
std::vector<K> mapKeys(std::map<K, V> inputMap) {
    std::vector<K> keys;
    for (const std::pair<K, V> &pair : inputMap) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::vector<std::string> splitString(std::string s, std::string separator) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string token;

    while (std::getline(ss, token, separator[0])) {
        result.push_back(token);
    }
    
    return result;
}

std::string joinString(const std::vector<std::string>& strings, const std::string& delimiter) {
    if (strings.empty()) return "";

    std::ostringstream oss;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i != 0) {
            oss << delimiter;
        }
        oss << strings[i];
    }

    return oss.str();
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

Chord parseChord(std::string string){
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

    static std::map<std::string, std::function<Chord(int)>> chordMap = {
      {"maj", [](int x) { return Chord{x, x + major3rd, x + perfect5th}; }},
      {"min", [](int x) { return Chord{x, x + minor3rd, x + perfect5th}; }},
      {"7", [](int x) { return Chord{x, x + major3rd, x + perfect5th, x + minor7th}; }},
      {"maj7", [](int x) { return Chord{x, x + major3rd, x + perfect5th, x + major7th}; }},
      {"min7", [](int x) { return Chord{x, x + minor3rd, x + perfect5th, x + minor7th}; }},
      {"dim7", [](int x) { return Chord{x, x + minor3rd, x + diminished5th, x + major6th}; }},
      {"min7b5", [](int x) { return Chord{x, x + minor3rd, x + diminished5th, x + minor7th}; }}
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

std::vector<Chord> parseChords(std::vector<std::string> chords) {
    std::vector<Chord> allChordNotes;
    for (std::string chord : chords) {
        Chord chordNotes = parseChord(chord);
        allChordNotes.push_back(chordNotes);
    }
    return allChordNotes;
}

std::string activeNotesString() {
    std::string result;
    for (size_t i = 0; i < PIANONOTESACTIVE.size(); ++i) {
        if (PIANONOTESACTIVE[i]) {
            result += NOTE_SYMBOLS[i%12] + " ";
        }
    }
    return result;
}

std::string getChordString(Chord chord){
    std::string result;
    for (int note : chord) {
        result += NOTE_SYMBOLS[note%12] + " ";
    }
    return result;
}

Chord removeActiveNotes(Chord& chord, std::array<bool,36>& activeNotes) {
    Chord result;
    for (int i : chord){
        if (!activeNotes[i]){
            result.push_back(i);
        }
    }
    return result;
} 

bool compareNotes(Chord& chord, std::array<bool,36>& activeNotes) {
    for (int i : chord){
        if (!activeNotes[i]){
            return false;
        }
    }
    return true;
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

void dummyPcKeyboard(std::array<bool, 36> &PIANONOTESACTIVE){
    static int keys[20] = { 
        KEY_A, KEY_W, KEY_S, KEY_E, KEY_D, KEY_F, KEY_T, KEY_G, KEY_Y, KEY_H, 
        KEY_U, KEY_J, KEY_K, KEY_O, KEY_L, KEY_P, KEY_SEMICOLON, KEY_APOSTROPHE, 
        KEY_RIGHT_BRACKET, KEY_BACKSLASH 
    };
    for (int i = 0; i < 20; i++) {
        if (IsKeyDown(keys[i]))
            PIANONOTESACTIVE[i] = true;
        else if (IsKeyUp(keys[i]))
            PIANONOTESACTIVE[i] = false;
    }
}

void drawPiano(Vector2 &windowSize, Chord &blueChord){
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
        bool isInBlueChord = std::find(blueChord.begin(), blueChord.end(), i) != blueChord.end();
        if (PIANONOTESACTIVE[i] && isInBlueChord) color = GREEN;
        else if (!PIANONOTESACTIVE[i] && isInBlueChord) color = BLUE;
        else if (PIANONOTESACTIVE[i]) color = RED;
        DrawRectangleRec(shapes.at(i), color);
        DrawRectangleLinesEx(shapes.at(i), 2, BLACK);
    }

    for (int i : blackKeys) {
        color = BLACK;
        bool isInBlueChord = std::find(blueChord.begin(), blueChord.end(), i) != blueChord.end();
        if (PIANONOTESACTIVE[i] && isInBlueChord) color = GREEN;
        else if (!PIANONOTESACTIVE[i] && isInBlueChord) color = BLUE;
        else if (PIANONOTESACTIVE[i]) color = RED;
        DrawRectangleRec(shapes.at(i), color);
        DrawRectangleLinesEx(shapes.at(i), 2, BLACK);
    }
}

void playPianoSounds(std::array<bool,36> &activeNotes, std::array<bool,36> &pastActiveNotes, std::array<Music,36> pianoSounds){
    for(size_t i = 0; i < activeNotes.size(); i++){
        if (!(pastActiveNotes[i]) && activeNotes[i]){
            if (IsMusicStreamPlaying(pianoSounds[i])) {
                StopMusicStream(pianoSounds[i]);
            }
            PlayMusicStream(pianoSounds[i]);
        }
    }
}

void parseInputString(
        char *input,
        std::vector<Chord> &parsedChordsResult
    ){
    std::vector<std::string> chords = splitString((std::string)input, ",");
    parsedChordsResult = parseChords(chords);
}

void renderInputScene(
        bool &loop,
        Vector2 &windowSize,
        char (&input)[256],
        bool &textBoxEditable,
        int &radioScrollIndex,
        int &radioActiveIndex,
        std::vector<Chord> &parsedChordsResult,
        Scene &currentScene
    ){
    char text[] = "Enter a list of chords:";
    DrawText(text, windowSize.x * 0.2f, windowSize.y * 0.4f, 20, LIGHTGRAY);
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) 
        && CheckCollisionPointRec(GetMousePosition(), (Rectangle){windowSize.x * 0.3f, windowSize.y * 0.5f, 300, 40})){
        textBoxEditable = !textBoxEditable;
    }
    
    GuiTextBox((Rectangle){windowSize.x * 0.3f, windowSize.y * 0.5f, 300, 40}, input, 256, textBoxEditable);
    
    if (IsKeyDown(KEY_ENTER)) {
        currentScene = SCENE_PIANO;
        parseInputString(input, parsedChordsResult);
    }

    std::vector<std::string> optionsKeys = mapKeys(OPTIONS);
    optionsKeys.insert(optionsKeys.begin(), "User Input");
    std::string optionsString = joinString(optionsKeys,";");

    GuiListView((Rectangle){windowSize.x * 0.3f, windowSize.y * 0.6f, 300, 100}, optionsString.c_str(), &radioScrollIndex, &radioActiveIndex);

    GuiCheckBox((Rectangle){windowSize.x * 0.3f, windowSize.y * 0.3f, 20, 20}, "Loop", &loop);

    if (radioActiveIndex != 0){
        strcpy(input, OPTIONS[optionsKeys[radioActiveIndex + 1]].data());
    }
}



void renderPianoScene(
        Vector2 &windowSize,
        bool loop,
        char (&input)[256],
        std::vector<Chord> &parsedChords,
        std::array<Music,36> pianoSounds,
        std::array<bool,36> lastPIANONOTESACTIVE,
        Scene &currentScene
    ){
    Chord& chord = parsedChords.front();
    drawPiano(windowSize,chord);
    playPianoSounds(PIANONOTESACTIVE,lastPIANONOTESACTIVE,pianoSounds);

    DrawText(activeNotesString().c_str(), windowSize.x * 0.2f, windowSize.y * 0.4f, 20, LIGHTGRAY);
    DrawText(getChordString(chord).c_str(), windowSize.x * 0.7f, windowSize.y * 0.4f, 20, DARKBLUE);

    if(compareNotes(chord,PIANONOTESACTIVE) && !parsedChords.empty()){
        parsedChords.erase(parsedChords.begin());

        if (!parsedChords.empty()){
            chord = parsedChords.front();
        } else if (loop){
            parseInputString(input, parsedChords);
        } else {
            std::cout << "Progression finished, please provide more chords\n";
            currentScene = SCENE_INPUT;
        }
    }
}


int main() {
    // Initialize variables
    bool loop = false;
    int radioScroll = 0;
    int radioActive = 0;
    bool textBoxEditable = true;
    RtMidiIn* midiin = initMidi();
    std::vector<Chord> parsedChords;
    Vector2 windowSize = {800, 450};
    Scene currentScene = SCENE_INPUT;
    char input[256] = "Dmin7,Cmaj7,Fmaj7";
    std::array<bool, 36> lastPIANONOTESACTIVE = {0};

    // Initialize window and audio
    SetTargetFPS(60);
    InitWindow(windowSize.x, windowSize.y, "Piano");
    InitAudioDevice();

    // Define piano sound file paths
    const std::array<const char*, 12> soundFiles = {
        "/Users/user/repos/cefa/src/assets/c1.wav",
        "/Users/user/repos/cefa/src/assets/c1s.wav",
        "/Users/user/repos/cefa/src/assets/d1.wav",
        "/Users/user/repos/cefa/src/assets/d1s.wav",
        "/Users/user/repos/cefa/src/assets/e1.wav",
        "/Users/user/repos/cefa/src/assets/f1.wav",
        "/Users/user/repos/cefa/src/assets/f1s.wav",
        "/Users/user/repos/cefa/src/assets/g1.wav",
        "/Users/user/repos/cefa/src/assets/g1s.wav",
        "/Users/user/repos/cefa/src/assets/a1.wav",
        "/Users/user/repos/cefa/src/assets/a1s.wav",
        "/Users/user/repos/cefa/src/assets/b1.wav"
    };

    // Load piano sounds
    std::array<Music, 36> pianoSounds;
    for(int octave = 0; octave < 3; octave++) {
        for(int note = 0; note < 12; note++) {
            int index = octave * 12 + note;
            pianoSounds[index] = LoadMusicStream(soundFiles[note]);
            pianoSounds[index].looping = false;
            SetMusicPitch(pianoSounds[index], 1.0f * std::pow(2.0f, octave));
        }
    }

    // Main game loop
    while (!WindowShouldClose()) {
        // Update audio streams
        for (auto& sound : pianoSounds) {
            UpdateMusicStream(sound);
        }

        // Handle keyboard input
        dummyPcKeyboard(PIANONOTESACTIVE);

        // Render
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Scene management
        switch (currentScene) {
            case SCENE_INPUT:
                renderInputScene(
                    loop,
                    windowSize,
                    input,
                    textBoxEditable,
                    radioScroll,
                    radioActive,
                    parsedChords,
                    currentScene
                );
                break;
            case SCENE_PIANO:
                renderPianoScene(
                    windowSize,
                    loop,
                    input,
                    parsedChords,
                    pianoSounds,
                    lastPIANONOTESACTIVE,
                    currentScene
                );
                break;
        }

        EndDrawing();
        
        // Store current piano state
        std::copy(PIANONOTESACTIVE.begin(), PIANONOTESACTIVE.end(), lastPIANONOTESACTIVE.begin());
    }

    // Cleanup
    for (auto& sound : pianoSounds) {
        UnloadMusicStream(sound);
    }
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
