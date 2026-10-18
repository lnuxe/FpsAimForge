#pragma once

#include <memory>
#include <vector>

#include "aim/common/random.h"
#include "aim/common/wall.h"
#include "aim/core/application.h"
#include "aim/core/target.h"
#include "aim/proto/scenario.pb.h"
#include "glm/vec2.hpp"  // IWYU pragma: keep
#include "glm/vec3.hpp"  // IWYU pragma: keep

namespace aim {

std::unique_ptr<MovementController> CreateCompositeMovementController(
    std::vector<std::unique_ptr<MovementController>> controllers);

class BasicWallMovementController : public MovementController {
 public:
  BasicWallMovementController();
  BasicWallMovementController(float speed, const glm::vec2& direction);
  BasicWallMovementController(float speed);

  virtual ~BasicWallMovementController();

  void UpdatePosition(float now_seconds, Target& t, const Room& room, float delta_seconds) override;

 protected:
  virtual void UpdateDirectionAndSpeed(Target& t, float delta_seconds) = 0;

  float GetNowSeconds() {
    return now_seconds_;
  }

  float speed_ = 0;
  glm::vec2 direction_{1.0f, 0.0f};
  const float original_speed_ = 0;

 private:
  float now_seconds_ = 0;
};

// Movement controller that allows the target to also change depth as it moves on the wall.
class WallDepthMovementController : public MovementController {
 public:
  WallDepthMovementController(float speed);

  virtual ~WallDepthMovementController();

  void UpdatePosition(float now_seconds, Target& t, const Room& room, float delta_seconds) override;

 protected:
  virtual void UpdateDirectionAndSpeed(Target& t, float delta_seconds) = 0;

  float GetNowSeconds() {
    return now_seconds_;
  }

  float speed_ = 0;
  glm::vec3 direction_{1.0f, 0.0f, 0.0f};
  const float original_speed_ = 0;

 private:
  float now_seconds_ = 0;
};

}  // namespace aim
