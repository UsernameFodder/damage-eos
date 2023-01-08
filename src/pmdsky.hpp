// Wrapper around pmdsky-debug headers, primarily for enum definitions

#ifndef PMDSKY_HPP_
#define PMDSKY_HPP_

namespace eos {
#define PMDSKY_UNSIZED_HEADERS
#include "pmdsky-debug/headers/pmdsky.h"
#undef PMDSKY_UNSIZED_HEADERS
} // namespace eos

#endif
