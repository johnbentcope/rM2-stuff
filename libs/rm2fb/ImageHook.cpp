#include "SharedBuffer.h"

#include <dlfcn.h>
#include <iostream>

// TODO: clean
extern bool inXochitl;

extern "C" {

void
_ZN6QImageC1EiiNS_6FormatE(void* that, int x, int y, int f) {
  static bool first_alloc = true;

  static const auto qImageCtor = (void (*)(void*, int, int, int))dlsym(
    RTLD_NEXT, "_ZN6QImageC1EiiNS_6FormatE");
  static const auto qImageCtorWithBuffer = (void (*)(
    void*, uint8_t*, int32_t, int32_t, int32_t, int, void (*)(void*), void*))
    dlsym(RTLD_NEXT, "_ZN6QImageC1EPhiiiNS_6FormatEPFvPvES2_");

  if (const auto& fb = SharedFB::getInstance(); inXochitl && x == fb_width &&
                                                y == fb_height && first_alloc &&
                                                fb.has_value()) {
    std::cerr << "REPLACING THE IMAGE with shared memory\n";
    first_alloc = false;

    qImageCtorWithBuffer(that,
                         reinterpret_cast<uint8_t*>(fb->mem.get()),
                         fb_width,
                         fb_height,
                         fb_width * fb_pixel_size,
                         f,
                         nullptr,
                         nullptr);
    return;
  }

  qImageCtor(that, x, y, f);
}
}
