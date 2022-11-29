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
#include <vector>
#include <algorithm> // in order to use "replace" function for testParsing

#include "structs.hpp"

using namespace std;


#define MAX_LINE_LEN 132
int sphere_count = 0;
int light_count = 0;

//vectors for the specific structs
vector <View> view;
vector <Sphere> spheres;
vector <Light> lights;
vector <Background> background_info;
vector <Output> output_name;


//Declare functions:
void populate_structs(char * lines);


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

/* Step 1A:
 *        Read file into a string variable  
 *
 */
string read_file(char* file_name) { 
    ifstream f(file_name);
    string file;
    if (f) {
        ostringstream ss;
        ss << f.rdbuf();
        file = ss.str();
    }
    return file;
}

/* Part 1B:
 *         Parse the stored string file into usable variables
 *
 */

void parse_file_string(string file) {
    // I want to tokenize the string first based on the end of line char, then on spaces
    // Splits str[] according to given delimiters.
    // and returns next token. It needs to be called
    // in a loop to get all tokens. It returns NULL
    // when there are no more tokens.

    char* next_line = NULL;
 
    //returns first line
    char* lines = strtok_s(&file[0], "\n", &next_line);
    
    while (lines != NULL) {
        populate_structs(lines);
        lines = strtok_s(NULL, "\n", &next_line);
    }
}


/* Part 1C:
 *         Populate the correct structs
 *
 */
