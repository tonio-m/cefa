#include <map>
#include "chord.h"
#include "utils.h"
#include "piano.h"
#include <raylib.h>
#include "midiHandler.h"
#define RAYGUI_IMPLEMENTATION
#include "../include/raygui/raygui.h"
#include "../include/rfxgen/rfxgen.h"

struct SceneController {
    enum Scene { SCENE_INPUT, SCENE_PIANO, SCENE_CONFIG };
    struct {
        bool loop;
        char textBox001Input[256] = "Dmin7,Cmaj7,Fmaj7";
        Scene currentScene = SCENE_INPUT;
        Vector2 windowSize = {800, 450};
        bool textBox001Editable;
        int listView001Active;
        int listview001ScrollIndex;
        float volume = 50;
        int DropdownBox001Active;
        int DropdownBox006Active;
        std::vector<Chord> parsedChords;
        std::array<Music,36> pianoSounds;
        bool usePcKeyboard;
        bool DropdownBox001EditMode;
        bool DropdownBox006EditMode;
        std::array<int,36> lastPianoNotesActive;
        std::vector<std::string> listView001Keys = {
            "User Input",
            "Cmaj/Amin ii-V-I",
            "Fmaj/Dmin ii-V-I",
            "Bbmaj/Gmin ii-V-I",
            "Ebmaj/Cmin ii-V-I",
            "Abmaj/Fmin ii-V-I",
            "Dbmaj/Bbmin ii-V-I",
            "Gbmaj/Ebmin ii-V-I",
            "Bmaj/Abmin ii-V-I",
            "Emaj/C#min ii-V-I",
            "Amaj/F#min ii-V-I",
            "Dmaj/Bmin ii-V-I",
            "Gmaj/Emin ii-V-I"
        };
        std::vector<std::string> listView001Values = {
            "Dmin7,Fmaj7,Amin,Cmaj7",
            "Dmin7,G7,Cmaj7,Fmaj7,Bmin7b5,E7,Amin7",
            "Gmin7,C7,Fmaj7,Bbmaj7,Emin7b5,A7,Dmin7",
            "Cmin7,F7,Bbmaj7,Ebmaj7,Amin7b5,D7,Gmin7",
            "Fmin7,Bb7,Ebmaj7,Abmaj7,Dmin7b5,G7,Cmin7",
            "Bbmin7,Eb7,Abmaj7,Dbmaj7,Gmin7b5,C7,Fmin7",
            "Ebmin7,Ab7,Dbmaj7,Gbmaj7,Cmin7b5,F7,Bbmin7",
            "Abmin7,Db7,Gbmaj7,Cbmaj7,Fmin7b5,Bb7,Ebmin7",
            "C#min7,F#7,Bmaj7,Emaj7,Bbmin7b5,Eb7,Abmin7",
            "F#min7,B7,Emaj7,Amaj7,D#min7b5,G#7,C#min7",
            "Bmin7,E7,Amaj7,Dmaj7,G#min7b5,C#7,F#min7",
            "Emin7,A7,Dmaj7,Gmaj7,C#min7b5,F#7,Bmin7",
            "Amin7,D7,Gmaj7,Cmaj7,F#min7b5,B7,Emin7"
        };
    } context;
    Piano piano;
    MidiHandler midihandler;
    Sound music;

    SceneController(){
        piano = Piano();
        piano.initSound();
        midihandler = MidiHandler();
        midihandler.init();
    }

