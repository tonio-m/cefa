#include "chord.h"

Chord& Chord::fromString(std::string string){            
    std::smatch match;
    static std::regex pattern("(^[A-Za-z]#?b?)(.*)$");

    std::regex_match(string, match, pattern);

    std::string note = match[1].str();
    std::string chordType = match[2].str();

    if ((noteMap.find(note) != noteMap.end()) && (qualityToVector.find(chordType) != qualityToVector.end())) {
        value = qualityToVector[chordType](noteMap[note]);
        return *this;
    } else {
        throw std::invalid_argument("Failed parsing chord for note " + note + " and chord type " + chordType + ".");
    }
}

std::string Chord::notesString() {
    std::string result;
    for (int note : value){
        result += noteStrings[note%12] + " ";
    }
    return result;
}


bool Chord::compareNotes(std::array<int,36>& activeNotes) {
    for (int i : value){
        if (i < 0 || i >= activeNotes.size() || !activeNotes[i]) {
            return false;
        }
    }
    return true;
} 

std::vector<Chord> parseChords(std::vector<std::string> chords) {
    std::vector<Chord> allChordNotes;
    for (std::string chord : chords) {
        Chord chordNotes = Chord().fromString(chord);
        allChordNotes.push_back(chordNotes);
    }
    return allChordNotes;
}
