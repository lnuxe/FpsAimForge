#pragma once

#include <optional>

#include "aim/common/field.h"
#include "aim/proto/scenario.pb.h"

namespace aim {

enum StrafeDirectionType : int {
  LEFT_RIGHT = 0,
  UP_DOWN = 1,
  FORWARD_BACK = 2,
};

struct StrafeDirectionInfo {
  StrafeDirectionType strafe_direction;
  PtrField<ProfileListInfo> profiles_info;
  google::protobuf::RepeatedPtrField<StrafeProfile>* profile_list;
  Bounds* bounds;
  Bounds* relative_bounds;
  Field<Direction> direction_field;
};

struct StrafeInfo {
  std::optional<StrafeDirectionInfo> left_right;
  std::optional<StrafeDirectionInfo> up_down;
  std::optional<StrafeDirectionInfo> forward_back;
};

StrafeInfo GetStrafeInfo(ScenarioDef& def);
void DrawStrafeTab(StrafeDirectionInfo& strafe_info);
void DrawStrafeTabs(ScenarioDef& def);

}  // namespace aim
