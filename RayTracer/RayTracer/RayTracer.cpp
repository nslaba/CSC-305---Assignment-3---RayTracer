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
    // Modify the forloop so you switch top and bottom rows
    for (int j = Height-1; j >=0; j--) {// j goes from 0 to 600
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

    // The solutions' vector of vec2's parameters are:
    // --> x is t (ie vector scaler to reach intersection point) 
    // --> y is j=the numbered sphere in the forloop
    vector <vec2> solutions;    
    //cout << "initial solutions size is: " << solutions.size() << "\n";
    /*//cout << "spheres size is: " << spheres.size();*/
    for (int j=0; j < spheres.size(); j++) {
       // cout << "In for loop for spheres in find closest intersection func\n";
        /* STEP 1.c) invert ray for each sphere based on the sphere's scalars. */
        vec4 inverted_starting_point = spheres[j].inverseScaleTranspose * ray.starting_point;
        vec4 inverted_direction = spheres[j].inverseScaleTranspose * ray.direction;      
        //cout << "ray's starting point is: " << ray.starting_point.x << ray.starting_point.y << ray.starting_point.z << ray.starting_point.w<<"\n";
        //cout << "\nray's direction is: " << ray.direction.x << ray.direction.y << ray.direction.z << ray.direction.w<<"\n";
        //Problem: determinant is ALWAYS pos!!
        // First store S (inverted starting point) and c (inverted direction) in NON-homogeneous coordinates
        vec3 S = { inverted_starting_point.x, inverted_starting_point.y, inverted_starting_point.z };
        vec3 c = { inverted_direction.x, inverted_direction.y, inverted_direction.z };    
        //cout << "inverted starting point is: " << inverted_starting_point.x << inverted_starting_point.y << inverted_starting_point.z << "\n";
        /* STEP 1.d) Use the quadratic formula to check for solutions (intersections)
         * --> If (determinant >= 0) then find the solution(s) representing the intersection pt(s) */
        const float determinant = pow(dot(S, c), 2) - (pow(length(c), 2) * (pow(length(S), 2) - 1));
        //cout << length(c) << "\n";
        //cout << "S is: " << S.x << " " << S.y << " " << S.z << "\n";
        //cout << "c is: " << c.x << " " << c.y << " " << c.z << "\n";
        //cout << "the determinant is: " << determinant << "\n";
        if (determinant >= 0.0){
            //set the boolean to true suggesting that there has been an intersection
            
       /*     if (inter_pair.first) {
                cout << "TRUE 1\n";
            }*/
            // In this case the ray hits the sphere either once or twice, so find the solution for t
            float t1 = -(dot(S, c) / pow(length(c), 2)) - (sqrt(determinant)/pow(length(c),2));
            float t2 = -(dot(S, c) / pow(length(c), 2)) + (sqrt(determinant) / pow(length(c), 2));
            //cout << "t1 is: " << t1 << "\n";
            //cout << "t2 is: " << t2 << "\n";
            //push the smaller one & save which sphere was affected by storing the j value
            if (t1 < t2) {
                //push t1 to solutions vector
                solutions.push_back(vec2{ t1,j });
                spheres[j].cannonicalIntersectionPoint = inverted_starting_point + t1 * inverted_direction;
                inter_pair.first = true;
            }
            else {
                solutions.push_back(vec2{ t2,j });
                spheres[j].cannonicalIntersectionPoint = inverted_starting_point + t1 * inverted_direction;
                inter_pair.first = true;
            }
        }        
    }
    //if (inter_pair.first) {
    //    cout << "TRUE 2\n";
    //}
    //If no solutions found then return a null vec4
    //cout << "there are this many solutions: "<< solutions.size()<<"\n";
    if (inter_pair.first!=true) {
        //cout << "no solution";
        return inter_pair;
    }

    /* STEP 2.b) pick the smallest intersection */
    inter_pair.second.order = solutions[0].y;
    inter_pair.second.t = solutions[0].x;
    //cout << "t1 in solutions is: " << solutions[0].x << "\n";
    //inter_pair.first = true;
    for (int i = 1; i < solutions.size(); i++) {
       // cout << "current t in solutions is: " << solutions[i].x << "\n";
        if (solutions[i].x < inter_pair.second.t) {
           // cout << "in if statement\n";
            inter_pair.second.t = solutions[i].x;
            inter_pair.second.order = solutions[i].y;
        }
    }
   // cout << "final smallest t is saved as: " << inter_pair.second.t << "\n";
    
    /* STEP 2.b) Delete all the entries in the vector of "intersections" so that it can be reused */
    solutions.clear();

    //find the normal at that intersection point
    vec4 normal = { spheres[inter_pair.second.order].cannonicalIntersectionPoint };
    normal = normal * spheres[inter_pair.second.order].inverseScaleTranspose;
    
    inter_pair.second.normal = normalize(vec3{normal});

       
    inter_pair.second.intersection_point = ray.starting_point + inter_pair.second.t * ray.direction;
    //Save the view vector towards the intersection point
    inter_pair.second.view_vector = vec4{ -normalize(vec3{inter_pair.second.intersection_point}),0 };
   
    //cout << inter_pair.second.intersection_point.x << inter_pair.second.intersection_point.y << inter_pair.second.intersection_point.z << "\n";
    //Return the intersected sphere object
    return inter_pair;
}

