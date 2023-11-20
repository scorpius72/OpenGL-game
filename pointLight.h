//
//  pointLight.h
//  test
//
//  Created by Nazirul Hasan on 22/9/23.
//

#ifndef pointLight_h
#define pointLight_h
#include<iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"
using namespace std;

class PointLight {
public:
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 spotpos[20]; 
    float k_c;
    float k_l;
    float k_q;
    int lightNumber;

    PointLight() {}

    PointLight(float posX, float posY, float posZ, float ambR, float ambG, float ambB, float diffR, float diffG, float diffB, float specR, float specG, float specB, float constant, float linear, float quadratic, int num, float spX = 0.0f , float spY = 0.0f , float spZ = 0.0f) {

        position = glm::vec3(posX, posY, posZ);
        ambient = glm::vec3(ambR, ambG, ambB);
        diffuse = glm::vec3(diffR, diffG, diffB);
        specular = glm::vec3(specR, specG, specB);
        k_c = constant;
        k_l = linear;
        k_q = quadratic;
        lightNumber = num;
        spotpos[num] = glm::vec3(spX, spY, spZ);
    }
    void setUpPointLight(Shader& lightingShader)
    {
        //spotpos[0] = glm::vec3(0.0, -5.0, -50.0);
        lightingShader.use();
        //std::cout << ambientOn << ' ' << diffuseOn << ' ' << specularOn << std::endl;
        if (lightNumber == 2) {
            lightingShader.setVec3("pointLights[0].position", position);
            lightingShader.setVec3("pointLights[0].ambient", ambientOn * ambient);
            lightingShader.setVec3("pointLights[0].diffuse", diffuseOn * diffuse);
            lightingShader.setVec3("pointLights[0].specular", specularOn * specular);
            lightingShader.setFloat("pointLights[0].k_c", k_c);
            lightingShader.setFloat("pointLights[0].k_l", k_l);
            lightingShader.setFloat("pointLights[0].k_q", k_q);
        }

        else if (lightNumber == 1)
        {
            lightingShader.setVec3("lookatpointofsopt[1]", spotpos[1]);
            lightingShader.setVec3("spotLights[1].position", position);
            lightingShader.setVec3("spotLights[1].ambient", ambientOn * ambient);
            lightingShader.setVec3("spotLights[1].diffuse", diffuseOn * diffuse);
            lightingShader.setVec3("spotLights[1].specular", specularOn * specular);
            lightingShader.setFloat("spotLights[1].k_c", k_c);
            lightingShader.setFloat("spotLights[1].k_l", k_l);
            lightingShader.setFloat("spotLights[1].k_q", k_q);
        }
        else if (lightNumber == 100)
        {
            lightingShader.setVec3("directionLights[0].position", position);
            lightingShader.setVec3("directionLights[0].ambient", ambientOn * ambient);
            lightingShader.setVec3("directionLights[0].diffuse", diffuseOn * diffuse);
            lightingShader.setVec3("directionLights[0].specular", specularOn * specular);
            lightingShader.setFloat("directionLights[0].k_c", k_c);
            lightingShader.setFloat("directionLights[0].k_l", k_l);
            lightingShader.setFloat("directionLights[0].k_q", k_q);
        }
        if (lightNumber >= 4) {
            string lightN = to_string(lightNumber);
            lightingShader.setVec3("lookatpointofsopt[" + lightN + "]", spotpos[lightNumber]);
            lightingShader.setVec3("spotLights[" + lightN + "].position", position);
            lightingShader.setVec3("spotLights[" + lightN + "].ambient", ambientOn * ambient);
            lightingShader.setVec3("spotLights[" + lightN + "].diffuse", diffuseOn * diffuse);
            lightingShader.setVec3("spotLights[" + lightN + "].specular", specularOn * specular);
            lightingShader.setFloat("spotLights[" + lightN + "].k_c", k_c);
            lightingShader.setFloat("spotLights[" + lightN + "].k_l", k_l);
            lightingShader.setFloat("spotLights[" + lightN + "].k_q", k_q);
        }
        /*
        else
        {
            lightingShader.setVec3("pointLights[3].position", position);
            lightingShader.setVec3("pointLights[3].ambient", ambientOn * ambient);
            lightingShader.setVec3("pointLights[3].diffuse", diffuseOn * diffuse);
            lightingShader.setVec3("pointLights[3].specular", specularOn * specular);
            lightingShader.setFloat("pointLights[3].k_c", k_c);
            lightingShader.setFloat("pointLights[3].k_l", k_l);
            lightingShader.setFloat("pointLights[3].k_q", k_q);
        }*/
    }
    bool ison = 1;
    bool isOn()
    {
        return ison;
    }
    void turnOff()
    {
        ison = 0;
        ambientOn = 0.0;
        diffuseOn = 0.0;
        specularOn = 0.0;
    }
    void turnOn()
    {
        ison = 1;
        ambientOn = 1.0;
        diffuseOn = 1.0;
        specularOn = 1.0;
    }
    void turnAmbientOn()
    {
        ambientOn = 1.0;
        //diffuseOn = 0.0;
        //specularOn = 0.0;
    }
    void turnAmbientOff()
    {
        ambientOn = 0.0;
        //diffuseOn = 0.0;
        //specularOn = 0.0;
    }
    void turnDiffuseOn()
    {
        //ambientOn = 0.0;
        diffuseOn = 1.0;
        //specularOn = 0.0;
    }
    void turnDiffuseOff()
    {
        //ambientOn = 0.0;
        diffuseOn = 0.0;
        //specularOn = 0.0;
    }
    void turnSpecularOn()
    {
        /*ambientOn = 0.0;
        diffuseOn = 0.0;*/
        specularOn = 1.0;
    }
    void turnSpecularOff()
    {
        /*
        ambientOn = 0.0;
        diffuseOn = 0.0;*/
        specularOn = 0.0;
    }
private:
    float ambientOn = 1.0;
    float diffuseOn = 1.0;
    float specularOn = 1.0;
};

#endif /* pointLight_h */
