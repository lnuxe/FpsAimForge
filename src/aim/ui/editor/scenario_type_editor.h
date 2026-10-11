#pragma once

#include <string>

#include "aim/proto/scenario.pb.h"

namespace ImGui {
class MultilineTextEntryDialog;
}  // namespace ImGui

namespace aim {

class Application;

void InitializeScenarioType(ScenarioDef& def, ScenarioDef::TypeCase scenario_type);

void DrawScenarioTypeTabs(ScenarioDef& def, Application* app);

void DrawShotTypeEditor(ShotType& s);

void DrawReferenceEditor(ScenarioDef& def,
                         Application* app,
                         std::string* error_message_out,
                         bool* editing_room);

}  // namespace aim
