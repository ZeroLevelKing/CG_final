#include <iostream>
#include <vector>
#include <algorithm>
#include <stack>
#include <cmath>
#include <random>
#include <ctime>
#include <SFML/Graphics.hpp>

using namespace std;

// 点的结构体
struct Point {
    double x, y;

    Point(double x = 0, double y = 0) : x(x), y(y) {}

    // 重载运算符便于比较
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }

    // 用于输出点
    friend ostream& operator<<(ostream& os, const Point& p) {
        os << "(" << p.x << ", " << p.y << ")";
        return os;
    }
};

// 生成随机点集
vector<Point> generateRandomPoints(int numPoints, double minX = 0, double maxX = 100,
    double minY = 0, double maxY = 100) {
    vector<Point> points;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> distX(minX, maxX);
    uniform_real_distribution<double> distY(minY, maxY);

    // 使用set来避免重复点
    vector<Point> uniquePoints;

    for (int i = 0; i < numPoints; i++) {
        Point p(distX(gen), distY(gen));
        uniquePoints.push_back(p);
    }

    // 移除重复点（基于浮点精度比较，实际应用中可能需要更精确的比较）
    sort(uniquePoints.begin(), uniquePoints.end(), [](const Point& a, const Point& b) {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
        });

    auto last = unique(uniquePoints.begin(), uniquePoints.end(), [](const Point& a, const Point& b) {
        return abs(a.x - b.x) < 1e-9 && abs(a.y - b.y) < 1e-9;
        });

    uniquePoints.erase(last, uniquePoints.end());

    return uniquePoints;
}

// 计算叉积 (P1P2) × (P1P3)
double crossProduct(const Point& p1, const Point& p2, const Point& p3) {
    return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}

// 计算两点间距离的平方
double distanceSquared(const Point& p1, const Point& p2) {
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    return dx * dx + dy * dy;
}

// 卷包裹法（Gift Wrapping Algorithm）计算凸包
vector<Point> giftWrapping(vector<Point>& points) {
    int n = points.size();
    if (n < 3) {
        return points; // 点太少，直接返回
    }

    // 找到最左边的点（y坐标最小的，如果x相同）
    int leftmost = 0;
    for (int i = 1; i < n; i++) {
        if (points[i].x < points[leftmost].x ||
            (points[i].x == points[leftmost].x && points[i].y < points[leftmost].y)) {
            leftmost = i;
        }
    }

    vector<Point> hull; // 凸包点集

    int p = leftmost; // 当前凸包点
    int q; // 下一个候选点

    do {
        hull.push_back(points[p]);

        q = (p + 1) % n; // 初始候选点

        // 寻找下一个凸包点
        for (int i = 0; i < n; i++) {
            if (i == p || i == q) continue;

            double cross = crossProduct(points[p], points[i], points[q]);

            if (cross > 0) {
                // 更新候选点
                q = i;
            }
            else if (cross == 0) {
                // 共线情况，选择距离更远的点
                if (distanceSquared(points[p], points[i]) > distanceSquared(points[p], points[q])) {
                    q = i;
                }
            }
        }

        p = q; // 移动到下一个点

    } while (p != leftmost && hull.size() <= n); // 回到起点时结束

    return hull;
}

// 显示点集信息
void displayPoints(const vector<Point>& points, const string& title = "Point Set") {
    cout << title << " (" << points.size() << " points):" << endl;
    for (size_t i = 0; i < points.size(); i++) {
        cout << points[i];
        if (i < points.size() - 1) cout << ", ";
        if ((i + 1) % 5 == 0) cout << endl; // 每5个点换行
    }
    cout << "\n" << endl;
}

// 显示凸包信息
void displayConvexHull(const vector<Point>& hull) {
    cout << "Convex hull contains " << hull.size() << " points:" << endl;
    for (size_t i = 0; i < hull.size(); i++) {
        cout << "Point " << i + 1 << ": " << hull[i] << endl;
    }
    cout << endl;
}

