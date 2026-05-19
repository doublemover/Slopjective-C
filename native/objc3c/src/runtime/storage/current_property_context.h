#pragma once

namespace objc3c::runtime {

int ReadCurrentPropertyI32();
void WriteCurrentPropertyI32(int value);
int ExchangeCurrentPropertyI32(int value);
int BindCurrentPropertyContextForTesting(
    int receiver, const char *class_name, const char *property_name);
void ClearCurrentPropertyContextForTesting();

}  // namespace objc3c::runtime
