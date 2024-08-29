#include <raylib.h>
#include <RtMidi.h>

std::array<bool, 36> PIANONOTESACTIVE;
std::array<bool, 36> PIANONOTESBLUE;

struct Piano {
    RtMidiIn *midiin = nullptr;
    std::array<Rectangle, 36> shapes;
    std::vector<int> blackKeys = {1, 3, 6, 8, 10, 13, 15, 18, 20, 22, 25, 27, 30, 32, 34};
    std::array<std::string, 12> noteSymbols = {"c", "c#", "d", "d#", "e", "f", "f#", "g", "g#", "a", "a#", "b"};
    std::vector<int> whiteKeys = {0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21, 23, 24, 26, 28, 29, 31, 33, 35};
    Piano(float startingX, float startingY, float size) {
        int octave = 0;
        float keyWidth = size;
        float keyHeight = size * 5.0f;
        for (int octave = 0; octave < 3; octave++){
            shapes[(octave*12)+0] = {  startingX,                      startingY,   keyWidth * 1.5f, keyHeight };    // c
            shapes[(octave*12)+2] = {  startingX + 1.5f *  keyWidth,   startingY,   keyWidth * 1.5f, keyHeight };    // d
            shapes[(octave*12)+4] = {  startingX + 3.0f *     keyWidth,   startingY,   keyWidth * 1.5f, keyHeight }; // e
            shapes[(octave*12)+5] = {  startingX + 4.5f *     keyWidth,   startingY,   keyWidth * 1.5f, keyHeight }; // f
            shapes[(octave*12)+7] = {  startingX + 6.0f *     keyWidth,   startingY,   keyWidth * 1.5f, keyHeight }; // g
            shapes[(octave*12)+9] = {  startingX + 7.5f *     keyWidth,   startingY,   keyWidth * 1.5f, keyHeight }; // a
            shapes[(octave*12)+11] = { startingX + 9.0f *    keyWidth,   startingY,   keyWidth * 1.5f, keyHeight };  // b
            shapes[(octave*12)+1] = {  startingX +      keyWidth,   startingY,   keyWidth, keyHeight * 0.7f};        // c#
            shapes[(octave*12)+3] = {  startingX + 2.5f *  keyWidth,   startingY,   keyWidth, keyHeight * 0.7f};     // d#
            shapes[(octave*12)+6] = {  startingX + 5.5f *  keyWidth,   startingY,   keyWidth, keyHeight * 0.7f};     // f#
            shapes[(octave*12)+8] = {  startingX + 7.0f *  keyWidth,   startingY,   keyWidth, keyHeight * 0.7f};     // g#
            shapes[(octave*12)+10] = { startingX + 8.5f * keyWidth,   startingY,   keyWidth, keyHeight * 0.7f};      // a#
            startingX = startingX + 10.5f * keyWidth;
        }
    }
    void drawNotes(std::vector<int> arr, bool black = false){
        for (int i : arr) {
            Color color = (black) ? BLACK : RAYWHITE;
            if (PIANONOTESBLUE[i]) {
                color = BLUE;
            }
            else if (PIANONOTESACTIVE[i]) {
                color = RED;
                PIANONOTESBLUE[i] = false;
            }
            DrawRectangleRec(shapes.at(i), color);
            DrawRectangleLinesEx(shapes.at(i), 2, BLACK);
        }
    };
    void draw(std::vector<int> blueChord){
        for (auto i: blueChord){
            PIANONOTESBLUE[i] = true;
        }
        drawNotes(whiteKeys);
        drawNotes(blackKeys,true);
    }
    std::string activeNotesString() const {
        std::string _activeNotes;
        for (size_t i = 0; i < PIANONOTESACTIVE.size(); ++i) {
            if (PIANONOTESACTIVE[i]) {
                _activeNotes += noteSymbols[i%12] + " ";
            }
        }
        return _activeNotes;
    }
    static void midiCallback(double deltatime, std::vector<unsigned char> *message, void *userData) {
        if (message->size() > 1) {
            int noteIndex = static_cast<int>(message->at(1)) - 48;
            PIANONOTESACTIVE[noteIndex] = !(PIANONOTESACTIVE[noteIndex]);
        }
    }
    void initMidi(){
      try {
        midiin = new RtMidiIn(RtMidi::MACOSX_CORE);
        midiin->openPort(0);
        std::cout << "Device name: " << midiin->getPortName(0) << "\n";
        midiin->setCallback(midiCallback);
        midiin->ignoreTypes(false, false, false);
      } catch (RtMidiError &error) {
        error.printMessage();
      }
    }
};
