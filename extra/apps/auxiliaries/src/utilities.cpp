#include "utilities.h"

void getImageNameByIdx(const int idx, const std::string &ext,
    std::string &name, const int len)
{
    std::ostringstream sidx;
    sidx << idx;
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(len) << 
        std::right << sidx.str() << "." << ext;
    name = oss.str();
}
