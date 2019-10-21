#include "../include/bwlabel.h"
#include <memory.h>

qc::FindContours_TwoPass::~FindContours_TwoPass()
{
    release();
}

int qc::FindContours_TwoPass::init(const uint &rows, const uint &cols)
{
    this->rows = rows;
    this->cols = cols;
    cutRows = rows - 1; // 边界处不计算，提高效率
    cutCols = cols - 1; // 边界处不计算，提高效率
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
    belongMap.clear();

    uint index = 0, index_wait_add = 0;
    uint cur = 0, top = 0, left = 0;
    uint run = 0;
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
                    dst[cur] = ++run;
                    blobs.push_back(std::make_tuple(1, j, i, j, i, 0, 0, 0, 0));
                    continue;
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
                ++std::get<0>(blobs[index]);           // area
                if(!src[i * cols + j + 1]){
                    // 如果右侧没有值，则判断最大x值
                    if(std::get<1>(blobs[index]) < j)
                        std::get<1>(blobs[index]) = j; // max_x
                }
                if(!src[(i + 1) * cols + j]){
                    // 如果下方没有值，则判断最大y值
                    if(std::get<2>(blobs[index]) < i)
                        std::get<2>(blobs[index]) = i; // max_y
                }
                if(!src[left]){
                    // 如果左侧没有值，则判断最小x值
                    if(std::get<3>(blobs[index]) > j)
                        std::get<3>(blobs[index]) = j; // min_x
                }
                if(!src[top]){
                    // 如果上方没有值，则判断最小y值
                    if(std::get<4>(blobs[index]) > i)
                        std::get<4>(blobs[index]) = i; // min_y
                }
            }
        }
    }

    if(run == 0)
        return blobs;

    // 反向遍历归属表
    for(auto it = belongMap.rbegin(); it != belongMap.rend(); ++it){
        index = it->first - 1;
        index_wait_add = it->second - 1;
        std::get<0>(blobs[index_wait_add]) += std::get<0>(blobs[index]);    // area
        if(std::get<1>(blobs[index_wait_add]) < std::get<1>(blobs[index]))
            std::get<1>(blobs[index_wait_add]) = std::get<1>(blobs[index]); // max_x
        if(std::get<2>(blobs[index_wait_add]) < std::get<2>(blobs[index]))
            std::get<2>(blobs[index_wait_add]) = std::get<2>(blobs[index]); // max_y
        if(std::get<3>(blobs[index_wait_add]) > std::get<3>(blobs[index]))
            std::get<3>(blobs[index_wait_add]) = std::get<3>(blobs[index]); // min_x
        if(std::get<4>(blobs[index_wait_add]) > std::get<4>(blobs[index]))
            std::get<4>(blobs[index_wait_add]) = std::get<4>(blobs[index]); // min_y
        std::get<0>(blobs[index]) = 0; // in order to delete it.
    }

    std::vector<blob> blobs_new;
    for(auto it = blobs.begin(); it != blobs.end(); ++it){
        if(std::get<0>(*it) > 1){
            std::get<5>(*it) = (std::get<1>(*it) + std::get<3>(*it)) / 2; // x = (max_x + min_x) / 2
            std::get<6>(*it) = (std::get<2>(*it) + std::get<4>(*it)) / 2; // y = (max_y + min_y) / 2
            std::get<7>(*it) = std::get<1>(*it) - std::get<3>(*it) + 1;   // w = max_x - min_x + 1
            std::get<8>(*it) = std::get<2>(*it) - std::get<4>(*it) + 1 ;  // h = max_y - min_y + 1
            blobs_new.push_back(*it);
        }
    }

    return blobs_new;
}
