#include "piano.h"

Piano::~Piano(){
    for (auto& sound : pianoSounds) {
        // UnloadMusicStream(sound);
        UnloadSound(sound);
    }
    CloseAudioDevice();
}

void Piano::initSound(){
    InitAudioDevice();
    pianoSounds = {
        LoadSound("/Users/user/repos/cefa/assets/piano/c1.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/c1s.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/d1.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/d1s.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/e1.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/f1.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/f1s.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/g1.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/g1s.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/a1.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/a1s.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/b1.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/c2.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/c2s.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/d2.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/d2s.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/e2.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/f2.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/f2s.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/g2.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/g2s.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/a2.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/a2s.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/b2.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/c3.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/c3s.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/d3.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/d3s.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/e3.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/f3.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/f3s.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/g3.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/g3s.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/a3.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/a3s.wav"),
        LoadSound("/Users/user/repos/cefa/assets/piano/b3.wav")
    };

    std::cout << "Sound Loaded successfully" << std::endl;
}

void Piano::setVolume(int volume){
    for (auto& sound : pianoSounds){
        SetSoundVolume(sound, volume/100.0f);
    }
}

void Piano::draw(Vector2 &windowSize, Chord &blueChord, std::array<int,36> &pianoNotesActive){
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
        bool isInBlueChord = std::find(blueChord.value.begin(), blueChord.value.end(), i) != blueChord.value.end();
        if (pianoNotesActive[i] && isInBlueChord) color = GREEN;
        else if (!pianoNotesActive[i] && isInBlueChord) color = BLUE;
        else if (pianoNotesActive[i]) color = RED;
        DrawRectangleRec(shapes.at(i), color);
        DrawRectangleLinesEx(shapes.at(i), 2, BLACK);
    }
    for (int i : blackKeys) {
        color = BLACK;
        bool isInBlueChord = std::find(blueChord.value.begin(), blueChord.value.end(), i) != blueChord.value.end();
        if (pianoNotesActive[i] && isInBlueChord) color = GREEN;
        else if (!pianoNotesActive[i] && isInBlueChord) color = BLUE;
        else if (pianoNotesActive[i]) color = RED;
        DrawRectangleRec(shapes.at(i), color);
        DrawRectangleLinesEx(shapes.at(i), 2, BLACK);
    }
}

void Piano::playSounds(std::array<int,36> &activeNotes, std::array<int,36> &pastActiveNotes){
    // TODO: debug this, seems to not be working
    for(size_t i = 0; i < activeNotes.size(); i++){
        if (!(pastActiveNotes[i] == 1) && activeNotes[i] == 1){
            std::cout << "Playing Note: " << i << std::endl;
            if (IsSoundPlaying(pianoSounds[i])) {
                StopSound(pianoSounds[i]);
            }
            PlaySound(pianoSounds[i]);
        }
    }
}

void dummyPcKeyboard(std::array<int, 36> &PIANONOTESACTIVE){
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
