//
//  main.cpp
//  3D Object Drawing
//
//  Created by Nazirul Hasan on 4/9/23.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "basic_camera.h"
#include "pointLight.h"
#include "cube.h"
#include "stb_image.h"
#include <iostream>
#include <chrono>   // for std::chrono
#include <cstdint>  // for uint32_t
#include <cstdlib>  // for rand

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax);

// this is for new objects

void roadDraw(Cube& road, Cube& footPath, Shader& lightingShaderWithTexture, glm::mat4 model);
void buildingDraw(Cube* building, Shader& lightingShaderWithTexture, glm::mat4 alTogether);
void lampDraw(Cube& lamp, Shader& lightingShaderWithTexture, glm::mat4 alTogether);
void bikeDraw(unsigned int& circleVAO, unsigned int& toolVAO, unsigned int& bikeCircleVAO, unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 alTogether);
void cubeMoving(unsigned int& circleVAO, Shader& lightingShader, glm::mat4 alTogether);
void personDraw(unsigned int& circleVAO, unsigned int& toolVAO, unsigned int& bikeCircleVAO, unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 alTogether);

uint32_t GetMillis() {
    using namespace std::chrono;
    return static_cast<uint32_t>(duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()).count());
}

int randomNumber(int l, int r) {
    r++;
    return rand() % (r - l) + l;
}

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

// modelling transform
float rotateAngle_X = 0.0;
float rotateAngle_Y = 0.0;
float rotateAngle_Z = 0.0;
float rotateAxis_X = 0.0;
float rotateAxis_Y = 0.0;
float rotateAxis_Z = 1.0;
float translate_X = 0.0;
float translate_Y = 0.0;
float translate_Z = 0.0;
float scale_X = 1.0;
float scale_Y = 1.0;
float scale_Z = 1.0;
float bikeSpeed = 0.0f;
float bikeTranslate_x = 0.0f;
float bikeTranslate_y = 0.0f;
float bikeTranslate_z = 0.0f;
float intoCurrent_z = 0.0f;


// camera
Camera camera(glm::vec3(0.0f, -0.2f, 5.2f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float eyeX = 0.0, eyeY = 1.0, eyeZ = 3.0;
float lookAtX = 0.0, lookAtY = 0.0, lookAtZ = 0.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);

// lights
// positions of the point lights
glm::vec3 pointLightPositions[] = {
    glm::vec3(0.0f,  4.50f,  -4.0f), /// Spot
    glm::vec3(-0.5f,  1.5f,  -5.0f),   /// Point
    glm::vec3(0.0f,  11.5f,  0.5f), /// Sun
    //glm::vec3(-1.5f,  -1.5f,  0.0f)
};
PointLight pointlight1(

    pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z,  // position
    0.5f, 0.5f, 0.5f,      // ambient
    0.5f, 0.5f, 0.5f,       // diffuse
    0.5f, 0.5f, 0.5f,       // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    1       // light number
);
PointLight pointlight2(

    pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z,  // position
    0.5f, 0.5f, 0.5f,      // ambient
    0.5f, 0.5f, 0.5f,       // diffuse
    0.5f, 0.5f, 0.5f,       // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    2       // light number
);


PointLight pointlight3(

    pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z,  // position
    0.5f, 0.5f, 0.5f,     // ambient
    0.5f, 0.5f, 0.5f,      // diffuse
    0.5f, 0.5f, 0.5f,          // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    100       // light number
);


// light settings
bool onOffPointToggle = true;
bool onOffSpotToggle = true;
bool onOffDirectToggle = true;
bool ambientToggle = true;
bool diffuseToggle = true;
bool specularToggle = true;
bool pointLightOn = true;
bool personRide = true;
bool IntroWaling = true;

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

unsigned int cubeVAO, cubeVBO, cubeEBO;
unsigned int cVAO, cVBO, cEBO;

void drawCube(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f), float r = 1.0f, float g = 1.0f, float b = 1.0f)
{
    lightingShader.use();

    lightingShader.setVec3("material.ambient", glm::vec3(r, g, b));
    lightingShader.setVec3("material.diffuse", glm::vec3(r, g, b));
    lightingShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    lightingShader.setFloat("material.shininess", 32.0f);

    lightingShader.setMat4("model", model);

    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
// drawing cylinder
void drawCylinder(unsigned int& cVAO, Shader& lightingShader, glm::mat4 model, float r, float g, float b)
{
    lightingShader.use(); 

    lightingShader.setVec3("material.ambient", glm::vec3(r, g, b));
    lightingShader.setVec3("material.diffuse", glm::vec3(r, g, b));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 60.0f);

    lightingShader.setMat4("model", model);

    glBindVertexArray(cVAO);
    glDrawElements(GL_TRIANGLES, 120, GL_UNSIGNED_INT, 0);
}

// drawing circle and cylinder
// drawing cylinder
void drawCircle(unsigned int& circleVAO, unsigned int& toolVAO, Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f), bool type  =true, float r = 1.0f, float g = 1.0f, float b = 1.0f )
{
    lightingShader.use();

    lightingShader.setVec3("material.ambient", glm::vec3(r, g, b));
    lightingShader.setVec3("material.diffuse", glm::vec3(r, g, b));
    lightingShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    lightingShader.setFloat("material.shininess", 32.0f);

    lightingShader.setMat4("model", model);

    
    if (type) {
        glBindVertexArray(circleVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 199);

        glBindVertexArray(toolVAO);
        //glDrawElements(GL_TRIANGLE_STRIP, 402, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 402);
       
    }
    else {
        glBindVertexArray(toolVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 402);
    }
    

    //for circle use this line
      
}
void drawBikeCircle(unsigned int& bikeCircleVAO, Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f), float r = 1.0f, float g = 1.0f, float b = 1.0f)
{
    lightingShader.use();

    lightingShader.setVec3("material.ambient", glm::vec3(r, g, b));
    lightingShader.setVec3("material.diffuse", glm::vec3(r, g, b));
    lightingShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    lightingShader.setFloat("material.shininess", 32.0f);

    lightingShader.setMat4("model", model);


    
     glBindVertexArray(bikeCircleVAO);
     glDrawArrays(GL_TRIANGLE_STRIP, 0, 402);


    //for circle use this line

}

