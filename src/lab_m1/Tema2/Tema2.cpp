#include "lab_m1/Tema2/Tema2.h"
#include "lab_m1/Tema2/Tema2transform3D.h"
#include "Tema2_camera.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    renderCameraTarget = false;
    left = 0.01f;
    top = 10.0f;
    camera = new implemented::Cameraa();
    camera->Set(glm::vec3(0, 1, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    mainTank.hp = 5;
    mainTank.x = 0;
    mainTank.z = 0;
    mainTank.tank_angle = 0;
    mainTank.turela_angle = 0;
    mainTank.turela_angle_sus = 0;
    mainTank.projectile_cooldown = 1.0f;
    
    raza = glm::distance(glm::vec3(0, 0, 0), glm::vec3(0.4f, 0, 0.7f));


    for (int i = 0; i < nr_buildlings; i++) {
        Building a;
        a.scale1 = 4 + rand() % 15;
        a.scale2 = 4 + rand() % 15;
        a.scale3 = 4 + rand() % 15;
        int b = rand();
        int c = rand();
        int z = rand();
        
        a.x = rand() % 70; 
        a.y = rand() % 80;
        a.z = rand() % 70;


        if (b % 2) a.x = -a.x;
       // if (c % 2) a.y = -a.y;
        if (z % 2) a.z = -a.z;
        
        while (mainTank.x <= a.x + a.scale1 / 2 && mainTank.x >= a.x - a.scale1 / 2 && mainTank.z <= a.z + a.scale3 / 2 && mainTank.z >= a.z - a.scale3 / 2) {
            a.x = rand() % 70;
            a.z = rand() % 70;

            if (b % 2) a.x = -a.x;
          //  if (c % 2) a.y = -a.y;
            if (z % 2) a.z = -a.z;
        //    cout << "intra aici" << endl;
        }

      //  cout << mainTank.x << " " << a.x << " " << a.scale1 << " " << a.z << " " << a.scale3 << " " << mainTank.z<<endl;

        
        
        a.r = glm::distance(glm::vec3(0, 0, 0), glm::vec3(a.scale1, 0, a.scale3));

        buildlings.push_back(a);
    }


    for (int i = 0; i < nr_enemies; i++) {
        Tank a;
        a.x = rand() % 70;
        a.z = rand() % 70;
        a.tank_angle = 0;
        a.turela_angle = 0;
        a.turela_angle_sus = 0;
        a.projectile_cooldown = 0.6f;
        a.hp = 5;
        a.direction = rand() % 4;
        a.direction_timer = (rand() % 4)/2.0f;
        a.stop = 0;
       // cout << a.direction_timer <<" "<<a.direction<< endl;


        int b = rand();
        int z = rand();

        


        if (b % 2) a.x = -a.x;
        if (z % 2) a.z = -a.z;

        for (int j = 0; j < nr_buildlings; j++) {
            int ok = 0;
            while (a.x <= buildlings[j].x + buildlings[j].scale1 / 2 && a.x >= buildlings[j].x - buildlings[j].scale1 / 2 && a.z <= buildlings[j].z + buildlings[j].scale3 / 2 && a.z >= buildlings[j].z - buildlings[j].scale3 / 2) {
                a.x = rand() % 70;
                a.z = rand() % 70;

                if (b % 2) a.x = -a.x;
                if (z % 2) a.z = -a.z;
                ok = 1;
               // std::cout << "ia sa vedem;"<<endl;
            }
            if (ok == 1) j = 0;
            
        }
        enemies.push_back(a);

    }




   



    {
        Shader* shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }




    {
        Mesh* mesh = new Mesh("ground");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }


    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* mesh = new Mesh("tun");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "tun.stl");
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* mesh = new Mesh("senila");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "senila.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* mesh = new Mesh("turela");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "turela.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // TODO(student): After you implement the changing of the projection
    // parameters, remove hardcodings of these parameters
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);

}


void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0.55, 0.79, 0.9, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);


    
}


