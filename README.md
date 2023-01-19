### RayTracer ~ Assignment 3 ~ CSC 305

## Task

Build simple ray tracer program creates ppm images of spheres by reading the present text files' specifications.

## Criteria
- Build a raytracer using either c++, python or java
- The system only needs to handle the rendering of ellipsoids
- It will have a fixed camera situated at the origin in a right-handed coordinate system
- It will be looking down the negative z-axis
- Local illumination, reflections, and shadows will also need to be implemented.
- The program should take a single argument, which is the name of the file to be parsed
- Output should be a ppm image

## Summary
I have chosen to work in C++ as it is the fastest option. With C++ my images take a maximum of several seconds to be produced, whereas with python they could have taken up to several minutes. 
The Raytracer is set up to support local illumination (Ambient, Diffuse and Specular), reflections and shadows using recursion.

Illumination:
<img width="296" alt="image" src="https://user-images.githubusercontent.com/77686772/213516026-e0d95cc5-1020-4341-beee-b9faf225a24f.png">

Specular light:
<img width="278" alt="image" src="https://user-images.githubusercontent.com/77686772/213516660-4ebb9e20-0ada-4f58-bb89-430e6cd3cd2b.png">

Reflections:
<img width="281" alt="image" src="https://user-images.githubusercontent.com/77686772/213516956-ffb2a74b-0ac3-49db-bd82-560ac94b9867.png">

Shadows:
<img width="294" alt="image" src="https://user-images.githubusercontent.com/77686772/213516339-02479661-8701-4c06-984b-e1eef1caf113.png">




## Functionality


## Design


What I have done:
Within my main function I first parse the input
file by calling "parse_file_string(read_file[argv[1]])"
all my parsing is done in a seperate file called Parsing.cpp
and Parsing.h contains all the declerations for Parsing.cpp.

Parsing.cpp-->
takes the input file and copies it into a string in order to not
have to work with the file throughout. 
Then it calls populate_structs which populates the nescessary structs 
defined in structs.hpp.

After parsing, my mane updates the nescessary matrices for each sphere
that are all stored as structs within a "spheres" vector.

Once all of my variables have been updated, the main goes on to create the 
correct view plane in order to generate the right rays. It computes the Width
and the Height and creates an array of pixel structs which hold rgb components.
My main is now ready to create each ray by creating a double forloop and computing
Uc and Vr. With this it has enough information to create a new Ray struct (defined
in structs.hpp) with a starting point (vec3), a normalized direction (vec3), depth 
(for recursion) and a boolean to state whether we are dealing with a reflected ray or not.
To compute the color of this ray, raytrace(ray) is called returning its color.

Raytrace: 
a recursive function that repeatedly finds the closest intersection with the help of 
"compute_closest_intersection" for the particular ray it is dealing with. It then checks
if there has been an intersection or not and if there isn't it returns black for a reflected 
ray and returns the background color for an original ray. If there has been an intersection 
though, it then computes all the shadow components for each light shining on it with "shadow_ray"
function. It then reduces its depth and calls itself again with a reflected ray of the previous ray.


I have omitted:
Nothing in particular. 