void circlePointCreation(float* circle1, float* sideOfTool, float height)
{
    const float pi = 3.14159265359;
    const int numPoints = 200;
    const float radius = 0.5;

    for (int i = 0; i < numPoints; ++i) {
        float theta = 2.0 * pi * i / numPoints;
        float z = radius * std::cos(theta);
        float x = radius * std::sin(theta);

        // Calculate the z-coordinate to stay on the sphere's surface
        float y = 0.0;  // Assuming the sphere is centered at (0, 0, 0)

        circle1[i * 3 + 0] = x * 1.0f;
        circle1[i * 3 + 1] = y * 1.0f;
        circle1[i * 3 + 2] = z * 1.0f;

        sideOfTool[i * 6 + 0] = x;
        sideOfTool[i * 6 + 1] = y * 1.0f;
        sideOfTool[i * 6 + 2] = z * 1.0f;

        sideOfTool[i * 6 + 3] = x;
        sideOfTool[i * 6 + 4] = y * 1.0f - height;
        sideOfTool[i * 6 + 5] = z * 1.0f;

        //std::cout << circle1[i*6 + 0] << " " << circle1[i*6 + 1] << " " << circle1[i*6 + 2] << " " << circle1[i*6 + 3] << endl;
    }

    sideOfTool[200 * 6] = sideOfTool[0];
    sideOfTool[200 * 6 + 1] = sideOfTool[1];
    sideOfTool[200 * 6 + 2] = sideOfTool[2];
    sideOfTool[200 * 6 + 3] = sideOfTool[3];
    sideOfTool[200 * 6 + 4] = sideOfTool[4];
    sideOfTool[200 * 6 + 5] = sideOfTool[5];

    sideOfTool[201 * 6] = sideOfTool[6];
    sideOfTool[201 * 6 + 1] = sideOfTool[7];
    sideOfTool[201 * 6 + 2] = sideOfTool[8];
    sideOfTool[201 * 6 + 3] = sideOfTool[9];
    sideOfTool[201 * 6 + 4] = sideOfTool[10];
    sideOfTool[201 * 6 + 5] = sideOfTool[11];

}
void bikecirclePointCreation(float* circle1)
{
    const float pi = 3.14159265359;
    const int numPoints = 200;
    const float radius = 0.5;

    for (int i = 0; i < numPoints; ++i) {
        float theta = 2.0 * pi * i / numPoints;
        float z = radius * std::cos(theta);
        float x = radius * std::sin(theta);

        // Calculate the z-coordinate to stay on the sphere's surface
        float y = 0.0;  // Assuming the sphere is centered at (0, 0, 0)

        circle1[i * 6 + 0] = x;
        circle1[i * 6 + 1] = y * 1.0f;
        circle1[i * 6 + 2] = z * 1.0f;


        z = (radius * .6f) * std::cos(theta);
        x = (radius * .6f) * std::sin(theta);

        circle1[i * 6 + 3] = x;
        circle1[i * 6 + 4] = y * 1.0f;
        circle1[i * 6 + 5] = z * 1.0f;

        //std::cout << circle1[i*6 + 0] << " " << circle1[i*6 + 1] << " " << circle1[i*6 + 2] << " " << circle1[i*6 + 3] << endl;
    }

    circle1[200 * 6] = circle1[0];
    circle1[200 * 6 + 1] = circle1[1];
    circle1[200 * 6 + 2] = circle1[2];
    circle1[200 * 6 + 3] = circle1[3];
    circle1[200 * 6 + 4] = circle1[4];
    circle1[200 * 6 + 5] = circle1[5];

    circle1[201 * 6] = circle1[6];
    circle1[201 * 6 + 1] = circle1[7];
    circle1[201 * 6 + 2] = circle1[8];
    circle1[201 * 6 + 3] = circle1[9];
    circle1[201 * 6 + 4] = circle1[10];
    circle1[201 * 6 + 5] = circle1[11];

}

class Curve
{
public:
    vector<float> cntrlPoints;
    vector <float> coordinates;
    vector <float> normals;
    vector <int> indices;
    vector <float> vertices;
    const double pi = 3.14159265389;
    const int nt = 40;
    const int ntheta = 20;
    Curve(vector<float>& tmp)
    {
        this->cntrlPoints = tmp;
        this->fishVAO = hollowBezier(cntrlPoints.data(), ((unsigned int)cntrlPoints.size() / 3) - 1);
        /*cout << cntrlPoints.size() << endl;
        cout << coordinates.size() << endl;
        cout << normals.size() << endl;
        cout << indices.size() << endl;
        cout << vertices.size() << endl;*/
    }
    ~Curve()
    {
        glDeleteVertexArrays(1, &fishVAO);
        glDeleteVertexArrays(1, &bezierVAO);
        glDeleteBuffers(1, &bezierVBO);
        glDeleteBuffers(1, &bezierEBO);
    }
    void draw(Shader& lightingShader, glm::mat4 model)
    {
        /// Fish
        lightingShader.use();
        lightingShader.setMat4("model", model);
        lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.0f));
        lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.0f));
        lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        lightingShader.setFloat("material.shininess", 32.0f);

        glBindVertexArray(fishVAO);
        glDrawElements(GL_TRIANGLES,                    // primitive type
            (unsigned int)indices.size(),          // # of indices
            GL_UNSIGNED_INT,                 // data type
            (void*)0);                       // offset to indices

        // unbind VAO
        glBindVertexArray(0);
        /// End Fish
    }
