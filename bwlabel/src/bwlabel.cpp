#include "../include/bwlabel.h"
#include <iostream>
#include <memory.h>

qc::FindContours_TwoPass::~FindContours_TwoPass()
{
    release();
}

int qc::FindContours_TwoPass::init(uint rows, uint cols)
{
    this->rows = rows;
    this->cols = cols;
    dst = new (std::nothrow) uint[rows * cols];
    if(!dst)
        return QC_FAIL;

    return QC_SUCCESS;
}

void qc::FindContours_TwoPass::release()
{
    if(dst){
        delete [] dst;
        dst = nullptr;
    }
}

std::vector<qc::FindContours_TwoPass::blob> qc::FindContours_TwoPass::findContours(uchar *src)
{
    memset(dst, 0, static_cast<size_t>(rows * cols) * sizeof(uint));

    uint index = 0, index_wait_add = 0;
    uint cur = 0, top = 0, left = 0;
    uint number = 0;
    uint cutRows = rows - 1; // 边界处不计算，提高效率
    uint cutCols = cols - 1; // 边界处不计算，提高效率
    std::map<uint, uint> belongMap; // 归属表, 大的标记值属于小的标记值
    std::vector<blob> blobs;

    for(uint i = 1; i < cutRows; ++i){
        for(uint j = 1; j < cutCols; ++j){
            // 中间行
            cur = i * cols + j;
            top = (i - 1) * cols + j;
            left = i * cols + j - 1;
            if(src[cur]){
                // 当前位置有值
                if(!src[left] && !src[top]){
                    // 左侧和上方没有值，标记此处，标记+1
                    blobs.push_back(std::make_tuple(0, 0, 0, 0, 0, cutCols, cutRows, 0, 0, 0, 0));
                    ++number;
                    dst[cur] = number;
                }else if(src[left] && src[top]){
                    // 左侧和上方都有值，标记此处，标记为二者中的最小值, 且相邻的大值归属为小值, 相等则无归属关系
                    if(dst[left] > dst[top]){
                        dst[cur] = dst[top];
                        belongMap[dst[left]] = dst[top];
                    }else if(dst[left] < dst[top]){
                        dst[cur] = dst[left];
                        belongMap[dst[top]] = dst[left];
                    }else{
                        dst[cur] = dst[left];
                    }
                }else{
                    // 左侧和上方只有一处有值, 标记此处
                    dst[cur] = dst[top] | dst[left];
                }

                index = dst[cur] - 1;
                std::get<0>(blobs[index]) += 1; // sum
                std::get<1>(blobs[index]) += j; // sum_x
                std::get<2>(blobs[index]) += i; // sum_y
                if(std::get<3>(blobs[index]) < j)
                    std::get<3>(blobs[index]) = j; // max_x
                if(std::get<4>(blobs[index]) < i)
                    std::get<4>(blobs[index]) = i; // max_y
                if(std::get<5>(blobs[index]) > j)
                    std::get<5>(blobs[index]) = j; // min_x
                if(std::get<6>(blobs[index]) > i)
                    std::get<6>(blobs[index]) = i; // min_y
            }
        }
    }

    if(number == 0)
        return blobs;

    std::map<uint, uint> belongMap_new = belongMap;
    sortBelong(belongMap, belongMap_new); // 递归梳理归属关系

    for(auto it = belongMap_new.begin(); it != belongMap_new.end(); ++it){
        index = it->first - 1;
        index_wait_add = it->second - 1;

        std::get<0>(blobs[index_wait_add]) += std::get<0>(blobs[index]); // sum
        std::get<1>(blobs[index_wait_add]) += std::get<1>(blobs[index]); // sum_x
        std::get<2>(blobs[index_wait_add]) += std::get<2>(blobs[index]); // sum_y
        if(std::get<3>(blobs[index_wait_add]) < std::get<3>(blobs[index]))
            std::get<3>(blobs[index_wait_add]) = std::get<3>(blobs[index]); // max_x
        if(std::get<4>(blobs[index_wait_add]) < std::get<4>(blobs[index]))
            std::get<4>(blobs[index_wait_add]) = std::get<4>(blobs[index]); // max_y
        if(std::get<5>(blobs[index_wait_add]) > std::get<5>(blobs[index]))
            std::get<5>(blobs[index_wait_add]) = std::get<5>(blobs[index]); // min_x
        if(std::get<6>(blobs[index_wait_add]) > std::get<6>(blobs[index]))
            std::get<6>(blobs[index_wait_add]) = std::get<6>(blobs[index]); // min_y

        std::get<0>(blobs[index]) = 0; // in order to delete it.
    }

    for(auto it = blobs.begin(); it != blobs.end(); ){
        if(!std::get<0>(*it)){
            blobs.erase(it);
        }else{
            std::get<7>(*it) = std::get<1>(*it) / std::get<0>(*it) + 1; // x = sum_x / sum
            std::get<8>(*it) = std::get<2>(*it) / std::get<0>(*it) + 1; // y = sum_y / sum
            std::get<9>(*it) = std::get<3>(*it) - std::get<5>(*it) + 1; // w = max_x - min_x
            std::get<10>(*it) = std::get<4>(*it) - std::get<6>(*it) + 1; // h = max_y - min_y
            ++it;
        }
    }

    return blobs;
}

void qc::FindContours_TwoPass::sortBelong(std::map<uint, uint> &belongMap, std::map<uint, uint> &belongMap_new)
{
    bool find = false;
    for(auto it = belongMap.begin(); it != belongMap.end(); ++it){
        for(auto it_new = belongMap_new.begin(); it_new != belongMap_new.end(); ++it_new){
            if(it->first == it_new->second){
                find = true;
                it_new->second = it->second;
            }
        }
    }
    if(!find){
        return;
    }else{
        sortBelong(belongMap, belongMap_new);
    }
}
