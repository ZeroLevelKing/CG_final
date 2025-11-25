#pragma once
#ifndef VISUALIZE_H
#define VISUALIZE_H

#include "vector2.h"
#include "edge.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Visualizer {
public:
    // 可视化函数
    static void visualizePointsAndEdges(const std::vector<Vector2<float>>& points,
        const std::vector<Edge<float>>& edges,
        const std::string& windowTitle) {

        // 创建窗口
        sf::RenderWindow window(sf::VideoMode(800, 600), windowTitle);

        // 创建点形状
        std::vector<sf::RectangleShape*> squares;
        for (const auto& p : points) {
            sf::RectangleShape* c1 = new sf::RectangleShape(sf::Vector2f(4, 4));
            c1->setPosition(p.x, p.y);
            c1->setFillColor(sf::Color::Red);
            squares.push_back(c1);
        }

        // 创建边形状
        std::vector<std::vector<sf::Vertex>> lines;
        for (const auto& e : edges) {
            std::vector<sf::Vertex> line = {
                sf::Vertex(sf::Vector2f(e.p1.x + 2, e.p1.y + 2), sf::Color::Blue),
                sf::Vertex(sf::Vector2f(e.p2.x + 2, e.p2.y + 2), sf::Color::Blue)
            };
            lines.push_back(line);
        }

        std::cout << "可视化: " << points.size() << " 个点, " << edges.size() << " 条边" << std::endl;

        // 主循环
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }

            window.clear(sf::Color::White);

            // 绘制边
            for (const auto& line : lines) {
                if (line.size() >= 2) {
                    window.draw(&line[0], line.size(), sf::Lines);
                }
            }

            // 绘制点
            for (const auto& s : squares) {
                window.draw(*s);
            }

            window.display();
        }

        // 清理内存
        for (auto& square : squares) {
            delete square;
        }
    }
};

#endif