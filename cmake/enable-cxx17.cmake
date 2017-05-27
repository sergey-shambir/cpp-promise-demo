
function(custom_enable_cxx17 TARGET)
	if (MSVC)
		set_target_properties(${TARGET} PROPERTIES COMPILE_FLAGS "/std:c++latest")
	endif()
	target_compile_features(${TARGET} PUBLIC cxx_std_17)
endfunction(custom_enable_cxx17)