private:
    unsigned int fishVAO;
    unsigned int bezierVAO;
    unsigned int bezierVBO;
    unsigned int bezierEBO;


    unsigned int drawControlPoints()
    {
        unsigned int controlPointVAO;
        unsigned int controlPointVBO;

        glGenVertexArrays(1, &controlPointVAO);
        glGenBuffers(1, &controlPointVBO);

        glBindVertexArray(controlPointVAO);

        glBindBuffer(GL_ARRAY_BUFFER, controlPointVBO);
        glBufferData(GL_ARRAY_BUFFER, (unsigned int)cntrlPoints.size() * sizeof(float), cntrlPoints.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        return controlPointVAO;
    }

    long long nCr(int n, int r)
    {
        if (r > n / 2)
            r = n - r; // because C(n, r) == C(n, n - r)
        long long ans = 1;
        int i;

        for (i = 1; i <= r; i++)
        {
            ans *= n - r + i;
            ans /= i;
        }

        return ans;
    }
    void BezierCurve(double t, float xy[2], GLfloat ctrlpoints[], int L)
    {
        double y = 0;
        double x = 0;
        t = t > 1.0 ? 1.0 : t;
        for (int i = 0; i < L + 1; i++)
        {
            long long ncr = nCr(L, i);
            double oneMinusTpow = pow(1 - t, double(L - i));
            double tPow = pow(t, double(i));
            double coef = oneMinusTpow * tPow * ncr;
            x += coef * ctrlpoints[i * 3];
            y += coef * ctrlpoints[(i * 3) + 1];

        }
        xy[0] = float(x);
        xy[1] = float(y);
    }
    unsigned int hollowBezier(GLfloat ctrlpoints[], int L)
    {
        int i, j;
        float x, y, z, r;                //current coordinates
        float theta;
        float nx, ny, nz, lengthInv;    // vertex normal


        const float dtheta = 2 * pi / ntheta;        //angular step size

        float t = 0;
        float dt = 1.0 / nt;
        float xy[2];

        for (i = 0; i <= nt; ++i)              //step through y
        {
            BezierCurve(t, xy, ctrlpoints, L);
            r = xy[0];
            y = xy[1];
            theta = 0;
            t += dt;
            lengthInv = 1.0 / r;

            for (j = 0; j <= ntheta; ++j)
            {
                double cosa = cos(theta);
                double sina = sin(theta);
                z = r * cosa;
                x = r * sina;

                coordinates.push_back(x);
                coordinates.push_back(y);
                coordinates.push_back(z);

                // normalized vertex normal (nx, ny, nz)
                // center point of the circle (0,y,0)
                nx = (x - 0) * lengthInv;
                ny = (y - y) * lengthInv;
                nz = (z - 0) * lengthInv;

                normals.push_back(nx);
                normals.push_back(ny);
                normals.push_back(nz);

                theta += dtheta;
            }
        }
        // generate index list of triangles
        // k1--k1+1
        // |  / |
        // | /  |
        // k2--k2+1

        int k1, k2;
        for (int i = 0; i < nt; ++i)
        {
            k1 = i * (ntheta + 1);     // beginning of current stack
            k2 = k1 + ntheta + 1;      // beginning of next stack

            for (int j = 0; j < ntheta; ++j, ++k1, ++k2)
            {
                // k1 => k2 => k1+1
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);

                // k1+1 => k2 => k2+1
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }

        size_t count = coordinates.size();
        for (int i = 0; i < count; i += 3)
        {
            //cout << count << ' ' << i + 2 << endl;
            vertices.push_back(coordinates[i]);
            vertices.push_back(coordinates[i + 1]);
            vertices.push_back(coordinates[i + 2]);

            vertices.push_back(normals[i]);
            vertices.push_back(normals[i + 1]);
            vertices.push_back(normals[i + 2]);
        }

        glGenVertexArrays(1, &bezierVAO);
        glBindVertexArray(bezierVAO);

        // create VBO to copy vertex data to VBO
        glGenBuffers(1, &bezierVBO);
        glBindBuffer(GL_ARRAY_BUFFER, bezierVBO);           // for vertex data
        glBufferData(GL_ARRAY_BUFFER,                   // target
            (unsigned int)vertices.size() * sizeof(float), // data size, # of bytes
            vertices.data(),   // ptr to vertex data
            GL_STATIC_DRAW);                   // usage

        // create EBO to copy index data
        glGenBuffers(1, &bezierEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bezierEBO);   // for index data
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // target
            (unsigned int)indices.size() * sizeof(unsigned int),             // data size, # of bytes
            indices.data(),               // ptr to index data
            GL_STATIC_DRAW);                   // usage

        // activate attrib arrays
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        // set attrib arrays with stride and offset
        int stride = 24;     // should be 24 bytes
        glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (void*)(sizeof(float) * 3));

        // unbind VAO, VBO and EBO
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        return bezierVAO;
    }

};

vector < float > lampPoint = {
-0.1230, 2.1222, 5.1000,
-0.1581, 1.9845, 5.1000,
-0.1933, 1.8726, 5.1000,
-0.2460, 1.7047, 5.1000,
};
Curve* lampLight;

int ind = 0;

struct buildingPro {
    float st;
    float z;
    float height;
    int idx;
};

