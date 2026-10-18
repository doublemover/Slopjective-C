#ifndef OBJC3C_CLANG_FOUNDATION_SDK_IDENTITY_H
#define OBJC3C_CLANG_FOUNDATION_SDK_IDENTITY_H
#include <string>
namespace objc3::clang_foundation {
bool VerifyRelocatedSDK(const std::string &root, const std::string &driver_hash,
                        const std::string &resource_hash, std::string &driver,
                        std::string &resources, std::string &error);
}
#endif
