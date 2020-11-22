/*
 * Copyright (C) 2012 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include <unistd.h>
#include <math.h>       /* atan2 */
#include <functional>
#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/common/common.hh>
#include <ignition/math/Vector3.hh>
#include <ignition/math/Quaternion.hh>

#define GOAL_X 8
#define GOAL_Y 8

namespace gazebo
{

// Controlador PID discreto
class PID {
public:
    float kp, ki, kd; // Constantes del pid
    float derivator, integrator, integrator_max, integrator_min;
    float set_point, error;

    // Constructor por defecto
    PID() {
        kp = 2.0;
        ki = 0.0;
        kd = 1.0;
        derivator = 0.0;
        integrator = 0.0;
        integrator_max = 500.0;
        integrator_min = -500.0;

        set_point = 0.0;
        error = 0.0;
    }

    // Constructor con parcial (kp, ki, kd)
    PID(float p, float i, float d) {
        kp = p;
        ki = i;
        kd = d;
        derivator = 0.0;
        integrator = 0.0;
        integrator_max = 500.0;
        integrator_min = -500.0;

        set_point = 0.0;
        error = 0.0;
    }

    // Constructor completo
    PID(float p, float i, float d, float deriv, float integr, float integr_max, float integr_min) {
        kp = p;
        ki = i;
        kd = d;
        derivator = deriv;
        integrator = integr;
        integrator_max = integr_max;
        integrator_min = integr_min;

        set_point = 0.0;
        error = 0.0;
    }

    // Calcula la salida del PID segun una referencia de entrada y su propia realimentación
    float update(float current_value) {
        error = set_point - current_value;

        float P_value = kp * error;
        float D_value = kd * (error - derivator);
        derivator = error;

        integrator = integrator + error;

        if (integrator > integrator_max) {
            integrator = integrator_max;
        } else if (integrator < integrator_min) {
            integrator = integrator_min;
        }

        float I_value = integrator * ki;

        float PID_value = P_value + I_value + D_value;
        return PID_value;
    }
};


class ModelPush : public ModelPlugin
{
public:
    void Load(physics::ModelPtr _parent, sdf::ElementPtr /*_sdf*/)
    {
        // Store the pointer to the model
        model = _parent;

        // Inicialización de las puntos objetivos de cada PID
        pid_yaw.set_point = 0.0;

        pidx.set_point = GOAL_X;
        pidy.set_point = GOAL_Y;

        // Listen to the update event. This event is broadcast every simulation iteration.
        updateConnection = event::Events::ConnectWorldUpdateBegin(std::bind(&ModelPush::OnUpdate, this));
    }

    // Called by the world update start event
    void OnUpdate()
    {
        ignition::math::Pose3d pose = model->WorldPose();
        printf("At: %f %f %f %f %f %f\n", pose.Pos().X(), pose.Pos().Y(), pose.Pos().Z(), pose.Rot().Roll(), pose.Rot().Pitch(), pose.Rot().Yaw());

        float dx = GOAL_X - pose.Pos().X();  // Distancia a meta
        float dy = GOAL_Y - pose.Pos().Y();

        float goal_yaw = atan2(dy, dx);  // Heading a la meta
        pid_yaw.set_point = goal_yaw;

        float v_yaw = pid_yaw.update(pose.Rot().Yaw()); // Actualizamos las velocidades
        float velx = pidx.update(pose.Pos().X());
        float vely = pidy.update(pose.Pos().Y());

        velx = velx / 10;  // Reducimos las velocidades obtenidas
        vely = vely / 10;

        model->SetLinearVel(ignition::math::Vector3d(velx, vely, 0));
        model->SetAngularVel(ignition::math::Vector3d(0, 0, v_yaw));
    }

private:
    physics::ModelPtr model; // Pointer to the model
    event::ConnectionPtr updateConnection; // Pointer to the update event connection

    PID pid_yaw; // PID yaw

    PID pidx; // PID x
    PID pidy; // PID y
};

// Register this plugin with the simulator
GZ_REGISTER_MODEL_PLUGIN(ModelPush)
}
