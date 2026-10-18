#include "basic_movement_controller.h"

#include "aim/common/util.h"
#include "aim/core/application.h"
#include "aim/core/target.h"
#include "glm/vec2.hpp"  // IWYU pragma: keep
#include "google/protobuf/message_lite.h"

using google::protobuf::RepeatedPtrField;

namespace aim {
namespace {

class CompositeMovementController : public MovementController {
 public:
  CompositeMovementController(std::vector<std::unique_ptr<MovementController>> controllers)
      : controllers_(std::move(controllers)) {}

  void UpdatePosition(float now_seconds,
                      Target& t,
                      const Room& room,
                      float delta_seconds) override {
    for (auto& controller : controllers_) {
      controller->UpdatePosition(now_seconds, t, room, delta_seconds);
    }
  }

 private:
  std::vector<std::unique_ptr<MovementController>> controllers_;
};

}  // namespace

BasicWallMovementController::BasicWallMovementController() {}

BasicWallMovementController::BasicWallMovementController(float speed, const glm::vec2& direction)
    : speed_(speed), direction_(glm::normalize(direction)), original_speed_(speed) {}

BasicWallMovementController::BasicWallMovementController(float speed)
    : speed_(speed), original_speed_(speed) {}

BasicWallMovementController::~BasicWallMovementController() {}

void BasicWallMovementController::UpdatePosition(float now_seconds,
                                                 Target& t,
                                                 const Room& room,
                                                 float delta_seconds) {
  now_seconds_ = now_seconds;
  if (!t.wall_position.has_value()) {
    t.wall_position = glm::vec2(0.0f);
  }
  UpdateDirectionAndSpeed(t, delta_seconds);
  glm::vec2 new_position = *t.wall_position + (direction_ * (delta_seconds * speed_));
  t.wall_position = new_position;
  t.position = WallPositionToWorldPosition(*t.wall_position, t.radius, room, t.wall_depth);
}

WallDepthMovementController::WallDepthMovementController(float speed)
    : speed_(speed), original_speed_(speed) {}

WallDepthMovementController::~WallDepthMovementController() {}

void WallDepthMovementController::UpdatePosition(float now_seconds,
                                                 Target& t,
                                                 const Room& room,
                                                 float delta_seconds) {
  now_seconds_ = now_seconds;
  if (!t.wall_position.has_value()) {
    t.wall_position = glm::vec2(0.0f);
  }
  UpdateDirectionAndSpeed(t, delta_seconds);

  glm::vec3 wall_pos = t.GetWallPosition3();
  glm::vec3 next_position = wall_pos + (direction_ * (delta_seconds * speed_));

  t.wall_position = next_position;
  t.wall_depth = next_position.z;
  t.position = WallPositionToWorldPosition(*t.wall_position, t.radius, room, t.wall_depth);
}

std::unique_ptr<MovementController> CreateCompositeMovementController(
    std::vector<std::unique_ptr<MovementController>> controllers) {
  return std::make_unique<CompositeMovementController>(std::move(controllers));
}

}  // namespace aim
