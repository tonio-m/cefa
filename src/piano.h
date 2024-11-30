#pragma once
#include "iostream"
#include <array>
#include "chord.h"
#include <raylib.h>

struct Piano {
    // std::array<Music, 36> pianoSounds;
    std::array<Sound, 36> pianoSounds;
    std::array<const char*, 36> soundFiles;

    ~Piano();
    void initSound();
    void draw(Vector2 &windowSize, Chord &blueChord, std::array<int,36> &pianoNotesActive);
    void playSounds(std::array<int,36> &activeNotes, std::array<int,36> &pastActiveNotes);
};
void dummyPcKeyboard(std::array<int, 36> &PIANONOTESACTIVE);
