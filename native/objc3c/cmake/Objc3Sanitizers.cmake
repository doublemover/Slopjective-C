option(OBJC3C_ENABLE_ASAN "Build objc3c native targets with AddressSanitizer when supported." OFF)
option(OBJC3C_ENABLE_UBSAN "Build objc3c native targets with UndefinedBehaviorSanitizer when supported." OFF)

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
  endif()
endfunction()
