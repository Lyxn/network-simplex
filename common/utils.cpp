//
// Created by LyxnO on 2019/11/9.
//

#include "utils.h"

#include <algorithm>
#include <sstream>

namespace network {

void SplitStr(const std::string &str, const std::string &sep, std::vector<std::string> &items) {
    size_t sep_size = sep.size();
    size_t sep_pos = str.find(sep);
    size_t lst_pos = 0;
    while (sep_pos != std::string::npos) {
        items.push_back(str.substr(lst_pos, sep_pos - lst_pos));
        lst_pos = sep_pos + sep_size;
        sep_pos = str.find(sep, lst_pos);
    }
    if (lst_pos <= str.size()) {
        items.push_back(str.substr(lst_pos));
    }
}

}