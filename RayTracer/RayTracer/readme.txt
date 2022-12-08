README for RayTracer
Assignment 3 CSC 305
Natalie Slaba


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