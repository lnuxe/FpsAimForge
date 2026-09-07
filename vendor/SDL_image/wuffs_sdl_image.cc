#include "wuffs_sdl_image.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include <vector>

#define WUFFS_IMPLEMENTATION
#define WUFFS_CONFIG__STATIC_FUNCTIONS

#define WUFFS_CONFIG__MODULES
#define WUFFS_CONFIG__MODULE__ADLER32
#define WUFFS_CONFIG__MODULE__AUX__BASE
#define WUFFS_CONFIG__MODULE__AUX__IMAGE
#define WUFFS_CONFIG__MODULE__BASE
#define WUFFS_CONFIG__MODULE__CRC32
#define WUFFS_CONFIG__MODULE__DEFLATE
#define WUFFS_CONFIG__MODULE__PNG
#define WUFFS_CONFIG__MODULE__ZLIB

// Defining the WUFFS_CONFIG__DST_PIXEL_FORMAT__ENABLE_ALLOWLIST (and the
// associated ETC__ALLOW_FOO) macros are optional, but can lead to smaller
// programs (in terms of binary size). By default (without these macros),
// Wuffs' standard library can decode images to a variety of pixel formats,
// such as BGR_565, BGRA_PREMUL or RGBA_NONPREMUL. The destination pixel format
// is selectable at runtime. Using these macros essentially makes the selection
// at compile time, by narrowing the list of supported destination pixel
// formats. The FOO in ETC__ALLOW_FOO should match the pixel format passed (as
// part of the wuffs_base__image_config argument) to the decode_frame method.
//
// If using the wuffs_aux C++ API, without overriding the SelectPixfmt method,
// the implicit destination pixel format is BGRA_PREMUL.
#define WUFFS_CONFIG__DST_PIXEL_FORMAT__ENABLE_ALLOWLIST
#define WUFFS_CONFIG__DST_PIXEL_FORMAT__ALLOW_BGRA_NONPREMUL

// If building this program in an environment that doesn't easily accommodate
// relative includes, you can use the script/inline-c-relative-includes.go
// program to generate a stand-alone C file.
#include "wuffs-v0.3.c"

class Wuffs_Load_RW_Callbacks : public wuffs_aux::DecodeImageCallbacks {
 public:
  Wuffs_Load_RW_Callbacks() : m_surface(NULL) {}

  ~Wuffs_Load_RW_Callbacks() {
    if (m_surface) {
      SDL_UnlockSurface(m_surface);
      SDL_DestroySurface(m_surface);
      m_surface = NULL;
    }
  }

  SDL_Surface*  //
  TakeSurface() {
    if (!m_surface) {
      return NULL;
    }
    SDL_UnlockSurface(m_surface);
    SDL_Surface* ret = m_surface;
    m_surface = NULL;
    return ret;
  }

 private:
  wuffs_base__pixel_format  //
  SelectPixfmt(const wuffs_base__image_config& image_config) override {
    // Regardless of endianness, SDL_PIXELFORMAT_BGRA32 (from a few lines
    // below) is equivalent to WUFFS_BASE__PIXEL_FORMAT__BGRA_NONPREMUL.
    return wuffs_base__make_pixel_format(WUFFS_BASE__PIXEL_FORMAT__BGRA_NONPREMUL);
  }

  AllocPixbufResult  //
  AllocPixbuf(const wuffs_base__image_config& image_config,
              bool allow_uninitialized_memory) override {
    if (m_surface) {
      SDL_UnlockSurface(m_surface);
      SDL_DestroySurface(m_surface);
      m_surface = NULL;
    }
    uint32_t w = image_config.pixcfg.width();
    uint32_t h = image_config.pixcfg.height();
    if ((w > 0xFFFFFF) || (h > 0xFFFFFF)) {
      return AllocPixbufResult("Wuffs_Load_RW_Callbacks: image is too large");
    }
    uint32_t sdl_pixelformat = SDL_PIXELFORMAT_BGRA32;

    // (§) Uncomment this line of code to invert the BGRA/RGBA color order.
    // This isn't a generally useful feature for an image viewer, but it should
    // make it obvious, when pressing the TAB key, whether you're using the
    // Wuffs (inverted) or SDL_image (correct) decoder.
    //
    // sdl_pixelformat = SDL_PIXELFORMAT_RGBA32;

    m_surface = SDL_CreateSurface(static_cast<int>(w), static_cast<int>(h), SDL_PIXELFORMAT_BGRA32);
    if (!m_surface) {
      return AllocPixbufResult("Wuffs_Load_RW_Callbacks: SDL_CreateRGBSurface failed");
    }
    SDL_LockSurface(m_surface);
    wuffs_base__pixel_buffer pixbuf;
    wuffs_base__status status =
        pixbuf.set_interleaved(&image_config.pixcfg,
                               wuffs_base__make_table_u8(static_cast<uint8_t*>(m_surface->pixels),
                                                         m_surface->w * 4,
                                                         m_surface->h,
                                                         m_surface->pitch),
                               wuffs_base__empty_slice_u8());
    if (!status.is_ok()) {
      SDL_UnlockSurface(m_surface);
      SDL_DestroySurface(m_surface);
      m_surface = NULL;
      return AllocPixbufResult(status.message());
    }
    return AllocPixbufResult(wuffs_aux::MemOwner(NULL, &free), pixbuf);
  }

  SDL_Surface* m_surface;
};

std::pair<SDL_Surface*, std::string> IMG_LoadWuffs_IO(const std::vector<uint8_t>& file_content) {
  Wuffs_Load_RW_Callbacks callbacks;
  wuffs_aux::sync_io::MemoryInput input(const_cast<uint8_t*>(file_content.data()),
                                        file_content.size());

  wuffs_aux::DecodeImageResult res = wuffs_aux::DecodeImage(callbacks, input);
  if (!res.error_message.empty()) {
    return std::make_pair<SDL_Surface*, std::string>(NULL, std::move(res.error_message));
  }
  return std::make_pair<SDL_Surface*, std::string>(callbacks.TakeSurface(), std::string());
}
