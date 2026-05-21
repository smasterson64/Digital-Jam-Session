#ifndef __AS6_H__
#define __AS6_H__

#include "AudioDevice.hpp"
#include "Camera3D.hpp"
#include "Matrix.hpp"
#include "Model.hpp"
#include "Vector4.hpp"
#include "raylib-cpp.hpp"
#include <chrono>
#include <deque>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include <memory>

#include <BufferedRaylib.hpp>

#define SKYBOX_IMPLEMENTATION
#include "skybox.hpp"

#define GUI_LAYOUT_NAME_IMPLEMENTATION
#include "GUI.h"

// notes enum. helps with calculating the pitch of the desired note
enum Note {
    C = 0,
    Csharp,
    D,
    Dsharp,
    E,
    F,
    Fsharp,
    G,
    Gsharp,
    A,
    Asharp,
    B,
    C2
};


//chord struct: a chord is a group of notes specified by a root note and the quality of the chord
struct Chord {
    
    //The root note is the starting place of the chord
    Note rootNote = C;

    //there are many possible chord qualities, but I'm sticking with these few for now
    std::vector<Note> notes;
    enum Quality {
        Major,
        Major7,
        Major9,
        Minor,
        Minor7,
        Minor9,
        Dominant,
        Diminished,
        Augmented,
        Augmented7
    };

    // This is the current quality of this chord
    Quality chordQuality = Major;

    void setRoot(Note note) { rootNote = note; }

    void setQuality(Quality q) {
        // Clear the notes vector so we can add the desired notes of the new chord quality as needed
        notes.clear();

        //root note will always be added to the chord
        notes.emplace_back(rootNote);
        chordQuality = q;

        //add to the notes vector according to chord quality
        switch (chordQuality) {
        case Major:
            notes.emplace_back(static_cast<Note>((int)rootNote + 4));  // Major 3rd
            notes.emplace_back(static_cast<Note>((int)rootNote + 7));  // perfect 5th
            notes.emplace_back(static_cast<Note>((int)rootNote + 12)); // Octave
            break;
        case Major7:
            notes.emplace_back(static_cast<Note>((int)rootNote + 4));  // Major 3rd
            notes.emplace_back(static_cast<Note>((int)rootNote + 7));  // perfect 5th
            notes.emplace_back(static_cast<Note>((int)rootNote + 11)); // Major 7th
            break;
        case Major9:
            notes.emplace_back(static_cast<Note>((int)rootNote + 4));  // Major 3rd
            notes.emplace_back(static_cast<Note>((int)rootNote + 7));  // perfect 5th
            notes.emplace_back(static_cast<Note>((int)rootNote + 11)); // Major 7th
            notes.emplace_back(static_cast<Note>((int)rootNote + 14)); // Major 9th
            break;
        case Minor:
            notes.emplace_back(static_cast<Note>((int)rootNote + 3));  // Minor 3rd
            notes.emplace_back(static_cast<Note>((int)rootNote + 7));  // perfect 5th
            notes.emplace_back(static_cast<Note>((int)rootNote + 12)); // Octave
            break;
        case Minor7:
            notes.emplace_back(static_cast<Note>((int)rootNote + 3));  // Minor 3rd
            notes.emplace_back(static_cast<Note>((int)rootNote + 7));  // perfect 5th
            notes.emplace_back(static_cast<Note>((int)rootNote + 10)); // Minor 7th
            break;
        case Minor9:
            notes.emplace_back(static_cast<Note>((int)rootNote + 3));  // Minor 3rd
            notes.emplace_back(static_cast<Note>((int)rootNote + 7));  // perfect 5th
            notes.emplace_back(static_cast<Note>((int)rootNote + 10)); // Minor 7th
            notes.emplace_back(static_cast<Note>((int)rootNote + 14)); // Minor 9th
            break;
        case Dominant:
            notes.emplace_back(static_cast<Note>((int)rootNote + 4));  // Major 3rd
            notes.emplace_back(static_cast<Note>((int)rootNote + 7));  // perfect 5th
            notes.emplace_back(static_cast<Note>((int)rootNote + 10)); // Dominant 7th
            break;
        case Diminished:
            notes.emplace_back(static_cast<Note>((int)rootNote + 3)); // Minor 3rd
            notes.emplace_back(
                static_cast<Note>((int)rootNote + 6)); // diminished 5th
            notes.emplace_back(
                static_cast<Note>((int)rootNote + 9)); // diminished 7th
            break;
        case Augmented:
            notes.emplace_back(static_cast<Note>((int)rootNote + 4));  // Major 3rd
            notes.emplace_back(static_cast<Note>((int)rootNote + 8));  // augmented 5th
            notes.emplace_back(static_cast<Note>((int)rootNote + 12)); // Octave
            break;
        case Augmented7:
            notes.emplace_back(static_cast<Note>((int)rootNote + 4));  // Major 3rd
            notes.emplace_back(static_cast<Note>((int)rootNote + 8));  // augmented 5th
            notes.emplace_back(static_cast<Note>((int)rootNote + 10)); // Dominant 7th
            break;
        }
    }
};



