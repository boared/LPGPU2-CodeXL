
add_library(Qt5::GLTFParser MODULE IMPORTED)

_populate_3DRenderer_plugin_properties(GLTFParser RELEASE "sceneparsers/libgltfsceneparser.so")

list(APPEND Qt53DRenderer_PLUGINS Qt5::GLTFParser)
