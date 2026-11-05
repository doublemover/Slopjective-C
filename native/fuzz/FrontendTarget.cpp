#include "FrontendTarget.h"
#include "clang_foundation/frontend_api.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/SHA256.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <string_view>

namespace objc3::fuzz {
namespace {
[[noreturn]] void Failure(const char *message) {
  std::fprintf(stderr, "OBJ3_FRONTEND_PROPERTY: %s\n", message);
  std::abort();
}

std::string Hash(const FrontendObservation &value) {
  const std::string record = std::to_string(value.disposition) + ":" +
      std::to_string(value.diagnostics) + ":" + value.digest;
  return llvm::toHex(llvm::SHA256::hash({
      reinterpret_cast<const std::uint8_t *>(record.data()), record.size()}), true);
}
} // namespace

FrontendObservation Frontend(const std::uint8_t *data, std::size_t size) {
  FrontendObservation observation;
  if (size < 4 || size > kMaxFrontendInput) {
    observation.disposition = 2;
    observation.digest = Hash(observation);
    return observation;
  }
  const char *source = reinterpret_cast<const char *>(data + 4);
  const std::size_t source_size = size - 4;
  const std::array<const char *, 3> arguments = {
      "-Wno-everything", "-ferror-limit=4", "-fno-caret-diagnostics"};
  objc3c_frontend_v2_options base{};
  base.struct_size = sizeof(base);
  base.abi_version = OBJC3C_FRONTEND_V2_ABI;
  base.input_path = (data[0] % 4 < 2) ? "fuzz.cc" : "fuzz.mm";
  base.source = source;
  base.source_size = source_size;
  base.working_directory = ".";
  base.arguments = arguments.data();
  base.argument_count = arguments.size();
  base.language = data[0] % 4;
  base.emit = 0; // Syntax/sema only: byte inputs cannot write or execute programs.
  base.profile = data[1] % 4;
  base.objc3 = data[2] & 1;
  base.arc = (data[2] >> 1) & 1;
  base.strict_concurrency = (data[2] >> 2) & 1;
  base.module_name = "FuzzInput";
  base.package_identity = "objc3.fuzz.local";

  objc3c_frontend_v2_result *result = nullptr;
  int disposition = 0;
  if (data[3] & 1) {
    objc3c_frontend_v3_options options{};
    options.struct_size = sizeof(options);
    options.abi_version = OBJC3C_FRONTEND_V3_ABI;
    options.base = base;
    if (data[3] & 2) {
      const char *root = std::getenv("OBJC3_FUZZ_IMPORT_ROOT");
      const char *fingerprint = std::getenv("OBJC3_FUZZ_IMPORT_FINGERPRINT");
      const char *package = std::getenv("OBJC3_FUZZ_IMPORT_PACKAGE");
      if (!root || !*root || !fingerprint || !*fingerprint || !package || !*package)
        Failure("qualification-owned import fixture is unavailable");
      const std::array<const char *, 1> search_paths = {root};
      objc3c_frontend_v3_import imported{};
      imported.struct_size = sizeof(imported);
      imported.canonical_name = "Fuzz.Dependency";
      imported.semantic_fingerprint = fingerprint;
      imported.package_identity = package;
      imported.public_import = (data[3] >> 4) & 1;
      imported.reexported = (data[3] >> 5) & 1;
      objc3c_frontend_v3_spi_grant grant{"Fuzz.Dependency", "Testing"};
      options.base.language = 2 + (data[0] & 1);
      options.base.objc3 = 1;
      options.base.target_triple = "x86_64-unknown-linux-gnu";
      options.base.objc_runtime = "gnustep-2.2";
      options.interface_search_paths = search_paths.data();
      options.interface_search_path_count = search_paths.size();
      options.imports = &imported;
      options.import_count = 1;
      options.source_directed_imports = (data[3] >> 2) & 1;
      if (data[3] & 8) {
        options.spi_grants = &grant;
        options.spi_grant_count = 1;
      }
      disposition = objc3c_frontend_v3_compile(&options, &result);
    } else {
      disposition = objc3c_frontend_v3_compile(&options, &result);
    }
  } else {
    disposition = objc3c_frontend_v2_compile(&base, &result);
  }
  if (disposition < 0 || disposition > 3 || (disposition == 2 && result))
    Failure("embedding API returned an invalid disposition/result pair");
  observation.disposition = disposition;
  if (result) {
    observation.diagnostics = objc3c_frontend_v2_diagnostic_count(result);
    if (observation.diagnostics > 4096)
      Failure("embedding API exceeded the bounded diagnostic inventory");
    std::string record = std::to_string(objc3c_frontend_v2_result_success(result));
    for (std::size_t index = 0; index != observation.diagnostics; ++index) {
      record += ':' + std::to_string(objc3c_frontend_v2_diagnostic_id(result, index));
      record += ':' + std::to_string(objc3c_frontend_v2_diagnostic_severity(result, index));
      if (const char *message = objc3c_frontend_v2_diagnostic_message(result, index))
        record.append(message, std::min<std::size_t>(std::char_traits<char>::length(message), 4096));
    }
    observation.digest = llvm::toHex(llvm::SHA256::hash({
        reinterpret_cast<const std::uint8_t *>(record.data()), record.size()}), true);
#if OBJC3_FUZZ_FRONTEND_CLEANUP_FAULT
    Failure("injected result-cleanup omission");
#else
    objc3c_frontend_v2_result_destroy(result);
#endif
  } else {
    observation.digest = Hash(observation);
  }
  return observation;
}

std::vector<std::uint8_t> FrontendSeed(unsigned variant) {
  static constexpr std::string_view sources[] = {
      "int answer(void) { return 42; }\n",
      "@interface Value @end\n@implementation Value @end\n",
      "@interface Broken\n",
      "typedef __objc3_optional(int) Maybe; Maybe value;\n",
      "@import Fuzz.Dependency;\nint use_provider(void) { return provider(); }\n",
      "template<class T> T identity(T x) { return x; }\n",
      "int f(void) { return unknown_name; }\n",
      ""};
  const auto source = sources[variant % std::size(sources)];
  std::vector<std::uint8_t> result = {
      static_cast<std::uint8_t>(variant % 4),
      static_cast<std::uint8_t>(variant % 4),
      static_cast<std::uint8_t>(variant),
      static_cast<std::uint8_t>(variant == 4 ? 7 : variant == 5 ? 3 : variant & 1)};
  result.insert(result.end(), source.begin(), source.end());
  return result;
}
} // namespace objc3::fuzz
