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

//only one view Struct
View view;
vector <Sphere> spheres;
vector <Light> lights;
Background background_info;
Output output_name;
vector <Ray> rays;

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
    //cout << "current line before: " << current_line << "\n";
    string buffer(current_line);
    std::replace(buffer.begin(), buffer.end(), '\t', ' ');
    current_line = &buffer[0];
    //cout << "current line after: " << current_line << "\n";
    //cout << "buffer after is: " << buffer;

    // Get the first word
    char* next_word = NULL;
    char* words = strtok_s(current_line, " ", &next_word);

    if (strcmp(words, "NEAR") == 0) {
        //cout << "\nin near\n";
        //Get the near parameter
        words = strtok_s(NULL, " ", &next_word);
        view.near = stof(words); // convert string to float
    }
    else if (strcmp(words, "LEFT") == 0) {
        //cout << "\nin left\n";
        //Get the left parameter
        words = strtok_s(NULL, " ", &next_word);
        view.left = stof(words);

    }
    else if (strcmp(words, "RIGHT") == 0) {
        //cout << "\nin right\n";
        //Get the "right" parameter
        words = strtok_s(NULL, " ", &next_word);
        view.right = stof(words);
    }
    else if (strcmp(words, "BOTTOM") == 0) {
        //cout << "\nin bottom\n";
        //Get the near parameter
        words = strtok_s(NULL, " ", &next_word);
        view.bottom = stof(words);
    }
    else if (strcmp(words, "TOP") == 0) {
        //cout << "\nin top\n";
        //Get the near parameter
        words = strtok_s(NULL, " ", &next_word);
        view.top = stof(words);

    }
    else if (strcmp(words, "RES") == 0) {
        //cout << "\nin res\n";
        //Get the res x parameter
        words = strtok_s(NULL, " ", &next_word);
        view.resolution.x = stof(words);

        //Get the res y parameter
        words = strtok_s(NULL, " ", &next_word);
        view.resolution.y = stof(words);
    }
    else if (strcmp(words, "SPHERE") == 0) {
        //cout << "\nin sphere\n";
        //Get the name
        Sphere s1 = Sphere();

        words = strtok_s(NULL, " ", &next_word);
        s1.name = words;

        // get the x position
        words = strtok_s(NULL, " ", &next_word);
        s1.position.x = stof(words);

        // get the y position
        words = strtok_s(NULL, " ", &next_word);
        s1.position.y = stof(words);

        // get the z position
        words = strtok_s(NULL, " ", &next_word);
        s1.position.z = stof(words);

        // get the x scalar
        words = strtok_s(NULL, " ", &next_word);
        s1.scaler.x = stof(words);

        // get the y scalar
        words = strtok_s(NULL, " ", &next_word);
        s1.scaler.y = stof(words);

        // get the z scalar
        words = strtok_s(NULL, " ", &next_word);
        s1.scaler.z = stof(words);

        // get the x color
        words = strtok_s(NULL, " ", &next_word);
        s1.color.x = stof(words);

        // get the y color
        words = strtok_s(NULL, " ", &next_word);
        s1.color.y = stof(words);

        // get the z color
        words = strtok_s(NULL, " ", &next_word);
        s1.color.z = stof(words);

        // get the ka
        words = strtok_s(NULL, " ", &next_word);
        s1.ka = stof(words);

        // get the kd
        words = strtok_s(NULL, " ", &next_word);
        s1.kd = stof(words);

        // get the ks
        words = strtok_s(NULL, " ", &next_word);
        s1.ks = stof(words);


        // get the kr
        words = strtok_s(NULL, " ", &next_word);
        s1.kr = stof(words);

        // get the spec exp
        words = strtok_s(NULL, " ", &next_word);
        s1.spec_exp = stof(words);
        ////cout << "the spec exp is: " << spheres[spheres.size() - 1].spec_exp;

       spheres.push_back(s1);
       /*
        cout << "in Sphere struct there is the following sphere: \n";
        cout << "sphere:\n" << spheres[spheres.size() - 1].name << "\n"
            << spheres[spheres.size() - 1].position.x << " " << spheres[spheres.size() - 1].position.y
            << " " << spheres[spheres.size() - 1].position.z << " " << spheres[spheres.size() - 1].scaler.x
            << " " << spheres[spheres.size() - 1].scaler.y << " " << spheres[spheres.size() - 1].scaler.z
            << " " << spheres[spheres.size() - 1].color.x << " " << spheres[spheres.size() - 1].color.y
            << " " << spheres[spheres.size() - 1].color.z << " " << spheres[spheres.size() - 1].ka
            << " " << spheres[spheres.size() - 1].kd << " " << spheres[spheres.size() - 1].ks << " " << spheres[spheres.size() - 1].kr << " " << spheres[spheres.size() - 1].spec_exp << "\n";*/

    }
    else if (strcmp(words, "LIGHT") == 0) {
        //cout << "\nin light\n";
        //Get the name
        Light l1 = Light();
        words = strtok_s(NULL, " ", &next_word);
        l1.name = words;
         
        // get the x position
        words = strtok_s(NULL, " ", &next_word);
        l1.position.x = stof(words);

        // get the y position
        words = strtok_s(NULL, " ", &next_word);
        l1.position.y = stof(words);

        // get the z position
        words = strtok_s(NULL, " ", &next_word);
        l1.position.z = stof(words);

        // get the x color
        words = strtok_s(NULL, " ", &next_word);
        l1.color.x = stof(words);

        // get the y color
        words = strtok_s(NULL, " ", &next_word);
        l1.color.y = stof(words);

        // get the z color
        words = strtok_s(NULL, " ", &next_word);
        l1.color.z = stof(words);

        lights.push_back(l1);
        //cout << "The light vector contains the following Light struct: \n";
        //cout << "light:\n" << lights[lights.size() - 1].name << "\n"
        /*    << lights[lights.size() - 1].position.x << " " << lights[lights.size() - 1].position.y
            << " " << lights[lights.size() - 1].position.z << " " << " "
            << lights[lights.size() - 1].color.x << " " << lights[lights.size() - 1].color.y
            << " "*/ //<< lights[lights.size() - 1].color.z << "\n";
    }
    else if (strcmp(words, "BACK") == 0) {
        //cout << "\nin back\n";
        //Get the x color
        words = strtok_s(NULL, " ", &next_word);
        background_info.color.x = stof(words);


        //get the y color
        words = strtok_s(NULL, " ", &next_word);
        background_info.color.y = stof(words);

        //get the z color
        words = strtok_s(NULL, " ", &next_word);
        background_info.color.z = stof(words);

        //cout << "the background_info vector contains the following Background struct with color: \n";
        //cout << "background color is:\n" << background_info[background_info.size() - 1].color.x << " " << background_info[background_info.size() - 1].color.y
            //<< " " << background_info[background_info.size() - 1].color.z << "\n";

    }
    else if (strcmp(words, "AMBIENT") == 0) {
        //cout << "\nin ambient\n";
        //Get the x ambient      
        words = strtok_s(NULL, " ", &next_word);
        background_info.ambient.x = stof(words);

        //get the y ambient
        words = strtok_s(NULL, " ", &next_word);
        background_info.ambient.y = stof(words);

        //get the z color
        words = strtok_s(NULL, " ", &next_word);
        background_info.ambient.z = stof(words);

        //cout << "the background_info vector contains the following Background struct with ambient: \n";
        //cout << background_info[background_info.size() - 1].ambient.x << " " << background_info[background_info.size() - 1].ambient.y
            //<< " " << background_info[background_info.size() - 1].ambient.z << "\n";
    }
    else {
        //cout << "\nin output\n";
        //Get the output parameter:
        words = strtok_s(NULL, " ", &next_word);
        //There is only one output so the output vector of Output structs should be empty   
        output_name.name = words;
     
        //cout << "the output_name vector contains the following Output struct with the name: "
            //<< output_name[0].name << "\n";

    }
}