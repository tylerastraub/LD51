cmake_minimum_required(VERSION 3.17.0)
project(LD51 VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})

set(LD51_VERSION "1.0.0")

if(WIN32)
    set(SDL2_INCLUDE_DIR "C:/Program Files/mingw64/include/SDL2")
    set(SDL2_LIBRARY_DIR "C:/Program Files/mingw64/lib")
    set(SDL2_IMAGE_INCLUDE_DIR "C:/SDL_image/include/SDL2")
    set(SDL2_IMAGE_LIBRARY_DIR "C:/SDL_image/lib")
    set(SDL2_TTF_INCLUDE_DIR "C:/SDL_ttf/include/SDL2")
    set(SDL2_TTF_LIBRARY_DIR "C:/SDL_ttf/lib")
elseif(APPLE)
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2TTF REQUIRED)
endif()

set(SOURCE_INCLUDES
    ${PROJECT_SOURCE_DIR}/lib
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/Engine
    ${PROJECT_SOURCE_DIR}/src/Engine/Audio
    ${PROJECT_SOURCE_DIR}/src/Engine/RandomUtilities
    ${PROJECT_SOURCE_DIR}/src/Engine/StandardTypes
    ${PROJECT_SOURCE_DIR}/src/Entity
    ${PROJECT_SOURCE_DIR}/src/Entity/Components
    ${PROJECT_SOURCE_DIR}/src/Entity/Components/Generic
    ${PROJECT_SOURCE_DIR}/src/Entity/Components/Input
    ${PROJECT_SOURCE_DIR}/src/Entity/Components/Physics
    ${PROJECT_SOURCE_DIR}/src/Entity/Components/Render
    ${PROJECT_SOURCE_DIR}/src/Entity/Components/Specialized
    ${PROJECT_SOURCE_DIR}/src/Entity/Core
    ${PROJECT_SOURCE_DIR}/src/Entity/Systems
    ${PROJECT_SOURCE_DIR}/src/Entity/Prefabs
    ${PROJECT_SOURCE_DIR}/src/Input
    ${PROJECT_SOURCE_DIR}/src/Level
    ${PROJECT_SOURCE_DIR}/src/Localization
    ${PROJECT_SOURCE_DIR}/src/Render
    ${PROJECT_SOURCE_DIR}/src/Render/Effects
    ${PROJECT_SOURCE_DIR}/src/States
    )
