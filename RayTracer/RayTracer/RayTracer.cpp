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
    // The forloop is modified to flip the image upside down:
    for (int j = Height-1; j >=0; j--) {
        fwrite(&pixels[j * Width * 3], 3, Width, fp);
    }

    fclose(fp);
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// My code /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


/* *****************************************************************************************************************************************
 * Part 1 of raytracer function:
 * HELPER FUNCTION =   Compute_closest_intersection
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
 *********************************************************************************************************************************************/
pair <bool,IntersectedSphere> compute_closest_intersection(Ray ray) {
    /* STEP 1.a) Initialize a pair to be returned */
    pair <bool, IntersectedSphere> inter_pair;
    inter_pair.first = false;

    // The solutions' vector of vec2's parameters are:
    // --> x is t (ie vector scaler to reach intersection point) 
    // --> y is j=the numbered sphere in the forloop
    vector <vec2> solutions;    

    for (int j=0; j < spheres.size(); j++) {
 
        /* STEP 1.c) invert ray for each sphere based on the sphere's scalars. */
        vec4 inverted_starting_point = spheres[j].inverseTransform * vec4{ ray.starting_point,1 };
        vec4 inverted_direction = spheres[j].inverseTransform * vec4{ ray.direction,0 };
       
        // First store S (inverted starting point) and c (inverted direction) in NON-homogeneous coordinates
        vec3 S = { inverted_starting_point};
        vec3 c = { inverted_direction};    
      
        /* STEP 1.d) Use the quadratic formula to check for solutions (intersections)
         *          If (determinant >= 0) then find the solution(s) representing the intersection pt(s) */
        const float determinant = pow(dot(S, c), 2) - (pow(length(c), 2) * (pow(length(S), 2) - 1));

        if (determinant >= 0.0){
           
            // In this case the ray hits the sphere either once or twice, so find the solution for t
            float t1 = -(dot(S, c) / pow(length(c), 2)) - (sqrt(determinant)/pow(length(c),2));
            float t2 = -(dot(S, c) / pow(length(c), 2)) + (sqrt(determinant) / pow(length(c), 2));
            
            /* t1<t2 because of their formulas. We want the smallest positive t (beyond or on
             * the near plane*/
            if (t1 >= 0 ) {
                solutions.push_back(vec2{ t1,j });
                spheres[j].cannonicalIntersectionPoint = inverted_starting_point + t1 * inverted_direction;
                inter_pair.first = true;
            }
            else if (t1 < 0 && t2 >= 0) {
                solutions.push_back(vec2{ t2,j });
                spheres[j].cannonicalIntersectionPoint = -(inverted_starting_point + t2 * inverted_direction);
                inter_pair.first = true;
            }

        }        
    }

    //If no solutions found then return a null vec4
    if (inter_pair.first!=true) {
        return inter_pair;
    }

    /* STEP 2.b) pick the smallest intersection */
    inter_pair.second.order = solutions[0].y;
    inter_pair.second.t = solutions[0].x;
   
    /* Find the closest intersection based on the smallest t value*/
    for (int i = 1; i < solutions.size(); i++) {       
        if (solutions[i].x < inter_pair.second.t) {          
            inter_pair.second.t = solutions[i].x;
            inter_pair.second.order = solutions[i].y;
        }
    }
       
    /* STEP 2.b) Delete all the entries in the vector of "intersections" so that it can be reused */
    solutions.clear();

    //find the normal at that intersection point
    vec4 normal = vec4{ spheres[inter_pair.second.order].cannonicalIntersectionPoint,0 };
    normal = normal * spheres[inter_pair.second.order].inverseTransform;
    
    inter_pair.second.normal = normalize(vec3{normal});

    inter_pair.second.intersection_point = ray.starting_point + inter_pair.second.t * ray.direction;
    //Save the view vector towards the intersection point
    inter_pair.second.view_vector = -normalize(inter_pair.second.intersection_point);

    return inter_pair;
}



/* ****************************************************************************************************************************************************
 * Part 2 of raytracer function:
 * HELPER FUNCTION =   shadow_ray
 *
 * Parameters:  Light light: the current light struct we are dealing with; IntersectedSphere: the intersected_sphere struct we are dealing
 *              with that was computed by compute_closest_intersection in raytracer.
 * Output:      vec3 representing rgb
 *
 * Purpose:     For the particular light we are dealing with, we innitialize a shadow_ray which goes from the light source towards the direction
 *              of the intersection we are dealing with. The direction is normalized. Then we compute the closest intersection for this shadow ray
 *              going from the light source. If there has been an intersection, then we check if the intersected sphere of the shadow ray is not the
 *              same as that of the original intersection. If it isn't then that means that there is a sphere closer to the light than the original 
 *              intersection so the original intersection is in shadow. In this case we simply return black.
 *              in the case the original intersection is not in shadow, then we go ahead and compute the diffuse and the specular for it. We then add
 *              them together and return them as rgb
 ******************************************************************************************************************************************************/
