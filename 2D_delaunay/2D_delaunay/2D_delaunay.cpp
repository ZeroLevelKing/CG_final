#include <iostream>
#include <vector>
#include <chrono>
#include <limits>

#include "vector2.h"
#include "triangle.h"
#include "delaunay.h"
#include "io.h"
#include "visualize.h"

// 处理单个文件
void processFile(int fileNum, bool saveResults = true) {
    std::cout << "\n=== 处理文件 " << fileNum << " ===" << std::endl;

    // 构建文件路径
    std::string dataFile = "../data/" + std::to_string(fileNum) + ".txt";
    std::string resultDir = "../result/" + std::to_string(fileNum) + "/";

    // 如果上级目录不存在，尝试当前目录
    if (!FileIO::pathExists("../data")) {
        dataFile = "./data/" + std::to_string(fileNum) + ".txt";
        resultDir = "./result/" + std::to_string(fileNum) + "/";
        std::cout << "使用当前目录下的data文件夹" << std::endl;
    }

    // 检查数据文件是否存在
    if (!FileIO::pathExists(dataFile)) {
        std::cout << "数据文件不存在: " << dataFile << "，跳过" << std::endl;
        return;
    }

    // 读取点数据
    std::vector<Vector2<float>> points = FileIO::readPointsFromFile(dataFile);
    if (points.empty()) {
        std::cerr << "文件 " << fileNum << " 没有数据或读取失败，跳过" << std::endl;
        return;
    }

    if (saveResults) {
        // 保存原始点数据
        if (!FileIO::savePointsToFile(points, resultDir + "points_original.txt")) {
            std::cerr << "保存原始点数据失败" << std::endl;
        }
    }

    // 计时开始
    auto startTime = std::chrono::high_resolution_clock::now();

    // 进行Delaunay三角剖分
    std::cout << "开始三角剖分..." << std::endl;
    Delaunay<float> triangulation;
    const std::vector<Triangle<float>> triangles = triangulation.triangulate(points);
    const std::vector<Edge<float>> edges = triangulation.getEdges();

    // 计时结束
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "生成 " << triangles.size() << " 个三角形" << std::endl;
    std::cout << "生成 " << edges.size() << " 条边" << std::endl;
    std::cout << "计算耗时: " << duration.count() << " 毫秒" << std::endl;

    if (saveResults) {
        // 保存结果数据
        if (!FileIO::savePointsToFile(points, resultDir + "points_processed.txt")) {
            std::cerr << "保存处理后的点数据失败" << std::endl;
        }

        if (!FileIO::saveEdgesToFile(edges, resultDir + "edges.txt")) {
            std::cerr << "保存边数据失败" << std::endl;
        }

        if (!FileIO::saveTrianglesToFile(triangles, resultDir + "triangles.txt")) {
            std::cerr << "保存三角形数据失败" << std::endl;
        }

        // 保存统计信息
        if (!FileIO::saveStatisticsToFile(fileNum, points.size(), triangles.size(), edges.size(),
            duration.count(), "../result/statistics.csv")) {
            std::cerr << "保存统计信息失败" << std::endl;
        }
    }

    // 输出统计信息
    std::cout << "文件 " << fileNum << " 处理完成:" << std::endl;
    std::cout << "  - 点数: " << points.size() << std::endl;
    std::cout << "  - 三角形数: " << triangles.size() << std::endl;
    std::cout << "  - 边数: " << edges.size() << std::endl;
    std::cout << "  - 计算耗时: " << duration.count() << " 毫秒" << std::endl;

    if (saveResults) {
        std::cout << "  - 结果保存到: " << resultDir << std::endl;
    }
}