//Base component class -- sets up skeleton for Setup, Update and Cleanup
class Component {
  public:

    //Attached this component is attached to. helps with 2-way communication b/w component and entity
    struct Entity *attached;

    virtual void Setup() = 0;
    virtual void Update(float dt) = 0;
    virtual void Cleanup() = 0;
};

//List of components; functionality is for adding and getting components
struct Entity {
    std::vector<std::shared_ptr<Component>> components;

    template <std::derived_from<Component> T>
    T &AddComponent() {
        auto out = components.emplace_back(std::make_shared<T>());
        out->attached = this;
        return (T &)*out;
    }

    template <std::derived_from<Component> T>
    std::optional<std::reference_wrapper<T>> GetComponent() {
        for (auto &component : components) {
            T *cast = dynamic_cast<T *>(component.get());
            if (cast)
                return *cast;
        }

        return {};
        // throw std::runtime_error("Component Not Attached");
    }
};



//Transform component: keeps track of state
struct TransformComponent : public Component {
    raylib::Vector3 position = {0, 0, 0};
    raylib::Quaternion rotation = raylib::Quaternion::Identity();

    //The transforms in this project also keep track of whether or not the entity is currently selected
    bool isSelected = false;

    void Setup() override {}
    void Update(float dt) override {}
    void Cleanup() override {}
};

// Input component: keeps track of which keys are being presse
struct InputComponent : public Component {
    raylib::BufferedInput input;

    // button for playing a random note
    bool RandomNoteValue = false;

    //If noteValue < 0, do not play a note, otherwise, notevalue is the same as the Notes ennum
    int noteValue = -1;

    void Setup() override {

        //setup "piano keys"
        input["C1"] = raylib::Action::key(KEY_Z)
                          .AddPressedCallback([this]() { noteValue = 0; })
                          .AddReleasedCallback([this]() { noteValue = -1; })
                          .move();

        input["C#"] = raylib::Action::key(KEY_S)
                          .AddPressedCallback([this]() { noteValue = 1; })
                          .AddReleasedCallback([this]() { noteValue = -1; })
                          .move();

        input["D"] = raylib::Action::key(KEY_X)
                         .AddPressedCallback([this]() { noteValue = 2; })
                         .AddReleasedCallback([this]() { noteValue = -1; })
                         .move();

        input["D#"] = raylib::Action::key(KEY_D)
                          .AddPressedCallback([this]() { noteValue = 3; })
                          .AddReleasedCallback([this]() { noteValue = -1; })
                          .move();

        input["E"] = raylib::Action::key(KEY_C)
                         .AddPressedCallback([this]() { noteValue = 4; })
                         .AddReleasedCallback([this]() { noteValue = -1; })
                         .move();

        input["F"] = raylib::Action::key(KEY_V)
                         .AddPressedCallback([this]() { noteValue = 5; })
                         .AddReleasedCallback([this]() { noteValue = -1; })
                         .move();

        input["F#"] = raylib::Action::key(KEY_G)
                          .AddPressedCallback([this]() { noteValue = 6; })
                          .AddReleasedCallback([this]() { noteValue = -1; })
                          .move();

        input["G"] = raylib::Action::key(KEY_B)
                         .AddPressedCallback([this]() { noteValue = 7; })
                         .AddReleasedCallback([this]() { noteValue = -1; })
                         .move();

        input["G#"] = raylib::Action::key(KEY_H)
                          .AddPressedCallback([this]() { noteValue = 8; })
                          .AddReleasedCallback([this]() { noteValue = -1; })
                          .move();

        input["A"] = raylib::Action::key(KEY_N)
                         .AddPressedCallback([this]() { noteValue = 9; })
                         .AddReleasedCallback([this]() { noteValue = -1; })
                         .move();

        input["A#"] = raylib::Action::key(KEY_J)
                          .AddPressedCallback([this]() { noteValue = 10; })
                          .AddReleasedCallback([this]() { noteValue = -1; })
                          .move();

        input["B"] = raylib::Action::key(KEY_M)
                         .AddPressedCallback([this]() { noteValue = 11; })
                         .AddReleasedCallback([this]() { noteValue = -1; })
                         .move();

        input["C2"] = raylib::Action::key(KEY_COMMA)
                          .AddPressedCallback([this]() { noteValue = 12; })
                          .AddReleasedCallback([this]() { noteValue = -1; })
                          .move();

        //Random Note button
        input["Random"] =
            raylib::Action::key(KEY_SPACE)
                .AddPressedCallback([this]() {
                    RandomNoteValue = true;
                })
                .AddReleasedCallback([this]() { RandomNoteValue = false; })
                .move();

              
    }
    void Update(float dt) override { input.PollEvents(); }
    void Cleanup() override {}
};

