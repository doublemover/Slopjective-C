#include "clang_foundation/driver_native.h"
#include "clang_foundation/sdk_identity.h"
#include "clang_foundation/source_identity.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Types.h"
#include "clang/Options/Options.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Program.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

extern char **environ;

bool UsesInterfaceCompiler(int argc, char **argv) {
  // These operations use the same in-process Clang frontend, with semantic
  // publication and transactional outputs rather than a linker invocation.
  for (int i = 1; i < argc; ++i) {
    std::string_view arg(argv[i]);
    if (arg == "--") break;
    if (arg == "--diagnostics-json" || arg == "--emit-object" || arg == "--emit-llvm" ||
        arg == "--emit-interface" || arg == "--project-ownership" ||
        arg == "--export-declaration" || arg == "--module-source-root" ||
        arg == "--export-c-async" || arg == "--emit-c-async-header" ||
        arg == "--export-c-throws" || arg == "--emit-c-header" ||
        arg == "--emit-swift-overlay" || arg == "--swift-c-module" ||
        arg == "--module-owned-names" || arg == "--import-spi" ||
        arg == "--import-module" || arg == "--emit-dependency-record" ||
        arg == "--source-imports" || arg == "--meta-package" || arg == "--meta-lock")
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
  using namespace clang::options;
  namespace types = clang::driver::types;
  const auto &option_table = clang::getDriverOptTable();
  const llvm::opt::Visibility visibility(ClangOption);
  const llvm::opt::InputArgList original(argv + 1, argv + argc);
  const unsigned original_count = argc - 1;
  std::vector<std::string> forwarded;
  bool objcxx = false;
  for (unsigned i = 0; i < original_count;) {
    const std::string_view arg(original.getArgString(i));
    if (arg == "--sdk-root") {
      if (++i == original_count) { std::cerr << "--sdk-root requires a path\n"; return 2; }
      root = original.getArgString(i++);
    } else if (arg == "--objc3") {
      // Objective-C inputs already select the SDK's language version.
      ++i;
    } else if (arg == "--objcxx") {
      objcxx = true;
      ++i;
    } else if (arg == "--arc") {
      forwarded.push_back("-fobjc-arc");
      ++i;
    } else if (arg == "--runtime" || arg == "--profile" || arg == "--target") {
      if (++i == original_count) { std::cerr << arg << " requires a value\n"; return 2; }
      forwarded.push_back((arg == "--runtime" ? "-fobjc-runtime=" :
                           arg == "--profile" ? "-fobjc3-profile=" : "--target=") +
                          std::string(original.getArgString(i++)));
    } else if (arg == "--strict-concurrency") {
      forwarded.push_back("-fobjc3-strict-concurrency");
      ++i;
    } else if (arg == "--") {
      // Clang's own options remain options after the interface CLI delimiter.
      ++i;
    } else {
      // Advance over Clang operands before looking for another SDK option.
      unsigned start = i;
      option_table.ParseOneArg(original, i, visibility);
      i = std::min(i, original_count);
      for (; start < i; ++start)
        forwarded.emplace_back(original.getArgString(start));
    }
  }
  llvm::SmallVector<const char *, 32> clang_arguments;
  for (const auto &arg : forwarded)
    clang_arguments.push_back(arg.c_str());
  unsigned missing_index = 0, missing_count = 0;
  const auto parsed = option_table.ParseArgs(clang_arguments, missing_index, missing_count,
                                             visibility);
  const bool help = parsed.hasArg(OPT_help);
  const bool version = parsed.hasArg(OPT__version);
  const bool query = help || version || parsed.hasArg(OPT_print_resource_dir);
  const auto *last_input = parsed.getLastArg(OPT_INPUT);
  const bool force_objc = parsed.hasArg(OPT_ObjC);
  const bool force_objcxx = parsed.hasArg(OPT_ObjCXX);
  const auto default_objc = !force_objc && (objcxx || force_objcxx)
                                ? types::TY_ObjCXX : types::TY_ObjC;
  bool cxx = false, objc = false, source = false, header = false;
  llvm::SmallVector<std::pair<unsigned, types::ID>, 4> default_languages;
  auto language = types::TY_Nothing;
  for (const auto *input : parsed.filtered(OPT_x, OPT_INPUT)) {
    if (input->getOption().matches(OPT_x)) {
      language = types::lookupTypeForTypeSpecifier(input->getValue());
      continue;
    }
    auto type = language;
    if (language == types::TY_Nothing) {
      const auto extension = fs::path(input->getValue()).extension().string();
      llvm::StringRef suffix(extension);
      suffix.consume_front(".");
      type = types::lookupTypeForExtension(suffix);
      if (extension == ".objc3" ||
          (objcxx && (types::isDerivedFromC(type) || extension.empty()))) {
        type = default_objc;
        default_languages.emplace_back(input->getIndex(), type);
      } else if ((force_objc || force_objcxx) &&
                 type != types::TY_INVALID && type != types::TY_Object) {
        type = default_objc;
      }
    }
    if (type == types::TY_INVALID)
      continue;
    objc |= types::isObjC(type);
    cxx |= types::isCXX(type);
    source |= types::isAcceptedByClang(type);
    header |= types::onlyPrecompileType(type);
  }
  const bool link = !header &&
      !parsed.hasArg(OPT_c, OPT_S, OPT_E, OPT_fsyntax_only, OPT_M, OPT_MM,
                     OPT__precompile, OPT_emit_ast, OPT__analyze);
  if (objc) {
    const auto *runtime = parsed.getLastArg(OPT_fobjc_runtime_EQ);
    if (runtime && std::string_view(runtime->getValue()) != "gnustep-2.2") {
      std::cerr << "Linux Objective-C 3 requires GNUstep 2.2; rebuild with the current SDK\n";
      return 2;
    }
  }
  // Apply SDK extension and language aliases only to their inferred inputs.
  const unsigned last_input_index = last_input ? last_input->getIndex() : 0;
  for (auto input = default_languages.rbegin(); input != default_languages.rend(); ++input) {
    const auto [index, type] = *input;
    if (index < last_input_index)
      forwarded.insert(forwarded.begin() + index + 1, {"-x", "none"});
    forwarded.insert(forwarded.begin() + index, {"-x", types::getTypeName(type)});
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
  if (objc) args.insert(args.end(), {"-fobjc-version=3", "-fobjc-runtime=gnustep-2.2",
                                    "-fblocks", "-fobjc-arc"});
  // Defaults precede user arguments so explicit ARC/MRC/runtime choices win.
  args.insert(args.end(), forwarded.begin(), forwarded.end());
  // Leave a missing compiler operand at the end for Clang to diagnose.
  if (!query && !missing_count) {
    args.push_back("-I" + (root / "include").string());
    if (link) args.insert(args.end(), {"-L" + (root / "lib").string(),
        "-fuse-ld=lld", "-lobjc", "-Wl,-rpath," + (root / "lib").string(),
        // DWARF 5 address pools must distinguish discarded COMDAT helpers
        // from live code at address zero. In particular, separate Block
        // providers commonly emit identical copy/dispose helpers.
        "-Wl,-z,dead-reloc-in-nonalloc=.debug_addr=0xffffffffffffffff",
        "-Wl,-z,dead-reloc-in-nonalloc=.debug_info=0xffffffffffffffff"});
  }
  if (version)
    std::cout << "objc3c frontend " << CompiledFrontendSourceIdentity() << '\n' << std::flush;
  if (help)
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
