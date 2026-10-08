option(OBJC3C_ENABLE_ASAN "Build objc3c native targets with AddressSanitizer when supported." OFF)
option(OBJC3C_ENABLE_UBSAN "Build objc3c native targets with UndefinedBehaviorSanitizer when supported." OFF)
set(OBJC3C_SANITIZER_VARIANT "release" CACHE STRING "Explicit objc3c native runtime sanitizer variant: release, address, or undefined.")
set_property(CACHE OBJC3C_SANITIZER_VARIANT PROPERTY STRINGS release address undefined)

set(OBJC3C_SANITIZER_VARIANT_ALLOWED_VALUES release address undefined)
if (NOT "${OBJC3C_SANITIZER_VARIANT}" IN_LIST OBJC3C_SANITIZER_VARIANT_ALLOWED_VALUES)
  message(FATAL_ERROR "OBJC3C_SANITIZER_VARIANT must be one of: release, address, undefined")
endif()
if (OBJC3C_SANITIZER_VARIANT STREQUAL "address")
  if (OBJC3C_ENABLE_UBSAN)
    message(FATAL_ERROR "OBJC3C_SANITIZER_VARIANT=address cannot be combined with OBJC3C_ENABLE_UBSAN")
  endif()
  set(OBJC3C_ENABLE_ASAN ON CACHE BOOL "Build objc3c native targets with AddressSanitizer when supported." FORCE)
elseif (OBJC3C_SANITIZER_VARIANT STREQUAL "undefined")
  if (OBJC3C_ENABLE_ASAN)
    message(FATAL_ERROR "OBJC3C_SANITIZER_VARIANT=undefined cannot be combined with OBJC3C_ENABLE_ASAN")
  endif()
  set(OBJC3C_ENABLE_UBSAN ON CACHE BOOL "Build objc3c native targets with UndefinedBehaviorSanitizer when supported." FORCE)
elseif (OBJC3C_ENABLE_ASAN OR OBJC3C_ENABLE_UBSAN)
  message(FATAL_ERROR "OBJC3C_ENABLE_ASAN/OBJC3C_ENABLE_UBSAN require OBJC3C_SANITIZER_VARIANT=address or undefined")
endif()

function(objc3c_apply_sanitizers target_name)
  set(objc3c_enabled_sanitizers "")
  if (OBJC3C_ENABLE_ASAN)
    list(APPEND objc3c_enabled_sanitizers address)
  endif()
  if (OBJC3C_ENABLE_UBSAN)
    list(APPEND objc3c_enabled_sanitizers undefined)
  endif()

  if (objc3c_enabled_sanitizers)
    list(JOIN objc3c_enabled_sanitizers "," objc3c_sanitizer_flags)
    target_compile_options(${target_name} PUBLIC
      -fsanitize=${objc3c_sanitizer_flags}
      -fno-omit-frame-pointer
    )
    target_link_options(${target_name} PUBLIC
      -fsanitize=${objc3c_sanitizer_flags}
    )
    if (OBJC3C_ENABLE_UBSAN)
      target_compile_options(${target_name} PUBLIC
        -fsanitize-trap=undefined
      )
      target_link_options(${target_name} PUBLIC
        -fsanitize-trap=undefined
      )
    endif()
    if (WIN32 AND OBJC3C_ENABLE_UBSAN)
      target_link_libraries(${target_name} PUBLIC
        DbgHelp
      )
    endif()
  endif()
endfunction()
