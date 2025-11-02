/*
Kurskod: DT169G
Namn: Erik Hägglund, Michael Jaegtnes
Beskrivning: Programmet skapar en action klient som publicerar actions i form av mål var vår robot ska åka
Datum: 2025 - 10 - 25

*/

#ifndef Nav_To_Pose_HPP
#define Nav_To_Pose_HPP

#include <rclcpp/rclcpp.hpp>
#include "nav2_msgs/action/navigate_to_pose.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include <vector>
#include <chrono>

using NavigateToPose = nav2_msgs::action::NavigateToPose;
using GoalHandle = rclcpp_action::ClientGoalHandle<NavigateToPose>;

class Nav_To_Pose : public rclcpp::Node {
public:
    Nav_To_Pose();
    void send_goal(float x, float y);
    void wait_for_server();


private:
    // std::vector<Position> poseVec;
    rclcpp::TimerBase::SharedPtr timer_;
    std::size_t current_station{0};


    void result_callback(const GoalHandle::WrappedResult & result);
    rclcpp_action::Client<NavigateToPose>::SharedPtr client_;    
    void feedback_callback(GoalHandle::SharedPtr,  const std::shared_ptr<const NavigateToPose::Feedback> feedback);
    void goal_response_callback(GoalHandle::SharedPtr goal_handle);
    void timer_callback();

};

#endif