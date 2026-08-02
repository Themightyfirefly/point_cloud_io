/*
 * Read.cpp
 *
 *  Created on: Aug 7, 2013
 *      Author: Péter Fankhauser
 *   Institute: ETH Zurich, Autonomous Systems Lab
 */

#include "point_cloud_io/Read.hpp"

#include <filesystem>
#include <thread>
#include <chrono>
#include <functional>

// PCL
#include <pcl/io/ply_io.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl_conversions/pcl_conversions.h>

#ifdef HAVE_VTK
#include <pcl/io/vtk_lib_io.h>
#endif

namespace point_cloud_io {

Read::Read(const rclcpp::NodeOptions & options) : rclcpp::Node("point_cloud_io_read", options),
  pointCloudMessage_(std::make_shared<sensor_msgs::msg::PointCloud2>()),
  updateDuration_(rclcpp::Duration::from_seconds(1.0))
{
  if (!readParameters()) {
    rclcpp::shutdown();
  }
  pointCloudPublisher_ = this->create_publisher<sensor_msgs::msg::PointCloud2>(pointCloudTopic_, rclcpp::QoS(1));
  initialize();
}

bool Read::readParameters() {
  this->get_parameter("file_path", filePath_);
  this->get_parameter("topic", pointCloudTopic_);
  this->get_parameter("frame", pointCloudFrameId_);

  double updateRate;
  this->declare_parameter<double>("rate", 0.0);
  this->get_parameter("rate", updateRate);
  if (updateRate == 0.0) {
    isContinuouslyPublishing_ = false;
  } else {
    isContinuouslyPublishing_ = true;
    updateDuration_.from_seconds(1.0 / updateRate);
  }

  return true;
}

void Read::initialize() {
  if (!readFile(filePath_, pointCloudFrameId_)) {
    rclcpp::shutdown();
  }

  if (isContinuouslyPublishing_) {
    timer_ = this->create_wall_timer(
      std::chrono::nanoseconds(updateDuration_.nanoseconds()),
      std::bind(&Read::timerCallback, this));
  } else {
    std::this_thread::sleep_for(std::chrono::seconds(1));  // Need this to get things ready before publishing.
    if (!publish()) {
      RCLCPP_ERROR(this->get_logger(), "Something went wrong when trying to read and publish the point cloud file.");
    }
    rclcpp::shutdown();
  }
}

bool Read::readFile(const std::string& filePath, const std::string& pointCloudFrameId) {
  if (std::filesystem::path(filePath).extension() == ".ply") {
    // Load .ply file.
    pcl::PointCloud<pcl::PointXYZRGBNormal> pointCloud;
    if (pcl::io::loadPLYFile(filePath, pointCloud) != 0) {
      return false;
    }

    // Define PointCloud2 message.
    pcl::toROSMsg(pointCloud, *pointCloudMessage_);
  }
#ifdef HAVE_VTK
  else if (std::filesystem::path(filePath).extension() == ".vtk") {
    // Load .vtk file.
    pcl::PolygonMesh polygonMesh;
    pcl::io::loadPolygonFileVTK(filePath, polygonMesh);

    // Define PointCloud2 message.
    pcl_conversions::moveFromPCL(polygonMesh.cloud, *pointCloudMessage_);
  }
#endif
  else {
    RCLCPP_ERROR(this->get_logger(), "Data format not supported.");
    return false;
  }

  pointCloudMessage_->header.frame_id = pointCloudFrameId;

  RCLCPP_INFO(this->get_logger(), "Loaded point cloud with %d points.", pointCloudMessage_->height * pointCloudMessage_->width);
  return true;
}

void Read::timerCallback() {
  if (!publish()) {
    RCLCPP_ERROR(this->get_logger(), "Something went wrong when trying to read and publish the point cloud file.");
  }
}

bool Read::publish() {
  pointCloudMessage_->header.stamp = this->now();
  if (pointCloudPublisher_->get_subscription_count() > 0u) {
    pointCloudPublisher_->publish(*pointCloudMessage_);
    RCLCPP_INFO(this->get_logger(), "Point cloud published to topic \"%s\".", pointCloudTopic_.c_str());
  }
  return true;
}

}  // namespace point_cloud_io
