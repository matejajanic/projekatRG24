# Space Station Simulation

mi22030 - Mateja Janic  
This project simulates a small space station model with some added features

## Controls

Left mouse click + mouse movement -> Camera movement  
W key -> Forward
A key -> Left
S key -> Back
D key -> Right
Up arrow -> Up
Down arrow -> Down
Space -> Space shuttle activation
P -> Sharpen effect
B -> Bloom effect
(tip) Sharpen and Bloom can't be on at the same time (mutual effect won't be visible)
Enter -> Project GUI

## Features

The model is implemented with two types of lighting, directional and spot lighting.
Space button triggers Space shuttle movement that starts from the camera's position
and extends until the end of the skybox.
After 3 seconds, a shooting star is shown flying by. The shooting star will
always be visible as it is programmed to start in the camera's visible field, and
it travels in the same way as the space shuttle, until the end of the skybox.
Even though asteroids don't normally leave a trace while travelling through space,
this feature was added for additional visibility of the object.
Camera movement is described above.
The bloom effect, which is activated by pressing the B key, can be visible on the
turquoise parts of the space station.
The sharpen effect is activated by pressing the P key, and it gives the model a
comic book-like look.
All the dimensions are empirically determined and generally aren't a product of
a more detailed mathematical process (except for the trajectories).

### Fundamental:

[x] Model with lighting
[x] Two types of lighting with customizable colors and movement through GUI or ACTIONS
[x] {Space button} ---Triggers---> {Space shuttle}, (not related) AFTER_3_SECONDS---Triggers---> {Shooting star}

### Group A:

[x] Frame-buffers with post-processing   
[ ] Instancing  
[ ] Off-screen Anti-Aliasing  
[ ] Parallax Mapping

### Group B:

[x] Bloom with the use of HDR  
[ ] Deferred Shading  
[ ] Point Shadows  
[ ] SSAO

### Engine improvement:

[ ] ...

## Models:

[link1]   
[link2]

## Textures

[link1]  
[link2]

{OTHER-RESOURCES}
