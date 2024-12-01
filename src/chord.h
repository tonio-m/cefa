#pragma once
#include <vector>
#include <regex>
#include <map>

struct Chord {
    inline static std::array<std::string, 12> noteStrings = {"c", "c#", "d", "d#", "e", "f", "f#", "g", "g#", "a", "a#", "b"};
    enum semitones {
        minor3rd = 3,
        major3rd = 4,
        diminished5th = 6,
        perfect5th = 7,
        major6th = 9,
        minor7th = 10,
        major7th = 11
    };
    inline static std::map<std::string, int> noteMap = {
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
    inline static std::map<std::string, std::function<std::vector<int>(int)>> qualityToVector = {
        {"maj", [](int x) { return std::vector<int>{x, x + major3rd, x + perfect5th}; }},
        {"min", [](int x) { return std::vector<int>{x, x + minor3rd, x + perfect5th}; }},
        {"7", [](int x) { return std::vector<int>{x, x + major3rd, x + perfect5th, x + minor7th}; }},
        {"maj7", [](int x) { return std::vector<int>{x, x + major3rd, x + perfect5th, x + major7th}; }},
        {"min7", [](int x) { return std::vector<int>{x, x + minor3rd, x + perfect5th, x + minor7th}; }},
        {"dim7", [](int x) { return std::vector<int>{x, x + minor3rd, x + diminished5th, x + major6th}; }},
        {"min7b5", [](int x) { return std::vector<int>{x, x + minor3rd, x + diminished5th, x + minor7th}; }}
    };

    std::string rootNote;
    std::string chordQuality;
    std::vector<int> value;

    Chord& fromString(std::string string);
    std::string notesString();

    bool compareNotes(std::array<int,36>& activeNotes);
};

std::vector<Chord> parseChords(std::vector<std::string> chords);