vector < buildingPro > buildingPoints1 , buildingPoints2;
void setBuildingPoints()
{
    float st = 0.0f;
    while (st<600.0f)
    {
        float z = randomNumber(4, 5);
        st += z + 0.5f;
        
        int idx = randomNumber(0, 3);
        buildingPro tmp;
        tmp.st = st;
        tmp.z = z;
        tmp.height = randomNumber(5, 9);
        tmp.idx = idx;
        buildingPoints1.push_back(tmp);
        
    }

  /*  for (auto i : buildingPoints1) {
        cout << i.second.first << " ";
    }*/

    st = 0.0f;
    while (st < 600.0f)
    {
        float z = randomNumber(4, 5);
        st += z + 0.5f;

        int idx = randomNumber(0, 3);
        buildingPro tmp;
        tmp.st = st;
        tmp.z = z;
        tmp.height = randomNumber(5, 9);
        tmp.idx = idx;
        buildingPoints2.push_back(tmp);

    }
}
int main()
{

    srand(GetMillis());

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CSE 4208: Computer Graphics Laboratory", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------

    const int parts = 50;
    const float pi = 3.1415926535;
    const float angle = pi * 2.0f / parts;
    float points[200000]{}, radius = 1.0f;

    points[ind++] = 0.0f;
    points[ind++] = 0.0f;
    points[ind++] = 0.0f;
    for (int i = 1; i <= parts; i++) {
        points[ind++] = radius * sin(angle * i);
        points[ind++] = -radius * cos(angle * i);
        points[ind++] = 0.0f;
    }

    for (float r = radius - 0.005f, z = 0.005f; r > 0.0f; r -= 0.005f, z += 0.005f)
    {
        for (int i = 1; i <= parts + 1; i++) {
            points[ind++] = (r + 0.005) * sin(angle * i);
            points[ind++] = -(r + 0.005) * cos(angle * i);
            points[ind++] = z - 0.005f;

            points[ind++] = r * sin(angle * i);
            points[ind++] = -r * cos(angle * i);
            points[ind++] = z;
        }
    }
    for (float r = radius - 0.005f, z = -0.005f; r > 0.0f; r -= 0.005f, z -= 0.005f)
    {
        for (int i = 1; i <= parts + 1; i++) {
            points[ind++] = (r + 0.005) * sin(angle * i);
            points[ind++] = -(r + 0.005) * cos(angle * i);
            points[ind++] = z + 0.005f;

            points[ind++] = r * sin(angle * i);
            points[ind++] = -r * cos(angle * i);
            points[ind++] = z;
        }
    }

    /// Sphere
    unsigned int VBOCL, shpareVAO;
    glGenVertexArrays(1, &shpareVAO);
    glGenBuffers(1, &VBOCL);
    glBindVertexArray(shpareVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBOCL);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    //cyllinder

    float ver_arr[] = {

    1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    0.809017f, 1.0f, 0.587785f, 0.809017f, 1.0f, 0.587785f,
    0.309017f, 1.0f, 0.951057f, 0.309017f, 1.0f, 0.951057f,
    -0.309017f, 1.0f, 0.951057f, -0.309017f, 1.0f, 0.951057f,
    -0.809017f, 1.0f, 0.587785f, -0.809017f, 1.0f, 0.587785f,
    -1.0f, 1.0f, 1.22465e-16f, -1.0f, 1.0f, 1.22465e-16f,
    -0.809017f, 1.0f, -0.587785f, -0.809017f, 1.0f, -0.587785f,
    -0.309017f, 1.0f, -0.951057f, -0.309017f, 1.0f, -0.951057f,
    0.309017f, 1.0f, -0.951057f, 0.309017f, 1.0f, -0.951057f,
    0.809017f, 1.0f, -0.587785f, 0.809017f, 1.0f, -0.587785f,

    1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f,
    0.809017f, -1.0f, 0.587785f, 0.809017f, -1.0f, 0.587785f,
    0.309017f, -1.0f, 0.951057f, 0.309017f, -1.0f, 0.951057f,
    -0.309017f, -1.0f, 0.951057f, -0.309017f, -1.0f, 0.951057f,
    -0.809017f, -1.0f, 0.587785f, -0.809017f, -1.0f, 0.587785f,
    -1.0f, -1.0f, 1.22465e-16f, -1.0f, -1.0f, 1.22465e-16f,
    -0.809017f, -1.0f, -0.587785f, -0.809017f, -1.0f, -0.587785f,
    -0.309017f, -1.0f, -0.951057f, -0.309017f, -1.0f, -0.951057f,
    0.309017f, -1.0f, -0.951057f, 0.309017f, -1.0f, -0.951057f,
    0.809017f, -1.0f, -0.587785f, 0.809017f, -1.0f, -0.587785f,


    1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f,
    0.809017f, -1.0f, 0.587785f, 0.809017f, -1.0f, 0.587785f,
    0.309017f, -1.0f, 0.951057f, 0.309017f, -1.0f, 0.951057f,
    -0.309017f, -1.0f, 0.951057f, -0.309017f, -1.0f, 0.951057f,
    -0.809017f, -1.0f, 0.587785f, -0.809017f, -1.0f, 0.587785f,
    -1.0f, -1.0f, 1.22465e-16f, -1.0f, -1.0f, 1.22465e-16f,
    -0.809017f, -1.0f, -0.587785f, -0.809017f, -1.0f, -0.587785f,
    -0.309017f, -1.0f, -0.951057f, -0.309017f, -1.0f, -0.951057f,
    0.309017f, -1.0f, -0.951057f, 0.309017f, -1.0f, -0.951057f,
    0.809017f, -1.0f, -0.587785f, 0.809017f, -1.0f, -0.587785f,

    1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    0.809017f, 1.0f, 0.587785f, 0.809017f, 1.0f, 0.587785f,
    0.309017f, 1.0f, 0.951057f, 0.309017f, 1.0f, 0.951057f,
    -0.309017f, 1.0f, 0.951057f, -0.309017f, 1.0f, 0.951057f,
    -0.809017f, 1.0f, 0.587785f, -0.809017f, 1.0f, 0.587785f,
    -1.0f, 1.0f, 1.22465e-16f, -1.0f, 1.0f, 1.22465e-16f,
    -0.809017f, 1.0f, -0.587785f, -0.809017f, 1.0f, -0.587785f,
    -0.309017f, 1.0f, -0.951057f, -0.309017f, 1.0f, -0.951057f,
    0.309017f, 1.0f, -0.951057f, 0.309017f, 1.0f, -0.951057f,
    0.809017f, 1.0f, -0.587785f, 0.809017f, 1.0f, -0.587785f,


    0.0,-1.0,0.0, 0.0,-1.0,0.0,
    0.0,1.0,0.0, 0.0,1.0,0.0

    };

    unsigned int ind_arr[] = {
        0, 11, 1,
        11, 0, 10,
        1, 12, 2,
        12, 1, 11,
        2, 13, 3,
        13, 2, 12,
        3, 14, 4,
        14, 3, 13,
        4, 15, 5,
        15, 4, 14,
        5, 16, 6,
        16, 5, 15,
        6, 17, 7,
        17, 6, 16,
        7, 18, 8,
        18, 7, 17,
        8, 19, 9,
        19, 8, 18,
        9, 10, 0,
        10, 9, 19,

        40,20,21,
        40,21,22,
        40,22,23,
        40,23,24,
        40,24,25,
        40,25,26,
        40,26,27,
        40,27,28,
        40,28,29,
        40,29,20,

        41,30,31,
        41,31,32,
        41,32,33,
        41,33,34,
        41,34,35,
        41,35,36,
        41,36,37,
        41,37,38,
        41,38,39,
        41,39,30


        /*
        21,10,11,
        21,11,12,
        21,12,13,
        21,13,14,
        21,14,15,
        21,15,16,
        21,16,17,
        21,17,18,
        21,18,19,
        21,19,10*/

    };

    glGenVertexArrays(1, &cVAO);
    glGenBuffers(1, &cVBO);
    glGenBuffers(1, &cEBO);

    glBindVertexArray(cVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ver_arr), ver_arr, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ind_arr), ind_arr, GL_STATIC_DRAW);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);
    //end of cylingder

    float cube_vertices[] = {
        // positions      // normals
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f
    };
    unsigned int cube_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // ------------------------------------------   this part is for circle -----------------------
    float circle1[200 * 3];
    float sideOfTool[204 * 6];
    float bikeCirclePoints[204 * 6];
    float height = 0.2;

    circlePointCreation(circle1, sideOfTool, height);
    bikecirclePointCreation(bikeCirclePoints);
    //

    //circle1
    unsigned int circleVBO, circleVAO;
    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &circleVBO);
    glBindVertexArray(circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle1), circle1, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
   
    //circle part 2
    unsigned int toolVBO, toolVAO;
    glGenVertexArrays(1, &toolVAO);
    glGenBuffers(1, &toolVBO);
    glBindVertexArray(toolVAO);
    glBindBuffer(GL_ARRAY_BUFFER, toolVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sideOfTool), sideOfTool, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int bikeCircleVBO, bikeCircleVAO;
    glGenVertexArrays(1, &bikeCircleVAO);
    glGenBuffers(1, &bikeCircleVBO);
    glBindVertexArray(bikeCircleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, bikeCircleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bikeCirclePoints), bikeCirclePoints, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // ---------------------------------- circle creating end here ----------------------------------- 
    
    Shader lightingShaderWithTexture("vertexShaderForPhongShadingWithTexture.vs", "fragmentShaderForPhongShadingWithTexture.fs");
    Shader lightingShader("vertexShaderForPhongShading.vs", "fragmentShaderForPhongShading.fs");
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");

    // object textureing part

    // this is for road
    string diffuseMapPathroad = "Texture_img/road.png";
    string specularMapPathroad = "Texture_img/road.png";
    unsigned int diffMaproad = loadTexture(diffuseMapPathroad.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMaproad = loadTexture(specularMapPathroad.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube road = Cube(diffMaproad, specMaproad, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    // this is for footpath
    diffuseMapPathroad = "Texture_img/footpath.png";
    specularMapPathroad = "Texture_img/footpath.png";
    unsigned int diffMapFoot = loadTexture(diffuseMapPathroad.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMapFoot = loadTexture(specularMapPathroad.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube footPath = Cube(diffMapFoot, specMapFoot, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);


    // this is for building
    Cube building[4];
    diffuseMapPathroad = "Texture_img/building1.png";
    specularMapPathroad = "Texture_img/building1.png";
    unsigned int diffMapBuilding1 = loadTexture(diffuseMapPathroad.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMapBuilding1 = loadTexture(specularMapPathroad.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    building[0] = Cube(diffMapBuilding1, specMapBuilding1, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    diffuseMapPathroad = "Texture_img/building2.png";
    specularMapPathroad = "Texture_img/building2.png";
    unsigned int diffMapBuilding2 = loadTexture(diffuseMapPathroad.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMapBuilding2 = loadTexture(specularMapPathroad.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    building[1] = Cube(diffMapBuilding2, specMapBuilding2, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    diffuseMapPathroad = "Texture_img/building3.png";
    specularMapPathroad = "Texture_img/building3.png";
    unsigned int diffMapBuilding3 = loadTexture(diffuseMapPathroad.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMapBuilding3 = loadTexture(specularMapPathroad.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    building[2] = Cube(diffMapBuilding3, specMapBuilding3, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    diffuseMapPathroad = "Texture_img/building4.png";
    specularMapPathroad = "Texture_img/building4.png";
    unsigned int diffMapBuilding4 = loadTexture(diffuseMapPathroad.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMapBuilding4 = loadTexture(specularMapPathroad.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    building[3] = Cube(diffMapBuilding4, specMapBuilding4, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    setBuildingPoints();
    //return 0;

    // this section is for road lamp
    diffuseMapPathroad = "Texture_img/lamp.png";
    specularMapPathroad = "Texture_img/lamp.png";
    unsigned int diffMaplamp = loadTexture(diffuseMapPathroad.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMaplamp = loadTexture(specularMapPathroad.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube lamp = Cube(diffMaplamp, specMaplamp, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    Curve lampShape(lampPoint);
    lampLight = &lampShape;
    //Sphere sphere = Sphere();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.529f, 0.8078f, 0.9215f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShaderWithTexture.use();
        lightingShaderWithTexture.setVec3("viewPos", camera.Position);

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
        lightingShaderWithTexture.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        //glm::mat4 view = basic_camera.createViewMatrix();
        lightingShaderWithTexture.setMat4("view", view);

        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // Modelling Transformation
        glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

        lightingShaderWithTexture.use();
        // point light 1
        pointlight1.setUpPointLight(lightingShaderWithTexture);
        // point light 2
        pointlight2.setUpPointLight(lightingShaderWithTexture);
        // point light 3
        pointlight3.setUpPointLight(lightingShaderWithTexture);
       
        roadDraw(road, footPath, lightingShaderWithTexture, model);
        buildingDraw(building, lightingShaderWithTexture, model);
        lampDraw(lamp, lightingShaderWithTexture, model);
        

        // --------------------- this part is for bike --------------------------------------//
        lightingShader.use();
        // point light 1
        //pointlight1.setUpPointLight(lightingShader);
        // point light 2
        pointlight2.setUpPointLight(lightingShader);
        // point light 3
        pointlight3.setUpPointLight(lightingShader);


        if (false) {
            bikeTranslate_x = 0.0; bikeTranslate_y = 0.0; bikeTranslate_z = 0.0f;

            if (intoCurrent_z < -15.0f) {
                camera.ProcessKeyboard(BACKWARD, -intoCurrent_z/2.5);
                IntroWaling = false;
            }
            else {
                camera.ProcessKeyboard(FORWARD, deltaTime);
                intoCurrent_z -= deltaTime * 2.5;

                model = glm::mat4(1.0);
                translateMatrix = glm::translate(model, glm::vec3(-4.0f, -4.0f, -10.0f + intoCurrent_z));
                //cout << bikeTranslate_x << endl;
                rotateXMatrix = glm::rotate(identityMatrix, glm::radians(00.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                rotateYMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0, 2.0, 2.0));
                model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

                personDraw(circleVAO, toolVAO, bikeCircleVAO, cubeVAO, lightingShader, model);

            }

        }
        else {
            model = glm::mat4(1.0);
            translateMatrix = glm::translate(model, glm::vec3(bikeTranslate_x - 3.0f, bikeTranslate_y - 1.5f, bikeTranslate_z - 3.0f));
            //cout << bikeTranslate_x << endl;
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(00.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5, 1.5, 1.5));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

            //cubeMoving(cubeVAO, lightingShader, model);
            bikeDraw(circleVAO, toolVAO, bikeCircleVAO, cubeVAO, lightingShader, view * model);
        }
        
        

        //drawCube(cubeVAO, lightingShader, model);
        /*drawCircle(circleVAO , toolVAO, lightingShader, model , true);
        drawBikeCircle(bikeCircleVAO, lightingShader, model);*/

        // also draw the lamp object(s)
        //ourShader.use();
        //ourShader.setMat4("projection", projection);
        //ourShader.setMat4("view", view);

        //// we now draw as many light bulbs as we have point lights.
        //for (unsigned int i = 0; i < 3; i++)
        //{
        //    model = glm::mat4(1.0f);
        //    model = glm::translate(model, pointLightPositions[i]);
        //    model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
        //    //cube.drawCube(ourShader, model, 0.8f, 0.8f, 0.8f);
        //}

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void personDraw(unsigned int& circleVAO, unsigned int& toolVAO, unsigned int& bikeCircleVAO, unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 alTogether)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.53f, 0.17f, 2.06f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.3, 0.1));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.41f, 0.432f, 0.4023f);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.53f, 0.17f, 1.9f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.3, 0.1));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.41f, 0.432f, 0.4023f);

    // ---------------------------------this is the body part

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.52f, 0.63f, 1.98f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.26, 2.3, 0.26));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.21f, 0.22f, 0.23f);

    //  - ---------------------------- this is the leg part -------------------------------

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.53f, -0.09f, 2.06f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.5, 0.1));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.7291f, 0.596f, 0.403f);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.53f, -0.09f, 1.9f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.5, 0.1));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.7291f, 0.596f, 0.403f);

    //------------------------------- this is the hand part
    
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.56f, 0.58f, 1.9f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(40.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 2.0, 0.1));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.7291f, 0.596f, 0.403f);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.56f, 0.58f, 2.05f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(-40.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 2.0, 0.1));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.7291f, 0.596f, 0.403f);
}
// testing part of bike moving
void cubeMoving(unsigned int& circleVAO, Shader& lightingShader, glm::mat4 alTogether)
{
    glm::mat4 modelMatrixForContainer = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 translate = glm::mat4(1.0f);
    glm::mat4 scale = glm::mat4(1.0f);

    translate = glm::translate(model, glm::vec3(bikeTranslate_x, bikeTranslate_y, bikeTranslate_z));
    
    scale = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    modelMatrixForContainer = alTogether * model * scale;

    drawCube(cubeVAO, lightingShader, modelMatrixForContainer);
}

