#pragma once
#include "sc_point.h"
#include "sc_deformation.h"

namespace StudyCorr
{
    // 2D 变形向量
    struct DeformationVector2D
    {
        float u, ux, uy, uxx, uxy, uyy;
        float v, vx, vy, vxx, vxy, vyy;

        DeformationVector2D() : u(0), ux(0), uy(0), uxx(0), uxy(0), uyy(0),
            v(0), vx(0), vy(0), vxx(0), vxy(0), vyy(0) {
        }
    };

    // 2D 应变向量
    struct StrainVector2D
    {
        float exx, eyy, exy;

        StrainVector2D() : exx(0), eyy(0), exy(0) {}
    };

    // 2D 结果向量
    struct Result2D
    {
        float u0, v0, zncc, iteration, convergence, feature;

        Result2D() : u0(0), v0(0), zncc(0), iteration(0), convergence(0), feature(0) {}
    };

    // 立体 2D 结果向量
    struct Result2DS
    {
        float r1r2_zncc, r1t1_zncc, r1t2_zncc, r2_x, r2_y, t1_x, t1_y, t2_x, t2_y;

        Result2DS() : r1r2_zncc(0), r1t1_zncc(0), r1t2_zncc(0), r2_x(0), r2_y(0),
            t1_x(0), t1_y(0), t2_x(0), t2_y(0) {
        }
    };

    // 3D 变形向量
    struct DeformationVector3D
    {
        float u, ux, uy, uz;
        float v, vx, vy, vz;
        float w, wx, wy, wz;

        DeformationVector3D() : u(0), ux(0), uy(0), uz(0),
            v(0), vx(0), vy(0), vz(0),
            w(0), wx(0), wy(0), wz(0) {
        }
    };

    // 3D 位移向量
    struct DisplacementVector3D
    {
        float u, v, w;

        DisplacementVector3D() : u(0), v(0), w(0) {}
    };

    // 3D 应变向量
    struct StrainVector3D
    {
        float exx, eyy, ezz;
        float exy, eyz, ezx;

        StrainVector3D() : exx(0), eyy(0), ezz(0), exy(0), eyz(0), ezx(0) {}
    };

    // 3D 结果向量
    struct Result3D
    {
        float u0, v0, w0, zncc, iteration, convergence, feature;

        Result3D() : u0(0), v0(0), w0(0), zncc(0), iteration(0), convergence(0), feature(0) {}
    };

    // 2D DIC 的 POI 类
    class POI2D : public Point2D
    {
    public:
        DeformationVector2D deformation;
        Result2D result;
        StrainVector2D strain;
        Point2D subset_radius;

        POI2D(int x, int y);
        POI2D(float x, float y);
        POI2D(Point2D location);
        ~POI2D();

        // 重置除位置外的数据
        void clear();
    };

    // 立体 DIC 的 POI 类
    class POI2DS : public Point2D
    {
    public:
        DisplacementVector3D deformation;
        Result2DS result;
        Point3D ref_coor, tar_coor;
        StrainVector3D strain;
        Point2D subset_radius;

        POI2DS(int x, int y);
        POI2DS(float x, float y);
        POI2DS(Point2D location);
        ~POI2DS();

        // 重置除位置外的数据
        void clear();
    };

    // DVC 的 POI 类
    class POI3D : public Point3D
    {
    public:
        DeformationVector3D deformation;
        Result3D result;
        StrainVector3D strain;
        Point3D subset_radius;

        POI3D(int x, int y, int z);
        POI3D(float x, float y, float z);
        POI3D(Point3D location);
        ~POI3D();

        // 重置除位置外的数据
        void clear();
    };
}

