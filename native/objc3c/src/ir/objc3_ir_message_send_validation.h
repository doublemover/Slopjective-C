#pragma once

#include <cstddef>
#include <string>

struct Objc3Program;

bool ValidateObjc3IRMessageSendArityContract(
    const Objc3Program &program, std::size_t runtime_dispatch_arg_slots,
    std::string &error);
