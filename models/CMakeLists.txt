set(MODELS viking_room.obj)

foreach(MODEL ${MODELS})
	configure_file(${MODEL} ${MODEL} COPYONLY)
endforeach()
