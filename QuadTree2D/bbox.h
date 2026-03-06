#pragma once

#include "raylib.h"

#include "circle.h"

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
        return circle->get_position().x >= top_left_position.x &&
            circle->get_position().x <= top_left_position.x + size.x &&
            circle->get_position().y >= top_left_position.y &&
            circle->get_position().y <= top_left_position.y + size.y;
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