void populate_structs(char* current_line) {
    // Replace all tabs with a space (nescessary for testParsing)
    cout << "current line before: " << current_line << "\n";
    string buffer(current_line);
    std::replace(buffer.begin(), buffer.end(), '\t', ' ');
    current_line = &buffer[0];
    cout << "current line after: " << current_line << "\n";
    cout << "buffer after is: " << buffer;
    
    // Get the first word
    char* next_word = NULL;
    char* words = strtok_s(current_line, " ", &next_word);
   
    if (strcmp(words, "NEAR")==0) {
        cout << "\nin near\n";
        //Get the near parameter
        words = strtok_s(NULL, " ", &next_word); // would this be on \n?
        if (view.size() == 0) { // view is a vector of View structs. If it's empty then
            //no view struct created yet
            View v = View();
            v.near = stof(words); // convert string to float
            view.push_back(v);    // push the newly created View struct v into the vector of structs view
            cout << "in View struct near is: " << v.near << "\n"; // print it out for reference
        }
        else { // the vector of View structs isn't empty so there already is a View struct created
            //a view struct already exists so push into it
            view[0].near = stof(words); //In all cases there will only be one view struct
            cout << "in View struct near is: " << view[0].near << "\n";
        }
    }
    else if (strcmp(words, "LEFT")==0) {
        cout << "\nin left\n";
        //Get the left parameter
        words = strtok_s(NULL, " ", &next_word);
        if (view.size() == 0) { // the vector of View structs is empty
            //create a new View struct to push into vector view of Views.
            View v = View();
            v.left = stof(words);
            view.push_back(v);
            cout << "in View struct left is: " << v.left << "\n";
        }
        else {
            //a view struct already exists so push into it
            view[0].left = stof(words); // In all cases there will only be one View struct
            cout << "in View struct left is: " << view[0].left << "\n";
        }
    }
    else if (strcmp(words, "RIGHT")==0) {
        cout << "\nin right\n";
        //Get the "right" parameter
        words = strtok_s(NULL, " ", &next_word);
        if (view.size() == 0) { //the vector of View structs is empty
            //no view struct created yet
            View v = View();
            v.right = stof(words);
            view.push_back(v);
            cout << "in View struct right is: " << v.right << "\n";
        }
        else {
            //a view struct already exists so push into it
            view[0].right = stof(words);
            cout << "in View struct right is: " << view[0].right << "\n";
        }
    }
    else if (strcmp(words, "BOTTOM")==0){
        cout << "\nin bottom\n";
        //Get the near parameter
        words = strtok_s(NULL, " ", &next_word);
        if (view.size() == 0) { // the vector of View structs is empty
            //no view struct created yet
            View v = View();
            v.bottom = stof(words);
            view.push_back(v);
            cout << "in View struct bottom is: " << v.bottom << "\n";
        }
        else {
            //a view struct already exists so push into it
            view[0].bottom = stof(words);
            cout << "in View struct bottom is: " << view[0].bottom << "\n";
        }
    }
    else if (strcmp(words, "TOP")==0) {
        cout << "\nin top\n";
        //Get the near parameter
        words = strtok_s(NULL, " ", &next_word);
        if (view.size() == 0) { // No vector of View structs created yet
            //no view struct created yet
            View v = View();
            v.top = stof(words);
            view.push_back(v);
            cout << "in View struct top is: " << v.top << "\n";
        }
        else {
            //a view struct already exists so push into it
            view[0].top = stof(words);
            cout << "in View struct view is: " << view[0].top << "\n";
        }
    }
    else if (strcmp(words, "RES")==0) {
        cout << "\nin res\n";
        //Get the res x parameter
        words = strtok_s(NULL, " ", &next_word);
        if (view.size() == 0) { // No vector of View structs exists yet
            //no view struct created yet
            View v = View();
            v.resolution.x = stof(words);
            view.push_back(v);
            cout << "in View struct Res x is: " << v.resolution.x << "\n";
        }
        else {
            //a view struct already exists so push into it
            view[0].resolution.x = stof(words);
            cout << "in View struct Res x is: " << view[0].resolution.x << "\n";
        }

        //Get the res y parameter
        words = strtok_s(NULL, " ", &next_word);
        view[0].resolution.y = stof(words);
        cout << "in View struct Res y is: " << view[0].resolution.y << "\n";
    }
    else if (strcmp(words, "SPHERE")==0) {
        cout << "\nin sphere\n";
        //Get the name
        words = strtok_s(NULL, " ", &next_word);
        if (spheres.size() == 0) { // no vector of Sphere structs created yet
            //no sphere struct created yet
            Sphere s1 = Sphere();
            s1.name = words;
            spheres.push_back(s1);
            
        }
        else {
            //a view struct already exists so push into it
            spheres[spheres.size() - 1].name = words;
       
        }

        // get the x position
        words = strtok_s(NULL, " ", &next_word);
        spheres[spheres.size() - 1].position.x = stof(words);

        // get the y position
        words = strtok_s(NULL, " ", &next_word);
        spheres[spheres.size() - 1].position.y = stof(words);

        // get the z position
        words = strtok_s(NULL, " ", &next_word);
        spheres[spheres.size() - 1].position.z = stof(words);

        // get the x scalar
        words = strtok_s(NULL, " ", &next_word);
        spheres[spheres.size() - 1].scaler.x = stof(words);

        // get the y scalar
        words = strtok_s(NULL, " ", &next_word);
        spheres[spheres.size() - 1].scaler.y = stof(words);

        // get the z scalar
        words = strtok_s(NULL, " ", &next_word);
        spheres[spheres.size() - 1].scaler.z = stof(words);

        // get the x color
        words = strtok_s(NULL, " ", &next_word);
        spheres[spheres.size() - 1].color.x = stof(words);

        // get the y color
        words = strtok_s(NULL, " ", &next_word);
        spheres[spheres.size() - 1].color.y = stof(words);

        // get the z color
        words = strtok_s(NULL, " ", &next_word);
        spheres[spheres.size() - 1].color.z = stof(words);

        // get the ka
        words = strtok_s(NULL, " ", &next_word);
        spheres[spheres.size() - 1].ka = stof(words);

        // get the kd
        words = strtok_s(NULL, " ", &next_word);
        spheres[spheres.size() - 1].kd = stof(words);

        // get the ks
        words = strtok_s(NULL, " ", &next_word);
        spheres[spheres.size() - 1].ks = stof(words);


        // get the kr
        words = strtok_s(NULL, " ", &next_word);
        spheres[spheres.size() - 1].kr = stof(words);

        // get the spec exp
        words = strtok_s(NULL, " ", &next_word);
        spheres[spheres.size() - 1].spec_exp = stof(words);
        //cout << "the spec exp is: " << spheres[spheres.size() - 1].spec_exp;

      
        cout << "in Sphere struct there is the following sphere: \n";
        cout << "sphere:\n" << spheres[spheres.size() - 1].name << "\n" 
            << spheres[spheres.size() - 1].position.x<< " " <<spheres[spheres.size() - 1].position.y
            << " " << spheres[spheres.size() - 1].position.z<< " " << spheres[spheres.size() - 1].scaler.x
            << " " << spheres[spheres.size() - 1].scaler.y<< " " << spheres[spheres.size() - 1].scaler.z
            << " " << spheres[spheres.size() - 1].color.x<< " " << spheres[spheres.size() - 1].color.y
            << " " << spheres[spheres.size() - 1].color.z<< " " << spheres[spheres.size() - 1].ka
            << " " << spheres[spheres.size() - 1].kd <<" " << spheres[spheres.size() - 1].ks << " " << spheres[spheres.size() - 1].kr << " " << spheres[spheres.size() - 1].spec_exp << "\n";

    }
    else if (strcmp(words, "LIGHT") == 0) {
        cout << "\nin light\n";
        //Get the name
        words = strtok_s(NULL, " ", &next_word);
        if (lights.size() == 0) { // The vector of Light structs is empty
            //no light struct created yet
            Light l1 = Light();
            l1.name = words;
            lights.push_back(l1);

        }
        else {
            //a light struct already exists so push into it
            lights[lights.size() - 1].name = words;
        }

        // get the x position
        words = strtok_s(NULL, " ", &next_word);
        lights[lights.size() - 1].position.x = stof(words);

        // get the y position
        words = strtok_s(NULL, " ", &next_word);
        lights[lights.size() - 1].position.y = stof(words);

        // get the z position
        words = strtok_s(NULL, " ", &next_word);
        lights[lights.size() - 1].position.z = stof(words);

        // get the x color
        words = strtok_s(NULL, " ", &next_word);
        lights[lights.size() - 1].color.x = stof(words);

        // get the y color
        words = strtok_s(NULL, " ", &next_word);
        lights[lights.size() - 1].color.y = stof(words);

        // get the z color
        words = strtok_s(NULL, " ", &next_word);
        lights[lights.size() - 1].color.z = stof(words);

        cout << "The light vector contains the following Light struct: \n";
        cout << "light:\n" << lights[lights.size() - 1].name << "\n"
            << lights[lights.size() - 1].position.x << " " << lights[lights.size() - 1].position.y
            << " " << lights[lights.size() - 1].position.z << " " << " "
            << lights[lights.size() - 1].color.x << " " << lights[lights.size() - 1].color.y
            << " " << lights[lights.size() - 1].color.z << "\n";
    }
    else if (strcmp(words, "BACK") == 0) {
        cout << "\nin back\n";
        //Get the x color
        words = strtok_s(NULL, " ", &next_word);
        if (background_info.size() == 0) { // the background_info vector of Background structs is empty
            //no background_info struct created yet
            Background b = Background();
            b.color.x = stof(words);
            background_info.push_back(b);

        }
        else {
            //a light struct already exists so push into it
            background_info[background_info.size() - 1].color.x = stof(words);
        }

        //get the y color
        words = strtok_s(NULL, " ", &next_word);
        background_info[background_info.size() - 1].color.y = stof(words);

        //get the z color
        words = strtok_s(NULL, " ", &next_word);
        background_info[background_info.size() - 1].color.z = stof(words);

        cout << "the background_info vector contains the following Background struct with color: \n";
        cout << "background color is:\n" << background_info[background_info.size() - 1].color.x << " " << background_info[background_info.size() - 1].color.y
            << " " << background_info[background_info.size() - 1].color.z << "\n";
            
    }
    else if (strcmp(words, "AMBIENT")==0) {
        cout << "\nin ambient\n";
        //Get the x ambient
        words = strtok_s(NULL, " ", &next_word);
        if (background_info.size() == 0) { //The background_info vector of Background structs is empty
            //no background_info struct created yet
            Background b = Background();
            b.ambient.x = stof(words);
            background_info.push_back(b);

        }
        else {
            //an ambient struct already exists so push into it
            background_info[background_info.size() - 1].ambient.x = stof(words);
        }

        //get the y ambient
        words = strtok_s(NULL, " ", &next_word);
        background_info[background_info.size() - 1].ambient.y = stof(words);

        //get the z color
        words = strtok_s(NULL, " ", &next_word);
        background_info[background_info.size() - 1].ambient.z = stof(words);

        cout << "the background_info vector contains the following Background struct with ambient: \n";
        cout << background_info[background_info.size() - 1].ambient.x << " " << background_info[background_info.size() - 1].ambient.y
            << " " << background_info[background_info.size() - 1].ambient.z << "\n";
    }
    else {
        cout << "\nin output\n";
        //Get the output parameter:
        words = strtok_s(NULL, " ", &next_word);
        //There is only one output so the output vector of Output structs should be empty
        Output out = Output();
        out.name = words;
        output_name.push_back(out);
        cout << "the output_name vector contains the following Output struct with the name: "
            << output_name[0].name << "\n";

    }
}
//*****************************************************************************************************//
//***** STEP 2: Set the camera & the image plane ******************************************************//
//*****************************************************************************************************//
// Set the viewport and resolution in camera coordinates
// Img parameters:
// 2W, 2H given by TOP, BOTTOM, LEFT, RIGHT in view struct
// number of Pixels is nCols x nRows --> in our case it's resolution in view struct
// camera parameters:
// Camera coord sys (eye, u, v, n), where n is the normal of the viewing plane, and u v are the span of the viewing plane & uvn are 
// orthogonal to each other
// Image plane at n = -near where near is in view struct