//Music Source: Plays background music and keeps track of position in song and updates the currently active chord notes
struct MusicSourceComponent : public Component {
    raylib::Music *music;
    float tempo = 120.0 / 60;   // in beats per second
    float chordTempo = 30 / 60; // chord changes per second
    float length = 0;           // how long the track is
    float activeChordIndex = 0;
    std::vector<Chord> chords;
    Chord activeChord;

    void Setup() override {
        length = music->GetTimeLength();

        // HACK -- set chords for test song
        Chord CMaj7(C);
        CMaj7.setQuality(Chord::Major7);
        Chord DMin7(D);
        DMin7.setQuality(Chord::Minor7);
        Chord G7(G);
        G7.setQuality(Chord::Dominant);
        Chord Csharpdim(Csharp);
        Csharpdim.setQuality(Chord::Diminished);
        Chord EMin7(E);
        EMin7.setQuality(Chord::Minor7);
        Chord EbMin7(Dsharp);
        EbMin7.setQuality(Chord::Minor7);
        chords.reserve(16);

        chords.emplace_back(DMin7);
        chords.emplace_back(G7);
        chords.emplace_back(CMaj7);
        chords.emplace_back(CMaj7);
        chords.emplace_back(DMin7);
        chords.emplace_back(G7);
        chords.emplace_back(CMaj7);
        chords.emplace_back(Csharpdim);
        chords.emplace_back(DMin7);
        chords.emplace_back(G7);
        chords.emplace_back(EMin7);
        chords.emplace_back(EbMin7);
        chords.emplace_back(DMin7);
        chords.emplace_back(G7);
        chords.emplace_back(CMaj7);
        chords.emplace_back(CMaj7);

        if (!chords.empty()) {
            activeChord = chords[0];
        }

        //begin music playback
        music->Play();
    }
    void Update(float dt) override {
        music->Update();

        if (chords.empty())
            return;

        // copilot helped here getting the index to update every 2 seconds
        float playedSeconds = music->GetTimePlayed();
        int newChordIndex = static_cast<int>(playedSeconds / 2.0f) %
                            static_cast<int>(chords.size());
        if (newChordIndex < 0)
            newChordIndex += chords.size();

        if (newChordIndex != static_cast<int>(activeChordIndex)) {
            activeChordIndex = newChordIndex;
            activeChord = chords[activeChordIndex];
        }
    }
    void Cleanup() override {}
};

struct NoteCounterComponent : public Component {
    int counts[14] = {};
    std::string noteNames[14] = {"C: ","C#: ","D: ","D#: ","E: ","F: ","F#: ","G: ","G#: ","A: ","A#: ","B: ","C2: ", "random: "};
    raylib::Text noteTexts[14];

    std::string noteCounts[14] = {"0","0","0","0","0","0","0","0","0","0","0","0","0","0"};
    raylib::Text valueTexts[14];


    void Setup() override {
        for (int i = 0; i < 14; ++i){
            noteTexts[i].SetText(noteNames[i]);
        }
    }
    void Update(float dt) override {}

