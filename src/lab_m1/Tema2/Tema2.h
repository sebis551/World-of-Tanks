#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/Tema2_camera.h"


namespace m1
{
    struct Tank {
        float x;
        float z;
        int hp;
        float tank_angle;
        float turela_angle;
        float turela_angle_sus;
        float projectile_cooldown;
        int direction;
        float direction_timer;
        int stop = 0;
};

    struct Projectile {
        float x;
        float z;
        float y;
        float angle;
        float angle_sus;
        float timer;
    };


    struct Building {
        float x;
        float z;
        float y;
        float scale1;
        float scale2;
        float scale3;
        float r;
    };
    class Tema2 : public gfxc::SimpleScene
    {
     public:
         Tema2();
        ~Tema2();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color = glm::vec3(1), const int HP = 3 );
        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

     protected:
        implemented::Cameraa *camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;
        float fov = 60.0f;
        float top, left;

        // TODO(student): If you need any other class variables, define them here.

        Tank mainTank;
        std::vector<Projectile> proiectile;
        std::vector<Projectile> proiectile_enemy;
        std::vector<Tank> enemies;
        std::vector<Building> buildlings;

        int nr_enemies = 8 + rand() % 25 + 32;
        int nr_buildlings = 8 + rand() % 26;

        float timer = 0;
        float raza = 0;
        int stop = 0;
        int score = 0;
        int score_displayed = 0;

    };
}   // namespace m1
