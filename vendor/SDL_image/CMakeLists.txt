add_library(SDL_image
  wuffs_sdl_image.h
  wuffs_sdl_image.cc)

target_include_directories(SDL_image PUBLIC .)
target_link_libraries(SDL_image PUBLIC SDL3::SDL3)
