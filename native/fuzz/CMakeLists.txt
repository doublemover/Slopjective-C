cmake_minimum_required(VERSION 4.4.5)
project(objc3_codec_fuzz LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(OBJC3_FUZZ_FOUNDATION "" CACHE PATH "Matched ASan/UBSan foundation")
set(OBJC3_FUZZ_GENERATED_HEADERS "" CACHE PATH "Admitted native frontend generated headers")
set(OBJC3_FUZZ_FRONTEND_DEFINITIONS "" CACHE STRING "Exact compile-session identity definitions")
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux" OR NOT OBJC3_FUZZ_FOUNDATION OR NOT OBJC3_FUZZ_GENERATED_HEADERS)
  message(FATAL_ERROR "Codec fuzzing requires native Linux and matching foundation/generated headers")
endif()
find_package(LLVM 23.1.5 EXACT REQUIRED CONFIG PATHS "${OBJC3_FUZZ_FOUNDATION}/lib/cmake/llvm" NO_DEFAULT_PATH)
find_package(Clang REQUIRED CONFIG PATHS "${OBJC3_FUZZ_FOUNDATION}/lib/cmake/clang" NO_DEFAULT_PATH)
find_package(OpenSSL 3.5 REQUIRED COMPONENTS Crypto)
set(_native_archive "${OBJC3_FUZZ_GENERATED_HEADERS}/libobjc3c_clang_foundation.a")
if(NOT EXISTS "${_native_archive}")
  message(FATAL_ERROR "Codec naming dependencies require the matching admitted native archive")
endif()
set(_native "${CMAKE_CURRENT_LIST_DIR}/../objc3c/src")
add_library(objc3_codec_fuzz_objects OBJECT
  "${_native}/clang_foundation/interface_codec.cpp"
  "${_native}/clang_foundation/interface_contract.cpp"
  "${_native}/clang_foundation/module_names.cpp"
  "${_native}/clang_foundation/runtime_interface_types.cpp")
add_library(objc3_codec_fuzz_support OBJECT CodecTarget.cpp)
add_library(objc3_codec_property_fault OBJECT CodecTarget.cpp)
target_compile_definitions(objc3_codec_property_fault PRIVATE OBJC3_FUZZ_PROPERTY_FAULT=1)
add_library(objc3_codec_boundary_fault OBJECT CodecTarget.cpp)
target_compile_definitions(objc3_codec_boundary_fault PRIVATE OBJC3_FUZZ_PROPERTY_FAULT=2)
foreach(_target objc3_codec_fuzz_objects objc3_codec_fuzz_support objc3_codec_property_fault objc3_codec_boundary_fault)
  target_compile_features(${_target} PRIVATE cxx_std_20)
  target_include_directories(${_target} PRIVATE "${_native}" "${OBJC3_FUZZ_GENERATED_HEADERS}" ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_compile_options(${_target} PRIVATE -O1 -gline-tables-only -UNDEBUG -fno-omit-frame-pointer
    -fsanitize=fuzzer-no-link,address,undefined -fno-sanitize=function,vptr -fno-sanitize-recover=undefined
    -fprofile-instr-generate -fcoverage-mapping -fprofile-update=atomic)
endforeach()

add_library(objc3_frontend_fuzz_objects OBJECT
  "${_native}/clang_foundation/compile_session.cpp"
  "${_native}/clang_foundation/frontend_api.cpp")
add_library(objc3_frontend_fuzz_support OBJECT FrontendTarget.cpp)
add_library(objc3_frontend_cleanup_fault OBJECT FrontendTarget.cpp)
target_compile_definitions(objc3_frontend_cleanup_fault PRIVATE OBJC3_FUZZ_FRONTEND_CLEANUP_FAULT=1)
foreach(_target objc3_frontend_fuzz_objects objc3_frontend_fuzz_support objc3_frontend_cleanup_fault)
  target_compile_features(${_target} PRIVATE cxx_std_20)
  target_include_directories(${_target} PRIVATE "${_native}" "${OBJC3_FUZZ_GENERATED_HEADERS}" ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_compile_options(${_target} PRIVATE -O1 -gline-tables-only -UNDEBUG -fno-omit-frame-pointer
    -fsanitize=fuzzer-no-link,address,undefined -fno-sanitize=function,vptr -fno-sanitize-recover=undefined
    -fprofile-instr-generate -fcoverage-mapping -fprofile-update=atomic)
endforeach()
target_compile_definitions(objc3_frontend_fuzz_objects PRIVATE ${OBJC3_FUZZ_FRONTEND_DEFINITIONS})

add_executable(objc3-frontend-fuzzer FrontendFuzzerEntry.cpp
  $<TARGET_OBJECTS:objc3_frontend_fuzz_support> $<TARGET_OBJECTS:objc3_frontend_fuzz_objects>)
add_executable(objc3-frontend-replay FrontendReplay.cpp
  $<TARGET_OBJECTS:objc3_frontend_fuzz_support> $<TARGET_OBJECTS:objc3_frontend_fuzz_objects>)
add_executable(objc3-frontend-cleanup-fault FrontendFuzzerEntry.cpp
  $<TARGET_OBJECTS:objc3_frontend_cleanup_fault> $<TARGET_OBJECTS:objc3_frontend_fuzz_objects>)
foreach(_target objc3-frontend-fuzzer objc3-frontend-replay objc3-frontend-cleanup-fault)
  target_compile_features(${_target} PRIVATE cxx_std_20)
  target_include_directories(${_target} PRIVATE "${_native}" ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_compile_options(${_target} PRIVATE -O1 -gline-tables-only -UNDEBUG -fno-omit-frame-pointer
    -fsanitize=address,undefined -fno-sanitize=function,vptr -fno-sanitize-recover=undefined)
  target_link_options(${_target} PRIVATE -fuse-ld=lld -fsanitize=address,undefined
    -fno-sanitize-recover=undefined -fprofile-instr-generate "LINKER:--build-id=sha1")
  target_link_libraries(${_target} PRIVATE "${_native_archive}" clang-cpp LLVM OpenSSL::Crypto)
  set_target_properties(${_target} PROPERTIES BUILD_RPATH "${OBJC3_FUZZ_FOUNDATION}/lib")
endforeach()
foreach(_target objc3-frontend-fuzzer objc3-frontend-cleanup-fault)
  target_compile_options(${_target} PRIVATE -fsanitize=fuzzer-no-link)
  target_link_options(${_target} PRIVATE -fsanitize=fuzzer)
endforeach()
add_executable(objc3-codec-fuzzer FuzzerEntry.cpp StructuredMutator.cpp
  $<TARGET_OBJECTS:objc3_codec_fuzz_support> $<TARGET_OBJECTS:objc3_codec_fuzz_objects>)
add_executable(objc3-codec-raw-fuzzer FuzzerEntry.cpp
  $<TARGET_OBJECTS:objc3_codec_fuzz_support> $<TARGET_OBJECTS:objc3_codec_fuzz_objects>)
add_executable(objc3-codec-replay Replay.cpp
  $<TARGET_OBJECTS:objc3_codec_fuzz_support> $<TARGET_OBJECTS:objc3_codec_fuzz_objects>)
add_executable(objc3-codec-property-fault FuzzerEntry.cpp
  $<TARGET_OBJECTS:objc3_codec_property_fault> $<TARGET_OBJECTS:objc3_codec_fuzz_objects>)
add_executable(objc3-codec-boundary-fault FuzzerEntry.cpp
  $<TARGET_OBJECTS:objc3_codec_boundary_fault> $<TARGET_OBJECTS:objc3_codec_fuzz_objects>)
foreach(_target objc3-codec-fuzzer objc3-codec-raw-fuzzer objc3-codec-replay objc3-codec-property-fault objc3-codec-boundary-fault)
  target_compile_features(${_target} PRIVATE cxx_std_20)
  target_include_directories(${_target} PRIVATE "${_native}" ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_compile_options(${_target} PRIVATE -O1 -gline-tables-only -UNDEBUG -fno-omit-frame-pointer
    -fsanitize=address,undefined -fno-sanitize=function,vptr -fno-sanitize-recover=undefined)
  target_link_options(${_target} PRIVATE -fuse-ld=lld -fsanitize=address,undefined
    -fno-sanitize-recover=undefined -fprofile-instr-generate "LINKER:--build-id=sha1")
  # The instrumented codec/contract/naming/runtime TUs above own the target.
  # Uncalled AST-naming helpers still reference native emitter symbols. Resolve
  # them from the matched ASan/UBSan archive; do not substitute test stubs.
  target_link_libraries(${_target} PRIVATE "${_native_archive}" clang-cpp LLVM OpenSSL::Crypto)
  set_target_properties(${_target} PROPERTIES BUILD_RPATH "${OBJC3_FUZZ_FOUNDATION}/lib")
endforeach()
foreach(_target objc3-codec-fuzzer objc3-codec-raw-fuzzer objc3-codec-property-fault objc3-codec-boundary-fault)
  target_compile_options(${_target} PRIVATE -fsanitize=fuzzer-no-link)
  target_link_options(${_target} PRIVATE -fsanitize=fuzzer)
endforeach()

# A real omitted-feedback control compiles the production decoder with the
# same ASan/UBSan route but no engine counters. It is never linked into targets.
add_library(objc3_codec_no_feedback OBJECT EXCLUDE_FROM_ALL "${_native}/clang_foundation/interface_codec.cpp")
target_compile_features(objc3_codec_no_feedback PRIVATE cxx_std_20)
target_include_directories(objc3_codec_no_feedback PRIVATE "${_native}" ${LLVM_INCLUDE_DIRS})
target_compile_options(objc3_codec_no_feedback PRIVATE -O1 -gline-tables-only -UNDEBUG
  -fsanitize=address,undefined -fno-sanitize=function,vptr -fno-sanitize-recover=undefined)
