# tetrisAI-Cpp

This is a program made to run tetris, it uses OpenGL and is programmed for linux.<br>
It is written entirely in C++, which was not nearly as fun as i expected with openGL<br>

The program has 3 major components, the engine, the AI and the options,<br>
all the options are set via #define so i can optimise the code around the various options.<br>

There are two main options
* Speed<br>
  This is how fast the program does movements, in slowmode it takes 4 moves to place a peice,
  each with a sleep of (0.03) seconds inbetween, this is very nice to view and light on the cpu too.<br>
  In fastmode it runs so fast (on my machine) it is hard to recognise as tetris, this is only here so you
  can enjoy the AIs full potential.
* Goal<br>
  There are two goals the AI can preform
  * Regular<br>
    This just plays tetris and plays it well, it has never lost <br>
    even after leaving it on max speed for a couple days
  * Image <br>
    Set base .txt to a file of spaces and lower case Os and it will play tetris in a way to make that image<br>
    there is an example there of a diamond shape, this is ment as a present for my morail.
    
