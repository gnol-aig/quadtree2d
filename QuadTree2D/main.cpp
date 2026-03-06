#include "raylib.h"
#include "raymath.h"
#include "math.h"

#include <random>
#include <vector>
#include <stack>
#include <memory>

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 1000;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> distr(-100, 100);
std::uniform_real_distribution<> distrx(20, WINDOW_WIDTH - 20);
std::uniform_real_distribution<> distry(20, WINDOW_HEIGHT - 20);

class Circle;
void resolve_circle_collision(Circle* c1, Circle* c2);

template <typename T>
T map_range(T value, T src_start, T src_end, T dst_start, T dst_end) {
    // Ensure the value is within the source range, if desired for clamping
    // if (value < src_start) value = src_start;
    // if (value > src_end) value = src_end;

    T src_range = src_end - src_start;
    T dst_range = dst_end - dst_start;

    // The core mapping formula
    T result = dst_start + ((value - src_start) * dst_range / src_range);
    return result;
}


class Circle {
private:
    float x;
    float y;
    float radius;
    float vx;
    float vy;
    float speed;

public:
    Circle(float p_x, float p_y, float p_radius, float init_vx, float init_vy)
        : x(p_x), y(p_y), radius(p_radius), vx(init_vx), vy(init_vy) {
        speed = sqrt(vx * vx + vy * vy);
    }

    Vector2 get_center() {
        return { x, y };
    }

    inline float get_x() {
        return x;
    }

    inline void set_x(float p_x) {
        x = p_x;
    }

    inline float get_y() {
        return y;
    }

    inline void set_y(float p_y) {
        y = p_y;
    }

    inline float get_vx() {
        return vx;
    }

    inline void set_vx(float p_vx) {
        vx = p_vx;
    }

    inline float get_vy() {
        return vy;
    }

    inline void set_vy(float p_vy) {
        vy = p_vy;
    }

    inline Vector2 get_velocity() {
        return { vx, vy };
    }

    inline float get_radius() {
        return radius;
    }

    inline float get_speed() {
        return speed;
    }

    float get_distance(Circle* other) {
        float dx = other->get_x() - x;
        float dy = other->get_y() - y;

        return sqrt(dx * dx + dy * dy);
    }

    void move() {
        if (x <= 0 || x >= WINDOW_WIDTH) vx *= -1;
        if (y <= 0 || y >= WINDOW_HEIGHT) vy *= -1;

        float dt = GetFrameTime();
        x += vx * dt;
        y += vy * dt;
    }

    bool is_out_of_bound() {
        return x <= 0 || x >= WINDOW_WIDTH || y <= 0 || y >= WINDOW_HEIGHT;
    }

    void show() {
        float red = map_range<float>(x, 0, 1000, 0, 255);
        float blue = map_range<float>(y, 0, 1000, 0, 255);
        DrawCircle(x, y, radius, Color{(unsigned char)red, 0, (unsigned char)blue, 255});
    }
};

class BBox {
public:
    Vector2 top_left_position;
    Vector2 size;

    BBox() {

    }

    BBox(Vector2 position, Vector2 size)
        : top_left_position(position), size(size) {
    }

    bool contains(Circle* circle) {
        return circle->get_x() >= top_left_position.x &&
            circle->get_x() <= top_left_position.x + size.x &&
            circle->get_y() >= top_left_position.y &&
            circle->get_y() <= top_left_position.y + size.y;
    }

    bool intersects(BBox box) {
        if (top_left_position.x > box.top_left_position.x + box.size.x || 
            top_left_position.x + size.x < box.top_left_position.x ||
            top_left_position.y > box.top_left_position.y + box.size.y ||
            top_left_position.y + size.y < box.top_left_position.y)
        {
            return false; 
        }

        return true;
    }

    inline Vector2 get_position() {
        return top_left_position;
    }

    inline Vector2 get_size() {
        return size;
    }

    void draw() {
        DrawRectangleLines(top_left_position.x, top_left_position.y, size.x, size.y, BLACK);
    }
};

class QuadTree2D : public std::enable_shared_from_this<QuadTree2D> {
private:
    std::shared_ptr<QuadTree2D> topleft;
    std::shared_ptr<QuadTree2D> topright;
    std::shared_ptr<QuadTree2D> bottomleft;
    std::shared_ptr<QuadTree2D> bottomright;

    int threadhold = 5;

    bool is_divided = false;

public:
    BBox box;
    std::vector<Circle*> list;

    QuadTree2D(Vector2 p, Vector2 s) {
        box = BBox(p, s);
        topleft = nullptr;
        topright = nullptr;
        bottomleft = nullptr;
        bottomright = nullptr;
    }

    void insert(Circle* circle) {

        if (list.size() < threadhold) {
            list.push_back(circle);
            return;
        }

        if (!is_divided) {
            divide();
            is_divided = true;
        }

        if (topleft->box.contains(circle)) {
            topleft->insert(circle);
        }
        else if (topright->box.contains(circle)) {
            topright->insert(circle);
        }
        else if (bottomleft->box.contains(circle)) {
            bottomleft->insert(circle);
        }
        else {
            bottomright->insert(circle);
        }
    }

