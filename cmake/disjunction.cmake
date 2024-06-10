# check for exactly 1 option is set by logical or

function(disjunction error_msg)
	set(COUNTER 0)
	foreach (ITEM IN LISTS ${ARGN})
		if (${ITEM})
			math(EXPR COUNTER "${COUNTER} + 1")
		endif ()
	endforeach ()

	if (${COUNTER} GREATER 1 OR NOT ${COUNTER})
		message(FATAL_ERROR "${error_msg} ${ARGN} set to ${COUNTER} values")
	endif ()
endfunction()
