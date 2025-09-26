#ifndef __CONDITIONALNOEXCEPT_H
#define __CONDITIONALNOEXCEPT_H

#define noxnd noexcept(!IS_DEBUG)

#endif //ConditionalNoexcept.h
