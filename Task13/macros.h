//
// Created by jakubszwedowicz on 1/4/25.
//
#pragma once

#ifdef __GNUC__
#define LIKELY(x) __builtin_expect(!!(x), 1)
#else
#define LIKELY(x) (x)
#endif