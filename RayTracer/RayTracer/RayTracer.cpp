// RayTracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream> //input output stream --> a file in the standard library
//#include <cmath>
#include <fstream>
#include <sstream>
#include <cstdlib> //For exit function
#include <cstdio>
#include <string>
#include <map>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include <algorithm> // in order to use "replace" function for testParsing

#include "structs.hpp"
#include "Parsing.h"

using namespace std;




/* Copied function for creating a ppm from a binary*/
void save_imageP6(int Width, int Height, const char* fname, unsigned char* pixels) {
    FILE* fp;
    const int maxVal = 255;

    printf("Saving image %s: %d x %d\n", fname, Width, Height);
    fp = fopen(fname, "wb");
    if (!fp) {
        printf("Unable to open file '%s'\n", fname);
        return;
    }
    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", Width, Height);
    fprintf(fp, "%d\n", maxVal);

    for (int j = 0; j < Height; j++) {
        fwrite(&pixels[j * Width * 3], 3, Width, fp);
    }

    fclose(fp);
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// My code /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


/* Part 1:
 * FUNCTION =   Compute_closest_intersection
 * 
 * Parameters:  a ray object
 * Output:      a pair with first being a boolean letting the program know if the sphere was intersected or not and the second being
                an IntersectedSphere struct object containing the t value, the intersection point, the order number of the sphere in the
                case where the first==true
 * 
 * Purpose:     For each sphere in the vector array of spheres, check if the ray intersects them. If it does then take the smallest 
 *              intersection along with the sphere number and store them as x,y coordinates within the solutions vector array of vec2's. 
 *              Then for each solutions' vec2, pick out the smallest x value (ie the smallest t value) and save its y component as the 
 *              intersected_sphere integer which represent's the ith sphere we are dealing with.
 */
pair <bool,IntersectedSphere> compute_closest_intersection(Ray ray) {
    /* STEP 1.a) Initialize a pair to be returned */
    pair <bool, IntersectedSphere> inter_pair;
    inter_pair.first = false;

    /* STEP 1.b) Create a new intersected sphere object in order to save the t, the order (j value) and the intersection point*/
    IntersectedSphere intersected_sphere = IntersectedSphere();
    inter_pair.second = intersected_sphere;

    // The solutions' vector of vec2's parameters are:
    // --> x is t (ie vector scaler to reach intersection point) 
    // --> y is j=the numbered sphere in the forloop
    vector <vec2> solutions; 
    
    for (int j=0; j < spheres.size(); j++) {
        /* STEP 1.c) invert ray for each sphere based on the sphere's scalars. */
        vec4 inverted_starting_point = spheres[j].inverseScaleTranspose * ray.starting_point;
        vec4 inverted_direction = spheres[j].inverseScaleTranspose * ray.direction;
        

        /* STEP 1.d) Use the quadratic formula to check for solutions (intersections)
         * --> If (determinant >= 0) then find the solution(s) representing the intersection pt(s) */

        // First store S (inverted starting point) and c (inverted direction) in NON-homogeneous coordinates
        vec3 S = { inverted_starting_point.x, inverted_starting_point.y, inverted_starting_point.z };
        vec3 c = { inverted_direction.x, inverted_direction.y, inverted_direction.z };
        
        const float determinant = pow(dot(S, c), 2) - pow(length(c), 2) * (pow(length(S), 2) - 1);
        
        if (determinant >= 0.0){
            // In this case the ray hits the sphere either once or twice, so find the solution for t
            float t1 = -(dot(S, c) / pow(length(c), 2)) - (sqrt(determinant)/pow(length(c),2));
            float t2 = -(dot(S, c) / pow(length(c), 2)) + (sqrt(determinant) / pow(length(c), 2));

            //push the smaller one & save which sphere was affected by storing the j value
            if (t1 < t2) {
                //push t1 to solutions vector
                solutions.push_back(vec2{ t1,j });
                spheres[j].cannonicalIntersectionPoint = inverted_starting_point + t1 * inverted_direction;
            }
            else {
                solutions.push_back(vec2{ t2,j });
                spheres[j].cannonicalIntersectionPoint = inverted_starting_point + t1 * inverted_direction;
            }
        }        
    }

    //If no solutions found then return a null vec4
    if (solutions.size() == 0) {
        return inter_pair;
    }

    /* STEP 2.b) pick the smallest intersection */
    inter_pair.second.order = solutions[0].y;
    inter_pair.second.t = solutions[0].x;

    for (int i = 1; i < solutions.size(); i++) {
        if (solutions[i].x < inter_pair.second.t) {

            inter_pair.second.t = solutions[i].x;
            inter_pair.second.order = solutions[i].y;
        }
    }
    
    /* STEP 2.b) Delete all the entries in the vector of "intersections" so that it can be reused */
    solutions.clear();

    //find the normal at that intersection point
    vec4 normal = { spheres[intersected_sphere.order].cannonicalIntersectionPoint - vec4{spheres[intersected_sphere.order].position,0} };
    normal = normal * spheres[intersected_sphere.order].inverseScaleTranspose;
    normal = normalize(normal);
    inter_pair.second.normal = normal;

    //find the reflected vector for later
    inter_pair.second.reflected_ray = -2 * dot(normal, ray.direction) * normal + ray.direction;


    inter_pair.second.intersection_point = ray.starting_point + intersected_sphere.t * ray.direction;
    
    //Return the intersected sphere object
    return inter_pair;
}

/* Part 4:
 *         check if there is a shadow at the intersection pt if there isn't then add the ADS light, if there is then don't
 */
vec3 shadow_ray(Light light, IntersectedSphere intersected_sphere) {
    vec3 color{ 0,0,0 };
    // Create a ray with starting point being the intersection_point and direction being the normalized dir towards the light
    Ray ray = Ray();
    ray.starting_point={intersected_sphere.intersection_point};
    ray.direction = { vec4{light.position,1.0} - intersected_sphere.intersection_point };
    ray.direction = normalize(ray.direction);

    //check if in shadow. If yes, then don't add if no then compute ADS and add
    //It will be in shadow if there is an object in front of the light so check for the closest intersection from the light and if that intersection doesn't equal
    //our intersection_point then too bad (will have to be careful with floating pt error!!). Or going from the object to the light. Either way
    pair <bool, IntersectedSphere> closest_intersection = compute_closest_intersection(ray);
    if (!closest_intersection.first) { //Then it does intersect some spheres then check if the t value is less than the distance to the light
        float light_distance = length(vec4{ vec4{light.position,0} - intersected_sphere.intersection_point });
        float sphere_distance = length(vec3{ closest_intersection.second.intersection_point - intersected_sphere.intersection_point });
        if (sphere_distance <= light_distance) {
            return color;
        }
    }


 
    //otherwise compute ADS
    // r = Light.color.x * Ka + Lightx*Kd(N dot lightRayDir) + Lightx * Ks * (R dot V)^ n
    
    //I'm missing (R dot V) ^ n
    color.x = light.color.x * spheres[intersected_sphere.order].ka 
        + light.color.x*spheres[intersected_sphere.order].kd * dot(intersected_sphere.normal, ray.direction) 
        + light.color.x* spheres[intersected_sphere.order].ks;
    //* pow(dot(intersected_sphere.reflected_ray, ray.direction), spheres[intersected_sphere.order].spec_exp) 

    color.y = light.color.y * spheres[intersected_sphere.order].ka
        + light.color.y * spheres[intersected_sphere.order].kd * dot(intersected_sphere.normal, ray.direction)
        + light.color.y * spheres[intersected_sphere.order].ks;

    color.x = light.color.z * spheres[intersected_sphere.order].ka
        + light.color.z * spheres[intersected_sphere.order].kd * dot(intersected_sphere.normal, ray.direction)
        + light.color.z * spheres[intersected_sphere.order].ks;
    return color;
}

vec3 raytrace(Ray ray) {
    
    //Continue raytracing until maxed out on depth
    if (ray.depth <= 0) {
        //If maxed out on depth then return black
        return { 0,0,0 };
    }
    //update ray depth to not raytrace forever
    ray.depth--;

    /* STEP 1: check for the closest intersection with all objects*/
    pair <bool,IntersectedSphere> closest_intersection = compute_closest_intersection(ray);
    //cout << "closest intersected sphere is numbered: " << closest_intersection.second.order << "\n";
    //vec4 intersection_point{ 0,0,0, 1 };//for now so that it compiles
    if (!closest_intersection.first) {
        // if there is no intersection then return the background color
        //cout << background_info[0].color.x << background_info[0].color.y << background_info[0].color.z;
        return background_info[0].color;
    }
    cout << "something has been intersected\n";

    ///* STEP 2: check for shadows and add them up*/
    //vec3 color_shadow= shadow_ray(lights[0], closest_intersection.second);
    //for (int i = 1; i < lights.size(); i++) {
    //    color_shadow.x += shadow_ray(lights[i], closest_intersection.second).x;
    //    color_shadow.y += shadow_ray(lights[i], closest_intersection.second).y;
    //    color_shadow.z += shadow_ray(lights[i], closest_intersection.second).z;

    //}
    ///*STEP 3: raytrace for reflected rays*/
    ////I need to somehow get which sphere i'm dealing with based on the closest intersection so that i can calculate the normal. Why don't I just make a global variable? sphere_num that gets updated whenever I call closest intersection pt. 
    //// Does this mean I reflect the ray over a normal on the sphere? YES--> How can i get the normal???

    //// initialize a new ray for the reflected ray
    //Ray reflected_ray = Ray();
    //reflected_ray.direction = closest_intersection.second.reflected_ray;
    //reflected_ray.depth = 3;
    //reflected_ray.starting_point = closest_intersection.second.intersection_point;
    //vec3 color_reflected = raytrace(reflected_ray);

    //cout << "the closest intersected sphere is: "<<(closest_intersection.second.order);
    //return (vec3{ spheres[closest_intersection.second.order].color + spheres[closest_intersection.second.order].kr * color_reflected});
}

int main(int argc, char *argv[]) {
    parse_file_string(read_file(argv[1]));

    //// STEP 1.b: update the inverse scale and inverse transpose matrices of each sphere. 
    //mat4x4 unit_matrix = { 1, 0, 0, 0,
    //                       0, 1, 0, 0,
    //                       0, 0, 1, 0,
    //                       0, 0, 0, 1 };
    //for (Sphere sphere : spheres) {
    //    sphere.transpose = translate(unit_matrix, sphere.position);
    //    sphere.scaleTranspose = scale(sphere.transpose, sphere.scaler);
    //    sphere.inverseScaleTranspose = inverse(sphere.scaleTranspose);

    //}

    //// STEP 2: create correct variables
    //int Width = view[0].resolution.x;
    //int Height = view[0].resolution.y;
    //const char *fname6 = output_name[0].name.c_str();
    //Pixel* pixels = new Pixel[Width*Height];

    //// Step 3: populate pixels
    //for (int i = 0; i < Height; i++) {
    //    for (int j = 0; j < Width; j++) {

    //        // 1.--> Construct a ray, where ray is a unit vector from eye to pixel
    //        int row_number = i;
    //        int column_number = j;
    //        // --> get pixel position
    //        int width_screen = view[0].right;
    //        int height_screen = view[0].top;
    //        float Uc = -width_screen + width_screen*(2 * column_number / Width);
    //        float Vr = -height_screen + height_screen * (2 * row_number / Height);

    //        //innitialize a new ray struct object:
    //        Ray ray = Ray();
    //        ray.starting_point.x = Uc;
    //        ray.starting_point.y = Vr;
    //        ray.starting_point.z = -view[0].near;

    //        ray.direction.x = Uc;
    //        ray.direction.y = Vr;
    //        ray.direction.z = -view[0].near;

    //        // --> make it a unit vector
    //        ray.direction = normalize(ray.direction);
    //        ray.depth = 3;
    //        // 2.--> ray trace:
    //        vec3 final_color = raytrace(ray);
    //        //cout << "ray color: " << final_color.x << final_color.y << final_color.z << "\n";
    //        pixels[i*Height + j].r =final_color.x*255;
    //        pixels[i*Height + j].g =final_color.y*255;
    //        pixels[i*Height + j].b =final_color.z*255;
    //       // cout << "pixel rgb is: " << pixels[i * Width + j].r << pixels[i * Width + j].g << pixels[i * Width + j].b << "\n";
    //      /*  pixels[i*Width + j].r = 100;
    //        pixels[i * Width + j].g = 0;
    //        pixels[i * Width + j].b = 0;*/
    //    } 
    //}

    //save_imageP6(Width, Height, fname6, reinterpret_cast<unsigned char*> (pixels));
    ////// TESTING FIND Intersection:

    //// create a new ray object
    Ray ray = Ray();
    ray.starting_point={ -1, 0, 0, 1 };
    ray.direction = { 1, 0, 0, 0 };
    ray.direction = normalize(ray.direction);
    ray.depth = 2;
    // create a new sphere
    Sphere sphere = Sphere();
    sphere.position = { 3, 0, 0 };
    sphere.scaler = { 2, 2, 2 };

    mat4x4 unit_matrix = { 1, 0, 0, 0,
                           0, 1, 0, 0,
                           0, 0, 1, 0,
                           0, 0, 0, 1 };

    sphere.transpose = translate(unit_matrix, sphere.position);
    sphere.scaleTranspose = scale(sphere.transpose, sphere.scaler);
    sphere.inverseScaleTranspose = inverse(sphere.scaleTranspose);
    spheres.push_back(sphere);

    pair <bool, IntersectedSphere> pair = compute_closest_intersection(ray);
    vec4 intersection_point = pair.second.intersection_point;
    cout << "The hard coded intersection is: " << intersection_point.x<< " " << intersection_point.y << " " << intersection_point.z << "\n";

    return 0;
}