set(SOURCES
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/Engine/FileIO.cpp
    ${PROJECT_SOURCE_DIR}/src/Engine/Game.cpp
    ${PROJECT_SOURCE_DIR}/src/Engine/Settings.cpp
    ${PROJECT_SOURCE_DIR}/src/Engine/Timer.cpp
    ${PROJECT_SOURCE_DIR}/src/Engine/Audio/Audio.cpp
    ${PROJECT_SOURCE_DIR}/src/Engine/RandomUtilities/RandomGen.cpp
    ${PROJECT_SOURCE_DIR}/src/Entity/Components/Render/SpritesheetPropertiesComponent.cpp
    ${PROJECT_SOURCE_DIR}/src/Entity/Core/EntityManager.cpp
    ${PROJECT_SOURCE_DIR}/src/Entity/Systems/CameraSystem.cpp
    ${PROJECT_SOURCE_DIR}/src/Entity/Systems/CollisionSystem.cpp
    ${PROJECT_SOURCE_DIR}/src/Entity/Systems/DeathSystem.cpp
    ${PROJECT_SOURCE_DIR}/src/Entity/Systems/InputSystem.cpp
    ${PROJECT_SOURCE_DIR}/src/Entity/Systems/PhysicsSystem.cpp
    ${PROJECT_SOURCE_DIR}/src/Entity/Systems/RenderSystem.cpp
    ${PROJECT_SOURCE_DIR}/src/Entity/Systems/RespawnSystem.cpp
    ${PROJECT_SOURCE_DIR}/src/Entity/Systems/ScriptSystem.cpp
    ${PROJECT_SOURCE_DIR}/src/Entity/Prefabs/Checkpoint.cpp
    ${PROJECT_SOURCE_DIR}/src/Entity/Prefabs/Goal.cpp
    ${PROJECT_SOURCE_DIR}/src/Entity/Prefabs/Engine.cpp
    ${PROJECT_SOURCE_DIR}/src/Entity/Prefabs/Player.cpp
    ${PROJECT_SOURCE_DIR}/src/Entity/Prefabs/Pickup.cpp
    ${PROJECT_SOURCE_DIR}/src/Entity/Prefabs/Projectile.cpp
    ${PROJECT_SOURCE_DIR}/src/States/GameState.cpp
    ${PROJECT_SOURCE_DIR}/src/States/State.cpp
    ${PROJECT_SOURCE_DIR}/src/States/MainMenuState.cpp
    ${PROJECT_SOURCE_DIR}/src/Render/DialogueBox.cpp
    ${PROJECT_SOURCE_DIR}/src/Render/Spritesheet.cpp
    ${PROJECT_SOURCE_DIR}/src/Render/Text.cpp
    ${PROJECT_SOURCE_DIR}/src/Render/Effects/ScreenShake.cpp
    ${PROJECT_SOURCE_DIR}/src/Input/Controller.cpp
    ${PROJECT_SOURCE_DIR}/src/Input/Keyboard.cpp
    ${PROJECT_SOURCE_DIR}/src/Input/Mouse.cpp
    ${PROJECT_SOURCE_DIR}/src/Level/Level.cpp
    ${PROJECT_SOURCE_DIR}/src/Level/LevelParser.cpp
    )

if(WIN32)
    include_directories(${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR} ${SOURCE_INCLUDES})
    add_executable(LD51 ${SOURCES})
    # remove -mconsole for release builds
    target_link_libraries(LD51 -lmingw32 ${SDL2_LIBRARY_DIR}/libSDL2main.a ${SDL2_LIBRARY_DIR}/libSDL2.dll.a ${SDL2_IMAGE_LIBRARY_DIR}/libSDL2_image.dll.a ${SDL2_TTF_LIBRARY_DIR}/libSDL2_ttf.dll.a -mwindows)
    add_custom_command(TARGET LD51 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/res/ $<TARGET_FILE_DIR:LD51>/res/)
    add_custom_command(TARGET LD51 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${PROJECT_SOURCE_DIR}/settings.cfg $<TARGET_FILE_DIR:LD51>/settings.cfg)
elseif(APPLE)
    include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIR} ${SOURCE_INCLUDES})
    add_executable(LD51 MACOSX_BUNDLE ${SOURCES})
    target_link_libraries(LD51 ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2TTF_LIBRARY})
    set_target_properties(LD51 PROPERTIES
        BUNDLE True
        MACOSX_BUNDLE_EXECUTABLE_NAME LD51
        MACOSX_BUNDLE_ICON_FILE icon.png
        MACOSX_BUNDLE_GUI_IDENTIFIER com.straub.LD51
        MACOSX_BUNDLE_LONG_VERSION_STRING ${LD51_VERSION}
        MACOSX_BUNDLE_BUNDLE_NAME LD51
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${LD51_VERSION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${LD51_VERSION}
        MACOSX_BUNDLE_COPYRIGHT "© Tyler Straub 2022"
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/cmake/Info.plist.in)
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(\"${CMAKE_BINARY_DIR}/LD51.app\"   \"\"   \"${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2TTF_LIBRARY}\")
        " COMPONENT Runtime)
    add_custom_command(TARGET LD51 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/res/ $<TARGET_FILE_DIR:LD51>/../Resources/res/)
    add_custom_command(TARGET LD51 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${PROJECT_SOURCE_DIR}/settings.cfg $<TARGET_FILE_DIR:LD51>/../Resources/settings.cfg)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
