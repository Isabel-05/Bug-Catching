name: 
Bug Catcher 

how to play / rules:
When you open the game a start-menu with a "Start" and "Exit" button will appear.
Once you click "Start", bugs will start spawning and moving across the screen.
There is a 60 second timer at the top that will start counting down to 0.
By holding down the left mouse button and moving the mouse you can draw a lasso around the bugs (the lasso needs to roughly connect back where it started to count).
If a bug is detected inside of the lasso, it will be despawned and you will gain points which you can track with the score counter at the top right of the screen.
No keyboard inputs are needed.

There are three different types of bugs:
- Ladybug
- Moth
- Butterfly

Each one has a different movement pattern.
The Ladybug is slower and only changes its course rarely.
The Butterfly is very fast and switches around a lot.
The moth is somewhere in between.

Because of that they also have different spawn rates and give different amounts of points.
- Ladybug = 1 point
- Moth = 2 points
- Butterfly = 3 points

Try to get as many points as possible within the 60 seconds!
Your final score will be shown at the end and you can try again by clicking "Retry" 


About Window Resolution:
By default, the resolution is set to 1080x1080 but if that doesn't fit your screen you can change it.
At the very top of Bug.h right under the includes, there's a global variable called WINDOWSIZE you can adjust to what fits best. (then rebuild)
The window is always square though, that's not adjustable.

External Resources:
- SDL3 library
- GLM library
- Sprite pack from itch.io: https://deirkart.itch.io/pixel-universe-bugs
- WR Franklin: Point-in-Poly algorithm
- Mike Shaw: SDL3 Tutorial playlist https://www.youtube.com/playlist?list=PLvv0ScY6vfd-RZSmGbLkZvkgec6lJ0BfX
  (used for learning the window & renderer creation, input handling and sprite rendering)
