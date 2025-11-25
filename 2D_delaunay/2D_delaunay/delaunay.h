#ifndef H_DELAUNAY
#define H_DELAUNAY

#include "vector2.h"
#include "edge.h"
#include "triangle.h"

#include <vector>
#include <algorithm>

template <class T>
class Delaunay
{
public:
    using TriangleType = Triangle<T>;
    using EdgeType = Edge<T>;
    using VertexType = Vector2<T>;

    //Deluanay 三角剖分核心算法  ---  逐点插入法
    const std::vector<TriangleType>& triangulate(std::vector<VertexType>& vertices)
    {
        // 将点云拷贝一份
        _vertices = vertices;

        // 计算超级三角形的一些参数
        T minX = vertices[0].x;
        T minY = vertices[0].y;
        T maxX = minX;
        T maxY = minY;

        //计算超级三角形的上下左右边界
        for (std::size_t i = 0; i < vertices.size(); ++i)
        {
            if (vertices[i].x < minX) minX = vertices[i].x;
            if (vertices[i].y < minY) minY = vertices[i].y;
            if (vertices[i].x > maxX) maxX = vertices[i].x;
            if (vertices[i].y > maxY) maxY = vertices[i].y;
        }

        const T dx = maxX - minX;
        const T dy = maxY - minY;
        const T deltaMax = std::max(dx, dy);
        const T midx = half(minX + maxX);
        const T midy = half(minY + maxY);

        //尽可能扩大超级三角形范围，可以取比20更大的数字
        const VertexType p1(midx - 20 * deltaMax, midy - deltaMax);
        const VertexType p2(midx, midy + 20 * deltaMax);
        const VertexType p3(midx + 20 * deltaMax, midy - deltaMax);

        //std::cout << "Super triangle " << std::endl << Triangle(p1, p2, p3) << std::endl;

        // Create a list of triangles, and add the supertriangle in it
        //将超级三角形添加至 _triangles
        _triangles.push_back(TriangleType(p1, p2, p3));

        //开始依次遍历每个点
        for (auto p = begin(vertices); p != end(vertices); p++)
        {
            //std::cout << "Traitement du point " << *p << std::endl;
            //std::cout << "_triangles contains " << _triangles.size() << " elements" << std::endl;

            //构造变量用来存储临时新产生的边
            std::vector<EdgeType> polygon;

            //依次遍历 _triangles  中的每个三角形
            for (auto& t : _triangles)
            {
                //std::cout << "Processing " << std::endl << *t << std::endl;

                if (t.circumCircleContains(*p))  //如果包含点 p，那么就要产生新的3条边
                {
                    //std::cout << "Pushing bad triangle " << *t << std::endl;
                    t.isBad = true;  //flag 发生改变，准备接下来在 _triangles  中将其剔除
                    polygon.push_back(t.e1);
                    polygon.push_back(t.e2);
                    polygon.push_back(t.e3);
                }
                else
                {
                    //std::cout << " does not contains " << *p << " in his circum center" << std::endl;
                }
            }
            //更新 _triangles
            _triangles.erase(std::remove_if(begin(_triangles), end(_triangles), [](TriangleType& t) {
                return t.isBad;
                }), end(_triangles));   //std::remove_if只移动不删除，erase将其删除，这里还用到了C++11的 lambda 表达式

            for (auto e1 = begin(polygon); e1 != end(polygon); ++e1)  //这个用来删除重复的边
            {
                for (auto e2 = e1 + 1; e2 != end(polygon); ++e2)
                {
                    if (almost_equal(*e1, *e2))
                    {
                        e1->isBad = true;
                        e2->isBad = true;
                    }
                }
            }

            //更新 polygon
            polygon.erase(std::remove_if(begin(polygon), end(polygon), [](EdgeType& e) {
                return e.isBad;
                }), end(polygon));

            for (const auto e : polygon)
                _triangles.push_back(TriangleType(e.p1, e.p2, *p));

        }

        //删除超级三角形
        _triangles.erase(std::remove_if(begin(_triangles), end(_triangles), [p1, p2, p3](TriangleType& t) {
            return t.containsVertex(p1) || t.containsVertex(p2) || t.containsVertex(p3);
            }), end(_triangles));

        for (const auto t : _triangles)
        {
            _edges.push_back(t.e1);
            _edges.push_back(t.e2);
            _edges.push_back(t.e3);
        }

        return _triangles;
    }

    const std::vector<TriangleType>& getTriangles() const { return _triangles; }
    const std::vector<EdgeType>& getEdges() const { return _edges; }
    const std::vector<VertexType>& getVertices() const { return _vertices; }

private:
    std::vector<TriangleType> _triangles;
    std::vector<EdgeType> _edges;
    std::vector<VertexType> _vertices;
};
#endif
