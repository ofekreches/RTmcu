#include "motor.h"
#include "configuration.h"
#include <esp_timer.h>


void initMotor(Motor *motor, Encoder enc, L298N driver, POS_PID pos_pid, VEL_PID vel_pid) {
    motor->desired_position = 0;
    motor->current_position = 0;
    motor->desired_velocity = 0;
    motor->current_velocity = 0;
    motor->last_position = 0;
    motor->controlMode = VELOCITY;
    motor->encoder = enc;
    motor->l298n = driver;
    motor->pos_pid = pos_pid;
    motor->vel_pid = vel_pid;
    motor->ticksPerTurn = TICKS_PER_TURN;
    motor->wheelDiameter = WHEEL_DIAMETER;
    motor->distancePerTick = (2.0 * PI) / motor->ticksPerTurn;  //in radiands
    motor->lastUpdateTime = esp_timer_get_time();
}

void computeVelocity(Motor *motor) {
    if (motor->current_position != motor->last_position) {
        int64_t currentTime = esp_timer_get_time(); // Get the current time in microseconds
        float deltaPosition = motor->current_position - motor->last_position;
        int64_t deltaTime = currentTime - motor->lastUpdateTime; // Time difference in microseconds
        float dt = deltaTime / 1000000.0; // Convert time difference to seconds
        motor->current_velocity = deltaPosition / dt;
        motor->lastUpdateTime = currentTime;
        motor->last_position = motor->current_position;
    } 
    else {
        motor->current_velocity = 0.0; 
    }
}

void updateMotor(Motor *motor) {  //updates position and velocity
    motor->current_position = getEncoderCount(&motor->encoder) * motor->distancePerTick;  //update position
    computeVelocity(motor); // update velocity
}


void motor_step(Motor *motor) {
    updateMotor(motor);
    float control_signal = 0;
    switch(motor->controlMode){

        case POSITION:
            control_signal = pos_pid_step(&motor->pos_pid, motor->desired_position , motor->current_position);
            break;

        case VELOCITY:
            control_signal = vel_pid_step(&motor->vel_pid, motor->desired_velocity , motor->current_velocity);
            break;
        }

    if (control_signal > 0) move_forward(&motor->l298n , control_signal);
    if (control_signal < 0) move_backward(&motor->l298n , -control_signal);
    if (control_signal == 0) stop(&motor->l298n);
}


