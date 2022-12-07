#pragma once

#include <iostream> //input output stream --> a file in the standard library
#include <fstream>
#include <sstream>
#include <cstdlib> //For exit function
#include <cstdio>
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/gtc/matrix_transform.hpp> //for inverse matrices
#include <cmath> //for square roots
using namespace std;
using namespace glm;

// Struct for view
struct View {
    float near;
    float left;
    float right;
    float top;
    float bottom;
    u32vec2 resolution;
};


// Struct for sphere
struct Sphere {
    string name;
    vec3 position;
    vec3 scaler;
    vec3 color;
    float ka;
    float kd;
    float ks;
    float kr;
    int spec_exp;
    mat4 transpose;//call translate
    mat4 scaleTranspose;//call scaleTranslate
    mat4 inverseScaleTranspose; //call inverseTranspose
    vec4 cannonicalIntersectionPoint;
};

//Struct for light
struct Light {
    string name;
    vec3 position;
    vec3 color;
};

//Struct for background
struct Background {
    vec3 color;
    vec3 ambient;
};

//Struct for output name
struct Output {
    string name;
};

//Struct for a pixel
struct Pixel {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

//Struct for a ray
struct Ray {
    string name;
    vec4 starting_point;
    vec4 direction;
    int depth;
};

//Struct for an intersected sphere
struct IntersectedSphere {
    int order;
    vec3 normal;
    float t;
    vec4 intersection_point;
    vec4 view_vector;
};
