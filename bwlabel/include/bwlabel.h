#ifndef BWLABEL_H
#define BWLABEL_H

#include <vector>
#include <tuple>
#include <map>

namespace qc{

    #ifndef QC_TYPEDEF
    #define QC_TYPEDEF
    typedef unsigned char uchar;
    typedef unsigned short int ushort;
    typedef unsigned int uint;
    typedef unsigned long int ulint;
    typedef const char cchar;
    typedef const int cint;
    typedef const unsigned int cuint;
    typedef const float cfloat;
    typedef const double cdouble;
    enum qcStatus{
        QC_SUCCESS  = 0,
        QC_FAIL     = -1,
    };
    #endif

    /*****************************
     * class   : 两阶段连通域标记算法--qc进化版
     * author  : qc
     * date    : 2019.10.17
     * email   : qcdq1314@aliyun.com
     * remarks : 边界宽度1像素不做处理
    ******************************/
    class FindContours_TwoPass final
    {
    public:
        // area, sum_x, sum_y, max_x, max_y, min_x, min_y, x, y, w, h
        /************************
         * function : 目标类型说明
         * 0        : 面积
         * 1        : x值的和
         * 2        : y值的和
         * 3        : x的最大值
         * 4        : y的最大值
         * 5        : x的最小值
         * 6        : y的最小值
         * 7        : x的平均值
         * 8        : y的平均值
         * 9        : 宽度
         * 10       : 高度
        ************************/
        using blob = std::tuple<uint, uint, uint, uint, uint, uint, uint, uint, uint, uint, uint>;

        FindContours_TwoPass() = default;

        /************************
         * function : 析构函数
         * explain  : 自动调用release函数
        ************************/
        ~FindContours_TwoPass();

        /************************
         * function : 初始化内存
         * rows     : 行
         * cols     : 列
         * return   : 0成功/-1失败
        ************************/
        int init(uint rows, uint cols);

        /************************
         * function : 释放内存
         * rows     : 行
         * cols     : 列
         * return   : 0成功/-1失败
        ************************/
        void release();

        /************************
         * function : 两阶段连通域标记算法--qc进化版
         * src      : 二值化数据
         * return   : 目标容器
        ************************/
        std::vector<blob> findContours(uchar *src);

    private:
        void sortBelong(std::map<uint, uint> &belongMap, std::map<uint, uint> &belongMap_new);

        uint rows = 0, cols = 0;
        uint *dst = nullptr;

    };
}
#endif // BWLABEL_H