    void divide() {
        Vector2 position = box.get_position();
        Vector2 size = box.get_size();
        Vector2 half_size = Vector2Scale(size, 0.5f);

        topleft = std::make_shared<QuadTree2D>(position, half_size);
        topright = std::make_shared<QuadTree2D>(
            Vector2{ position.x + half_size.x, position.y },
            half_size
        );
        bottomleft = std::make_shared<QuadTree2D>(
            Vector2{ position.x, position.y + half_size.y},
            half_size
        );
        bottomright = std::make_shared<QuadTree2D>(Vector2Add(position, half_size),
                                                   half_size);
    }

    void show() {
        //box.draw();

        for (int i = 0; i < list.size(); i++) {
            list[i]->show();
        }

        if (is_divided) {
            topleft->show();
            topright->show();
            bottomleft->show();
            bottomright->show();
        } 
    }

    void query(BBox range, std::vector<Circle*>& found) {
        std::stack<std::shared_ptr<QuadTree2D>> stack;
        stack.push(shared_from_this());

        while (!stack.empty()) {
            std::shared_ptr<QuadTree2D> current = stack.top();
            stack.pop();

            if (current->box.intersects(range)) {
                for (int i = 0; i < current->list.size(); i++) {
                    if (range.contains(current->list[i])) {
                        found.push_back(current->list[i]);
                    }
                }

                if (current->is_divided) {
                    stack.push(current->topleft);
                    stack.push(current->topright);
                    stack.push(current->bottomleft);
                    stack.push(current->bottomright);
                }
            }
        }
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

void resolve_circle_collision(Circle* circle1, Circle* circle2) {
    float a = circle1->get_radius();
    float b = circle2->get_radius();
    float c = circle1->get_distance(circle2);

    float amount = (a + b - c) / 2.0f;

    Vector2 c1_center = circle1->get_center();
    Vector2 c2_center = circle2->get_center();
    Vector2 new_direction = Vector2Normalize(Vector2Subtract(c1_center, c2_center));

    float new_c1_x = c1_center.x + amount * new_direction.x;
    float new_c1_y = c1_center.y + amount * new_direction.y;
    float new_c2_x = c2_center.x - amount * new_direction.x;
    float new_c2_y = c2_center.y - amount * new_direction.y;

    circle1->set_x(new_c1_x);
    circle1->set_y(new_c1_y);
    circle2->set_x(new_c2_x);
    circle2->set_y(new_c2_y);

    Vector2 new_c1_vel = Vector2Scale(new_direction, circle1->get_speed());
    Vector2 new_c2_vel = Vector2Scale(Vector2Scale(new_direction, -1), circle2->get_speed());
    circle1->set_vx(new_c1_vel.x);
    circle1->set_vy(new_c1_vel.y);
    circle2->set_vx(new_c2_vel.x);
    circle2->set_vy(new_c2_vel.y);
}

void check_circles_collision() {
    for (int i = 0; i < circles.size(); i++) {
        Circle* circle = circles[i];
        for (int j = 0; j < circles.size(); j++) {
            if (i == j) continue;

            Circle* other = circles[j];

            if (circle->get_x() == other->get_x() && circle->get_y() == other->get_y()) continue;

            float total_radius = circle->get_radius() + other->get_radius();

            if (circle->get_distance(other) < total_radius) {
                resolve_circle_collision(circle, other);
            }
        }
    }
}

void check_circles_collision(std::shared_ptr<QuadTree2D> quad_tree) {
    for (int i = 0; i < circles.size(); i++) {
        Circle* circle = circles[i];
        BBox range = BBox(Vector2{ circle->get_x() - circle->get_radius()*2, circle->get_y() - circle->get_radius() * 2 }, Vector2{ circle->get_radius() * 4, circle->get_radius() * 4});
        std::vector<Circle*> queried_circles;
        quad_tree->query(range, queried_circles);
        for (Circle* other : queried_circles) {
            if (circle->get_x() == other->get_x() && circle->get_y() == other->get_y()) continue;
            
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

    for (int i = 0; i < 2500; i++) {
        float random_x = distrx(gen);
        float random_y = distry(gen);
        create_circle(random_x, random_y, 3);
    }

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();

        std::shared_ptr<QuadTree2D> quad_tree = std::make_shared<QuadTree2D>(Vector2{ 0,0 }, Vector2{ WINDOW_WIDTH, WINDOW_HEIGHT });

        ClearBackground(RAYWHITE);
       
        const char* fpsText = 0;
        if (currentFps <= 0) fpsText = TextFormat("FPS: unlimited (%i)", GetFPS());
        else fpsText = TextFormat("FPS: %i (target: %i)", GetFPS(), currentFps);
        DrawText(fpsText, 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Frame time: %02.02f ms", GetFrameTime()), 10, 30, 20, DARKGRAY);
        
        move_circles();

        for (int i = 0; i < circles.size(); i++) {
            quad_tree->insert(circles[i]);
        }
        check_circles_collision(quad_tree);
        quad_tree->show();

  /*      check_circles_collision();
        draw_circles();*/

        
        EndDrawing();
    }
    CloseWindow();
    return 0;
}