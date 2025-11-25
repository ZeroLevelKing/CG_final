#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>

// CGAL头文件
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_3.h>
#include <CGAL/Polyhedron_3.h>

// 使用CGAL内核和点类型
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_3 Point_3;
typedef CGAL::Polyhedron_3<K> Polyhedron_3;

// 全局设置
namespace Config {
    const bool EXPORT_TECPLOT = true;
    const std::string DEFAULT_PATH = "../../protein/2HHB.pdb";
}

// 函数声明
std::string getFilePathFromUser();
std::vector<Point_3> readPDB(const std::string& filename);
void exportToTecplot(const std::vector<Point_3>& points, const std::string& filename = "../../result/protein_tecplot.dat");
std::vector<Point_3> computeConvexHull(const std::vector<Point_3>& points);
void exportConvexHullToTecplot(const std::vector<Point_3>& points,
    const std::vector<Point_3>& hullPoints,
    const std::string& filename = "../../result/convex_hull.dat");

int main() {
    // 获取文件路径
    std::string filepath = getFilePathFromUser();

    // 读取PDB文件
    auto points = readPDB(filepath);

    if (points.empty()) {
        std::cerr << "错误: 未能读取到任何有效数据" << std::endl;
        return 1;
    }

    std::cout << "成功读取点数: " << points.size() << std::endl;

    // 导出为Tecplot格式
    if (Config::EXPORT_TECPLOT) {
        exportToTecplot(points);
    }

    // 计算并导出凸包
    auto hullPoints = computeConvexHull(points);
    if (!hullPoints.empty()) {
        exportConvexHullToTecplot(points, hullPoints);
    }

    std::cout << "程序执行完成" << std::endl;
    return 0;
}

// 函数定义

std::string getFilePathFromUser() {
    std::string filepath;
    std::cout << "PDB文件路径 (回车使用 " << Config::DEFAULT_PATH << "): ";
    std::getline(std::cin, filepath);

    if (filepath.empty()) {
        filepath = Config::DEFAULT_PATH;
        std::cout << "使用默认路径: " << filepath << std::endl;
    }

    return filepath;
}

std::vector<Point_3> readPDB(const std::string& filename) {
    std::vector<Point_3> points;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "错误: 无法打开文件: " << filename << std::endl;
        return points;
    }

    std::string line;
    int count = 0;

    while (std::getline(file, line)) {
        if (line.substr(0, 4) == "ATOM" || line.substr(0, 6) == "HETATM") {
            if (line.length() >= 54) {
                try {
                    double x = std::stod(line.substr(30, 8));
                    double y = std::stod(line.substr(38, 8));
                    double z = std::stod(line.substr(46, 8));
                    points.push_back(Point_3(x, y, z));
                    count++;
                }
                catch (...) {
                    // 忽略解析错误
                }
            }
        }
    }

    file.close();
    std::cout << "成功解析原子数: " << count << std::endl;
    return points;
}

void exportToTecplot(const std::vector<Point_3>& points, const std::string& filename) {
    if (points.empty()) {
        std::cout << "点集为空，无法导出Tecplot格式" << std::endl;
        return;
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "错误: 无法创建Tecplot文件: " << filename << std::endl;
        return;
    }

    file << std::fixed;
    file.precision(6);

    // 计算质心
    Point_3 centroid(0, 0, 0);
    for (const auto& p : points) {
        centroid = Point_3(centroid.x() + p.x(), centroid.y() + p.y(), centroid.z() + p.z());
    }
    centroid = Point_3(centroid.x() / points.size(), centroid.y() / points.size(), centroid.z() / points.size());

    // 写入文件头
    file << "TITLE = \"Protein Atomic Coordinates\"" << std::endl;
    file << "VARIABLES = \"X\", \"Y\", \"Z\", \"Distance\", \"Atom_Index\"" << std::endl;
    file << "ZONE T=\"Atom Positions\"" << std::endl;
    file << "I=" << points.size() << ", J=1, K=1, ZONETYPE=Ordered" << std::endl;
    file << "DATAPACKING=POINT" << std::endl;

    // 写入数据点
    for (int i = 0; i < points.size(); i++) {
        const auto& p = points[i];
        double dx = p.x() - centroid.x();
        double dy = p.y() - centroid.y();
        double dz = p.z() - centroid.z();
        double distance = std::sqrt(dx * dx + dy * dy + dz * dz);

        file << p.x() << " " << p.y() << " " << p.z() << " " << distance << " " << i + 1 << std::endl;
    }

    file.close();
    std::cout << "Tecplot数据已导出到: " << filename << std::endl;
}

