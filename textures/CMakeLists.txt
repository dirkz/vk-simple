set(TEXTURES texture.jpg viking_room.png)

foreach(TEXTURE ${TEXTURES})
	configure_file(${TEXTURE} ${TEXTURE} COPYONLY)
endforeach()
