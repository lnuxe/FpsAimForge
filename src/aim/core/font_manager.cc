#include "font_manager.h"

#include <filesystem>
#include <optional>
#include <vector>

#include "aim/common/log.h"

namespace aim {
namespace {

std::optional<std::filesystem::path> FindCjkFontPath() {
  std::vector<std::filesystem::path> candidates;
#if defined(__APPLE__)
  candidates = {
      "/System/Library/Fonts/PingFang.ttc",
      "/System/Library/Fonts/STHeiti Light.ttc",
      "/Library/Fonts/Arial Unicode.ttf",
  };
#elif defined(_WIN32)
  candidates = {
      "C:/Windows/Fonts/msyh.ttc",
      "C:/Windows/Fonts/msyhbd.ttc",
      "C:/Windows/Fonts/simhei.ttf",
  };
#else
  candidates = {
      "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc",
      "/usr/share/fonts/truetype/noto/NotoSansCJK-Regular.ttc",
      "/usr/share/fonts/google-noto-cjk/NotoSansCJK-Regular.ttc",
  };
#endif
  for (const auto& path : candidates) {
    if (std::filesystem::exists(path)) {
      return path;
    }
  }
  return std::nullopt;
}

void MergeCjkGlyphs(ImFont* font, float size, float glyph_offset_y) {
  auto cjk_font_path = FindCjkFontPath();
  if (!cjk_font_path || font == nullptr) {
    return;
  }

  ImGuiIO& io = ImGui::GetIO();
  ImFontConfig config;
  config.MergeMode = true;
  config.PixelSnapH = true;
  config.GlyphOffset.y = glyph_offset_y;
  io.Fonts->AddFontFromFileTTF(cjk_font_path->string().c_str(),
                               size,
                               &config,
                               io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
}

}  // namespace

int FontManager::large_font_size() {
  return 38;
}

int FontManager::default_font_size() {
  return 22;
}

int FontManager::medium_font_size() {
  return 30;
}

bool FontManager::LoadFonts() {
  auto font_path = fonts_path_ / "Roboto-Regular.ttf";
  auto bold_font_path = fonts_path_ / "Roboto-Bold.ttf";
  auto material_icons_path = fonts_path_ / "MaterialIcons-Regular.ttf";
  // auto material_icons_path = fonts_path_ / "MaterialSymbolsOutlined.ttf";

  ImGuiIO& io = ImGui::GetIO();
  default_font_ = io.Fonts->AddFontFromFileTTF(font_path.string().c_str(), default_font_size());
  if (default_font_ == nullptr) {
    Logger::get()->error("Unable to load default font from: {}", font_path.string());
    return false;
  }
  MergeCjkGlyphs(default_font_, default_font_size(), 4);

  // See https://github.com/ocornut/imgui/issues/3247
  static const ImWchar icons_ranges[] = {0xE000, 0xFFFF, 0};
  ImFontConfig icons_config;
  icons_config.MergeMode = true;
  icons_config.PixelSnapH = true;
  icons_config.GlyphOffset.y = 4;

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
  MergeCjkGlyphs(large_font_, large_font_size(), 8);
  icons_config.GlyphOffset.y = 8;
  io.Fonts->AddFontFromFileTTF(
      material_icons_path.string().c_str(), large_font_size(), &icons_config, icons_ranges);

  medium_font_ = io.Fonts->AddFontFromFileTTF(font_path.string().c_str(), medium_font_size());
  if (medium_font_ == nullptr) {
    Logger::get()->error("Unable to load medium font from: {}", font_path.string());
    return false;
  }
  MergeCjkGlyphs(medium_font_, medium_font_size(), 6);
  icons_config.GlyphOffset.y = 6;
  io.Fonts->AddFontFromFileTTF(
      material_icons_path.string().c_str(), medium_font_size(), &icons_config, icons_ranges);

  default_bold_font_ =
      io.Fonts->AddFontFromFileTTF(bold_font_path.string().c_str(), default_font_size());
  if (default_bold_font_ == nullptr) {
    Logger::get()->error("Unable to load default bold font from: {}", bold_font_path.string());
    return false;
  }
  MergeCjkGlyphs(default_bold_font_, default_font_size(), 4);

  large_bold_font_ =
      io.Fonts->AddFontFromFileTTF(bold_font_path.string().c_str(), large_font_size());
  if (large_bold_font_ == nullptr) {
    Logger::get()->error("Unable to load large bold font from: {}", bold_font_path.string());
    return false;
  }
  MergeCjkGlyphs(large_bold_font_, large_font_size(), 8);

  medium_bold_font_ =
      io.Fonts->AddFontFromFileTTF(bold_font_path.string().c_str(), medium_font_size());
  if (medium_bold_font_ == nullptr) {
    Logger::get()->error("Unable to load medium bold font from: {}", bold_font_path.string());
    return false;
  }
  MergeCjkGlyphs(medium_bold_font_, medium_font_size(), 6);

  return true;
}

}  // namespace aim
