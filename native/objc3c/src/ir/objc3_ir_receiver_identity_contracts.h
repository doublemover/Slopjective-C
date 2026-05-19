#pragma once

#include <cstddef>

int NextNonZeroReceiverIdentityValue(std::size_t ordinal, int salt);
int BuildInstanceReceiverIdentityValue(int class_identity);
int BuildClassReceiverIdentityValue(int class_identity);
