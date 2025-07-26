#pragma once
#include "Avionics_HAL.h"

// warps HAL
#define CtorWrapper(ClassName, Member) \
    explicit ClassName(auto... arg) : Member(std::forward<decltype(arg)>(arg)...) {}
