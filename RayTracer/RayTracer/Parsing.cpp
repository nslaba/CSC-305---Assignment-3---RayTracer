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

vector <View> view;
vector <Sphere> spheres;
vector <Light> lights;
vector <Background> background_info;
vector <Output> output_name;
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
        words = strtok_s(NULL, " ", &next_word); // would this be on \n?
        if (view.size() == 0) { // view is a vector of View structs. If it's empty then
            //no view struct created yet
            View v = View();
            v.near = stof(words); // convert string to float
            view.push_back(v);    // push the newly created View struct v into the vector of structs view
            //cout << "in View struct near is: " << v.near << "\n"; // print it out for reference
        }
        else { // the vector of View structs isn't empty so there already is a View struct created
            //a view struct already exists so push into it
            view[0].near = stof(words); //In all cases there will only be one view struct
            //cout << "in View struct near is: " << view[0].near << "\n";
        }
    }
    else if (strcmp(words, "LEFT") == 0) {
        //cout << "\nin left\n";
        //Get the left parameter
        words = strtok_s(NULL, " ", &next_word);
        if (view.size() == 0) { // the vector of View structs is empty
            //create a new View struct to push into vector view of Views.
            View v = View();
            v.left = stof(words);
            view.push_back(v);
            //cout << "in View struct left is: " << v.left << "\n";
        }
        else {
            //a view struct already exists so push into it
            view[0].left = stof(words); // In all cases there will only be one View struct
            //cout << "in View struct left is: " << view[0].left << "\n";
        }
    }
    else if (strcmp(words, "RIGHT") == 0) {
        //cout << "\nin right\n";
        //Get the "right" parameter
        words = strtok_s(NULL, " ", &next_word);
        if (view.size() == 0) { //the vector of View structs is empty
            //no view struct created yet
            View v = View();
            v.right = stof(words);
            view.push_back(v);
            //cout << "in View struct right is: " << v.right << "\n";
        }
        else {
            //a view struct already exists so push into it
            view[0].right = stof(words);
            //cout << "in View struct right is: " << view[0].right << "\n";
        }
    }
    else if (strcmp(words, "BOTTOM") == 0) {
        //cout << "\nin bottom\n";
        //Get the near parameter
        words = strtok_s(NULL, " ", &next_word);
        if (view.size() == 0) { // the vector of View structs is empty
            //no view struct created yet
            View v = View();
            v.bottom = stof(words);
            view.push_back(v);
            //cout << "in View struct bottom is: " << v.bottom << "\n";
        }
        else {
            //a view struct already exists so push into it
            view[0].bottom = stof(words);
            //cout << "in View struct bottom is: " << view[0].bottom << "\n";
        }
    }
    else if (strcmp(words, "TOP") == 0) {
        //cout << "\nin top\n";
        //Get the near parameter
        words = strtok_s(NULL, " ", &next_word);
        if (view.size() == 0) { // No vector of View structs created yet
            //no view struct created yet
            View v = View();
            v.top = stof(words);
            view.push_back(v);
            //cout << "in View struct top is: " << v.top << "\n";
        }
        else {
            //a view struct already exists so push into it
            view[0].top = stof(words);
            //cout << "in View struct view is: " << view[0].top << "\n";
        }
    }
    else if (strcmp(words, "RES") == 0) {
        //cout << "\nin res\n";
        //Get the res x parameter
        words = strtok_s(NULL, " ", &next_word);
        if (view.size() == 0) { // No vector of View structs exists yet
            //no view struct created yet
            View v = View();
            v.resolution.x = stof(words);
            view.push_back(v);
            //cout << "in View struct Res x is: " << v.resolution.x << "\n";
        }
        else {
            //a view struct already exists so push into it
            view[0].resolution.x = stof(words);
            //cout << "in View struct Res x is: " << view[0].resolution.x << "\n";
        }

        //Get the res y parameter
        words = strtok_s(NULL, " ", &next_word);
        view[0].resolution.y = stof(words);
        //cout << "in View struct Res y is: " << view[0].resolution.y << "\n";
    }
    else if (strcmp(words, "SPHERE") == 0) {
        //cout << "\nin sphere\n";
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
        ////cout << "the spec exp is: " << spheres[spheres.size() - 1].spec_exp;


        //cout << "in Sphere struct there is the following sphere: \n";
        //cout << "sphere:\n" << spheres[spheres.size() - 1].name << "\n"
          /*  << spheres[spheres.size() - 1].position.x << " " << spheres[spheres.size() - 1].position.y
            << " " << spheres[spheres.size() - 1].position.z << " " << spheres[spheres.size() - 1].scaler.x
            << " " << spheres[spheres.size() - 1].scaler.y << " " << spheres[spheres.size() - 1].scaler.z
            << " " << spheres[spheres.size() - 1].color.x << " " << spheres[spheres.size() - 1].color.y
            << " " << spheres[spheres.size() - 1].color.z << " " << spheres[spheres.size() - 1].ka
            << " " <*///< spheres[spheres.size() - 1].kd << " " << spheres[spheres.size() - 1].ks << " " << spheres[spheres.size() - 1].kr << " " << spheres[spheres.size() - 1].spec_exp << "\n";

    }
    else if (strcmp(words, "LIGHT") == 0) {
        //cout << "\nin light\n";
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

        //cout << "the background_info vector contains the following Background struct with color: \n";
        //cout << "background color is:\n" << background_info[background_info.size() - 1].color.x << " " << background_info[background_info.size() - 1].color.y
            //<< " " << background_info[background_info.size() - 1].color.z << "\n";

    }
    else if (strcmp(words, "AMBIENT") == 0) {
        //cout << "\nin ambient\n";
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

        //cout << "the background_info vector contains the following Background struct with ambient: \n";
        //cout << background_info[background_info.size() - 1].ambient.x << " " << background_info[background_info.size() - 1].ambient.y
            //<< " " << background_info[background_info.size() - 1].ambient.z << "\n";
    }
    else {
        //cout << "\nin output\n";
        //Get the output parameter:
        words = strtok_s(NULL, " ", &next_word);
        //There is only one output so the output vector of Output structs should be empty
        Output out = Output();
        out.name = words;
        output_name.push_back(out);
        //cout << "the output_name vector contains the following Output struct with the name: "
            //<< output_name[0].name << "\n";

    }
}