void Tema2::Update(float deltaTimeSeconds)
{
    if (mainTank.hp <= 0) {
        stop = 1;
    }
    timer += deltaTimeSeconds;
    if (timer > 90) stop = 1;

    if (stop == 1 && score_displayed == 0) {
        cout << "Jocul s-a terminat. Ai obtinut scorul " << score << "!";
        score_displayed = 1;
    }

    mainTank.projectile_cooldown -= deltaTimeSeconds;

    if (stop == 1) {
        deltaTimeSeconds = 0;
    }
    for (int i = 0; i < nr_buildlings; i++) {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(buildlings[i].x, buildlings[i].scale2/2, buildlings[i].z));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(buildlings[i].scale1, buildlings[i].scale2, buildlings[i].scale3));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.2, 0.2, 0.2), 5);

        
        //modelMatrix = glm::mat4(1);
        //modelMatrix = glm::translate(modelMatrix, glm::vec3(buildlings[i].x + buildlings[i].scale1/2, 0, buildlings[i].z + buildlings[i].scale3 / 2));
        //RenderMesh(meshes["sphere"], shaders["Simple"], modelMatrix);



        float x1 = std::max(buildlings[i].x - buildlings[i].scale1/2, std::min(mainTank.x, buildlings[i].x + buildlings[i].scale1 / 2));
        float z1 = std::max(buildlings[i].z - buildlings[i].scale3 / 2, std::min(mainTank.z, buildlings[i].z + buildlings[i].scale3 / 2));
       
        float distance = glm::distance(glm::vec3(x1, 0, z1), glm::vec3(mainTank.x, 0, mainTank.z));

        float tank_radius = glm::distance(glm::vec3(mainTank.x + 0.2f , 0, mainTank.z + 0.35f), glm::vec3(mainTank.x, 0, mainTank.z));



       
        if (distance <= tank_radius) {
            //cout << distance << " " << tank_radius << endl;
            if (z1 == buildlings[i].z + buildlings[i].scale3 / 2 && x1 == mainTank.x) {
                mainTank.z += deltaTimeSeconds;
                camera->Set(camera->position + glm::vec3(0, 0, deltaTimeSeconds), glm::vec3(mainTank.x, 1, mainTank.z), camera->up);

            }
            if (z1 == mainTank.z && x1 == buildlings[i].x + buildlings[i].scale1 / 2) {
                mainTank.x += deltaTimeSeconds;
                camera->Set(camera->position + glm::vec3(deltaTimeSeconds, 0, 0), glm::vec3(mainTank.x, 1, mainTank.z), camera->up);

            }

            if (z1 == buildlings[i].z - buildlings[i].scale3 / 2 && x1 == mainTank.x) {
                mainTank.z -= deltaTimeSeconds;
                camera->Set(camera->position - glm::vec3(0, 0, deltaTimeSeconds), glm::vec3(mainTank.x, 1, mainTank.z), camera->up);
            }

            if (z1 == mainTank.z && x1 == buildlings[i].x - buildlings[i].scale1 / 2) {
                mainTank.x -= deltaTimeSeconds;
                camera->Set(camera->position - glm::vec3(deltaTimeSeconds, 0, 0), glm::vec3(mainTank.x, 1, mainTank.z), camera->up);

            }

        }
           //cout<<camera->GetTargetPosition()<<endl;
            
           
            for (int j = 0; j < enemies.size(); j++) {
                float x11 = std::max(buildlings[i].x - buildlings[i].scale1 / 2, std::min(enemies[j].x, buildlings[i].x + buildlings[i].scale1 / 2));
                float z11 = std::max(buildlings[i].z - buildlings[i].scale3 / 2, std::min(enemies[j].z, buildlings[i].z + buildlings[i].scale3 / 2));

                float distance1 = glm::distance(glm::vec3(x11, 0, z11), glm::vec3(enemies[j].x, 0, enemies[j].z));

                float tank_radius1 = glm::distance(glm::vec3(enemies[j].x + 0.2f, 0, enemies[j].z + 0.35f), glm::vec3(enemies[j].x, 0, enemies[j].z));




                if (distance1 <= tank_radius1) {
                    //cout << distance << " " << tank_radius << endl;
                    if (z11 == buildlings[i].z + buildlings[i].scale3 / 2 && x11 == enemies[j].x) {
                        enemies[j].z += deltaTimeSeconds;


                    }
                    if (z11 == enemies[j].z && x11 == buildlings[i].x + buildlings[i].scale1 / 2) {
                        enemies[j].x += deltaTimeSeconds;


                    }

                    if (z11 == buildlings[i].z - buildlings[i].scale3 / 2 && x11 == enemies[j].x) {
                        enemies[j].z -= deltaTimeSeconds;

                    }

                    if (z11 == enemies[j].z && x11 == buildlings[i].x - buildlings[i].scale1 / 2) {
                        enemies[j].x -= deltaTimeSeconds;


                    }
                }


            }

           // camera->TranslateForward(-deltaTimeSeconds * 0.5f);
            //camera->RotateThirdPerson_OY(-deltaTimeSeconds);
        



    }


    for (int j = 0; j < enemies.size(); j++) {
        if (enemies[j].hp <= 0) {
            score ++;
            enemies.erase(enemies.begin() + j);
            j = 0;


        }
    }


    for (int i = 0; i < enemies.size(); i++) {


        float distance5 = glm::distance(glm::vec3(enemies[i].x, 0, enemies[i].z), glm::vec3(mainTank.x, 0, mainTank.z));
        float raza3 = glm::distance(glm::vec3(0, 0, 0), glm::vec3(3.5f, 0, 3.5f));

        if (distance5 < (raza3 + raza)) {
         //   cout <<glm::atan2(mainTank.z, mainTank.x) <<" "<<glm::atan(mainTank.z / mainTank.x) << " " << RADIANS(mainTank.turela_angle) << endl;
          //  enemies[i].turela_angle =  glm::degrees(glm::atan(mainTank.x / mainTank.z));


            glm::vec2 vectorToCenter( enemies[i].x - mainTank.x, enemies[i].z - mainTank.z);

            float angle_radians = atan2(vectorToCenter.y, vectorToCenter.x);
            float angle_degrees = glm::degrees(angle_radians);

            angle_degrees = fmod(angle_degrees + 360, 360);
            
            
            enemies[i].turela_angle = 90.0f - angle_degrees - enemies[i].tank_angle;

            if (enemies[i].projectile_cooldown <= 0) {
                enemies[i].projectile_cooldown = 3.5f;
                Projectile proiectil;
                proiectil.angle = enemies[i].tank_angle + enemies[i].turela_angle;
                proiectil.angle_sus = enemies[i].turela_angle_sus;
                proiectil.x = enemies[i].x;
                proiectil.z = enemies[i].z;
                proiectil.y = 0.58f;
                proiectil.timer = 1.0f;


                proiectile_enemy.push_back(proiectil);
            }

           

        }

        enemies[i].projectile_cooldown -= deltaTimeSeconds;


        if (enemies[i].direction_timer <= 0) {
            enemies[i].direction = rand() % 4;
            enemies[i].direction_timer = (rand() % 5) / 2.0f;
            if (enemies[i].direction == 0) {
                enemies[i].direction_timer += 4.2;
            }

            if (enemies[i].direction == 1) {
                enemies[i].direction_timer += 1.4;
            }
           // cout << enemies[i].direction_timer << " " << enemies[i].direction << endl;
        }
        
            if (enemies[i].direction == 0) {

                enemies[i].x -= deltaTimeSeconds * sin(RADIANS(enemies[i].tank_angle));
                enemies[i].z -= deltaTimeSeconds * cos(RADIANS(enemies[i].tank_angle));

            }

            if (enemies[i].direction == 1) {

                enemies[i].x += deltaTimeSeconds * sin(RADIANS(enemies[i].tank_angle));
                enemies[i].z += deltaTimeSeconds * cos(RADIANS(enemies[i].tank_angle));
            }

            if (enemies[i].direction == 2) {

                enemies[i].tank_angle += 30 * deltaTimeSeconds;
            }

            if (enemies[i].direction == 3) {

                enemies[i].tank_angle -= 30 * deltaTimeSeconds;

            }





            enemies[i].direction_timer -= deltaTimeSeconds;
        


        float distance = glm::distance(glm::vec3(enemies[i].x, 0, enemies[i].z), glm::vec3(mainTank.x, 0, mainTank.z));

        if (distance < 2 * raza) {
            float p = 2 * raza - distance;
            float difX = mainTank.x - enemies[i].x;
            float difZ = mainTank.z - enemies[i].z;

            glm::vec3 P = glm::vec3(abs(p)) * glm::normalize(glm::vec3(difX, 0, difZ));

           mainTank.x += P.x * (0.5f);
           mainTank.z += P.z * (0.5f);

           //camera->Set(glm::vec3(mainTank.x + 0, 1, mainTank.z + 3.5f), glm::vec3(mainTank.x, 1, mainTank.z), glm::vec3(0, 1, 0));
           
           camera->TranslateForward(-deltaTimeSeconds * 0.5f);
         //  camera->Set(camera->position + glm::vec3(deltaTimeSeconds * 0.5f, 0, deltaTimeSeconds * 0.5f), glm::vec3(mainTank.x, 1, mainTank.z), camera->up);

          // cout << P.x<<endl;
            
            enemies[i].x += P.x * (-0.5f);
            enemies[i].z += P.z * (-0.5f);

            
        }

        
        

        for (int j = 0; j < enemies.size(); j++) {
            if (i != j) {
                float distance2 = glm::distance(glm::vec3(enemies[i].x, 0, enemies[i].z), glm::vec3(enemies[j].x, 0, enemies[j].z));

                if (distance2 < 2 * raza) {
                    float p2 = 2 * raza - distance;
                    float difX2 = enemies[j].x - enemies[i].x;
                    float difZ2 = enemies[j].z - enemies[i].z;

                    glm::vec3 P2 = glm::vec3(abs(p2)) * glm::normalize(glm::vec3(difX2, 0, difZ2));

                    enemies[j].x += P2.x * (0.05f);
                    enemies[j].z += P2.z * (0.05f);

                    //camera->Set(glm::vec3(mainTank.x + 0, 1, mainTank.z + 3.5f), glm::vec3(mainTank.x, 1, mainTank.z), glm::vec3(0, 1, 0));

                   
                    //  camera->Set(camera->position + glm::vec3(deltaTimeSeconds * 0.5f, 0, deltaTimeSeconds * 0.5f), glm::vec3(mainTank.x, 1, mainTank.z), camera->up);

                     // cout << P.x<<endl;

                    enemies[i].x += P2.x * (-0.05f);
                    enemies[i].z += P2.z * (-0.05f);


                }
            }
        }

        

        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(enemies[i].x, 0.57f, enemies[i].z));

            modelMatrix = glm::rotate(modelMatrix, RADIANS(enemies[i].turela_angle), glm::vec3(0, 1, 0));
            modelMatrix = glm::rotate(modelMatrix, RADIANS(enemies[i].tank_angle), glm::vec3(0, 1, 0));
            modelMatrix = glm::rotate(modelMatrix, RADIANS(enemies[i].turela_angle_sus), glm::vec3(1, 0, 0));
            modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));

            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.3f));

            // RenderMesh(meshes["tun"], shaders["VertexNormal"], modelMatrix);
            RenderSimpleMesh(meshes["tun"], shaders["LabShader"], modelMatrix, glm::vec3(1, 1, 1), enemies[i].hp);
        }


        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(enemies[i].x, 0.6f, enemies[i].z));
            // modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(0, 1, 0));
            modelMatrix = glm::rotate(modelMatrix, RADIANS(enemies[i].turela_angle), glm::vec3(0, 1, 0));
            modelMatrix = glm::rotate(modelMatrix, RADIANS(enemies[i].tank_angle), glm::vec3(0, 1, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.23f, 0.65f, 0.23f));
            // RenderMesh(meshes["turela"], shaders["Simple"], modelMatrix);
            RenderSimpleMesh(meshes["turela"], shaders["LabShader"], modelMatrix, glm::vec3(0, 0.8f, 0), enemies[i].hp);
        }

        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(enemies[i].x, 0, enemies[i].z));
            modelMatrix = glm::rotate(modelMatrix, RADIANS(enemies[i].tank_angle), glm::vec3(0, 1, 0));
            modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.25f, 0.1f, 0));
            // modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(0, 1, 0));
            modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(0, 1, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.72f, 0.4f, 0.4f));
            //RenderMesh(meshes["senila"], shaders["VertexNormal"], modelMatrix);
            RenderSimpleMesh(meshes["senila"], shaders["LabShader"], modelMatrix, glm::vec3(1, 1, 1), enemies[i].hp);
        }


        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(enemies[i].x, 0, enemies[i].z));
            modelMatrix = glm::rotate(modelMatrix, RADIANS(enemies[i].tank_angle), glm::vec3(0, 1, 0));
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0.25f, 0.1f, 0));
            // modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.25f + deltaTimeSeconds * sin(RADIANS(mainTank.tank_angle)), 0.1f, deltaTimeSeconds * cos(RADIANS(mainTank.tank_angle))));
            modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(0, 1, 0));
            // modelMatrix = glm::rotate(modelMatrix, RADIANS(mainTank.tank_angle), glm::vec3(0, 1, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.72f, 0.4f, 0.4f));
            // RenderMesh(meshes["senila"], shaders["VertexNormal"], modelMatrix);
            RenderSimpleMesh(meshes["senila"], shaders["LabShader"], modelMatrix, glm::vec3(1, 1, 1), enemies[i].hp);
        }

        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(enemies[i].x, 0.32f, enemies[i].z));
            modelMatrix = glm::rotate(modelMatrix, RADIANS(enemies[i].tank_angle), glm::vec3(0, 1, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4f, 0.7f, 0.7f));
            //RenderMesh(meshes["turela"], shaders["Simple"], modelMatrix);
            RenderSimpleMesh(meshes["turela"], shaders["LabShader"], modelMatrix, glm::vec3(0, 0.5, 0), enemies[i].hp);
        }
        
    }




    for (int i = 0; i < proiectile_enemy.size(); i++) {
        // std::cout << proiectile[i].angle << endl;
        proiectile_enemy[i].x -= 2 * deltaTimeSeconds * sin(RADIANS(proiectile_enemy[i].angle));
        proiectile_enemy[i].z -= 2 * deltaTimeSeconds * cos(RADIANS(proiectile_enemy[i].angle));
        if (proiectile_enemy[i].timer >= 0) {
            if (proiectile_enemy[i].timer < 0.3f) {
                proiectile_enemy[i].y += 2 * 0.6f * deltaTimeSeconds * sin(RADIANS(proiectile_enemy[i].angle_sus));
            }
            else {
                proiectile_enemy[i].y += 2 * deltaTimeSeconds * sin(RADIANS(proiectile_enemy[i].angle_sus));
            }
            proiectile_enemy[i].timer -= deltaTimeSeconds;
        }
        else {
            if (proiectile_enemy[i].timer < -0.5f) {
                proiectile_enemy[i].y -= 2 * deltaTimeSeconds * sin(RADIANS(proiectile_enemy[i].angle_sus));
                // cout << sin(RADIANS(proiectile[i].angle_sus)) << endl;
                if (sin(RADIANS(proiectile_enemy[i].angle_sus)) < 0.2) {
                    proiectile_enemy[i].y -= 2 * deltaTimeSeconds * 0.1;
                }
            }
            else {
                if (proiectile_enemy[i].timer < -0.2f) {
                    proiectile_enemy[i].y -= 2 * 0.7 * deltaTimeSeconds * sin(RADIANS(proiectile_enemy[i].angle_sus));
                }
            }
            proiectile_enemy[i].timer -= deltaTimeSeconds;
        }


        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(proiectile_enemy[i].x, proiectile_enemy[i].y, proiectile_enemy[i].z));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
        RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], modelMatrix, glm::vec3(0, 0, 0), 3);

        if (proiectile_enemy[i].y <= 0) {
            proiectile_enemy.erase(proiectile_enemy.begin() + i);
        }
        else {
            if (proiectile_enemy[i].y <= 0.57 + 0.2) {
                int okk = 0;
                
                    float distance2 = glm::distance(glm::vec3(mainTank.x, 0, mainTank.z), glm::vec3(proiectile_enemy[i].x, 0, proiectile_enemy[i].z));

                    if (distance2 <= raza) {
                        mainTank.hp--;
                        proiectile_enemy.erase(proiectile_enemy.begin() + i);
                        i--;
                        //  cout << enemies[c].hp << endl;

                        okk = 1;
                        break;
                    }
                
                if (okk == 1) continue;

            }

            for (int d = 0; d < buildlings.size(); d++) {
                

                float x1 = std::max(buildlings[d].x - buildlings[d].scale1 / 2, std::min(proiectile_enemy[i].x, buildlings[d].x + buildlings[d].scale1 / 2));
                float z1 = std::max(buildlings[d].z - buildlings[d].scale3 / 2, std::min(proiectile_enemy[i].z, buildlings[d].z + buildlings[d].scale3 / 2));

                float distance = glm::distance(glm::vec3(x1, 0, z1), glm::vec3(proiectile_enemy[i].x, 0, proiectile_enemy[i].z));

                float proiectile_radius = glm::distance(glm::vec3(proiectile_enemy[i].x + 0.05f, 0, proiectile_enemy[i].z + 0.05f), glm::vec3(proiectile_enemy[i].x, 0, proiectile_enemy[i].z));

                if (distance <= proiectile_radius) {
                    proiectile_enemy.erase(proiectile_enemy.begin() + i);
                    break;
                }

            }
        }
    }





















































    for (int i = 0; i < proiectile.size(); i++) {
      // std::cout << proiectile[i].angle << endl;
        proiectile[i].x -= 2 * deltaTimeSeconds * sin(RADIANS(proiectile[i].angle));
        proiectile[i].z -= 2 * deltaTimeSeconds * cos(RADIANS(proiectile[i].angle));
        if (proiectile[i].timer >= 0) {
            if (proiectile[i].timer < 0.3f) {
                proiectile[i].y += 2 * 0.6f * deltaTimeSeconds * sin(RADIANS(proiectile[i].angle_sus));
            }
            else {
                proiectile[i].y += 2 * deltaTimeSeconds * sin(RADIANS(proiectile[i].angle_sus));
            }
            proiectile[i].timer -= deltaTimeSeconds;
        }
        else {
            if (proiectile[i].timer < -0.5f) {
                proiectile[i].y -= 2 * deltaTimeSeconds * sin(RADIANS(proiectile[i].angle_sus));
                // cout << sin(RADIANS(proiectile[i].angle_sus)) << endl;
                if (sin(RADIANS(proiectile[i].angle_sus)) < 0.2) {
                    proiectile[i].y -= 2 * deltaTimeSeconds * 0.1;
                }
            }
            else {
                if (proiectile[i].timer < -0.2f) {
                    proiectile[i].y -= 2 * 0.7 * deltaTimeSeconds * sin(RADIANS(proiectile[i].angle_sus));
                }
            }
            proiectile[i].timer -= deltaTimeSeconds;
        }


        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(proiectile[i].x, proiectile[i].y, proiectile[i].z));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
        RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], modelMatrix, glm::vec3(0, 0, 0), 3);

        if (proiectile[i].y <= 0) {
            proiectile.erase(proiectile.begin() + i);
        }
        else {
            if (proiectile[i].y <= 0.57 + 0.2) {
                int okk = 0;
                for (int c = 0; c < enemies.size(); c++) {
                    float distance2 = glm::distance(glm::vec3(enemies[c].x, 0, enemies[c].z), glm::vec3(proiectile[i].x, 0, proiectile[i].z));

                    if (distance2 <= raza) {
                        enemies[c].hp--;
                        proiectile.erase(proiectile.begin() + i);
                        i--;
                      //  cout << enemies[c].hp << endl;

                        okk = 1;
                        break;
                    }
                }
                if (okk == 1) continue;
                
            }

            for (int d = 0; d < buildlings.size(); d++) {
               /* float distance2 = glm::distance(glm::vec3(buildlings[d].x, 0, buildlings[d].z), glm::vec3(proiectile[i].x, 0, proiectile[i].z));
                float distance3 = glm::distance(glm::vec3(buildlings[d].x, 0, buildlings[d].z), glm::vec3(buildlings[d].x + buildlings[d].scale1 / 2, 0, buildlings[d].z + buildlings[d].scale3 / 2));
               // cout << distance2 << " " << distance3 << endl;
                if (distance2 <= distance3) {
                    proiectile.erase(proiectile.begin() + i);
                    // cout << "da bos a atins" << endl;
                    cout << distance2 << " " << distance3 << " DA" << endl;
                    break;

                }*/

                float x1 = std::max(buildlings[d].x - buildlings[d].scale1 / 2, std::min(proiectile[i].x, buildlings[d].x + buildlings[d].scale1 / 2));
                float z1 = std::max(buildlings[d].z - buildlings[d].scale3 / 2, std::min(proiectile[i].z, buildlings[d].z + buildlings[d].scale3 / 2));

                float distance = glm::distance(glm::vec3(x1, 0, z1), glm::vec3(proiectile[i].x, 0, proiectile[i].z));

                float proiectile_radius = glm::distance(glm::vec3(proiectile[i].x + 0.05f, 0, proiectile[i].z + 0.05f), glm::vec3(proiectile[i].x, 0, proiectile[i].z));

                if (distance <= proiectile_radius) {
                    proiectile.erase(proiectile.begin() + i);
                    break;
                }

            }
        }
    }




















    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(mainTank.x, 0.57f, mainTank.z));
        
        modelMatrix = glm::rotate(modelMatrix, RADIANS(mainTank.turela_angle), glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(mainTank.tank_angle), glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(mainTank.turela_angle_sus), glm::vec3(1, 0, 0));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
        
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.2f, 0.3f));

       // RenderMesh(meshes["tun"], shaders["VertexNormal"], modelMatrix);
       RenderSimpleMesh(meshes["tun"], shaders["LabShader"], modelMatrix, glm::vec3(1, 1, 1), mainTank.hp);
    }


    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(mainTank.x, 0.6f, mainTank.z));
       // modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(mainTank.turela_angle), glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(mainTank.tank_angle), glm::vec3(0, 1, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.23f, 0.65f, 0.23f));
       // RenderMesh(meshes["turela"], shaders["Simple"], modelMatrix);
        RenderSimpleMesh(meshes["turela"], shaders["LabShader"], modelMatrix, glm::vec3(0, 0.8f, 0), mainTank.hp);
    }
    
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(mainTank.x, 0, mainTank.z));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(mainTank.tank_angle), glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.25f, 0.1f, 0));
       // modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(0, 1, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.72f, 0.4f, 0.4f));
         //RenderMesh(meshes["senila"], shaders["VertexNormal"], modelMatrix);
        RenderSimpleMesh(meshes["senila"], shaders["LabShader"], modelMatrix, glm::vec3(1, 1, 1), mainTank.hp);
    }


    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(mainTank.x, 0, mainTank.z));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(mainTank.tank_angle), glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.25f, 0.1f, 0));
       // modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.25f + deltaTimeSeconds * sin(RADIANS(mainTank.tank_angle)), 0.1f, deltaTimeSeconds * cos(RADIANS(mainTank.tank_angle))));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(0, 1, 0));
       // modelMatrix = glm::rotate(modelMatrix, RADIANS(mainTank.tank_angle), glm::vec3(0, 1, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.72f, 0.4f, 0.4f));
       // RenderMesh(meshes["senila"], shaders["VertexNormal"], modelMatrix);
        RenderSimpleMesh(meshes["senila"], shaders["LabShader"], modelMatrix, glm::vec3(1, 1, 1), mainTank.hp);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(mainTank.x, 0.32f, mainTank.z));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(mainTank.tank_angle), glm::vec3(0, 1, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4f, 0.7f, 0.7f));
        //RenderMesh(meshes["turela"], shaders["Simple"], modelMatrix);
        RenderSimpleMesh(meshes["turela"], shaders["LabShader"], modelMatrix, glm::vec3(0, 0.5, 0), mainTank.hp);
    }



    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(300, 300, 300));
        RenderMesh(meshes["ground"], shaders["Simple"], modelMatrix);
    }




























}


