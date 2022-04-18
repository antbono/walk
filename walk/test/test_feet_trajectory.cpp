// Copyright 2021 Kenji Brameld
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <vector>
#include "gtest/gtest.h"
#include "../src/feet_trajectory.hpp"
#include "walk_interfaces/msg/feet_trajectory_point.hpp"
#include "biped_interfaces/msg/phase.hpp"

TEST(TestFeetTrajectory, TestSmoothSteps)
{
  // In this test, we ensure that all consecutive points in the trajectory are within
  // a small margin, and there are no large changes in value.
  float footLiftAmp = 0.012;
  float period = 0.3;
  float dt = 0.01;
  walk_interfaces::msg::FeetTrajectoryPoint init;

  walk_interfaces::msg::FeetTrajectoryPoint step1;
  step1.forward_l = 0.02;
  step1.forward_r = -0.02;
  step1.left_l = 0.01;
  step1.left_r = -0.01;
  step1.heading_l = 0.6;
  step1.heading_r = -0.6;
  step1.footh_l = 0.0;
  step1.footh_r = 0.0;
  walk_interfaces::msg::FeetTrajectoryPoint step2;
  step2.forward_l = -0.04;
  step2.forward_r = -0.04;
  step2.left_l = 0.0;
  step2.left_r = 0.0;
  step2.heading_l = -0.1;
  step2.heading_r = -0.1;
  step2.footh_l = 0.0;
  step2.footh_r = 0.0;

  feet_trajectory::Params params{footLiftAmp, period, dt};

  biped_interfaces::msg::Phase phase1;
  phase1.phase = biped_interfaces::msg::Phase::RIGHT_SWING;
  std::vector<walk_interfaces::msg::FeetTrajectoryPoint> pointsStep1 =
    feet_trajectory::generate(params, phase1, init, step1);

  biped_interfaces::msg::Phase phase2;
  phase2.phase = biped_interfaces::msg::Phase::LEFT_SWING;
  std::vector<walk_interfaces::msg::FeetTrajectoryPoint> pointsStep2 =
    feet_trajectory::generate(params, phase2, step1, step2);

  std::vector<walk_interfaces::msg::FeetTrajectoryPoint> points;
  points.insert(points.end(), pointsStep1.begin(), pointsStep1.end());
  points.insert(points.end(), pointsStep2.begin(), pointsStep2.end());

  for (unsigned i = 0; i < points.size() - 1; ++i) {
    walk_interfaces::msg::FeetTrajectoryPoint & a = points.at(i);
    walk_interfaces::msg::FeetTrajectoryPoint & b = points.at(i + 1);

    EXPECT_NEAR(a.forward_l, b.forward_l, 0.01);
    EXPECT_NEAR(a.forward_r, b.forward_r, 0.01);
    EXPECT_NEAR(a.left_l, b.left_l, 0.01);
    EXPECT_NEAR(a.left_r, b.left_r, 0.01);
    EXPECT_NEAR(a.heading_l, b.heading_l, 0.1);
    EXPECT_NEAR(a.heading_r, b.heading_r, 0.1);
  }
}

TEST(TestFeetTrajectory, TestPointsSize)
{
  // In this test, we ensure that the number of points returned matches up
  // with what we expect from dt and period.
  float period = 0.3;
  float dt = 0.01;

  feet_trajectory::Params params{0, period, dt};

  std::vector<walk_interfaces::msg::FeetTrajectoryPoint> pointsStep =
    feet_trajectory::generate(
    params, biped_interfaces::msg::Phase{}, walk_interfaces::msg::FeetTrajectoryPoint{},
    walk_interfaces::msg::FeetTrajectoryPoint{});

  EXPECT_EQ(pointsStep.size(), 31u);  // 31 instead of 30 because it includes t = 0 and t = 0.3
}
