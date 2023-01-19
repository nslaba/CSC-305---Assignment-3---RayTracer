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
The Raytracer is set up to support local illumination (Ambient, Diffuse and Specular), reflections and shadows using recursion of depth 3.

Illumination:

<img width="296" alt="image" src="https://user-images.githubusercontent.com/77686772/213516026-e0d95cc5-1020-4341-beee-b9faf225a24f.png">

Specular light:

<img width="278" alt="image" src="https://user-images.githubusercontent.com/77686772/213516660-4ebb9e20-0ada-4f58-bb89-430e6cd3cd2b.png">

Reflections:

<img width="281" alt="image" src="https://user-images.githubusercontent.com/77686772/213516956-ffb2a74b-0ac3-49db-bd82-560ac94b9867.png">

Shadows:

<img width="294" alt="image" src="https://user-images.githubusercontent.com/77686772/213516339-02479661-8701-4c06-984b-e1eef1caf113.png">




## Functionality
My project first deals with the input text file by reading it, parsing it and populating the proper struct variables. All of my parsing is done in a seperate 
file called Parcing.cpp.

Once all of my variables are updated, the main in RayTracer.cpp constructs the correct view plane. Using the Width and the Height it creates an array of pixel structs which hold RGB components. Once the array of pixels and the view plane are generated, the main is ready to create the Ray struct (defined in structs.hpp) particular to the particular pixel. To compute the color of this ray Ray, ratrace(ray) is called returning its color in RGB. The color is then stored in the array of pixels.

Raytrace(ray):
raytrace is a recursive function that repeatedly finds the closest intersection for the particular ray. It checks if there has been an intersection or not and if there hasn't been any, then it returns black for a reflected ray and returns the background color for an original ray. If there has been an intersection, then it calculates the given shadows casted by each light (shadow_ray function). Lastly the depth of ray is reduced and raytracer is called again with the now reflected ray.


