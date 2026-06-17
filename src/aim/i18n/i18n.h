#pragma once

#include <string_view>

namespace aim {

class LocalStore;

enum class Language {
  kEnglish = 0,
  kChinese = 1,
};

void SetLanguage(Language language);
Language GetLanguage();

// Returns localized string; falls back to English if key or translation is missing.
const char* Tr(std::string_view key);

constexpr const char* kLanguageStoreKey = "Language";

void LoadLanguageFromStore(LocalStore& store);
void SaveLanguageToStore(LocalStore& store, Language language);

namespace i18n_key {

inline constexpr const char* kSettings = "settings";
inline constexpr const char* kLanguage = "language";
inline constexpr const char* kLanguageEnglish = "language_english";
inline constexpr const char* kLanguageChinese = "language_chinese";
inline constexpr const char* kCmPer360 = "cm_per_360";
inline constexpr const char* kDpi = "dpi";
inline constexpr const char* kDpiHelp = "dpi_help";
inline constexpr const char* kDpiPrompt = "dpi_prompt";
inline constexpr const char* kSet = "set";
inline constexpr const char* kClickToStart = "click_to_start";
inline constexpr const char* kTargetScore = "target_score";
inline constexpr const char* kStartNewRun = "start_new_run";
inline constexpr const char* kMenuSettings = "menu_settings";
inline constexpr const char* kMenuThemes = "menu_themes";
inline constexpr const char* kMenuCrosshairs = "menu_crosshairs";
inline constexpr const char* kMenuPlayTime = "menu_play_time";
inline constexpr const char* kMenuReaction = "menu_reaction";
inline constexpr const char* kMenuRestart = "menu_restart";
inline constexpr const char* kMenuExit = "menu_exit";
inline constexpr const char* kQuickSettingsHelp = "quick_settings_help";
inline constexpr const char* kAdjustCrosshairHelp = "adjust_crosshair_help";
inline constexpr const char* kFire = "fire";
inline constexpr const char* kRestartScenario = "restart_scenario";
inline constexpr const char* kNextScenario = "next_scenario";
inline constexpr const char* kEditScenario = "edit_scenario";
inline constexpr const char* kQuickSettings = "quick_settings";
inline constexpr const char* kQuickMetronome = "quick_metronome";
inline constexpr const char* kAdjustCrosshairSize = "adjust_crosshair_size";
inline constexpr const char* kKeybinds = "keybinds";
inline constexpr const char* kGlobal = "global";
inline constexpr const char* kScenario = "scenario";
inline constexpr const char* kCmPer360ScrollHelp = "cm_per_360_scroll_help";

}  // namespace i18n_key

}  // namespace aim