// SFML可视化函数
void visualizeConvexHull(const vector<Point>& points, const vector<Point>& hull,
    const string& title = "Convex Hull Visualization") {
    // 创建窗口
    const int windowWidth = 800;
    const int windowHeight = 600;
    const int margin = 50; // 边距

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), title);

    // 计算坐标缩放因子
    double minX = 0, maxX = 100, minY = 0, maxY = 100;
    if (!points.empty()) {
        minX = maxX = points[0].x;
        minY = maxY = points[0].y;
        for (const auto& p : points) {
            if (p.x < minX) minX = p.x;
            if (p.x > maxX) maxX = p.x;
            if (p.y < minY) minY = p.y;
            if (p.y > maxY) maxY = p.y;
        }
    }

    // 添加一些边距到数据范围
    double rangeX = maxX - minX;
    double rangeY = maxY - minY;
    minX -= rangeX * 0.1;
    maxX += rangeX * 0.1;
    minY -= rangeY * 0.1;
    maxY += rangeY * 0.1;
    rangeX = maxX - minX;
    rangeY = maxY - minY;

    // 坐标转换函数
    auto transformPoint = [&](const Point& p) -> sf::Vector2f {
        float x = margin + (p.x - minX) / rangeX * (windowWidth - 2 * margin);
        float y = windowHeight - margin - (p.y - minY) / rangeY * (windowHeight - 2 * margin);
        return sf::Vector2f(x, y);
        };

    // 创建点形状
    vector<sf::CircleShape> pointShapes;
    for (const auto& p : points) {
        sf::CircleShape pointShape(4);
        pointShape.setFillColor(sf::Color::White);
        auto pos = transformPoint(p);
        pointShape.setPosition(pos.x - 4, pos.y - 4);
        pointShapes.push_back(pointShape);
    }

    // 创建凸包形状
    sf::VertexArray hullLines(sf::LineStrip);
    for (const auto& p : hull) {
        hullLines.append(sf::Vertex(transformPoint(p), sf::Color::Green));
    }
    // 闭合凸包
    if (!hull.empty()) {
        hullLines.append(sf::Vertex(transformPoint(hull[0]), sf::Color::Green));
    }

    // 创建凸包点形状
    vector<sf::CircleShape> hullPointShapes;
    for (const auto& p : hull) {
        sf::CircleShape pointShape(6);
        pointShape.setFillColor(sf::Color::Red);
        auto pos = transformPoint(p);
        pointShape.setPosition(pos.x - 6, pos.y - 6);
        hullPointShapes.push_back(pointShape);
    }

    // 创建坐标轴
    sf::VertexArray axes(sf::Lines);
    // X轴
    axes.append(sf::Vertex(sf::Vector2f(margin, windowHeight - margin), sf::Color::Blue));
    axes.append(sf::Vertex(sf::Vector2f(windowWidth - margin, windowHeight - margin), sf::Color::Blue));
    // Y轴
    axes.append(sf::Vertex(sf::Vector2f(margin, windowHeight - margin), sf::Color::Blue));
    axes.append(sf::Vertex(sf::Vector2f(margin, margin), sf::Color::Blue)); // 修正了这里的错误

    // 创建信息文本
    sf::Font font;
    sf::Text infoText;
    // 尝试加载字体，如果失败则使用默认字体
    if (!font.loadFromFile("arial.ttf")) {
        // 如果arial.ttf不存在，尝试使用系统默认字体
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            // 如果还是失败，创建一个空字体对象
            font = sf::Font();
        }
    }

    infoText.setFont(font);
    infoText.setCharacterSize(16);
    infoText.setFillColor(sf::Color::Yellow);
    infoText.setPosition(10, 10);
    infoText.setString("Points: " + to_string(points.size()) +
        "\nHull Points: " + to_string(hull.size()) +
        "\nPress ESC to exit");

    // 主循环
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();
        }

        window.clear(sf::Color::Black);

        // 绘制坐标轴
        window.draw(axes);

        // 绘制凸包
        window.draw(hullLines);

        // 绘制点
        for (const auto& point : pointShapes) {
            window.draw(point);
        }

        // 绘制凸包点
        for (const auto& point : hullPointShapes) {
            window.draw(point);
        }

        // 绘制信息文本
        window.draw(infoText);

        window.display();
    }
}

int main() {
    // 设置随机种子
    srand(time(nullptr));

    cout << "2D Convex Hull Calculator - Gift Wrapping Algorithm" << endl;
    cout << string(50, '=') << endl;

    cout << "Random Point Set" << endl;
    auto randomPoints = generateRandomPoints(20, 0, 100, 0, 100);
    displayPoints(randomPoints, "Random Points");

    auto randomHull = giftWrapping(randomPoints);
    displayConvexHull(randomHull);

    // 可视化凸包
    cout << "Opening visualization window..." << endl;
    cout << "Press ESC or close the window to return to console" << endl;
    visualizeConvexHull(randomPoints, randomHull, "Convex Hull Visualization - Gift Wrapping");

    return 0;
}