/* Part 4:
 *         check if there is a shadow at the intersection pt if there isn't then add the ADS light, if there is then don't
 */
vec3 shadow_ray(Light light, IntersectedSphere intersected_sphere) {
    /*STEP 1: innitialize color as black since ambient will be present whether in shadow or not*/
    vec3 color = { 0,0,0 };

    /*STEP 2: Create a shadow ray (intersection pt-light) with starting point being the intersection_point and direction being the normalized dir towards the light*/
    Ray shadow_ray = Ray();
    shadow_ray.direction = { vec4{light.position,1.0} - intersected_sphere.intersection_point };
    shadow_ray.direction = -vec4{ normalize(vec3{ shadow_ray.direction }),0 };

    shadow_ray.starting_point = vec4{ light.position, 1 };
    //CHANGING THE DIRECTION OF SHADOW_RAY TO GO FROM LIGHT TOWARDS THE INTERSECTED PT
    //check if in shadow. If yes, then don't add diffuse and specular if no then compute DS
    pair <bool, IntersectedSphere> closest_intersection = compute_closest_intersection(shadow_ray);
    if (closest_intersection.first) { //Then it does intersect some spheres then check if the t value is less than the distance to the light
        if (closest_intersection.second.order != intersected_sphere.order){
            return color;
        }
    }

    //otherwise compute Diffuse
    color += light.color * spheres[intersected_sphere.order].kd * spheres[intersected_sphere.order].color * std::max(0.0f, dot(intersected_sphere.normal, -vec3{ shadow_ray.direction }));
    // Compute Specular
    vec3 reflected_vector = reflect(vec3{ shadow_ray.direction }, intersected_sphere.normal);
    color += light.color * spheres[intersected_sphere.order].ks * (float)pow(std::max(0.0f,dot(reflected_vector, vec3{intersected_sphere.view_vector})), spheres[intersected_sphere.order].spec_exp);
   
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
    /*cout << "in Raytrace func spheres size is: " << spheres.size();*/
    /* STEP 1: check for the closest intersection with all objects*/
    pair <bool,IntersectedSphere> closest_intersection = compute_closest_intersection(ray);

    if (!closest_intersection.first) {
        // if there is no intersection then return the background color
        //cout << background_info[0].color.x << background_info[0].color.y << background_info[0].color.z;
        return background_info.color;
    } 
    // From now on only dealing with intersected objects
    /* Initialize final color of object*/
    vec3 final_color;
    //Initialize ambient
    final_color =  background_info.ambient* spheres[closest_intersection.second.order].ka*spheres[closest_intersection.second.order].color;
    //cout << "something has been intersected\n";

    /* STEP 2: check for shadows and add them up*/
    //vec3 color_shadow = { 0,0,0 };
    final_color+= shadow_ray(lights[0], closest_intersection.second);
    for (int i = 1; i < lights.size(); i++) {
        final_color += shadow_ray(lights[i], closest_intersection.second);
    }
    /*STEP 3: raytrace for reflected rays*/
    //I need to somehow get which sphere i'm dealing with based on the closest intersection so that i can calculate the normal. Why don't I just make a global variable? sphere_num that gets updated whenever I call closest intersection pt. 
    // Does this mean I reflect the ray over a normal on the sphere? YES--> How can i get the normal???

    // initialize a new ray for the reflected ray
    //Ray reflected_ray = Ray();
    //reflected_ray.direction = closest_intersection.second.reflected_ray;
    //reflected_ray.depth = 3;
    //reflected_ray.starting_point = closest_intersection.second.intersection_point;
    //vec3 color_reflected = raytrace(reflected_ray);
    vec3 color_reflected = { 0,0,0 };
    //cout << final_color.x << final_color.y << final_color.z << "\n";
    //cout << "the closest intersected sphere is: "<<(closest_intersection.second.order);
    //return (vec3{ spheres[closest_intersection.second.order].color + spheres[closest_intersection.second.order].kr * color_reflected});
    return (final_color + spheres[closest_intersection.second.order].kr * color_reflected);
}