// project object creation part start from here
void roadDraw(Cube &road, Cube &footPath, Shader& lightingShaderWithTexture, glm::mat4 alTogether)
{
    lightingShaderWithTexture.use();
    glm::mat4 modelMatrixForContainer = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 translate = glm::mat4(1.0f);
    glm::mat4 scale = glm::mat4(1.0f);

    float st = 0.0f;
    for (int i = 0; i < 25; i++) {
        translate = glm::translate(model, glm::vec3(-5.0, -5.0, st));
        scale = glm::scale(model, glm::vec3(10, 0.2, -50));
        modelMatrixForContainer = alTogether * model * translate * scale;
        road.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        st -= 50.0f;
    }
    st = 0.0f;
    for (int i = 0; i < 250; i++) {
        translate = glm::translate(model, glm::vec3(-7.0, -5.0, st));
        scale = glm::scale(model, glm::vec3(2, 0.5, -5));
        modelMatrixForContainer = alTogether * model * translate * scale;
        footPath.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        translate = glm::translate(model, glm::vec3(5.0, -5.0, st));
        scale = glm::scale(model, glm::vec3(2, 0.5, -5));
        modelMatrixForContainer = alTogether * model * translate * scale;
        footPath.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);


        st -= 5.0f;
    }
}

void buildingDraw(Cube *building, Shader& lightingShaderWithTexture, glm::mat4 alTogether) {
    lightingShaderWithTexture.use();
    glm::mat4 modelMatrixForContainer = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 translate = glm::mat4(1.0f);
    glm::mat4 scale = glm::mat4(1.0f);

 
    for (auto i : buildingPoints1)
    {
        translate = glm::translate(model, glm::vec3(-12.0, -4.5, -i.st));
        scale = glm::scale(model, glm::vec3(5, i.height, i.z));
        modelMatrixForContainer = alTogether * model * translate * scale;
        building[i.idx].drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        
    }

    for (auto i : buildingPoints2)
    {
        translate = glm::translate(model, glm::vec3(7.0, -4.5, -i.st));
        scale = glm::scale(model, glm::vec3(5, i.height, i.z));
        modelMatrixForContainer = alTogether * model * translate * scale;
        building[i.idx].drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

    }
    
}

