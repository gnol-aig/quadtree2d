#include "raylib.h"
#include "math.h"
#include <random>
#include <vector>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> distr(-100, 100);
std::uniform_real_distribution<> distrx(20, 780);
std::uniform_real_distribution<> distry(20, 580);

class Circle {
private:
    float x;
    float y;
    float radius;
    float vx;
    float vy;

public:
    Circle(float p_x, float p_y, float p_radius, float init_vx, float init_vy) 
        : x(p_x), y(p_y), radius(p_radius), vx(init_vx), vy(init_vy) {}

    float* get_center() {
        float* center = new float[2];

        center[0] = x;
        center[1] = y;

        return center;
    }

    inline float get_x() {
        return x;
    }

    inline float get_y() {
        return y;
    }

    inline float get_radius() {
        return radius;
    }

    float get_distance(Circle* other) {
        float dx = other->get_x() - x;
        float dy = other->get_y() - y;

        return sqrt(dx * dx + dy * dy);
    }

    void move() {
        if (x <= 0 || x >= 800) vx *= -1;
        if (y <= 0 || y >= 600) vy *= -1;

        float dt = GetFrameTime();
        x += vx * dt;
        y += vy * dt;
    }

    bool is_out_of_bound() {
        return x <= 0 || x >= 800 || y <= 0 || y >= 600;
    }
};

std::vector<Circle*> circles;

void create_circle(float x, float y, float radius) {
    float vx = distr(gen);
    float vy = distr(gen);
    Circle* circle = new Circle(x, y, radius, vx, vy);

    circles.push_back(circle);
}

void draw_circles() {
    for (int i = 0; i < circles.size(); i++) {
        Circle* circle = circles[i];
        DrawCircle(circle->get_x(), circle->get_y(), circle->get_radius(), DARKGRAY);
    }
}

void move_circles() {
    for (int i = 0; i < circles.size(); i++) {
        Circle* circle = circles[i];
        circle->move();
    }
}

void check_circle_collision() {

}

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    int currentFps = 60;

    InitWindow(screenWidth, screenHeight, "raylib basic window");

    for (int i = 0; i < 100; i++) {
        float random_x = distrx(gen);
        float random_y = distry(gen);
        create_circle(random_x, random_y, 10);
    }

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(RAYWHITE);
       
        const char* fpsText = 0;
        if (currentFps <= 0) fpsText = TextFormat("FPS: unlimited (%i)", GetFPS());
        else fpsText = TextFormat("FPS: %i (target: %i)", GetFPS(), currentFps);
        DrawText(fpsText, 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Frame time: %02.02f ms", GetFrameTime()), 10, 30, 20, DARKGRAY);
        
        move_circles();
        draw_circles();
        
        EndDrawing();
    }
    CloseWindow();
    return 0;
}