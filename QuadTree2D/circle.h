#pragma once

#include "raylib.h"
#include "raymath.h"
#include <math.h>

#include "constants.h"

class Circle {
private:
    Vector2 center;
    Vector2 velocity;
    float radius;
    float speed;

public:
    Circle(Vector2 p_center, float p_radius, Vector2 init_velocity)
        :  center(p_center), radius(p_radius), velocity(init_velocity){
        speed = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    }

    Vector2 get_position() {
        return center;
    }

    inline void set_position(Vector2 position) {
        center = position;
    }

    inline Vector2 get_velocity() {
        return velocity;
    }

    inline void set_velocity(Vector2 vel) {
        velocity = vel;
    }


    inline float get_radius() {
        return radius;
    }

    inline float get_speed() {
        return speed;
    }

    float get_distance(Circle* other) {
        float dx = other->get_position().x - center.x;
        float dy = other->get_position().y - center.y;

        return sqrt(dx * dx + dy * dy);
    }

    void move() {
        if (center.x <= 0 || center.x >= WINDOW_WIDTH) velocity.x *= -1;
        if (center.y <= 0 || center.y >= WINDOW_HEIGHT) velocity.y *= -1;

        float dt = GetFrameTime();
        center = Vector2Add(center, Vector2Scale(velocity, dt));
    }

    bool is_out_of_bound() {
        return center.x <= 0 || center.x >= WINDOW_WIDTH || center.y <= 0 || center.y >= WINDOW_HEIGHT;
    }

    void show() {
        //float red = map_range<float>(x, 0, 1000, 0, 255);
        //float blue = map_range<float>(y, 0, 1000, 0, 255);
        DrawCircle(center.x, center.y, radius, Color{ 0, 0, 0, 255 });
    }
};
