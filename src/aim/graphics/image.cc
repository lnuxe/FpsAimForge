#include "image.h"

#include "aim/common/log.h"
#include "wuffs_sdl_image.h"

namespace aim {

SDL_Surface* LoadImageSurface(const std::filesystem::path& path) {
  size_t data_size = 0;
  void* file_data = SDL_LoadFile(path.string().c_str(), &data_size);
  if (!file_data) {
    Logger::get()->warn("Failed to load image file {}", path.string());
    return nullptr;
  }

  const auto* byte_ptr = static_cast<const uint8_t*>(file_data);
  std::vector<uint8_t> buffer(byte_ptr, byte_ptr + data_size);
  SDL_free(file_data);

  std::pair<SDL_Surface*, std::string> result = IMG_LoadWuffs_IO(buffer);
  SDL_Surface* surface = result.first;
  if (surface == nullptr) {
    Logger::get()->warn("Failed to load image {}, : {}", path.string(), result.second);
  }
  return surface;
}

Image::Image(const std::filesystem::path& path) {
  surface_ = LoadImageSurface(path);
  if (surface_ == nullptr) {
    Logger::get()->warn(
        "Failed to load image {}, IMG_GetError(): {}", path.string(), SDL_GetError());
    return;
  }
  width_ = surface_->w;
  height_ = surface_->h;
}

}  // namespace aim