void lampDraw(Cube& lamp, Shader& lightingShaderWithTexture, glm::mat4 alTogether)
{
    lightingShaderWithTexture.use();
    glm::mat4 modelMatrixForContainer = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 translate = glm::mat4(1.0f);
    glm::mat4 scale = glm::mat4(1.0f);

    float st = 0.0f;
    for (int i = 0; i < 25; i++) {
        translate = glm::translate(model, glm::vec3(-5.0, -5.0, st));
        scale = glm::scale(model, glm::vec3(0.2, 5, -0.2));
        modelMatrixForContainer = alTogether * model * translate * scale;
        lamp.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        translate = glm::translate(model, glm::vec3(-5.0, 0.0, st));
        scale = glm::scale(model, glm::vec3(1.0, 0.2, -0.2));
        modelMatrixForContainer = alTogether * model * translate * scale;
        lamp.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        translate = glm::translate(model, glm::vec3(-4.0, -2.6, st));
        scale = glm::scale(model, glm::vec3(1.0, 1.3, -0.2));
        modelMatrixForContainer = alTogether * model * translate * scale;
        lampLight->draw(lightingShaderWithTexture, modelMatrixForContainer);

        translate = glm::translate(model, glm::vec3(4.8, -5.0, st));
        scale = glm::scale(model, glm::vec3(0.2, 5, -0.2));
        modelMatrixForContainer = alTogether * model * translate * scale;
        lamp.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        translate = glm::translate(model, glm::vec3(5.0, 0.0, st));
        scale = glm::scale(model, glm::vec3(-1.0, 0.2, -0.2));
        modelMatrixForContainer = alTogether * model * translate * scale;
        lamp.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        translate = glm::translate(model, glm::vec3(4.0, -2.6, st));
        scale = glm::scale(model, glm::vec3(1.0, 1.3, -0.2));
        modelMatrixForContainer = alTogether * model * translate * scale;
        lampLight->draw(lightingShaderWithTexture, modelMatrixForContainer);

        st -= 50.0f;
    }
}

