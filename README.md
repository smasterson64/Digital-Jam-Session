# Digital-Jam-Session
An experiment with realtime music interaction

EXPLANATION:
    This program is an experiment with audio manipulation and tracking realtime playback. It's intended to
    work as sort of a skeleton for some sort of music-based game later down the line. 

    This is a 3D scene with several instruments you can select from to play. Pressing the bottom row
    of keys (Z through Comma) acts as a piano keyboard that plays the corresponding note on the selected 
    instrument.

    There is also real-time tracking of the music playing in the background. pressing the Space key will
    play a note that belongs to the chord currently playing in the background music. 

    Updated so the notes played on each instrument is tracked on screen.

    -Thanks to Free3d.com for the instrument models
    -Thanks to Freesounds.org for the instrument sound samples
    -I wrote the background music using MuseScore 3


BUILDING AND RUNNING:

    Raylib-cpp was used in the creation of this project.
    This can be fetched with the following command from the
    root directory of my github repo:
    
    git submodule update --init --recursive

   To compile the code, you can use the following commands
   from the AS6 folder:

   mkdir build
   cd build
   cmake ..
   make

   After running "make" you can type

   ./aS6
   to run the program. 

CONTROLS:
    Click on the Start button to change from the menu state to the game state
    TAB: switch between instruments. The active instrument is drawn with a bounding box
    Z: Play a concert C  pitch on your selected instrument
    S: Play a concert C# pitch on your selected instrument
    X: Play a concert D  pitch on your selected instrument
    D: Play a concert D# pitch on your selected instrument
    C: Play a concert E  pitch on your selected instrument
    V: Play a concert F  pitch on your selected instrument
    G: Play a concert F# pitch on your selected instrument
    B: Play a concert G  pitch on your selected instrument
    H: Play a concert G# pitch on your selected instrument
    N: Play a concert A  pitch on your selected instrument
    J: Play a concert A# pitch on your selected instrument
    M: Play a concert B  pitch on your selected instrument
    Comma: Play a concert C (up an octave from the original C pitch)  pitch on your selected instrument
    Space: Play a random note in the current chord of the song (Mash Space to do a solo!!!)


