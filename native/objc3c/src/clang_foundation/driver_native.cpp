#include "clang_foundation/driver_native.h"
#include "clang_foundation/sdk_identity.h"
#include "clang_foundation/source_identity.h"
#include "clang/Driver/Driver.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Program.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

extern char **environ;

bool UsesInterfaceCompiler(int argc, char **argv) {
  // These operations use the same in-process Clang frontend, with semantic
  // publication and transactional outputs rather than a linker invocation.
  for (int i = 1; i < argc; ++i) {
    std::string_view arg(argv[i]);
    if (arg == "--") break;
    if (arg == "--emit-object" || arg == "--emit-llvm" ||
        arg == "--emit-interface" || arg == "--project-ownership" ||
        arg == "--export-declaration" || arg == "--module-source-root" ||
        arg == "--module-owned-names" || arg == "--import-spi" ||
        arg == "--import-module" || arg == "--emit-dependency-record" ||
        arg == "--source-imports")
      return true;
  }
  return false;
}

int RunNativeDriver(int argc, char **raw_argv) {
  namespace fs = std::filesystem;
  using namespace objc3::clang_foundation;
  llvm::SmallVector<const char *, 32> expanded(raw_argv, raw_argv + argc);
  llvm::BumpPtrAllocator allocator;
  if (auto error = clang::driver::expandResponseFiles(expanded, false, allocator)) {
    std::cerr << llvm::toString(std::move(error)) << '\n';
    return 2;
  }
  argc = expanded.size();
  const char **argv = expanded.data();
  if (argc < 2) {
    std::cerr << "usage: objc3c <sources or objects> [-o program] [-c] [Clang options]\n"
                 "       objc3c <source> --emit-interface module.objc3i --module name --package identity\n";
    return 2;
  }
  const auto executable = fs::path(llvm::sys::fs::getMainExecutable(
      argv[0], reinterpret_cast<void *>(&RunNativeDriver)));
  auto root = executable.parent_path().parent_path();
  if (!fs::is_directory(root / "lib/clang"))
    root = fs::path(OBJC3C_FOUNDATION_DRIVER).parent_path().parent_path();
  const bool objcxx = std::any_of(argv + 1, argv + argc,
      [](const char *arg) { return std::string_view(arg) == "--objcxx"; });
  std::vector<std::string> forwarded;
  bool cxx = false, objc = false, source = false, query = false;
  bool header = false;
  for (int i = 1; i < argc; ++i) {
    std::string arg(argv[i]);
    if (arg == "--sdk-root") {
      if (++i == argc) { std::cerr << "--sdk-root requires a path\n"; return 2; }
      root = argv[i];
    } else if (arg == "--objc3") {
      objc = true;
    } else if (arg == "--objcxx") {
      cxx = objc = true;
    } else if (arg == "-x" || arg.starts_with("-x")) {
      // Clang language overrides also select linker runtime defaults. A .m
      // input explicitly compiled as Objective-C++ needs the C++ driver.
      std::string language;
      forwarded.push_back(arg);
      if (arg == "-x") {
        if (++i == argc) { std::cerr << "-x requires a language\n"; return 2; }
        language = argv[i];
        forwarded.push_back(language);
      } else {
        language = arg.substr(2);
      }
      if (language.find("c++") != std::string::npos) cxx = true;
      if (language.starts_with("objective-c")) objc = true;
      // Clang infers precompilation from a header language without requiring
      // -c. Linker inputs under that -x would become extra PCH output jobs.
      if (language.ends_with("-header")) header = true;
    } else if (arg == "--arc") {
      forwarded.push_back("-fobjc-arc");
    } else if (arg == "--runtime" || arg == "--profile" || arg == "--target") {
      if (++i == argc) { std::cerr << arg << " requires a value\n"; return 2; }
      forwarded.push_back((arg == "--runtime" ? "-fobjc-runtime=" :
                           arg == "--profile" ? "-fobjc3-profile=" : "--target=") +
                          std::string(argv[i]));
    } else if (arg == "--strict-concurrency") {
      forwarded.push_back("-fobjc3-strict-concurrency");
    } else if (arg == "--") {
      // Clang's own options remain options after the interface CLI delimiter.
      continue;
    } else {
      if (arg == "--help" || arg == "--version" || arg == "-print-resource-dir")
        query = true;
      const auto ext = fs::path(arg).extension();
      if (!arg.starts_with('-') && (ext == ".m" || ext == ".mm" || ext == ".objc3"))
        objc = true;
      if (!arg.starts_with('-') && (ext == ".mm" || ext == ".cpp" || ext == ".cc" || ext == ".cxx"))
        cxx = true;
      if (!arg.starts_with('-') && (ext == ".c" || ext == ".m" || ext == ".mm" ||
          ext == ".objc3" || ext == ".cpp" || ext == ".cc" || ext == ".cxx"))
        source = true;
      if (ext == ".objc3" && !arg.starts_with('-')) {
        forwarded.insert(forwarded.end(), {"-x", objcxx ? "objective-c++" : "objective-c", arg});
        const bool more_inputs = std::any_of(argv + i + 1, argv + argc, [](const char *next) {
          const std::string_view value(next);
          const auto suffix = fs::path(next).extension();
          return !value.starts_with('-') && (value.starts_with('@') || suffix == ".c" ||
              suffix == ".m" || suffix == ".mm" || suffix == ".cc" || suffix == ".cpp" ||
              suffix == ".cxx" || suffix == ".o" || suffix == ".a" || suffix == ".so");
        });
        if (more_inputs) forwarded.insert(forwarded.end(), {"-x", "none"});
      } else
        forwarded.push_back(arg);
    }
  }
  std::string clang, resources, error;
  if (!VerifyRelocatedSDK(root.string(), OBJC3C_FOUNDATION_DRIVER_SHA256,
                          OBJC3C_FOUNDATION_RESOURCE_SHA256, clang, resources, error)) {
    std::cerr << error << '\n';
    return 2;
  }
  // Linking object-only invocations must also resolve C++ runtime references.
  std::string program = cxx || (!source && !query) ? (root / "bin/clang++").string() : clang;
  std::vector<std::string> args{program, "-resource-dir", resources};
  if (objc) {
    std::string_view selected_runtime = "-fobjc-runtime=gnustep-2.2";
    for (const auto &arg : forwarded)
      if (arg.starts_with("-fobjc-runtime=")) selected_runtime = arg;
    if (selected_runtime != "-fobjc-runtime=gnustep-2.2") {
      std::cerr << "Linux Objective-C 3 requires GNUstep 2.2; rebuild with the current SDK\n";
      return 2;
    }
  }
  if (objc) args.insert(args.end(), {"-fobjc-version=3", "-fobjc-runtime=gnustep-2.2",
                                    "-fblocks", "-fobjc-arc"});
  if (objcxx) args.insert(args.end(), {"-x", "objective-c++"});
  // Defaults precede user arguments so explicit ARC/MRC/runtime choices win.
  args.insert(args.end(), forwarded.begin(), forwarded.end());
  if (!query) {
    args.insert(args.end(), {"-I" + (root / "include").string(),
        "-L" + (root / "lib").string(), "-fuse-ld=lld"});
    bool link = !header;
    for (const auto &arg : forwarded)
      if (arg == "-c" || arg == "-S" || arg == "-E" || arg == "-fsyntax-only" ||
          arg == "-M" || arg == "-MM") link = false;
    if (link) args.insert(args.end(), {"-lobjc", "-Wl,-rpath," + (root / "lib").string(),
        // DWARF 5 address pools must distinguish discarded COMDAT helpers
        // from live code at address zero. In particular, separate Block
        // providers commonly emit identical copy/dispose helpers.
        "-Wl,-z,dead-reloc-in-nonalloc=.debug_addr=0xffffffffffffffff",
        "-Wl,-z,dead-reloc-in-nonalloc=.debug_info=0xffffffffffffffff"});
  }
  for (const auto &arg : forwarded) {
    if (arg == "--version")
      std::cout << "objc3c frontend " << CompiledFrontendSourceIdentity() << '\n' << std::flush;
    if (arg == "--help")
      std::cout
          << "objc3c: native Objective-C 3 compiler (ARC by default).\n"
             "Use -fno-objc-arc for manual ownership; --emit-interface "
             "for semantic modules.\n"
             "Use --project-ownership arc|mrc to emit an opposite-mode "
             "closed C ABI interface.\n"
             "Use --export-declaration name to select public module roots.\n"
             "Clang compilation, linking, diagnostics and dependency "
             "options follow.\n\n"
          << std::flush;
  }
  // A bootstrap or unrelated SDK's LD_LIBRARY_PATH must never replace this
  // installation's libraries. Do not change the parent shell environment.
  std::vector<std::string> env;
  for (char **entry = environ; *entry; ++entry)
    if (!std::string_view(*entry).starts_with("LD_LIBRARY_PATH=")) env.emplace_back(*entry);
  env.push_back("LD_LIBRARY_PATH=" + (root / "lib").string());
  std::vector<llvm::StringRef> refs(args.begin(), args.end()), env_refs(env.begin(), env.end());
  int result = llvm::sys::ExecuteAndWait(program, refs, env_refs, {}, 0, 0, &error);
  if (result < 0) { std::cerr << "cannot execute SDK Clang: " << error << '\n'; return 3; }
  return result;
}
