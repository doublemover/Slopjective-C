#pragma once

namespace objc3c::runtime {

void RuntimeStoreThrownErrorI32(int *slot, int value);
int RuntimeLoadThrownErrorI32(const int *slot);
int RuntimeBridgeStatusErrorI32(int status_value, int mapped_error_value);
int RuntimeBridgeNSErrorErrorI32(int error_value);
int RuntimeBridgeForeignExceptionErrorI32(int foreign_kind, int payload_value,
                                          int mapped_error_value);

}  // namespace objc3c::runtime