    void DrawStats() {

        if (auto t = attached->GetComponent<TransformComponent>(); t) {
            auto &transform = t->get();
            if (transform.isSelected) {
  
                for(int i = 0; i < 14; ++i){
                    noteTexts[i].SetFontSize(20);
                    noteTexts[i].SetColor(raylib::Color::Black());
                    valueTexts[i].SetFontSize(20);
                    valueTexts[i].SetColor(raylib::Color::Black());

                   noteTexts[i].Draw(raylib::Vector2(20, i*20));
                   noteCounts[i] = std::to_string(counts[i]);
                   valueTexts[i].SetText(noteCounts[i]);
                   valueTexts[i].Draw(raylib::Vector2(100, i*20));
                                             std::cout<<"tawing stats" << std::endl;
                }

            }
        }   
    }
    void Cleanup() override {}
    void Increment(int index){
        counts[index]++;
    }
};

//Component for playing sound based on input
struct PlaySoundComponent : public Component {
    raylib::Sound *sound;
    MusicSourceComponent *source;

    void Setup() override {}
    void Update(float dt) override {

        //check if the attached entity is currently selecte
        if (auto t = attached->GetComponent<TransformComponent>(); t) {
            auto &transform = t->get();
            if (transform.isSelected) {

                //if entity is selected, check its attached input component
                if (auto i = attached->GetComponent<InputComponent>(); i) {
                    auto &input = i->get();

                    //Check if player has pressed a "piano key"
                    if (input.noteValue >= 0) {
                        // Make sure sound is not already playing, otherwise the sound will stutter
                        if (sound != NULL && !sound->IsPlaying()) {
                            float pitch = pow(2.0f, (float)input.noteValue / 12); // this is the formula for calculating a note in 12 tone equal temprament
                            std::cout << "Pitch: " << pitch << std::endl;
                            Play(pitch);

                            if (auto c = attached->GetComponent<NoteCounterComponent>(); c) {
                                auto &counter = c->get();
                                counter.Increment(input.noteValue);
                            }
                        }

                    //Check if player has pressed the "random note key"
                    } else if (input.RandomNoteValue == true) {

                        // make sure sound is not already playing
                        if (sound != NULL & !sound->IsPlaying()) {
                            
                            //make sure there are notes in the active chord
                            if (source->activeChord.notes.empty()) {
                                std::cout << "No notes in active chord" << std::endl;
                            } else {
                                //get a valid random index from the active chord
                                int randomNoteIndex = rand() % static_cast<int>(source->activeChord.notes.size());
                                float pitch =
                                    pow(2.0f,
                                        (float)source->activeChord.notes[randomNoteIndex] / 12);
                                Play(pitch);


                                if (auto c = attached->GetComponent<NoteCounterComponent>(); c) {
                                    auto &counter = c->get();
                                    counter.Increment(13);
                                }
                            }
                        }

                    //Release the held note if the user stops pressing the key
                    } else {
                        if (sound != NULL)
                            sound->Stop();
                    }
                }
            }
        }
    }
    void Cleanup() override {}
    void Play(float pitch) {

        //set the specified pitch of the sound and play it
        sound->SetPitch(pitch);
        sound->Play();
    }
};



struct DrawModelComponent : public Component {
    raylib::Model *model;

    void Setup() override {}
    void Update(float dt) override  {
        if (auto t = attached->GetComponent<TransformComponent>(); t) {
            auto &transform = t->get();
            raylib::Transform backupTransform = model->transform;
            model->transform = raylib::Transform(model->transform)
                                   .Translate(transform.position)
                                   .Rotate(transform.rotation);
            model->Draw({});

            if (transform.isSelected) {
                // create a bounding box from the transformed model
                raylib::BoundingBox bbox = model->GetTransformedBoundingBox();
                // draw the bounding box
                bbox.Draw({0, 0, 255, 255});
            }
            model->transform = backupTransform;
        }
    }
    void Cleanup() override {}
};



//State: A list of entities
struct State {
    std::vector<Entity> entities;

    State() : entities() {}

    virtual void Setup(){
        for (Entity &e : entities) {
            for (std::shared_ptr<Component> &c : e.components) {
                c->Setup();
            }
        }
    }
    virtual void Update(float dt){
        for (auto &e : entities) {
            for (auto &c : e.components) {
                        c->Update(dt);
            }
        }
    }
    virtual void Cleanup(){
        for (auto &e : entities) {
            for (auto &c : e.components) {
                        c->Cleanup();
            }
        }
    }
};

void SwitchStates(State*& currentState, State* newState){
    if (currentState) {
        currentState->Cleanup();
    }
    currentState = newState;
    if (currentState) {
        currentState->Setup();
    }
}

State* menuState = nullptr;
State* gameState = nullptr;
State* activeState = nullptr;


