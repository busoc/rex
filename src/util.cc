#include <sstream>
#include "util.h"

std::string util::hex::to_hex(char* buf, int size) {
  std::ostringstream os;
  for (int i = 0; i < size; i++) {
    unsigned char b = (unsigned char)(buf[i]);
    int high = b >> 4;
    int low = b & 0xF;

    os.put(alphabet[high]);
    os.put(alphabet[low]);
  }
  return os.str();
}

unsigned util::endian::swap_bytes(unsigned ui) {
  return (ui >> 24) |
     ((ui<<8) & 0x00FF0000) |
     ((ui>>8) & 0x0000FF00) |
     (ui << 24);
}

unsigned short util::endian::swap_bytes(unsigned short ui) {
  return (ui >> 8) | (ui << 8);
}

ever::instant util::time::time5::as_instant(bool raw) const {
  unsigned time = util::endian::swap_bytes(coarse);
  double ms = double(fine)/256;
  auto it = ever::instant{time, int(ms*1000)};

  if (!raw) {
    it = it.add(delta).to_gps();
  }
  return it;
}

ever::instant util::time::time6::as_instant(bool raw) const {
  double ms = double(fine)/65536;
  auto it = ever::instant{coarse, int(ms*1000)};

  if (!raw) {
    it = it.add(delta).to_gps();
  }
  return it;
}

ever::instant util::time::gps2unix(ever::instant it) {
  return it.add(delta);
}