int main(int argc, char *argv[]) {
    parse_file_string(read_file(argv[1]));
    //cout << "after parsing spheres size is: " << spheres.size();
    // STEP 1.b: update the inverse scale and inverse transpose matrices of each sphere. 
    mat4x4 unit_matrix = { 1, 0, 0, 0,
                           0, 1, 0, 0,
                           0, 0, 1, 0,
                           0, 0, 0, 1 };
        
    for (int m = 0; m < spheres.size();m++) {
        spheres[m].transpose = translate(unit_matrix, spheres[m].position);
        spheres[m].scaleTranspose = scale(spheres[m].transpose, spheres[m].scaler);
        spheres[m].inverseScaleTranspose = inverse(spheres[m].scaleTranspose);
    }

    //for (int q = 0; q < spheres.size(); q++) {
    //    cout << "\nTHE transpose matrix is:\n";

    //    for (int a = 0; a < 4; a++) {
    //        for (int b = 0; b < 4; b++)
    //        {
    //            //print the row of first matrix
    //            cout << spheres[q].transpose[a][b] << "   ";
    //        }
    //        cout << "\n";

    //    }
    //    cout << "\nTHE ScaleTranspose matrix is:\n";

    //    for (int c = 0; c < 4; c++) {
    //        for (int d = 0; d < 4; d++)
    //        {
    //            //print the row of first matrix
    //            cout << spheres[q].scaleTranspose[c][d] << "   ";
    //        }
    //        cout << "\n";
    //    }
    //    cout << "\nTHE InverseScaleTranspose matrix is:\n";

    //    for (int e = 0; e < 4; e++) {
    //        for (int f = 0; f < 4; f++)
    //        {
    //            //print the row of first matrix
    //            cout << spheres[q].inverseScaleTranspose[e][f] << "   ";
    //        }
    //        cout << "\n";
    //    }
    //}
    
    //for (int z = 0; z < spheres.size(); z++) {
    //    cout << "sphere:\n" << spheres[z].name << "\n"
    //        << spheres[z].position.x << " " << spheres[z].position.y
    //        << " " << spheres[z].position.z << " " << spheres[z].scaler.x
    //        << " " << spheres[z].scaler.y << " " << spheres[z].scaler.z
    //        << " " << spheres[z].color.x << " " << spheres[z].color.y
    //        << " " << spheres[z].color.z << " " << spheres[z].ka
    //        << " " << spheres[z].kd << " " << spheres[z].ks << " " << spheres[z].kr << " " << spheres[z].spec_exp << "\n";
    //}
    //cout << "there are " << lights.size() << " # of lights\n";
    //for (Light light : lights) {
    //    cout << "light:\n" << light.name << "\n"
    //       << light.position.x << " " << light.position.y
    //       << " " << light.position.z << " " << " "
    //       << light.color.x << " " << light.color.y
    //       << " "<< light.color.z << "\n";
    //}
    //cout << "Within the view Struct there is: " << view.near << view.left << view.right << view.top << view.bottom << view.resolution.x << view.resolution.y;

    //cout << "\nWithin the background_info struct there is: color" << background_info.color.x << background_info.color.y << background_info.color.z << " then ambient: " << background_info.ambient.x << background_info.ambient.y << background_info.ambient.z;
    
    //cout << "before pixel forloop spheres size is: " << spheres.size()<<"\n";
    
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
      /*      cout << "Uc is: " << Uc<<"\n\n";
            cout << "column number is: " << column_number << "\n\n";
            cout << "row_number is: " << row_number << "\n\n";
            cout << "Height is: " << Height << "\n\n";
            cout << "Width is: " << Height << "\n\n";
            cout << "Vr is: " << Vr << "\n\n";*/
            //innitialize a new ray struct object:
            Ray ray = Ray();
            ray.starting_point.x = Uc;
            ray.starting_point.y = Vr;
            ray.starting_point.z = -view.near;
            ray.starting_point.w = 1;
            
            ray.direction.x = Uc;
            ray.direction.y = Vr;
            ray.direction.z = -view.near;
            ray.direction.w = 0;

            // --> make it a unit vector
            ray.direction = normalize(ray.direction);
            ray.depth = 3;
            // 2.--> ray trace:
            //cout << "the starting pt in main is: "<<ray.starting_point.x<<" "<< ray.starting_point.y<<" "<< ray.starting_point.z<<"\n";
            vec3 final_color = raytrace(ray);
            //cout << "ray color: " << final_color.x << final_color.y << final_color.z << "\n";
            pixels[i*Width + j].r =std::min(final_color.x,1.0f)*255;
            pixels[i*Width + j].g = std::min(final_color.y,1.0f)*255;
            pixels[i*Width + j].b = std::min(final_color.z,1.0f)*255;
            
            //pixels[]
           // cout << "pixel rgb is: " << pixels[i * Width + j].r << pixels[i * Width + j].g << pixels[i * Width + j].b << "\n";
          /*  pixels[i*Width + j].r = 100;
            pixels[i * Width + j].g = 0;
            pixels[i * Width + j].b = 0;*/
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