///////////////////////////////////////////////////////////
////////// Main Game State ////////////////////////////////
///////////////////////////////////////////////////////////
struct GameState : public State{
    raylib::BufferedInput selectInput;
    int selected;
    raylib::Model ground;
    raylib::Model flute;
    raylib::Model piano;
    raylib::Model trumpet;
    raylib::Model violin;
    std::unique_ptr<raylib::Sound> fluteSound;
    std::unique_ptr<raylib::Sound> pianoSound;
    std::unique_ptr<raylib::Sound> trumpetSound;
    std::unique_ptr<raylib::Sound> violinSound;
    std::unique_ptr<cs381::SkyBox> skybox;
    raylib::Camera camera;
    std::unique_ptr<raylib::Music> testMusic;
    raylib::Texture snow;

    GameState() : selected(0), ground(), flute(), piano(), trumpet(), violin(), fluteSound(nullptr), pianoSound(nullptr), trumpetSound(nullptr), violinSound(nullptr), skybox(nullptr), camera(), testMusic(nullptr), snow() {}

    void Setup(){
        // Initialize camera/state values
        camera = raylib::Camera({0, 120, 300}, {0, 0, 0});
        skybox = std::make_unique<cs381::SkyBox>("textures/skybox.png");
        testMusic = std::make_unique<raylib::Music>("audio/testsong.wav");

        // Load models into member variables
        flute = raylib::Model("models/flute.glb");
        flute.transform =
            raylib::Transform(flute.transform).Scale(1.2).RotateY(raylib::Degree(270));

        piano = raylib::Model("models/piano.glb");
        piano.transform = raylib::Transform(piano.transform)
                                .Scale(0.4)
                                .RotateY(raylib::Degree(270));

        trumpet = raylib::Model("models/trumpet.glb");
        trumpet.transform = raylib::Transform(trumpet.transform)
                                .Scale(1.2)
                                .RotateY(raylib::Degree(270));

        violin = raylib::Model("models/violin.glb");
        violin.transform = raylib::Transform(violin.transform)
                                .Scale(1.2)
                                .RotateY(raylib::Degree(270));

        // load the audio samples for each instrument
        fluteSound = std::make_unique<raylib::Sound>("audio/ins_flute.wav");
        pianoSound = std::make_unique<raylib::Sound>("audio/ins_piano.wav");
        trumpetSound = std::make_unique<raylib::Sound>("audio/ins_trumpet.wav");
        violinSound = std::make_unique<raylib::Sound>("audio/ins_violin.wav");
        testMusic = std::make_unique<raylib::Music>("audio/testsong.wav");

        //Setup ground and skybox
        ground = raylib::Mesh::Plane(10000, 10000, 50, 50, 25).LoadModelFrom();
        raylib::Texture snow = raylib::Texture("textures/snow.jpg");
        ground.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = snow;

        // cs381::SkyBox skybox("textures/skybox.png");

        // raylib::Music testMusic("audio/testsong.wav");


        // add entities to the state
        entities.reserve(16);

        ////////////////////////////////////////
        // create music source//////////////////
        ////////////////////////////////////////
        auto &musicSource = entities.emplace_back();
        auto &ms = musicSource.AddComponent<MusicSourceComponent>();
        ms.music = testMusic.get();

        ////////////////////////////////////////
        // create flute ////////////////////////
        ////////////////////////////////////////
        auto &fl = entities.emplace_back();
        auto &t1 = fl.AddComponent<TransformComponent>();
        t1.position = raylib::Vector3{-200, 100, 0};
        t1.isSelected = true;
        auto &m1 = fl.AddComponent<DrawModelComponent>();
        m1.model = &flute;
        auto &s1 = fl.AddComponent<PlaySoundComponent>();
        s1.sound = fluteSound.get();
        s1.source = &ms;
        fl.AddComponent<InputComponent>();
        fl.AddComponent<NoteCounterComponent>();

        ////////////////////////////////////////
        // create piano ////////////////////////
        ////////////////////////////////////////
        auto &pno = entities.emplace_back();
        auto &t2 = pno.AddComponent<TransformComponent>();
        t2.position = raylib::Vector3{-100, 50, 0};
        auto &m2 = pno.AddComponent<DrawModelComponent>();
        m2.model = &piano;
        auto &s2 = pno.AddComponent<PlaySoundComponent>();
        s2.sound = pianoSound.get();
        s2.source = &ms;
        pno.AddComponent<InputComponent>();
        pno.AddComponent<NoteCounterComponent>();

        ////////////////////////////////////////
        // create trumpet //////////////////////
        ////////////////////////////////////////
        auto &tpt = entities.emplace_back();
        auto &t3 = tpt.AddComponent<TransformComponent>();
        t3.position = raylib::Vector3{0, 100, 0};
        auto &m3 = tpt.AddComponent<DrawModelComponent>();
        m3.model = &trumpet;
        auto &s3 = tpt.AddComponent<PlaySoundComponent>();
        s3.sound = trumpetSound.get();
        s3.source = &ms;
        tpt.AddComponent<InputComponent>();
        tpt.AddComponent<NoteCounterComponent>();

        ////////////////////////////////////////
        // create violin ///////////////////////
        ////////////////////////////////////////
        auto &vln = entities.emplace_back();
        auto &t4 = vln.AddComponent<TransformComponent>();
        t4.position = raylib::Vector3{100, 100, 0};
        auto &m4 = vln.AddComponent<DrawModelComponent>();
        m4.model = &violin;
        auto &s4 = vln.AddComponent<PlaySoundComponent>();
        s4.sound = violinSound.get();
        s4.source = &ms;
        vln.AddComponent<InputComponent>();
        vln.AddComponent<NoteCounterComponent>();

        selected = 1;

        


        // set selected flag on transform components based on which entity is selected
        selectInput["select"] =
            raylib::Action::key(KEY_TAB)
                .AddPressedCallback([this]() {
                    selected = (selected + 1) % entities.size();
                    for (int i = 0; i < entities.size(); i++) {
                        if (auto t = entities[i].GetComponent<TransformComponent>(); t) {
                            t->get().isSelected = (i == selected);
                        }
                    }
                })
                .move();

        //Entity setup loop
        for (Entity &e : entities) {
            for (std::shared_ptr<Component> &c : e.components) {
                c->Setup();
            }
        }
    }

