#include "font_manager.h"

#include "aim/common/log.h"
#include "aim/i18n/i18n.h"

namespace aim {
namespace {}  // namespace

int FontManager::large_font_size() {
  return 38;
}

int FontManager::default_font_size() {
  return 22;
}

int FontManager::medium_font_size() {
  return 30;
}

namespace {
const ImWchar kCjkRanges[] = {
    0x2E80, 0x2EFF,  // CJK Radicals
    0x3000, 0x30FF,  // CJK symbols/punctuation, Hiragana, Katakana
    0x3400, 0x4DBF,  // CJK Extension A
    0x4E00, 0x9FFF,  // CJK Unified Ideographs
    0xF900, 0xFAFF,  // CJK Compatibility Ideographs
    0xFF00, 0xFFEF,  // Full-width forms
    0,
};
}  // namespace

bool FontManager::LoadFonts() {
  auto font_path = fonts_path_ / "Roboto-Regular.ttf";
  auto bold_font_path = fonts_path_ / "Roboto-Bold.ttf";
  auto material_icons_path = fonts_path_ / "MaterialIcons-Regular.ttf";
  auto cjk_font_path = fonts_path_ / "NotoSansSC-Regular.otf";
  auto cjk_bold_font_path = fonts_path_ / "NotoSansSC-Bold.otf";

  ImGuiIO& io = ImGui::GetIO();
  default_font_ = io.Fonts->AddFontFromFileTTF(font_path.string().c_str(), default_font_size());
  if (default_font_ == nullptr) {
    Logger::get()->error("Unable to load default font from: {}", font_path.string());
    return false;
  }

  // See https://github.com/ocornut/imgui/issues/3247
  static const ImWchar icons_ranges[] = {0xE000, 0xFFFF, 0};
  ImFontConfig icons_config;
  icons_config.MergeMode = true;
  icons_config.PixelSnapH = true;
  icons_config.GlyphOffset.y = 4;

  // Merge a CJK font as a fallback so Chinese characters render instead of tofu boxes.
  ImFontConfig cjk_config;
  cjk_config.MergeMode = true;
  cjk_config.PixelSnapH = true;
  if (io.Fonts->AddFontFromFileTTF(cjk_font_path.string().c_str(), default_font_size(),
                                   &cjk_config, kCjkRanges) == nullptr) {
    Logger::get()->error("Unable to load CJK fallback font from: {}", cjk_font_path.string());
  }

  material_icons_font_ = io.Fonts->AddFontFromFileTTF(
      material_icons_path.string().c_str(), default_font_size(), &icons_config, icons_ranges);
  if (material_icons_font_ == nullptr) {
    Logger::get()->error("Unable to load material icons font from: {}",
                         material_icons_path.string());
    return false;
  }

  large_font_ = io.Fonts->AddFontFromFileTTF(font_path.string().c_str(), large_font_size());
  if (large_font_ == nullptr) {
    Logger::get()->error("Unable to load large font from: {}", font_path.string());
    return false;
  }
  cjk_config.GlyphOffset.y = 0;
  io.Fonts->AddFontFromFileTTF(
      cjk_font_path.string().c_str(), large_font_size(), &cjk_config, kCjkRanges);
  icons_config.GlyphOffset.y = 8;
  io.Fonts->AddFontFromFileTTF(
      material_icons_path.string().c_str(), large_font_size(), &icons_config, icons_ranges);

  medium_font_ = io.Fonts->AddFontFromFileTTF(font_path.string().c_str(), medium_font_size());
  if (medium_font_ == nullptr) {
    Logger::get()->error("Unable to load medium font from: {}", font_path.string());
    return false;
  }
  io.Fonts->AddFontFromFileTTF(
      cjk_font_path.string().c_str(), medium_font_size(), &cjk_config, kCjkRanges);
  icons_config.GlyphOffset.y = 6;
  io.Fonts->AddFontFromFileTTF(
      material_icons_path.string().c_str(), medium_font_size(), &icons_config, icons_ranges);

  default_bold_font_ =
      io.Fonts->AddFontFromFileTTF(bold_font_path.string().c_str(), default_font_size());
  if (default_bold_font_ == nullptr) {
    Logger::get()->error("Unable to load default bold font from: {}", bold_font_path.string());
    return false;
  }
  io.Fonts->AddFontFromFileTTF(
      cjk_bold_font_path.string().c_str(), default_font_size(), &cjk_config, kCjkRanges);

  large_bold_font_ =
      io.Fonts->AddFontFromFileTTF(bold_font_path.string().c_str(), large_font_size());
  if (large_bold_font_ == nullptr) {
    Logger::get()->error("Unable to load large bold font from: {}", bold_font_path.string());
    return false;
  }
  io.Fonts->AddFontFromFileTTF(
      cjk_bold_font_path.string().c_str(), large_font_size(), &cjk_config, kCjkRanges);
  icons_config.GlyphOffset.y = 8;
  io.Fonts->AddFontFromFileTTF(
      material_icons_path.string().c_str(), large_font_size(), &icons_config, icons_ranges);

  medium_bold_font_ =
      io.Fonts->AddFontFromFileTTF(bold_font_path.string().c_str(), medium_font_size());
  if (medium_bold_font_ == nullptr) {
    Logger::get()->error("Unable to load medium bold font from: {}", bold_font_path.string());
    return false;
  }
  io.Fonts->AddFontFromFileTTF(
      cjk_bold_font_path.string().c_str(), medium_font_size(), &cjk_config, kCjkRanges);

  return true;
}

}  // namespace aim
