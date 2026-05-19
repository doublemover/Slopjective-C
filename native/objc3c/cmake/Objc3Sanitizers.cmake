option(OBJC3C_ENABLE_ASAN "Build objc3c native targets with AddressSanitizer when supported." OFF)

function(objc3c_apply_sanitizers target_name)
  if (OBJC3C_ENABLE_ASAN)
    target_compile_options(${target_name} PUBLIC -fsanitize=address)
    target_link_options(${target_name} PUBLIC -fsanitize=address)
  endif()
endfunction()