    void Update(float dt){
        NoteCounterComponent* counter = nullptr;
        selectInput.PollEvents();

        // face the camera towards the selected entity if it has a transform
        // component
        if (auto t = entities[selected].GetComponent<TransformComponent>(); t) {
            auto &selectedTransform = t->get();
            camera.SetTarget(selectedTransform.position);
        } else {
            camera.SetTarget(raylib::Vector3{0, 0, 0});
        }


        camera.BeginMode();
        {

            skybox->Draw();
            //ground.Draw({});


            //Entity update loop
            for (auto &e : entities) {
                for (auto &c : e.components) {
                    c->Update(dt);
                }
            }
        }
        camera.EndMode();
        if (auto c = entities[selected].GetComponent<NoteCounterComponent>(); c) {
            counter = &c->get();
            counter->DrawStats();
        }
    }
};

struct MenuState : public State {
    GuiLayoutNameState gui_state; 

    MenuState() : State(), gui_state() {}

    void Setup() override {
        gui_state = InitGuiLayoutName();
    }
    void Update(float dt) override {
        GuiLayoutName(&gui_state);
        if(gui_state.Button001Pressed){
            SwitchStates(activeState, gameState);
        }
    }
    void Cleanup() override {}
};





template <typename T>
struct Delegate {};

template <typename Treturn, typename... Targs>
struct Delegate<Treturn(Targs...)> {
    std::vector<std::function<Treturn(Targs...)>> functions;

    void connect(const std::function<void(int)> &f) { functions.push_back(f); }

    Treturn operator()(Targs... args) {
        static_assert(std::is_same_v<Treturn, void>,
                      "HW EC: Only void return allowed ");
        for (auto &f : functions) {
            f(std::forward<Targs>(args)...);
        }
    }
};

int main() {

    //Create window
    raylib::Window window(800, 600, "CS381 - Assignment 6");

    //Initialize audio device
    raylib::AudioDevice audio;

    menuState = new MenuState();
    gameState = new GameState();
    activeState = menuState;

    activeState->Setup();



    ////////////////////////////////////////////////////////////////////
    ////////////// Main window loop/////////////////////////////////////
    ////////////////////////////////////////////////////////////////////


    while (!window.ShouldClose()) {


        window.BeginDrawing();
        {
            window.ClearBackground(DARKGRAY);
            float dt = window.GetFrameTime();
            activeState->Update(dt);
        }
        window.EndDrawing();
    }
    if(activeState) activeState->Cleanup();
    delete menuState;
    delete gameState;

}
#endif // __AS6_H__