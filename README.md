# 计算几何作业

## 项目概述

本作业旨在通过使用计算几何库CGAL，完成对蛋白质结构的凸包和alpha shape的计算，并实现2D凸包和Delaunay剖分算法。项目内容包括CGAL的安装与配置、蛋白质凸包计算、alpha shape分析以及自定义2D算法的实现与测试。

## 系统要求

- **操作系统**：Windows
- **编译环境**：C++17 Visual Studio
- **依赖库**：CGAL、SFML（用于可视化，已附带在作业文件中）

## 安装说明

### CGAL安装步骤（Windows系统）

本作业依赖CGAL库，请按以下步骤安装：

1. 安装Git到Windows系统
2. 使用Git克隆vcpkg仓库：
   bash
   git clone https://github.com/Microsoft/vcpkg.git

3. 进入vcpkg目录，运行批处理程序：
   bash
   bootstrap-vcpkg.bat

4. 使用vcpkg安装CGAL：
   bash
   .\vcpkg install cgal

5. 在Visual Studio中配置项目时，将C++标准设置为C++17或更高

### 依赖库配置

- SFML库已附带在作业文件中，无需额外安装
- 如需运行蛋白质可视化部分，建议安装ChimeraX（可选，用于查看PDB文件）

## 文件结构

```
项目根目录/

├── CGAL/                    # 包含两个使用CGAL库实现的程序
│   ├── CGAL_b/              # 3D凸包计算代码（使用CGAL）
│   ├── CGAL_c/              # 3D alpha shape计算代码（使用CGAL）
│   ├── protein/             # 存放蛋白质数据
│   └── result/              # 存放 .dat 文件，可以使用tectplot等可视化软件查看
├── 2D_convex/               # 2D凸包计算程序，SFML库文件已经包含在内
├── 2D_delaunay/             # 2D delaunay剖分计算程序，SFML库文件已包含在内
│   ├── result/              # 存放对于十组数据计算结果的文件夹
│   ├── data/                # 十组数据
│   └── ...                  # 源码和其余文件
├── report.pdf               # 作业报告
└── README.md                # 本文件
```

## 使用方法

### 蛋白质凸包和Alpha Shape计算

- **输入文件**：将蛋白质PDB文件放入`Protein/`目录（示例使用`2HHB.pdb`）
- **运行代码**：
  - 编译并运行`CGAL/CGAL_b/CGAL_b/CGAL_b.cpp`计算凸包
  - 编译并运行`CGAL/CGAL_c/CGAL_c/CGAL_c.cpp`计算alpha shape（alpha值默认为10.0）
- **输出**：结果保存为Tecplot格式文件，可在`CGAL/result/`目录查看
- **可视化**：使用Tecplot或类似工具查看点集和凸包结果

示例蛋白质2HHB（血红蛋白）的点集可视化结果可参考报告中的描述，凸包计算标记了凸包顶点，alpha shape计算显示了边界点。

### 2D凸包算法（卷包裹法）

- **代码文件**：`2D_convex/2D_convex/2D_convex.cpp`
- **算法描述**：采用卷包裹法（Gift Wrapping Algorithm），时间复杂度为O(nh)，其中n为点数，h为凸包顶点数。算法步骤包括选取x坐标最小点作为起点，迭代选择叉积为正的点形成凸包
- **运行**：编译后运行，程序使用SFML进行可视化，显示凸包多边形
- **结果**：生成凸包顶点集合，可视化结果由程序当场生成

### 2D Delaunay剖分算法（Bowyer-Watson）

- **代码文件**：`2D_delaunay/2D_delaunay/`目录下.h文件和.cpp文件
- **算法描述**：使用Bowyer-Watson算法，基于"空圆准则"实现三角剖分。算法通过逐点插入、坏三角形删除和新三角形生成步骤完成
- **时间复杂度**：最坏情况O(n²)，平均情况O(n log n)到O(n√n)
- **运行**：编译后运行，SFML可视化三角网
- **测试数据**：10组点集数据在`2D_delaunay/result/`目录下，结果可参考报告中的可视化描述

## 注意事项

- **CGAL安装**：务必先完成CGAL安装，否则3D计算代码无法运行
- **蛋白质数据**：如需计算其他蛋白质，请将PDB文件放入`Protein/`目录并修改代码中的文件路径。蛋白质列表包括胰岛素（1ZNI）、肌红蛋白（1MBN）等，详见报告中的表1
- **可视化**：2D算法使用SFML实时显示结果；3D结果建议使用Tecplot后处理
- **代码调试**：如果编译错误，请检查CGAL和SFML的路径配置及C++标准设置

## 结果展示

完整测试结果和输出文件请参考`results/`目录。蛋白质原始结构可视化示例（如2HHB）及算法结果详见报告中的文字描述和图表说明。

## 参考文献

- RCSB PDB数据库：https://www.rcsb.org/
- CGAL官方文档：参考安装步骤中的vcpkg指南

---

**朱林-2200011028**，如有问题请联系