    void renderInputScene(){
        DrawText("Enter a list of chords:", context.windowSize.x * 0.2f, context.windowSize.y * 0.4f, 20, LIGHTGRAY);

        // make textbox editable if you click on it (and make option = User Input) 
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), (Rectangle){context.windowSize.x * 0.3f, context.windowSize.y * 0.5f, 300, 40})){
            context.listView001Active = 0;
            context.textBox001Editable = !context.textBox001Editable;
        }
        GuiTextBox((Rectangle){context.windowSize.x * 0.3f, context.windowSize.y * 0.5f, 300, 40}, context.textBox001Input, 256, context.textBox001Editable);

        // press up/down to travel through the list
        if (IsKeyPressed(KEY_UP)) {
            context.listView001Active = std::max(context.listView001Active - 1,0);
            context.listview001ScrollIndex = std::max(context.listView001Active - 2,0);
        } else if (IsKeyPressed(KEY_DOWN)) {
            context.listView001Active = std::min(context.listView001Active + 1,(int)context.listView001Keys.size()-1);
            context.listview001ScrollIndex = std::min(context.listView001Active - 2, (int)context.listView001Keys.size()-1);
        }

        GuiListView(
            (Rectangle){context.windowSize.x * 0.3f, context.windowSize.y * 0.6f, 300, 100},
            joinString(context.listView001Keys,";").c_str(),
            &context.listview001ScrollIndex,
            &context.listView001Active);
        GuiCheckBox((Rectangle){context.windowSize.x * 0.7f, context.windowSize.y * 0.52f, 20, 20}, "Loop", &context.loop);

        // config button
        if (GuiButton((Rectangle){context.windowSize.x * 0.42f, context.windowSize.y * 0.85f, 100, 20}, "Config")) {
            context.currentScene = SCENE_CONFIG;
        }

        // overwrite User Input value with last selected option
        if (context.listView001Active != 0){ 
            strcpy(context.textBox001Input, context.listView001Values[context.listView001Active].data());
        }

        // go to piano Scene if ENTER is pressed
        if (IsKeyDown(KEY_ENTER)) {
            std::vector<std::string> chords = splitString((std::string)context.textBox001Input, ",");
            context.parsedChords = parseChords(chords);
            context.currentScene = SCENE_PIANO;
        }
    }

    void renderPianoScene(){
        Chord& chord = context.parsedChords.front();

        if (context.usePcKeyboard){
            dummyPcKeyboard(midihandler.pianoNotesActive);
        }

        piano.draw(context.windowSize, chord, midihandler.pianoNotesActive);
        piano.playSounds(midihandler.pianoNotesActive,context.lastPianoNotesActive);

        DrawText(midihandler.activeNotesString().c_str(), context.windowSize.x * 0.2f, context.windowSize.y * 0.4f, 20, LIGHTGRAY);
        DrawText(chord.notesString().c_str(), context.windowSize.x * 0.2f, context.windowSize.y * 0.45f, 20, DARKBLUE);
        
        if(chord.compareNotes(midihandler.pianoNotesActive) && !context.parsedChords.empty()){
            context.parsedChords.erase(context.parsedChords.begin());
    
            if (!context.parsedChords.empty()){
                chord = context.parsedChords.front();
            } else if (context.loop){
                std::vector<std::string> chords = splitString((std::string)context.textBox001Input, ",");
                context.parsedChords = parseChords(chords);
            } else {
                std::cout << "Progression finished, please provide more chords\n";
                context.currentScene = SCENE_INPUT;
            }
        }

        std::copy(midihandler.pianoNotesActive.begin(), midihandler.pianoNotesActive.end(), context.lastPianoNotesActive.begin());
    }

    void renderConfigScene(){
        if (context.DropdownBox001EditMode || context.DropdownBox006EditMode) GuiLock();
    
        GuiGroupBox((Rectangle){ 48, 72, 226, 155 }, "Settings");

        GuiSlider((Rectangle){ 98, 165, 120, 16 }, "Volume", NULL, &context.volume, 0, 100);
        GuiCheckBox((Rectangle){ 98, 189, 16, 14 }, "Use PC keyboard for input", &context.usePcKeyboard);

        if (GuiButton((Rectangle){ 98, 210, 120, 30 }, "Save Settings")) {
            context.currentScene = SCENE_INPUT;
        }

        // TODO: make this driver get the options from MidiHandler
        GuiLabel((Rectangle){ 58, 132, 120, 24 }, "Device");
        if (GuiDropdownBox((Rectangle){ 97, 132, 120, 24 }, "(0) Reface CS;(1) Minilab MkII", &context.DropdownBox006Active, context.DropdownBox006EditMode)) {
            context.DropdownBox006EditMode = !context.DropdownBox006EditMode;
        }

        // TODO: make this driver get the options from MidiHandler
        GuiLabel((Rectangle){ 58, 99, 120, 24 }, "Driver");
        if (GuiDropdownBox((Rectangle){ 97, 99, 120, 24 }, "MacOSX Core; Windows MM; Linux Alsa; Linux JACK", &context.DropdownBox001Active, context.DropdownBox001EditMode)) {
            context.DropdownBox001EditMode = !context.DropdownBox001EditMode;
        }

        GuiUnlock();
    }
    
    void renderCurrentScene(){
        switch (context.currentScene) {
            case SCENE_INPUT:
                renderInputScene();
                break;
            case SCENE_PIANO:
                renderPianoScene();
                break;
            case SCENE_CONFIG:
                renderConfigScene();
                break;
        }
    }
};

int main() {
    auto scenecontroller = SceneController();
    SetTargetFPS(60);
    InitWindow(scenecontroller.context.windowSize.x, scenecontroller.context.windowSize.y, "CEFA");
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        scenecontroller.renderCurrentScene();
        EndDrawing();
    }
    // Cleanup
    CloseWindow();
    return 0;
}

