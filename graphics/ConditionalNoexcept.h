#ifndef __CONDITIONALNOEXCEPT_H
#define __CONDITIONALNOEXCEPT_H

#define NOXND noexcept(!IS_DEBUG)

#endif //ConditionalNoexcept.h