int main(int argc, char *argv[]) //argc is argument count--> stores the # of command line arguments passed by the user incl the name of the program
// therefore if there is a command line arg then the value of argc is 2
// argv is argument vector --> an array of character pointers listing all the arguments
// argv[0] is the name of the program and argv[1] will be the name of the input file
{
    // STEP 1: READ the input FILE
    parse_file_string(read_file(argv[1]));

    // STEP 2: create correct variables
    int Width = view[0].resolution.x;
    int Height = view[0].resolution.y;
    const char *fname6 = output_name[0].name.c_str();
    Pixel* pixels = new Pixel[Width*Height];

    // Step 3: populate pixels
    // --> for each pixel
    for (int i = 0; i < Height; i++) {
        for (int j = 0; j < Width; j++) {

            // 1.--> Construct a ray, where ray is a unit vector from eye to pixel
            int row_number = i;
            int column_number = j;
            // --> get pixel position
            int width_screen = view[0].right;
            int height_screen = view[0].top;
            float Uc = -width_screen + width_screen*(2 * column_number / Width);
            float Vr = -height_screen + height_screen * (2 * row_number / Height);
            vec3 ray(Uc, Vr, -view[0].near);
            // --> make it a unit vector
            ray = normalize(ray);
            
            // 2.--> ray trace:
            // --> having a scalar value for t increase with iterations 

            // --> check for closest intersection

            // --> add up all colors

            //
            
            pixels[i].r = 255;
            pixels[i].g = 0;
            pixels[i].b = 200;
        }

        
    }



    save_imageP6(Width, Height, fname6, reinterpret_cast<unsigned char*> (pixels));
    return 0;
}

//stream is a sequence of characters

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