void bikeDraw(unsigned int& circleVAO, unsigned int& toolVAO, unsigned int& bikeCircleVAO, unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 alTogether)
{

    /*glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 translate = glm::mat4(1.0f);
    glm::mat4 translate2 = glm::mat4(1.0f);
    glm::mat4 scale = glm::mat4(1.0f);*/



    /*translate = glm::translate(model, glm::vec3(-translate_X, -translate_Y + 0.2, -translate_Z));
    scale = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    model = translate * scale;*/

    // ----------------------------------- first wheel -------------------------------//
    // first circle
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix , model;

    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5f, -0.2, 2.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 0.3, 0.3));
    model =alTogether *  translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
   
    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.1882f, 0.2f, 0.1921f);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5f, -0.2, 2.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 0.3, 0.3));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

    drawBikeCircle(bikeCircleVAO, lightingShader, model , 0.1882f, 0.2f, 0.1921f);

    // inner circle 
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5f, -0.2, 2.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 0.1, 0.1));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;


    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.1882f, 0.2f, 0.1921f);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5f, -0.2, 2.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 0.1, 0.1));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

    drawBikeCircle(bikeCircleVAO, lightingShader, model, 0.1882f, 0.2f, 0.1921f);
    // first wheel end ----------------------------

    // second wheel -----------------------------------

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.5f, -0.2, 2.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 0.3, 0.3));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.1882f, 0.2f, 0.1921f);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.5f, -0.2, 2.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 0.3, 0.3));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

    drawBikeCircle(bikeCircleVAO, lightingShader, model, 0.1882f, 0.2f, 0.1921f);

    // inner circle 
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.5f, -0.2, 2.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 0.1, 0.1));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;


    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.1882f, 0.2f, 0.1921f);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.5f, -0.2, 2.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 0.1, 0.1));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

    drawBikeCircle(bikeCircleVAO, lightingShader, model, 0.1882f, 0.2f, 0.1921f);

    // -------------------- forward circle green -------------------------------------
    // inner circle 
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.24f, -0.2, 2.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 0.1, 0.1));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;


    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.0f, 0.5019f, 0.0f);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.24f, -0.2, 2.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 0.1, 0.1));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

    drawBikeCircle(bikeCircleVAO, lightingShader, model, 0.0f, 0.5019f, 0.0f);

    // second wheel ------------------------------------------ end
     
    // stand part start from here --------------------------- 

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.2f, 0.5, 2.02f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02, 5.0, 0.02));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;


    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 1.0f, 0.0f, 0.0f);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.2f, 0.5, 1.93f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02, 5.0, 0.02));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 1.0f, 0.0f, 0.0f);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.25f, -0.2, 2.02f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(125.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02, 2.7, 0.02));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;


    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 1.0f, 0.0f, 0.0f);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.25f, -0.2, 1.93f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(125.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02, 2.7, 0.02));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;


    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 1.0f, 0.0f, 0.0f);

    // second wheel red rod

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.52f, -0.2, 1.93f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02, 1.4, 0.02));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 1.0f, 0.0f, 0.0f);

    // second wheel green rods
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.52f, -0.2, 1.93f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(100.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02, 1.4, 0.02));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.0f, 0.5019f, 0.0f);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.52f, -0.2, 1.93f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(80.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02, 1.4, 0.02));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.0f, 0.5019f, 0.0f);

    // last two red rod -------------------------------------------- 

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.48f, 0.14, 1.93f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(35.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02, 2.0, 0.02));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 1.0f, 0.0f, 0.0f);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.52f, -0.2, 1.93f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(145.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02, 2.0, 0.02));
    model = alTogether *  translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 1.0f, 0.0f, 0.0f);

    // red stand ends here -----------------------------------------

    // bike body parts start from here -----------------------------
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, -0.00, 2.06f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.3, 0.1));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;


    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.47f, 0.47f, 0.471f);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, -0.00, 1.88f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.3, 0.1));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;


    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.47f, 0.47f, 0.471f);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.1f, 0.15f, 2.06f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.0, 0.1));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;


    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.47f, 0.47f, 0.471f);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.1f, 0.15f, 1.88f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.0, 0.1));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;


    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.47f, 0.47f, 0.471f);

    // body upper part -------------------------------------
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.3f, 0.0f, 1.95f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.4, 0.1));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;


    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.47f, 0.47f, 0.471f);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.35f, 0.103f, 1.95f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.4, 0.1));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;


    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.40f, 0.40f, 0.30f);

    // last gass pipe
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.72f, -0.2f, 2.06f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.08, 1.2, 0.08));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;


    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.401f, 0.402f, 0.426f);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.72f, -0.2f, 1.90f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.08, 1.2, 0.08));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;


    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.401f, 0.402f, 0.426f);

    // head light
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.15f, 0.45f, 1.95f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 0.4, 0.1));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;


    drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.6f, 0.5f, 0.6f);

    // seat --------------------------------
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.5f, 0.125f, 1.9f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 0.01, 0.1));
    model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

    drawCube(cubeVAO, lightingShader, model, 0.3 , 0.3 , 0.3);


    // ---------------------- this section is for person -------------------------------------
    if (personRide) {
      
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.53f, 0.17f, 2.06f));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.3, 0.1));
        model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

        drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.41f, 0.432f, 0.4023f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.53f, 0.17f, 1.9f));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.3, 0.1));
        model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

        drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.41f, 0.432f, 0.4023f);

        // ---------------------------------this is the body part

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.50f, 0.53f, 1.98f));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.26, 1.7, 0.26));
        model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

        drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.21f, 0.22f, 0.23f);

        //  - ---------------------------- this is the leg part -------------------------------
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.27f, 0.15f, 2.06f));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.8, 0.1));
        model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

        drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.7291f, 0.596f, 0.403f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.27f, 0.15f, 1.9f));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.8, 0.1));
        model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

        drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.7291f, 0.596f, 0.403f);

        //------------------------------- this is the hand part
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.40f, 0.4f, 2.1f));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.3, 0.1));
        model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

        drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.7291f, 0.596f, 0.403f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.40f, 0.4f, 1.85f));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1, 1.3, 0.1));
        model = alTogether * translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

        drawCircle(circleVAO, toolVAO, lightingShader, model, false, 0.7291f, 0.596f, 0.403f);
    }
}