void Tema2::FrameEnd()
{
    //DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, const int HP)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    
    
    glUniform3fv(glGetUniformLocation(shader->program, "object_color"), 1, glm::value_ptr(color));
    glUniform1i(glGetUniformLocation(shader->program, "hp"), HP);





    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    


    // Bind view matrix
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);//
    GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}





void Tema2::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    // move the camera only if MOUSE_RIGHT button is pressed
   // if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float cameraSpeed = 0.1f;
        if (stop == 1) deltaTime = 0;

        if (window->KeyHold(GLFW_KEY_W)) {
            // TODO(student): Translate the camera forward
            mainTank.x -= deltaTime * sin(RADIANS(mainTank.tank_angle));
            mainTank.z -= deltaTime * cos(RADIANS(mainTank.tank_angle));
            camera->TranslateForward( deltaTime);
            
            

        }

        if (window->KeyHold(GLFW_KEY_A)) {
            // TODO(student): Translate the camera to the left
             mainTank.tank_angle += 30 * deltaTime;
             camera->RotateThirdPerson_OY( 30 *  deltaTime);
            //camera->TranslateRight(-deltaTime * cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_S)) {
            // TODO(student): Translate the camera backward
            mainTank.x += deltaTime * sin(RADIANS(mainTank.tank_angle));
            mainTank.z += deltaTime * cos(RADIANS(mainTank.tank_angle));
           // camera->TranslateForward(-deltaTime * cameraSpeed);
            camera->TranslateForward(-deltaTime);

        }

        if (window->KeyHold(GLFW_KEY_D)) {
            // TODO(student): Translate the camera to the right
            mainTank.tank_angle -= 30 * deltaTime;
            camera->RotateThirdPerson_OY(-30 * deltaTime);
            //camera->TranslateRight(deltaTime * cameraSpeed);

        }

        if (window->KeyHold(GLFW_KEY_Q)) {
            // TODO(student): Translate the camera downward
            camera->TranslateUpward(-deltaTime * cameraSpeed);

        }

        if (window->KeyHold(GLFW_KEY_E)) {
            // TODO(student): Translate the camera upward
            camera->TranslateUpward(deltaTime * cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_Q)) {
            // TODO(student): Translate the camera downward
            camera->TranslateUpward(-deltaTime * cameraSpeed);

        }

        if (window->KeyHold(GLFW_KEY_E)) {
            // TODO(student): Translate the camera upward
            camera->TranslateUpward(deltaTime * cameraSpeed);
        }

       
    }

   
}


