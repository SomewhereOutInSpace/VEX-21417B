#include "robokauz/PROS.hpp"
#include "robokauz/COMMON.hpp"
#include "robokauz/Autonomous/VectorMath.hpp"
#include "robokauz/Autonomous/IMUOdometry.hpp"

QAngle IMUOdometer::getRotation()
{
    return imu.get_rotation() * degree;
}

void IMUOdometer::updateState()
{
    previous_state = current_state;
    current_state = {getRotation(), encoder_left.get_value(), encoder_middle.get_value(), encoder_right.get_value()};
}

void IMUOdometer::updatePosition(Vector displacement_vector, QAngle heading)
{
    OdomState new_position;
    new_position.x = current_position.x + displacement_vector.x_component;
    new_position.y = current_position.y + displacement_vector.y_component;
    new_position.theta = heading;
    current_position = new_position;
}

void IMUOdometer::odometryLoop()
{
    std::uint32_t delay_timestamp = pros::millis();
    while(true)
    {
        if(imu.get_heading() != INFINITY)
        {
            updateState();

            int parallel_encoder_change = ((current_state.left - previous_state.left) + (current_state.right - previous_state.right)) / 2;
            QLength parallel_distance = ((parallel_encoder_change / 360.0) * wheel_circumfrence.convert(inch)) * inch;
            Vector parallel_displacement(parallel_distance, current_state.rotation);

            int perpendicular_encoder_change = (current_state.middle - previous_state.middle);
            int direction = sgnum(perpendicular_encoder_change); //1 indicates right, -1 indicates left
            QLength perpendicular_distance = (perpendicular_encoder_change / 360.0) * wheel_circumfrence;
            Vector perpendicular_displacement(perpendicular_distance, current_state.rotation + (90.0_deg * direction));
            
            Vector total_displacement = parallel_displacement + perpendicular_displacement;

            updatePosition(total_displacement, current_state.rotation);
        }
        pros::Task::delay_until(&delay_timestamp, 10);
    }
}

IMUOdometer::IMUOdometer(pros::IMU& inertial, EncoderGroup& encoders, QLength tracking_wheel_diameter) : imu(inertial), encoder_left(encoders.left), encoder_middle(encoders.middle), encoder_right(encoders.right)
{
    wheel_diameter = tracking_wheel_diameter;
    wheel_circumfrence = (1_pi * wheel_diameter.convert(inch)) * inch;

    current_position = {0_ft, 0_ft, 0_deg};

    pros::Task odomLoop([this] { odometryLoop(); }, TASK_PRIORITY_DEFAULT + 2);
}

OdomState IMUOdometer::getPosition()
{
    return current_position;
}