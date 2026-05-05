function(objc3c_enforce_source_size source_root max_lines)
  if (NOT EXISTS "${source_root}")
    message(FATAL_ERROR "objc3c source-size gate root does not exist: ${source_root}")
  endif()

  file(GLOB_RECURSE objc3c_size_gate_sources CONFIGURE_DEPENDS
    "${source_root}/*.c"
    "${source_root}/*.cc"
    "${source_root}/*.cpp"
    "${source_root}/*.h"
    "${source_root}/*.hpp"
  )

  set(oversized_sources "")
  foreach(source_path IN LISTS objc3c_size_gate_sources)
    file(STRINGS "${source_path}" source_lines)
    list(LENGTH source_lines line_count)
    if (line_count GREATER max_lines)
      file(RELATIVE_PATH relative_source "${CMAKE_CURRENT_SOURCE_DIR}" "${source_path}")
      list(APPEND oversized_sources "${relative_source}: ${line_count} lines")
    endif()
  endforeach()

  if (oversized_sources)
    list(JOIN oversized_sources "\n  " oversized_message)
    message(FATAL_ERROR
      "objc3c source-size gate failed; native source files must be <= ${max_lines} lines:\n  ${oversized_message}"
    )
  endif()
endfunction()

