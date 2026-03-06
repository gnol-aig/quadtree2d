#pragma once

#include <memory>
#include <vector>
#include <stack>

#include "raylib.h"
#include "circle.h"
#include "bbox.h"

class QuadTree : public std::enable_shared_from_this<QuadTree> {
private:
    std::shared_ptr<QuadTree> topleft;
    std::shared_ptr<QuadTree> topright;
    std::shared_ptr<QuadTree> bottomleft;
    std::shared_ptr<QuadTree> bottomright;

    int threshold = 4;

    bool is_divided = false;

public:
    BBox box;
    std::vector<Circle*> list;

    QuadTree(Vector2 p, Vector2 s) {
        box = BBox(p, s);
        topleft = nullptr;
        topright = nullptr;
        bottomleft = nullptr;
        bottomright = nullptr;
    }

    void insert(Circle* circle) {

        if (list.size() < threshold) {
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

        topleft = std::make_shared<QuadTree>(position, half_size);
        topright = std::make_shared<QuadTree>(
            Vector2{ position.x + half_size.x, position.y },
            half_size
        );
        bottomleft = std::make_shared<QuadTree>(
            Vector2{ position.x, position.y + half_size.y },
            half_size
        );
        bottomright = std::make_shared<QuadTree>(Vector2Add(position, half_size),
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
        std::stack<std::shared_ptr<QuadTree>> stack;
        stack.push(shared_from_this());

        while (!stack.empty()) {
            std::shared_ptr<QuadTree> current = stack.top();
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