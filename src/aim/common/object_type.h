#pragma once

#include <assert.h>

#include "aim/i18n/i18n.h"

namespace aim {

enum class ObjectType : int {
  SCENARIO = 1,
  PLAYLIST = 2,
  THEME = 3,
  CROSSHAIR = 4,
};

static std::string ObjectTypeToString(ObjectType t) {
  switch (t) {
    case ObjectType::PLAYLIST:
      return Tr("Playlist");
    case ObjectType::SCENARIO:
      return Tr("Scenario");
    case ObjectType::THEME:
      return Tr("Theme");
    case ObjectType::CROSSHAIR:
      return Tr("Crosshair");
  }

  assert(false && "Unhandled ObjectType");
  return "UnknownObjectType";
}

}  // namespace aim
