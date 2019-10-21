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
     * class   : 两阶段连通域标记算法--qc
     * author  : qc
     * date    : 2019.10.19
     * email   : qcdq1314@aliyun.com
     * remarks : 边界宽度1像素不做处理
    ******************************/
    class FindContours_TwoPass final
    {
    public:
        // area, max_x, max_y, min_x, min_y, x, y, w, h
        /************************
         * function : 目标类型说明
         * 0        : 面积
         * 1        : x的最大值
         * 2        : y的最大值
         * 3        : x的最小值
         * 4        : y的最小值
         * 5        : x的平均值
         * 6        : y的平均值
         * 7        : 宽度
         * 8        : 高度
        ************************/
        using blob = std::tuple<uint, uint, uint, uint, uint, uint, uint, uint, uint>;

        FindContours_TwoPass() = default;

        /************************
         * function : 析构函数
         * explain  : 自动调用release函数
        ************************/
        ~FindContours_TwoPass();

        /************************
         * function   : 初始化内存
         * rows       : 行
         * cols       : 列
         * return     : 0成功/-1失败
        ************************/
        int init(const uint &rows, const uint &cols);

        /************************
         * function : 释放内存
         * return   : 0成功/-1失败
        ************************/
        void release();

        /************************
         * function : 两阶段连通域标记算法--qc
         * src      : 二值化数据
         * return   : 目标容器
        ************************/
        std::vector<blob> findContours(uchar *src);

    private:
        uint rows = 0, cols = 0, cutRows = 0, cutCols = 0;
        uint *dst = nullptr;
        std::map<uint, uint> belongMap; // 归属表, 大的标记值属于小的标记值
    };
}
#endif // BWLABEL_H
