#include <string>
#include <vector>

#include "SDL3/SDL_surface.h"

std::pair<SDL_Surface*, std::string> IMG_LoadWuffs_IO(const std::vector<uint8_t>& file_content);