vec3 shadow_ray(Light light, IntersectedSphere original_intersection) {
    /*STEP 1: innitialize color as black since ambient will be present whether in shadow or not*/
    vec3 color = { 0,0,0 };

    /*STEP 2: Create a shadow ray (intersection pt-light) with starting point being the intersection_point and direction being the normalized dir towards the light*/
    Ray shadow_ray = Ray();
    //CHANGING THE DIRECTION OF SHADOW_RAY TO GO FROM LIGHT TOWARDS THE INTERSECTED PT INSTAD OF THE OTHER WAY AROUND
    //shadow_ray.direction =  original_intersection.intersection_point -light.position;
    //shadow_ray.direction = normalize( shadow_ray.direction );
    //shadow_ray.starting_point = light.position;

    //
    ///* STEP 3: Compute the closest intersection from the light position and then check if the object it intersects is a new object*/
    //pair <bool, IntersectedSphere> closest_intersection = compute_closest_intersection(shadow_ray);
    //if (closest_intersection.first) { 
    //    //It does intersect some spheres so check if the intersected sphere is the same as that of the original intersection
    //    if (closest_intersection.second.order != original_intersection.order){
    //        //return black
    //        return color; 
    //    }
    //}
    // 
    // CHANGED CODE: 
    shadow_ray.direction = light.position-original_intersection.intersection_point;
    /*shadow_ray.direction = normalize(shadow_ray.direction);*/
    shadow_ray.starting_point = light.position;


    /* STEP 3: Compute the closest intersection from the original intersection point of the non normalized direction and then check if 0 < t <1  and if so object will be in shadow*/
    pair <bool, IntersectedSphere> closest_intersection = compute_closest_intersection(shadow_ray);
    if (closest_intersection.first) {
        //It does intersect some spheres so check if the intersected sphere is the same as that of the original intersection
        if (closest_intersection.second.order != original_intersection.order) {
            //return black
            return color;
        }
    }

    shadow_ray.direction = normalize(shadow_ray.direction);
    // END OF CHANGED CODE
    /* STEP 4: Compute Diffuse*/
    color += light.color * spheres[original_intersection.order].kd * spheres[original_intersection.order].color * std::max(0.0f, dot(original_intersection.normal, -shadow_ray.direction));
    
    /*STEP 5: Compute Specular*/
    // calculate the reflected vector
    vec3 reflected_vector = reflect(shadow_ray.direction , original_intersection.normal);
    color += light.color * spheres[original_intersection.order].ks * (float)pow(std::max(0.0f, dot(reflected_vector, original_intersection.view_vector )), spheres[original_intersection.order].spec_exp);
 
    return color;
}

/* *****************************************************************************************************************************************
 * FUNCTION =   raytrace
 *
 * Parameters:  a ray object
 * Output:      the finalized vec3 color of the fired ray
 *
 * Purpose:     raytracer first checks the depth of the ray and returns black if the ray has reached its min depth (zero)
 *              it then computes the closest intersection in its direction by calling compute_closest_intersection(ray)
 *              which returns a pair of a boolean and an IntersectedSphere struct. The boolean is to check if there has been 
 *              an intersection or not. If there hasn't been an intersection and we are dealing with the original ray, then 
 *              raytracer returns the background color, or there hasn't been an intersection and we are dealing with a reflected ray
 *              then it returns black. 
 *              Otherwise if there has been an intersection then it computes whether or not it is in shadow or not for each light
 *              by calling shadow ray(). 
 *              Lastly, it reduces the ray's depth and finds a reflected ray with that reduced depth and calls itself recursively with the
 *              reflected ray.
 *              Finally it returns the local color and the reflected components added together.
 *********************************************************************************************************************************************/
vec3 raytrace(Ray ray) {
    
    //Return base case
    if (ray.depth <= 0) {
        //If maxed out on depth then return black
        return { 0,0,0 };
    }
 
    /* STEP 1: check for the closest intersection with all objects*/
    pair <bool,IntersectedSphere> closest_intersection = compute_closest_intersection(ray);

    if (!closest_intersection.first) {
        // if there is no intersection then return the background color if it's the OG ray or black if it's a reflected ray
        if (ray.reflected) {
            return { 0,0,0 };
        }
        else {
            return background_info.color;
        }       
    } 
    // From now on only dealing with intersected objects
    /* Initialize local color of object as ambient*/
    vec3 color_local =  background_info.ambient* spheres[closest_intersection.second.order].ka*spheres[closest_intersection.second.order].color;

    /* STEP 2: check for shadows and add them up*/
   
    color_local+= shadow_ray(lights[0], closest_intersection.second);
    for (int i = 1; i < lights.size(); i++) {
        color_local += shadow_ray(lights[i], closest_intersection.second);
    }
   
    /*STEP 3: raytrace for reflected rays*/
    ray.depth--;
    
    // initialize a new ray for the reflected ray
    Ray reflected_ray = Ray();
    reflected_ray.reflected = true;
    reflected_ray.direction = reflect(ray.direction, closest_intersection.second.normal);
    reflected_ray.depth = ray.depth;
    reflected_ray.starting_point = closest_intersection.second.intersection_point + 0.00001f*reflected_ray.direction;
    vec3 color_reflected = raytrace(reflected_ray);
    //vec3 color_reflected = { 0,0,0 };
 
    return (color_local + spheres[closest_intersection.second.order].kr * color_reflected);
}


