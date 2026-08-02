/*
 * Read.hpp
 *
 *  Created on: Aug 7, 2013
 *      Author: Péter Fankhauser
 *	 Institute: ETH Zurich, Autonomous Systems Lab
 */

#pragma once

// ROS
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>

namespace point_cloud_io {

class Read : public rclcpp::Node{
 public:
  /*!
   * Constructor.
   */
  explicit Read(const rclcpp::NodeOptions & options);

  /*!
   * Destructor.
   */
  virtual ~Read() = default;

 private:
  /*!
   * Reads and verifies the ROS parameters.
   * @return true if successful.
   */
  bool readParameters();

  /*!
   * Initializes node.
   */
  void initialize();

  /*!
   * Read the point cloud from a .ply or .vtk file.
   * @param filePath the path to the .ply or .vtk file.
   * @param pointCloudFrameId the id of the frame of the point cloud data.
   * @return true if successful.
   */
  bool readFile(const std::string& filePath, const std::string& pointCloudFrameId);

  /*!
   * Timer callback function.
   */
  void timerCallback();

  /*!
   * Publish the point cloud as a PointCloud2.
   * @return true if successful.
   */
  bool publish();

  //! Point cloud message to publish.
  sensor_msgs::msg::PointCloud2::SharedPtr pointCloudMessage_;

  //! Point cloud publisher.
  rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pointCloudPublisher_;

  //! Timer for publishing the point cloud.
  rclcpp::TimerBase::SharedPtr timer_;

  //! Path to the point cloud file.
  std::string filePath_;

  //! Point cloud topic to be published at.
  std::string pointCloudTopic_;

  //! Point cloud frame id.
  std::string pointCloudFrameId_;

  /*!
   * If true, continuous publishing is used.
   * If false, point cloud is only published once.
   */
  bool isContinuouslyPublishing_ = false;

  //! Duration between publishing steps.
  rclcpp::Duration updateDuration_;
};

}  // namespace point_cloud_io