std::vector<Point_3> computeConvexHull(const std::vector<Point_3>& points) {
    if (points.size() < 4) {
        std::cout << "点数量不足，无法计算三维凸包（至少需要4个点）" << std::endl;
        return {};
    }

    std::cout << "使用CGAL计算三维凸包..." << std::endl;

    // 计算凸包
    Polyhedron_3 poly;
    CGAL::convex_hull_3(points.begin(), points.end(), poly);

    // 提取凸包顶点
    std::vector<Point_3> hullPoints;
    for (auto vit = poly.vertices_begin(); vit != poly.vertices_end(); ++vit) {
        hullPoints.push_back(vit->point());
    }

    std::cout << "凸包计算完成" << std::endl;
    std::cout << "凸包顶点数: " << hullPoints.size() << std::endl;
    std::cout << "凸包面数: " << poly.size_of_facets() << std::endl;

    return hullPoints;
}

void exportConvexHullToTecplot(const std::vector<Point_3>& points,
    const std::vector<Point_3>& hullPoints,
    const std::string& filename) {
    if (points.empty() || hullPoints.empty()) {
        std::cout << "点集为空，无法导出凸包数据" << std::endl;
        return;
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "错误: 无法创建凸包Tecplot文件: " << filename << std::endl;
        return;
    }

    file << std::fixed;
    file.precision(6);

    // 创建凸包点标记集合
    std::set<Point_3> hullSet(hullPoints.begin(), hullPoints.end());

    // 计算质心
    Point_3 centroid(0, 0, 0);
    for (const auto& p : points) {
        centroid = Point_3(centroid.x() + p.x(), centroid.y() + p.y(), centroid.z() + p.z());
    }
    centroid = Point_3(centroid.x() / points.size(), centroid.y() / points.size(), centroid.z() / points.size());

    // 写入文件头
    file << "TITLE = \"Protein Convex Hull Analysis\"" << std::endl;
    file << "VARIABLES = \"X\", \"Y\", \"Z\", \"Distance\", \"On_Hull\", \"Atom_Index\"" << std::endl;

    // 区域1：所有点，标记是否在凸包上
    file << "ZONE T=\"All Points with Hull Markers\"" << std::endl;
    file << "I=" << points.size() << ", J=1, K=1, ZONETYPE=Ordered" << std::endl;
    file << "DATAPACKING=POINT" << std::endl;

    for (int i = 0; i < points.size(); i++) {
        const auto& p = points[i];
        double dx = p.x() - centroid.x();
        double dy = p.y() - centroid.y();
        double dz = p.z() - centroid.z();
        double distance = std::sqrt(dx * dx + dy * dy + dz * dz);
        int onHull = (hullSet.find(p) != hullSet.end()) ? 1 : 0;

        file << p.x() << " " << p.y() << " " << p.z() << " " << distance << " " << onHull << " " << i + 1 << std::endl;
    }

    file << std::endl;

    // 区域2：只包含凸包顶点
    file << "ZONE T=\"Convex Hull Vertices Only\"" << std::endl;
    file << "I=" << hullPoints.size() << ", J=1, K=1, ZONETYPE=Ordered" << std::endl;
    file << "DATAPACKING=POINT" << std::endl;

    for (int i = 0; i < hullPoints.size(); i++) {
        const auto& p = hullPoints[i];
        double dx = p.x() - centroid.x();
        double dy = p.y() - centroid.y();
        double dz = p.z() - centroid.z();
        double distance = std::sqrt(dx * dx + dy * dy + dz * dz);

        file << p.x() << " " << p.y() << " " << p.z() << " " << distance << " " << 1 << " " << i + 1 << std::endl;
    }

    file.close();
    std::cout << "凸包Tecplot数据已导出到: " << filename << std::endl;
    std::cout << "凸包覆盖率: " << (100.0 * hullPoints.size() / points.size()) << "%" << std::endl;
}