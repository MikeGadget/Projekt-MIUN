/*
Kurskod: DT169G
Namn: Erik Hägglund, Michael Jaegtnes
Beskrivning: Programmet skapar en action klient som publicerar actions i form av mål var vår robot ska åka
Datum: 2025 - 10 - 25

*/


#include "Nav_To_Pose.hpp"

Nav_To_Pose::Nav_To_Pose() : Node("nav_to_pose"){ //https://api.nav2.org/actions/jazzy/navigatetopose.html
    client_ = rclcpp_action::create_client<NavigateToPose>(this, "/navigate_to_pose");

}

void Nav_To_Pose::send_goal(float x, float y){ // funktion som skickar målet till action servern //ros2 topics info  // ros2 interface show nav2_msgs/action/NavigateToPose

    auto goal_msg = NavigateToPose::Goal();
    goal_msg.pose.header.frame_id = "map";
    goal_msg.pose.header.stamp = this->now();
    goal_msg.pose.pose.position.x = x;
    goal_msg.pose.pose.position.y = y;
    goal_msg.pose.pose.orientation.w = 1.0;  // ?    
    
    auto send_goal_options = rclcpp_action::Client<NavigateToPose>::SendGoalOptions();


    // kopplar svaren till servern https://docs.ros.org/en/foxy/Tutorials/Intermediate/Writing-an-Action-Server-Client/Cpp.html#writing-an-action-client
    send_goal_options.goal_response_callback  = 
        std::bind(&Nav_To_Pose::goal_response_callback, this,
            std::placeholders::_1);

    send_goal_options.feedback_callback = 
        std::bind(&Nav_To_Pose::feedback_callback, this,
            std::placeholders::_1, std::placeholders::_2);

    send_goal_options.result_callback = 
        std::bind(&Nav_To_Pose::result_callback, this,
            std::placeholders::_1);

    

    client_->async_send_goal(goal_msg, send_goal_options); // returner en goalHandle

}

void Nav_To_Pose::feedback_callback(GoalHandle::SharedPtr, const std::shared_ptr<const NavigateToPose::Feedback> feedback){ // tar emot feedback från servern

    RCLCPP_INFO(this->get_logger(), "Received feedback");
}

void Nav_To_Pose::goal_response_callback(GoalHandle::SharedPtr goal_handle) // tar emot om målet accepterades
{
    // auto goal_handle = future.get();
    if (!goal_handle) {
        RCLCPP_ERROR(this->get_logger(), "Goal was rejected by server");
    } else {
        RCLCPP_INFO(this->get_logger(), "Goal accepted by server, waiting for result");
    }
}

void Nav_To_Pose::result_callback(const GoalHandle::WrappedResult &result) // tar emot resultat från servern 
{
    switch (result.code) {
      case rclcpp_action::ResultCode::SUCCEEDED:
        std::cout << "succeed -------------------------------------------------"<< std::endl;
        // När klar kör mot nästa
        
        break;
      case rclcpp_action::ResultCode::ABORTED:
        RCLCPP_ERROR(this->get_logger(), "Goal was aborted");
        return;
      case rclcpp_action::ResultCode::CANCELED:
        RCLCPP_ERROR(this->get_logger(), "Goal was canceled");
        return;
      default:
        RCLCPP_ERROR(this->get_logger(), "Unknown result code");
        return;
    
    }
    // vid lyckat mål
    timer_ = this->create_wall_timer(std::chrono::milliseconds(5000), std::bind(&Nav_To_Pose::timer_callback, this));

}

void Nav_To_Pose::timer_callback() // skickar nästa mål https://docs.ros.org/en/foxy/Tutorials/Beginner-Client-Libraries/Writing-A-Simple-Cpp-Publisher-And-Subscriber.html#write-the-publisher-node
{
    timer_->cancel();
    current_station++;
    switch (current_station){
        case 1:
            send_goal(0.0, 2.0);
            break;
        case 2:
            send_goal(0.0, -2.0);
            break;        
        default:
            current_station = 0; // börja om till första kordinaten
            send_goal(2.0, 0.0);            
            
    }    
}

void Nav_To_Pose::wait_for_server() // väntar för server
{
    if(!client_->wait_for_action_server()){ 
        RCLCPP_ERROR(this->get_logger(), "Action server not available after waiting");
        rclcpp::shutdown();
    }
}

struct Position{





};


#include <iostream>

int main(int argc, char **argv)
{    
    std::cout<< "Nod startad" << std::endl;
    rclcpp::init(argc, argv);
    auto node = std::make_shared<Nav_To_Pose>(); // pekar till noden
    node->wait_for_server();    // väntar så att vi kommunicerar med action servern.
    node->send_goal(2.0, 0.0);     // skickar kordinater som roboten ska köra till. Ska lägga till radianer vid målet.
    rclcpp::spin(node);    
    rclcpp::shutdown();
    std::cout<< "Nod avslutad" << std::endl;
    return 0;
}

