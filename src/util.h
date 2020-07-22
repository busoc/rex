#ifndef __REX_UTIL__
#define __REX_UTIL__

#include "ever.h"

namespace util {
  namespace hex {
    const std::string alphabet = "0123456789ABCDEF";
    std::string to_hex(char* buf, int size);
  }

  namespace endian {
    unsigned swap_bytes(unsigned ui);
    unsigned short swap_bytes(unsigned short ui);
  }

  namespace time {
    const ever::instant unix{0};
    const ever::instant gps{315964800};
    // two lines bellow cause a segmentation fault???
    // const ever::instant unix{1970, 1, 1};
    // const ever::instant gps{1980, 1, 6};
    const long long delta = gps.diff(unix);

    #pragma pack(push, 1)
    struct time5 {
      unsigned coarse;
      unsigned char fine;

      ever::instant as_instant(bool raw = false) const;
    };

    struct time6 {
      unsigned coarse;
      unsigned short fine;

      ever::instant as_instant(bool raw = false) const;
    };
    #pragma pack(pop)

    ever::instant gps2unix(ever::instant it);
  }
}

#endif