// for curve


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime + .0);
        bikeTranslate_z -= deltaTime*2.5;
        
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime + 0.0);
        bikeTranslate_z += deltaTime*2.5;
        
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        
        bikeTranslate_x -= deltaTime*2.5;
        if (bikeTranslate_x < -4.42) bikeTranslate_x = -4.42;
        else {
            camera.ProcessKeyboard(LEFT, deltaTime);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        
        bikeTranslate_x += deltaTime*2.5;
        if (bikeTranslate_x > 4.6) bikeTranslate_x = 4.6;
        else {
            camera.ProcessKeyboard(RIGHT, deltaTime);
        }
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        if (rotateAxis_X) rotateAngle_X -= 0.1f;
        else if (rotateAxis_Y) rotateAngle_Y -= 0.1f;
        else rotateAngle_Z -= 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) translate_Y += 0.001f;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) translate_Y -= 0.001f;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) translate_X += 0.001f;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) translate_X -= 0.001f;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) translate_Z += 0.001f;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) translate_Z -= 0.001f;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        
        float rotationSpeed = 10.0f;
        // Rotate the camera clockwise
        camera.ProcessMouseMovement(rotationSpeed, 0.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        float rotationSpeed = -10.0f;

        // Rotate the camera clockwise
        camera.ProcessMouseMovement(rotationSpeed, 0.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) scale_Y += 0.001f;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) scale_Y -= 0.001f;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) scale_Z += 0.001f;
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) scale_Z -= 0.001f;

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
       /* rotateAngle_X += 0.1f;
        rotateAxis_X = 1.0f;
        rotateAxis_Y = 0.0f;
        rotateAxis_Z = 0.0f;*/
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    {
        rotateAngle_Y += 0.1f;
        rotateAxis_X = 0.0f;
        rotateAxis_Y = 1.0f;
        rotateAxis_Z = 0.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
     /*   rotateAngle_Z += 0.1f;
        rotateAxis_X = 0.0f;
        rotateAxis_Y = 0.0f;
        rotateAxis_Z = 1.0f;*/
    }

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        eyeX += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        eyeX -= 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        eyeZ += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        eyeZ -= 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        eyeY += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        eyeY -= 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        if (pointLightOn)
        {
            pointlight1.turnOff();
            pointlight2.turnOff();
            pointlight3.turnOff();
           
            pointLightOn = !pointLightOn;
        }
        else
        {
            pointlight1.turnOn();
            pointlight2.turnOn();
            pointlight3.turnOn();
           
            pointLightOn = !pointLightOn;
        }
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrappingModeS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrappingModeT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFilteringModeMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFilteringModeMax);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
