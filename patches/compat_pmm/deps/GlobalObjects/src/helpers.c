#include "helpers.h"
#include "global.h"

bool isSegmentedPtr(void *p) {
    return (u32)p >> 24 <= 0xF;
}
