#include "raylib.h"
#include "raymath.h"
#include "math.h"

#include <random>
#include <vector>
#include <stack>
#include <memory>

#include "circle.h"
#include "bbox.h"
#include "quadtree.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> distr(-100, 100);
std::uniform_real_distribution<> distrx(20, WINDOW_WIDTH - 20);
std::uniform_real_distribution<> distry(20, WINDOW_HEIGHT - 20);
std::uniform_int_distribution<> distrr(1, 6);

std::vector<Circle*> circles;

void create_circle(float x, float y, float radius) {
    float vx = distr(gen);
    float vy = distr(gen);
    Circle* circle = new Circle(Vector2{ x, y }, radius, Vector2{ vx, vy });

    circles.push_back(circle);
}

void draw_circles() {
    for (int i = 0; i < circles.size(); i++) {
        Circle* circle = circles[i];
        DrawCircle(circle->get_position().x,
                   circle->get_position().y,
                   circle->get_radius(),
                   DARKGRAY);
    }
}

void move_circles() {
    for (int i = 0; i < circles.size(); i++) {
        Circle* circle = circles[i];
        circle->move();
    }
}

void resolve_circle_collision(Circle* circle1, Circle* circle2) {
    float r1 = circle1->get_radius();
    float r2 = circle2->get_radius();
    float d = circle1->get_distance(circle2);

    float amount = (r1 + r2 - d) / 2.0f;

    Vector2 c1_center = circle1->get_position();
    Vector2 c2_center = circle2->get_position();
    Vector2 new_direction = Vector2Normalize(Vector2Subtract(c1_center, c2_center));

    float new_c1_x = c1_center.x + amount * new_direction.x;
    float new_c1_y = c1_center.y + amount * new_direction.y;
    float new_c2_x = c2_center.x - amount * new_direction.x;
    float new_c2_y = c2_center.y - amount * new_direction.y;

    circle1->set_position({ new_c1_x, new_c1_y });
    circle2->set_position({ new_c2_x, new_c2_y });

    Vector2 new_c1_vel = Vector2Scale(new_direction, circle1->get_speed());
    Vector2 new_c2_vel = Vector2Scale(Vector2Scale(new_direction, -1), circle2->get_speed());
    circle1->set_velocity({ new_c1_vel.x, new_c1_vel.y });
    circle2->set_velocity({new_c2_vel.x, new_c2_vel.y});
}

void check_circles_collision() {
    for (int i = 0; i < circles.size(); i++) {
        Circle* circle = circles[i];
        for (int j = 0; j < circles.size(); j++) {
            if (i == j) continue;

            Circle* other = circles[j];

            if (circle->get_position().x == other->get_position().x &&
                circle->get_position().y == other->get_position().y) continue;

            float total_radius = circle->get_radius() + other->get_radius();

            if (circle->get_distance(other) < total_radius) {
                resolve_circle_collision(circle, other);
            }
        }
    }
}

void check_circles_collision(std::shared_ptr<QuadTree> quad_tree) {
    for (int i = 0; i < circles.size(); i++) {
        Circle* circle = circles[i];
        float circle_radius = circle->get_radius();
        BBox range = BBox(
            Vector2{ circle->get_position().x - circle_radius - 20,
                     circle->get_position().y - circle_radius - 20},
            Vector2{ circle_radius + 40,
                     circle_radius + 40});
        std::vector<Circle*> queried_circles;
        quad_tree->query(range, queried_circles);
        for (Circle* other : queried_circles) {
            if (circle->get_position().x == other->get_position().x &&
                circle->get_position().y == other->get_position().y) continue;
            
            float total_radius = circle->get_radius() + other->get_radius();

            if (circle->get_distance(other) < total_radius) {
                resolve_circle_collision(circle, other);
            }
        }
    }
}

int main() {
    const int screenWidth = WINDOW_WIDTH;
    const int screenHeight = WINDOW_HEIGHT;
    int currentFps = 60;

    InitWindow(screenWidth, screenHeight, "raylib basic window");

    for (int i = 0; i < 5000; i++) {
        float random_x = distrx(gen);
        float random_y = distry(gen);
        create_circle(random_x, random_y, 3);
    }

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();

        std::shared_ptr<QuadTree> quad_tree = std::make_shared<QuadTree>(Vector2{ 0,0 }, Vector2{ WINDOW_WIDTH, WINDOW_HEIGHT });

        ClearBackground(RAYWHITE);
       
        const char* fpsText = 0;
        if (currentFps <= 0) fpsText = TextFormat("FPS: unlimited (%i)", GetFPS());
        else fpsText = TextFormat("FPS: %i (target: %i)", GetFPS(), currentFps);
        DrawText(fpsText, 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Frame time: %02.02f ms", GetFrameTime()), 10, 30, 20, DARKGRAY);
        
        Vector2 mousePoint = GetMousePosition();


        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            int r = distrr(gen);
            create_circle(mousePoint.x, mousePoint.y, r);
        }

        const char* CircleSizeText = TextFormat("Circles: %i", circles.size());;
        DrawText(CircleSizeText, 10, 50, 20, DARKGRAY);

        move_circles();
       
        for (int i = 0; i < circles.size(); i++) {
            quad_tree->insert(circles[i]);
        }

        check_circles_collision(quad_tree);
        quad_tree->show();

        //check_circles_collision();
        //draw_circles();

        EndDrawing();
    }
    CloseWindow();
    return 0;
}