int main(int argc, char *argv[]) {
    parse_file_string(read_file(argv[1]));
  
    // STEP 1.b: update the inverse scale and inverse transpose matrices of each sphere. 
    mat4x4 unit_matrix = { 1, 0, 0, 0,
                           0, 1, 0, 0,
                           0, 0, 1, 0,
                           0, 0, 0, 1 };
        
    for (int m = 0; m < spheres.size();m++) {
        spheres[m].translate = translate(unit_matrix, spheres[m].position);
        spheres[m].scaleTranslate = scale(spheres[m].translate, spheres[m].scaler);
        spheres[m].inverseTransform = inverse(spheres[m].scaleTranslate);
    }

    
    // STEP 2: create correct variables
    int Width = view.resolution.x;
    int Height = view.resolution.y;
    const char *fname6 = output_name.name.c_str();
    Pixel* pixels = new Pixel[Width*Height];

    float Uc;
    float Vr;

    // Step 3: populate pixels
    for (int i = 0; i < Height; i++) {
        for (int j = 0; j < Width; j++) {

            // 1.--> Construct a ray, where ray is a unit vector from eye to pixel
            int row_number = i;
            int column_number = j;
            // --> get pixel position
            int width_screen = view.right;
            int height_screen = view.top;
            Uc = -(float)width_screen + (float)width_screen*(2 * (float)column_number / (float)Width);
            Vr = -(float)height_screen + (float)height_screen * (2 * (float)row_number / (float)Height);

            //innitialize a new ray struct object:
            Ray ray = Ray();
            ray.starting_point.x = Uc;
            ray.starting_point.y = Vr;
            ray.starting_point.z = -view.near;
           
            ray.direction.x = Uc;
            ray.direction.y = Vr;
            ray.direction.z = -view.near;

            // --> make it a unit vector
            ray.direction = normalize(ray.direction);
            ray.depth = 3;
            ray.reflected = false;
            // 2.--> ray trace:
            vec3 final_color = raytrace(ray);
            pixels[i*Width + j].r =std::min(final_color.x,1.0f)*255;
            pixels[i*Width + j].g = std::min(final_color.y,1.0f)*255;
            pixels[i*Width + j].b = std::min(final_color.z,1.0f)*255;

        } 
    }

    //for each row
    save_imageP6(Width, Height, fname6, reinterpret_cast<unsigned char*> (pixels));
    ////// TESTING FIND Intersection:

    ////// create a new ray object
    //cout << "number of spheres is: " << spheres.size() << "\n";
    //Ray ray = Ray();
    //ray.starting_point={ 1, 3, 0, 1 };
    //ray.direction = { 1, -1, 0, 0 };
    //ray.direction = normalize(ray.direction);
    //ray.depth = 2;
    //// create a new sphere
    //Sphere sphere = Sphere();
    //sphere.position = { 3, 1, 0 };
    //sphere.scaler = { 2, 2, 2 };

    //mat4x4 unit_matrix = { 1, 0, 0, 0,
    //                       0, 1, 0, 0,
    //                       0, 0, 1, 0,
    //                       0, 0, 0, 1 };

    //sphere.transpose = translate(unit_matrix, sphere.position);
    //sphere.scaleTranspose = scale(sphere.transpose, sphere.scaler);
    //sphere.inverseScaleTranspose = inverse(sphere.scaleTranspose);
    //spheres.push_back(sphere);

    //// create a new sphere
    //Sphere sphere2 = Sphere();
    //sphere2.position = { 3, 1, 0 };
    //sphere2.scaler = { 3, 3, 3 };

    //mat4x4 unit_matrix2 = { 1, 0, 0, 0,
    //                       0, 1, 0, 0,
    //                       0, 0, 1, 0,
    //                       0, 0, 0, 1 };

    //sphere2.transpose = translate(unit_matrix2, sphere2.position);
    //sphere2.scaleTranspose = scale(sphere2.transpose, sphere2.scaler);
    //sphere2.inverseScaleTranspose = inverse(sphere2.scaleTranspose);
    //spheres.push_back(sphere2);
    //cout << "number of spheres is: " << spheres.size() << "\n";
    //pair <bool, IntersectedSphere> pair = compute_closest_intersection(ray);
    //vec4 intersection_point = pair.second.intersection_point;
    //cout << "The hard coded intersection is: " << intersection_point.x<< " " << intersection_point.y << " " << intersection_point.z << "\n";

    return 0;
}