void Tema2::OnKeyPress(int key, int mods)
{

}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event

    mainTank.turela_angle -= 0.2 * deltaX;

   
    if(mainTank.turela_angle_sus <= 90 && deltaY < 0)
    mainTank.turela_angle_sus -= 0.1 * deltaY; //in sus

    if (mainTank.turela_angle_sus >= 0 && deltaY > 0)
        mainTank.turela_angle_sus -= 0.2 * deltaY; //in jos




    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.01f;
        float sensivityOY = 0.001f;

        if (window->GetSpecialKeyState() == 0) {
            renderCameraTarget = false;
            // TODO(student): Rotate the camera in first-person mode around
            // OX and OY using `deltaX` and `deltaY`. Use the sensitivity
            // variables for setting up the rotation speed.
            camera->RotateFirstPerson_OY(-deltaX * sensivityOX);
            camera->RotateFirstPerson_OX(-deltaY * sensivityOY);
          

        }

        
    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
   // cout << button;
    // Add mouse button press event
    if (button == 1 && mainTank.projectile_cooldown <= 0) {
       // cout << "a intrat";
        mainTank.projectile_cooldown = 1.0f;
        Projectile proiectil;
        proiectil.angle = mainTank.tank_angle + mainTank.turela_angle;
        proiectil.angle_sus = mainTank.turela_angle_sus;
        proiectil.x = mainTank.x;
        proiectil.z = mainTank.z;
        proiectil.y = 0.58f;
        proiectil.timer = 2.5f;


        proiectile.push_back(proiectil);


    }
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
