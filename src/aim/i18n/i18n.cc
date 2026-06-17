#include "i18n.h"

#include <string_view>
#include <unordered_map>

#include "aim/core/local_store.h"

namespace aim {
namespace {

Language g_language = Language::kEnglish;

const std::unordered_map<std::string_view, const char*> kEnglishStrings{
    {i18n_key::kSettings, "Settings"},
    {i18n_key::kLanguage, "Language"},
    {i18n_key::kLanguageEnglish, "English"},
    {i18n_key::kLanguageChinese, "Chinese"},
    {i18n_key::kCmPer360, "cm/360"},
    {i18n_key::kDpi, "DPI"},
    {i18n_key::kDpiHelp, "DPI is used to calculate sensitivity given a cm/360 value."},
    {i18n_key::kDpiPrompt, "What is your mouse DPI?"},
    {i18n_key::kSet, "Set"},
    {i18n_key::kClickToStart, "Click to Start"},
    {i18n_key::kTargetScore, "Target score"},
    {i18n_key::kStartNewRun, "Start new run"},
    {i18n_key::kMenuSettings, "Settings"},
    {i18n_key::kMenuThemes, "Themes"},
    {i18n_key::kMenuCrosshairs, "Crosshairs"},
    {i18n_key::kMenuPlayTime, "Play time"},
    {i18n_key::kMenuReaction, "Reaction"},
    {i18n_key::kMenuRestart, "Restart"},
    {i18n_key::kMenuExit, "Exit"},
    {i18n_key::kQuickSettingsHelp,
     "Hold the key to bring up a settings menu which will close when the key is released. Scroll "
     "wheel can be used to adjust mouse sensitivity."},
    {i18n_key::kAdjustCrosshairHelp,
     "Hold the key to enable using the scroll wheel to adjust crosshair size."},
    {i18n_key::kFire, "Fire"},
    {i18n_key::kRestartScenario, "Restart Scenario"},
    {i18n_key::kNextScenario, "Next Scenario"},
    {i18n_key::kEditScenario, "Edit Scenario"},
    {i18n_key::kQuickSettings, "Quick Settings"},
    {i18n_key::kQuickMetronome, "Quick Metronome"},
    {i18n_key::kAdjustCrosshairSize, "Adjust Crosshair Size"},
    {i18n_key::kKeybinds, "Keybinds"},
    {i18n_key::kGlobal, "Global"},
    {i18n_key::kScenario, "Scenario"},
    {i18n_key::kCmPer360ScrollHelp,
     "Adjust within a run by holding \"s\" and using the scroll wheel"},
};

const std::unordered_map<std::string_view, const char*> kChineseStrings{
    {i18n_key::kSettings, "设置"},
    {i18n_key::kLanguage, "语言"},
    {i18n_key::kLanguageEnglish, "English"},
    {i18n_key::kLanguageChinese, "中文"},
    {i18n_key::kCmPer360, "cm/360"},
    {i18n_key::kDpi, "DPI"},
    {i18n_key::kDpiHelp, "DPI 用于根据 cm/360 计算灵敏度。"},
    {i18n_key::kDpiPrompt, "你的鼠标 DPI 是多少？"},
    {i18n_key::kSet, "确定"},
    {i18n_key::kClickToStart, "点击开始"},
    {i18n_key::kTargetScore, "目标分数"},
    {i18n_key::kStartNewRun, "开始新训练"},
    {i18n_key::kMenuSettings, "设置"},
    {i18n_key::kMenuThemes, "主题"},
    {i18n_key::kMenuCrosshairs, "准星"},
    {i18n_key::kMenuPlayTime, "练习时长"},
    {i18n_key::kMenuReaction, "反应时间"},
    {i18n_key::kMenuRestart, "重启"},
    {i18n_key::kMenuExit, "退出"},
    {i18n_key::kQuickSettingsHelp,
     "按住快捷键打开设置菜单，松开后关闭。可用滚轮调节鼠标灵敏度。"},
    {i18n_key::kAdjustCrosshairHelp, "按住快捷键并用滚轮调节准星大小。"},
    {i18n_key::kFire, "开火"},
    {i18n_key::kRestartScenario, "重启场景"},
    {i18n_key::kNextScenario, "下一场景"},
    {i18n_key::kEditScenario, "编辑场景"},
    {i18n_key::kQuickSettings, "快速设置"},
    {i18n_key::kQuickMetronome, "快速节拍器"},
    {i18n_key::kAdjustCrosshairSize, "调节准星大小"},
    {i18n_key::kKeybinds, "按键绑定"},
    {i18n_key::kGlobal, "全局"},
    {i18n_key::kScenario, "场景"},
    {i18n_key::kCmPer360ScrollHelp, "训练中按住 \"s\" 并用滚轮调节"},
};

const char* Lookup(std::string_view key) {
  const auto& table = g_language == Language::kChinese ? kChineseStrings : kEnglishStrings;
  auto it = table.find(key);
  if (it != table.end()) {
    return it->second;
  }
  auto en_it = kEnglishStrings.find(key);
  if (en_it != kEnglishStrings.end()) {
    return en_it->second;
  }
  return "???";
}

}  // namespace

void SetLanguage(Language language) {
  g_language = language;
}

Language GetLanguage() {
  return g_language;
}

const char* Tr(std::string_view key) {
  return Lookup(key);
}

void LoadLanguageFromStore(LocalStore& store) {
  auto language = store.GetInt(kLanguageStoreKey);
  if (language && *language == static_cast<int>(Language::kChinese)) {
    SetLanguage(Language::kChinese);
    return;
  }
  SetLanguage(Language::kEnglish);
}

void SaveLanguageToStore(LocalStore& store, Language language) {
  store.PutInt(kLanguageStoreKey, static_cast<int>(language));
  SetLanguage(language);
}

}  // namespace aim
