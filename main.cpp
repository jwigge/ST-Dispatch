//
//  main.cpp
//  ST-Dispatch
//
//  Created by Marcel Haupt on 08.03.16.
//  Copyright © 2016 Marcel Haupt. All rights reserved.
//


#include <iostream>
#include <chrono>
#include <cmath>
#include <GLFW/glfw3.h>
#include "networking.hpp"
#include "database.hpp"
#include "client.hpp"


//Constants
const int WIDTH = 300;
const int HEIGHT = 200;
const double FREQUENCY = 50;


//init functions
int initGL();
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void draw();


//global variables
Database *dbp; //used for access to database
GLFWwindow* window;




int main () {
    //init window and opengl
    if (initGL()) return -1;
    std::cout << "Window initialized, starting program" << std::endl;
    
    //init database
    Database db;
    dbp = &db;
    db.initData();
    
    //init clients
    Client testClient(&db, "127.0.0.1", 1111, 1112);
    
    //main loop
    do
    {
        //timer start
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        //std::chrono::high_resolution_clock::time_point wt1 = std::chrono::high_resolution_clock::now();
        
        //drawing GUI
        draw();
        
        //todo: sending messages
        testClient.sendInfo();
        
        //timer stop
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
        
        //sleeping
        double waittime = 1 / FREQUENCY - duration.count();
        if (waittime > 0)
            usleep(floor(waittime*0.9*1000*1000));
        
        //timer stop 2
        //std::chrono::high_resolution_clock::time_point wt2 = std::chrono::high_resolution_clock::now();
        //std::chrono::duration<double> wduration = std::chrono::duration_cast<std::chrono::duration<double>>(wt2 - wt1);
        //std::cout << wduration.count() << std::endl;
        
    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);
    
    glfwTerminate();
    std::cout << "Stutting down" << std::endl;
    
    return 0;
}




void draw() {
    glLoadIdentity();
    
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
    
    //stop
    if (dbp->sim_running.get())
        glColor3f(1.0, 0.0, 0.0);
    else
        glColor3f(0.5, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(0.00, 0.00, 0.0);
    glVertex3f(0.50, 0.00, 0.0);
    glVertex3f(0.50, 0.50, 0.0);
    glVertex3f(0.00, 0.50, 0.0);
    glEnd();
    
    //start
    if (!dbp->sim_running.get())
        glColor3f(0.0, 1.0, 0.0);
    else
        glColor3f(0.0, 0.5, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(0.00, 0.50, 0.0);
    glVertex3f(0.50, 0.50, 0.0);
    glVertex3f(0.50, 1.00, 0.0);
    glVertex3f(0.00, 1.00, 0.0);
    glEnd();
    
    //reset
    if (!dbp->sim_resetted.get())
        glColor3f(1.0, 1.0, 0.0);
    else
        glColor3f(0.5, 0.5, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(0.50, 0.00, 0.0);
    glVertex3f(1.00, 0.00, 0.0);
    glVertex3f(1.00, 1.00, 0.0);
    glVertex3f(0.50, 1.00, 0.0);
    glEnd();

    glFlush();
    
    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
}



int initGL() {
    // Initialise GLFW
    
    if (!glfwInit())
        
    {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }

    
    window = glfwCreateWindow(WIDTH, HEIGHT, "ST-Dispatch", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n";
        glfwTerminate();
        return 2;
    }
    glfwMakeContextCurrent(window);
    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    
    return 0;
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double px, py;
        glfwGetCursorPos(window, &px, &py);
        
        //std::cout << "Cursor Position: " << px << " / " << py << std::endl;
        
        if (px < WIDTH/2 && py < HEIGHT/2 && !dbp->sim_running.get()) {
            std::cout << "starting simulation" << std::endl;
            dbp->sim_running.set(true);
        }
        if (px < WIDTH/2 && py >= HEIGHT/2 && dbp->sim_running.get()) {
            std::cout << "stopping simulation" << std::endl;
            dbp->sim_running.set(false);
        }
        if (px >= WIDTH/2 && !dbp->sim_resetted.get()) {
            std::cout << "resetting simulation" << std::endl;
            dbp->sim_resetted.set(true);
            dbp->sim_running.set(false);
        }
    }
    
}