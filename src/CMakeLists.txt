cmake_minimum_required(VERSION 3.0)
project(Wow64Hook)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /MP -DUNICODE -D_UNICODE -D_WIN32_WINNT=_WIN32_WINNT_WIN7 /wd4273 /wd4005")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT") 
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3 /MP -DUNICODE -D_UNICODE -D_WIN32_WINNT=_WIN32_WINNT_WIN7")

set(ASMJIT_STATIC TRUE)
add_subdirectory(3rd-party/asmjit)
set_target_properties(asmjit PROPERTIES FOLDER 3rd-party/asmjit)
set_target_properties(asmjit PROPERTIES COMPILE_FLAGS "/wd4267")
target_compile_definitions(asmjit PUBLIC ASMJIT_ARCH_X64=1)

add_subdirectory(3rd-party/diana-dasm)
set_target_properties(diana PROPERTIES FOLDER 3rd-party/diana-dasm)

add_library(
	wow64ext
	3rd-party/rewolf-wow64ext/src/CMemPtr.h
	3rd-party/rewolf-wow64ext/src/internal.h
	3rd-party/rewolf-wow64ext/src/wow64ext.h
	3rd-party/rewolf-wow64ext/src/wow64ext.cpp
)
set_target_properties(wow64ext PROPERTIES FOLDER 3rd-party/rewolf-wow64ext)
target_compile_definitions(wow64ext PUBLIC WOW64EXT_EXPORTS=dllexport)

include_directories(.)
include_directories(3rd-party/diana-dasm/src/diana_core/diana_core)
include_directories(3rd-party/diana-dasm/src/diana_core/diana_core/diana_patchers)
include_directories(3rd-party/rewolf-wow64ext/src)
include_directories(3rd-party/asmjit/src/asmjit)

add_executable(
    hooker
	hooker/StdAfx.h
	hooker/StdAfx.cpp
	hooker/MemoryStream.h
	hooker/MemoryStream.cpp
	hooker/MemoryProvider.h
	hooker/MemoryProvider.cpp
	hooker/IMemoryReaderWriter.h
	hooker/Wow64MemoryReaderWriter.h
	hooker/Wow64MemoryReaderWriter.cpp
	hooker/DianaHook.h
	hooker/DianaHook.cpp
	hooker/Hooker.h
	hooker/Hooker.cpp
	hooker/HookGenerator.h
	hooker/HookGenerator.cpp
	hooker/Main.cpp
)
set_target_properties(hooker PROPERTIES COMPILE_FLAGS "/YuStdAfx.h")
set_source_files_properties(hooker/StdAfx.cpp PROPERTIES COMPILE_FLAGS "/YcStdAfx.h")
target_link_libraries(hooker wow64ext diana asmjit)

install(
    TARGETS hooker wow64ext diana
    DESTINATION "${CMAKE_SOURCE_DIR}/../bin"
)