// 处理随机例子
void processRandomExample() {
    std::cout << "\n=== 处理随机例子 ===" << std::endl;

    // 生成随机点
    int pointCount;
    std::cout << "请输入要生成的随机点数: ";
    std::cin >> pointCount;

    // 检查输入是否有效
    if (std::cin.fail() || pointCount <= 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "输入无效，使用默认值50" << std::endl;
        pointCount = 50;
    }

    std::vector<Vector2<float>> points = FileIO::generateRandomPoints(pointCount);

    // 显示处理前的点
    std::cout << "显示处理前的点..." << std::endl;
    std::vector<Edge<float>> emptyEdges;
    Visualizer::visualizePointsAndEdges(points, emptyEdges, "处理前 - 随机例子");

    // 计时开始
    auto startTime = std::chrono::high_resolution_clock::now();

    // 进行Delaunay三角剖分
    std::cout << "开始三角剖分..." << std::endl;
    Delaunay<float> triangulation;
    const std::vector<Triangle<float>> triangles = triangulation.triangulate(points);
    const std::vector<Edge<float>> edges = triangulation.getEdges();

    // 计时结束
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "生成 " << triangles.size() << " 个三角形" << std::endl;
    std::cout << "生成 " << edges.size() << " 条边" << std::endl;
    std::cout << "计算耗时: " << duration.count() << " 毫秒" << std::endl;

    // 显示处理后的结果
    std::cout << "显示处理后的结果..." << std::endl;
    Visualizer::visualizePointsAndEdges(points, edges, "处理后 - 随机例子");

    // 输出统计信息
    std::cout << "随机例子处理完成:" << std::endl;
    std::cout << "  - 点数: " << points.size() << std::endl;
    std::cout << "  - 三角形数: " << triangles.size() << std::endl;
    std::cout << "  - 边数: " << edges.size() << std::endl;
    std::cout << "  - 计算耗时: " << duration.count() << " 毫秒" << std::endl;
}

// 显示菜单
void showMenu() {
    std::cout << "\n==========================================" << std::endl;
    std::cout << "          Delaunay三角剖分程序" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "请选择计算模式:" << std::endl;
    std::cout << "1. 处理10个文件（无可视化，只保存数据）" << std::endl;
    std::cout << "2. 随机生成例子并可视化" << std::endl;
    std::cout << "3. 重新生成1-10的数据文件" << std::endl;
    std::cout << "0. 退出程序" << std::endl;
    std::cout << "请输入选择 (0-3): ";
}

int main() {
    int choice;

    do {
        showMenu();
        std::cin >> choice;

        // 检查输入是否有效
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "输入无效，请重新输入" << std::endl;
            continue;
        }

        // 先创建顶层结果目录
        if (!FileIO::createDirectories("../result")) {
            std::cerr << "警告: 无法创建顶层结果目录 ../result/" << std::endl;
            // 尝试创建当前目录下的result
            if (!FileIO::createDirectories("./result")) {
                std::cerr << "错误: 无法创建任何结果目录" << std::endl;
                return 1;
            }
        }

        switch (choice) {
        case 0:
            std::cout << "感谢使用，再见！" << std::endl;
            break;

        case 1:
            std::cout << "开始处理10个数据文件..." << std::endl;

            // 处理10个数据文件
            for (int i = 1; i <= 10; ++i) {
                processFile(i);
            }

            std::cout << "\n=== 所有文件处理完成 ===" << std::endl;
            std::cout << "结果保存在 result/ 目录下" << std::endl;
            std::cout << "统计信息保存在 result/statistics.csv" << std::endl;
            break;

        case 2:
            processRandomExample();
            break;

        case 3:
            if (FileIO::generateDataFiles()) {
                std::cout << "数据文件生成成功！" << std::endl;
            }
            else {
                std::cerr << "数据文件生成失败！" << std::endl;
            }
            break;

        default:
            std::cout << "无效的选择，请重新输入" << std::endl;
            break;
        }

        // 如果不是退出，等待用户按键继续
        if (choice != 0) {
            std::cout << "\n按回车键继续...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
        }

    } while (choice != 0);

    return 0;
}