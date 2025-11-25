#ifndef IO_H
#define IO_H

#include "vector2.h"
#include "edge.h"
#include "triangle.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <random>
#ifdef _WIN32
#include <direct.h>
#endif

class FileIO {
public:
    // 跨平台目录创建函数
    static bool createDirectory(const std::string& path) {
#ifdef _WIN32
        return _mkdir(path.c_str()) == 0;
#else
        return mkdir(path.c_str(), 0755) == 0;
#endif
    }

    // 递归创建目录
    static bool createDirectories(const std::string& path) {
        // 先检查目录是否已存在
        struct stat info;
        if (stat(path.c_str(), &info) == 0) {
            if (info.st_mode & S_IFDIR) {
                return true;
            }
            else {
                std::cerr << "路径存在但不是目录: " << path << std::endl;
                return false;
            }
        }

        // 递归创建父目录
        size_t pos = path.find_last_of('/');
        if (pos != std::string::npos) {
            std::string parent = path.substr(0, pos);
            if (!parent.empty() && !createDirectories(parent)) {
                return false;
            }
        }

        // 创建当前目录
        return createDirectory(path);
    }

    // 检查文件或目录是否存在
    static bool pathExists(const std::string& path) {
        struct stat info;
        return stat(path.c_str(), &info) == 0;
    }

    // 读取点数据从文件
    static std::vector<Vector2<float>> readPointsFromFile(const std::string& filename) {
        std::vector<Vector2<float>> points;

        if (!pathExists(filename)) {
            std::cerr << "文件不存在: " << filename << std::endl;
            return points;
        }

        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "无法打开文件: " << filename << std::endl;
            return points;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            float x, y;
            if (iss >> x >> y) {
                points.push_back(Vector2<float>(x, y));
            }
        }

        file.close();
        std::cout << "从 " << filename << " 读取了 " << points.size() << " 个点" << std::endl;
        return points;
    }

    // 保存点到文件
    static bool savePointsToFile(const std::vector<Vector2<float>>& points, const std::string& filename) {
        // 确保目录存在
        size_t lastSlash = filename.find_last_of('/');
        if (lastSlash != std::string::npos) {
            std::string dir = filename.substr(0, lastSlash);
            if (!createDirectories(dir)) {
                std::cerr << "无法创建目录: " << dir << std::endl;
                return false;
            }
        }

        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "无法创建文件: " << filename << std::endl;
            return false;
        }

        for (const auto& p : points) {
            file << p.x << " " << p.y << "\n";
        }

        file.close();
        std::cout << "点数据保存到: " << filename << std::endl;
        return true;
    }

    // 保存边到文件
    static bool saveEdgesToFile(const std::vector<Edge<float>>& edges, const std::string& filename) {
        // 确保目录存在
        size_t lastSlash = filename.find_last_of('/');
        if (lastSlash != std::string::npos) {
            std::string dir = filename.substr(0, lastSlash);
            if (!createDirectories(dir)) {
                std::cerr << "无法创建目录: " << dir << std::endl;
                return false;
            }
        }

        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "无法创建文件: " << filename << std::endl;
            return false;
        }

        for (const auto& e : edges) {
            file << e.p1.x << " " << e.p1.y << " " << e.p2.x << " " << e.p2.y << "\n";
        }

        file.close();
        std::cout << "边数据保存到: " << filename << std::endl;
        return true;
    }

    // 保存三角形到文件
    static bool saveTrianglesToFile(const std::vector<Triangle<float>>& triangles, const std::string& filename) {
        // 确保目录存在
        size_t lastSlash = filename.find_last_of('/');
        if (lastSlash != std::string::npos) {
            std::string dir = filename.substr(0, lastSlash);
            if (!createDirectories(dir)) {
                std::cerr << "无法创建目录: " << dir << std::endl;
                return false;
            }
        }

        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "无法创建文件: " << filename << std::endl;
            return false;
        }

        for (const auto& t : triangles) {
            file << t.p1.x << " " << t.p1.y << " "
                << t.p2.x << " " << t.p2.y << " "
                << t.p3.x << " " << t.p3.y << "\n";
        }

        file.close();
        std::cout << "三角形数据保存到: " << filename << std::endl;
        return true;
    }

    // 保存统计信息到文件
    static bool saveStatisticsToFile(int fileNum, int pointCount, int triangleCount, int edgeCount,
        long long duration, const std::string& filename) {
        // 确保目录存在
        size_t lastSlash = filename.find_last_of('/');
        if (lastSlash != std::string::npos) {
            std::string dir = filename.substr(0, lastSlash);
            if (!createDirectories(dir)) {
                std::cerr << "无法创建目录: " << dir << std::endl;
                return false;
            }
        }

        std::ofstream file(filename, std::ios::app); // 追加模式
        if (!file.is_open()) {
            std::cerr << "无法创建文件: " << filename << std::endl;
            return false;
        }

        // 如果是第一次写入，添加标题行
        static bool firstWrite = true;
        if (firstWrite) {
            file << "文件编号,点数,三角形数,边数,计算耗时(毫秒)\n";
            firstWrite = false;
        }

        file << fileNum << "," << pointCount << "," << triangleCount << ","
            << edgeCount << "," << duration << "\n";

        file.close();
        std::cout << "统计信息保存到: " << filename << std::endl;
        return true;
    }

    // 生成随机二维点
    static float RandomFloat(float a, float b) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(a, b);
        return dis(gen);
    }

    // 生成随机点集
    static std::vector<Vector2<float>> generateRandomPoints(int count) {
        std::vector<Vector2<float>> points;

        for (int i = 0; i < count; ++i) {
            points.push_back(Vector2<float>(RandomFloat(0, 800), RandomFloat(0, 600)));
        }

        std::cout << "生成了 " << points.size() << " 个随机点" << std::endl;
        return points;
    }

    // 生成1-10的数据文件
    static bool generateDataFiles() {
        std::cout << "开始生成1-10的数据文件..." << std::endl;

        // 确保data目录存在
        if (!createDirectories("../data")) {
            std::cerr << "无法创建data目录" << std::endl;
            return false;
        }

        // 为每个文件生成不同数量的点
        std::vector<int> pointCounts = { 20, 30, 40, 50, 60, 70, 80, 90, 100, 110 };

        for (int i = 1; i <= 10; ++i) {
            std::string filename = "../data/" + std::to_string(i) + ".txt";
            int pointCount = pointCounts[i - 1];

            std::vector<Vector2<float>> points = generateRandomPoints(pointCount);

            if (!savePointsToFile(points, filename)) {
                std::cerr << "生成文件 " << i << " 失败" << std::endl;
                return false;
            }

            std::cout << "生成文件 " << i << " 完成，包含 " << pointCount << " 个点" << std::endl;
        }

        std::cout << "所有数据文件生成完成" << std::endl;
        return true;
    }
};

#endif