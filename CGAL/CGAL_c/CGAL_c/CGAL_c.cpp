#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>

// CGAL头文件
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Alpha_shape_3.h>
#include <CGAL/Alpha_shape_cell_base_3.h>
#include <CGAL/Alpha_shape_vertex_base_3.h>
#include <CGAL/Delaunay_triangulation_3.h>

// 使用CGAL内核和点类型
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_3 Point_3;
typedef K::FT FT;

// Alpha Shape相关类型定义
typedef CGAL::Alpha_shape_vertex_base_3<K> Vb;
typedef CGAL::Alpha_shape_cell_base_3<K> Fb;
typedef CGAL::Triangulation_data_structure_3<Vb, Fb> Tds;
typedef CGAL::Delaunay_triangulation_3<K, Tds> Triangulation_3;
typedef CGAL::Alpha_shape_3<Triangulation_3> Alpha_shape_3;

// 全局设置
namespace Config {
    const bool EXPORT_TECPLOT = false;
    const std::string DEFAULT_PATH = "../../protein/2HHB.pdb";
    const double ALPHA_VALUE = 10.0; // Alpha Shape参数
}

// 函数声明
std::string getFilePathFromUser();
std::vector<Point_3> readPDB(const std::string& filename);
void exportToTecplot(const std::vector<Point_3>& points, const std::string& filename = "../../result/protein_tecplot.dat");
std::vector<Point_3> computeAlphaShape(const std::vector<Point_3>& points);
void exportAlphaShapeToTecplot(const std::vector<Point_3>& points,
    const std::vector<Point_3>& alphaShapePoints,
    const std::string& filename = "../../result/alpha_shape.dat");

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

    // 计算并导出Alpha Shape
    auto alphaShapePoints = computeAlphaShape(points);
    if (!alphaShapePoints.empty()) {
        exportAlphaShapeToTecplot(points, alphaShapePoints);
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

std::vector<Point_3> computeAlphaShape(const std::vector<Point_3>& points) {
    if (points.size() < 4) {
        std::cout << "点数量不足，无法计算Alpha Shape（至少需要4个点）" << std::endl;
        return {};
    }

    std::cout << "使用CGAL计算Alpha Shape，alpha = " << Config::ALPHA_VALUE << "..." << std::endl;

    // 创建Alpha Shape对象
    Alpha_shape_3 alpha_shape(points.begin(), points.end());

    // 设置alpha值
    FT alpha = Config::ALPHA_VALUE;
    alpha_shape.set_alpha(alpha);

    // 提取Alpha Shape边界上的点
    std::vector<Point_3> alphaShapePoints;

    // 获取所有Alpha Shape面
    std::vector<Alpha_shape_3::Facet> facets;
    alpha_shape.get_alpha_shape_facets(std::back_inserter(facets), Alpha_shape_3::REGULAR);

    // 收集所有边界点
    std::set<Point_3> uniquePoints;
    for (const auto& facet : facets) {
        // 获取面的顶点
        Alpha_shape_3::Vertex_handle vh[3];
        if (alpha_shape.classify(facet) == Alpha_shape_3::REGULAR) {
            Alpha_shape_3::Cell_handle ch = facet.first;
            int index = facet.second;
            for (int i = 0; i < 3; i++) {
                vh[i] = ch->vertex((index + i + 1) % 4);
                uniquePoints.insert(vh[i]->point());
            }
        }
    }

    // 转换为向量
    alphaShapePoints.assign(uniquePoints.begin(), uniquePoints.end());

    std::cout << "Alpha Shape计算完成" << std::endl;
    std::cout << "Alpha Shape边界点数: " << alphaShapePoints.size() << std::endl;
    std::cout << "Alpha Shape面数: " << facets.size() << std::endl;

    return alphaShapePoints;
}

void exportAlphaShapeToTecplot(const std::vector<Point_3>& points,
    const std::vector<Point_3>& alphaShapePoints,
    const std::string& filename) {
    if (points.empty() || alphaShapePoints.empty()) {
        std::cout << "点集为空，无法导出Alpha Shape数据" << std::endl;
        return;
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "错误: 无法创建Alpha Shape Tecplot文件: " << filename << std::endl;
        return;
    }

    file << std::fixed;
    file.precision(6);

    // 创建Alpha Shape点标记集合
    std::set<Point_3> alphaShapeSet(alphaShapePoints.begin(), alphaShapePoints.end());

    // 计算质心
    Point_3 centroid(0, 0, 0);
    for (const auto& p : points) {
        centroid = Point_3(centroid.x() + p.x(), centroid.y() + p.y(), centroid.z() + p.z());
    }
    centroid = Point_3(centroid.x() / points.size(), centroid.y() / points.size(), centroid.z() / points.size());

    // 写入文件头
    file << "TITLE = \"Protein Alpha Shape Analysis\"" << std::endl;
    file << "VARIABLES = \"X\", \"Y\", \"Z\", \"Distance\", \"On_Alpha_Shape\", \"Atom_Index\"" << std::endl;

    // 区域1：所有点，标记是否在Alpha Shape边界上
    file << "ZONE T=\"All Points with Alpha Shape Markers\"" << std::endl;
    file << "I=" << points.size() << ", J=1, K=1, ZONETYPE=Ordered" << std::endl;
    file << "DATAPACKING=POINT" << std::endl;

    for (int i = 0; i < points.size(); i++) {
        const auto& p = points[i];
        double dx = p.x() - centroid.x();
        double dy = p.y() - centroid.y();
        double dz = p.z() - centroid.z();
        double distance = std::sqrt(dx * dx + dy * dy + dz * dz);
        int onAlphaShape = (alphaShapeSet.find(p) != alphaShapeSet.end()) ? 1 : 0;

        file << p.x() << " " << p.y() << " " << p.z() << " " << distance << " " << onAlphaShape << " " << i + 1 << std::endl;
    }

    file << std::endl;

    // 区域2：只包含Alpha Shape边界点
    file << "ZONE T=\"Alpha Shape Boundary Points Only\"" << std::endl;
    file << "I=" << alphaShapePoints.size() << ", J=1, K=1, ZONETYPE=Ordered" << std::endl;
    file << "DATAPACKING=POINT" << std::endl;

    for (int i = 0; i < alphaShapePoints.size(); i++) {
        const auto& p = alphaShapePoints[i];
        double dx = p.x() - centroid.x();
        double dy = p.y() - centroid.y();
        double dz = p.z() - centroid.z();
        double distance = std::sqrt(dx * dx + dy * dy + dz * dz);

        file << p.x() << " " << p.y() << " " << p.z() << " " << distance << " " << 1 << " " << i + 1 << std::endl;
    }

    file.close();
    std::cout << "Alpha Shape Tecplot数据已导出到: " << filename << std::endl;
    std::cout << "Alpha Shape边界点覆盖率: " << (100.0 * alphaShapePoints.size() / points.size()) << "%" << std::endl;
}