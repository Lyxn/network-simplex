//
// Created by LyxnO on 2019/11/9.
//

#ifndef NETWORK_SIMPLEX_COMMON_H
#define NETWORK_SIMPLEX_COMMON_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
                TypeName(const TypeName&); TypeName& operator=(const TypeName&);
#endif

#include <functional>

namespace network {

enum ReturnStatus {
    RET_SUCCESS = 0,
    RET_FAIL = 1,
    RET_ERROR = 2,
};

}

#endif //NETWORK_SIMPLEX_COMMON_H
