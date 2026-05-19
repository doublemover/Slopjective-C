function(objc3c_module_name output_var domain leaf)
  set(${output_var} "objc3c_${domain}_${leaf}" PARENT_SCOPE)
endfunction()
