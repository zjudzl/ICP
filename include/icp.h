#ifndef ICP_H
#define ICP_H
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <sophus/se3.hpp>
#include "dualquat.h"

float ICP(pcl::PointCloud<pcl::PointXYZ>::Ptr reference,
    pcl::PointCloud<pcl::PointXYZ>::Ptr source, Eigen::Matrix<float, 4, 4> &Trs);

DualQuat<float> localize(pcl::PointCloud<pcl::PointXYZ>::Ptr reference,
    pcl::PointCloud<pcl::PointXYZ>::Ptr source, std::vector<int> matched);

#endif
