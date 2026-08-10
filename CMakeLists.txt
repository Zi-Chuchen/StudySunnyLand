cmake_minimum_required(VERSION 3.16)
project(SunnyLand LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(SOURCE_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(ASSETS_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/assets)

# =======================
# 依赖策略控制
# =======================
option(USE_LOCAL_DEPS "Use local external/ instead of FetchContent" ON)

# =======================
# 进度条函数（纯 ASCII）
# =======================
function(print_progress current total msg)
    math(EXPR percent "${current} * 100 / ${total}")
    math(EXPR filled  "${percent} * 20 / 100")
    math(EXPR empty   "20 - ${filled}")

    string(REPEAT "#" ${filled} bar)
    string(REPEAT "." ${empty} space)

    message(STATUS "[${current}/${total}] [${bar}${space}] ${percent}% ${msg}")
endfunction()

# =======================
# 依赖列表
# =======================
set(_DEPS
    SDL3
    SDL3_image
    SDL3_mixer
    SDL3_net
    SDL3_ttf
    glm
    json
    spdlog
)

list(LENGTH _DEPS _TOTAL_DEPS)

message(STATUS "+------------------------------------------------+")
message(STATUS "|     SunnyLand Dependency Check & Download     |")
message(STATUS "|     Total: ${_TOTAL_DEPS} libraries           |")
message(STATUS "+------------------------------------------------+")

# =======================
# FetchContent 基础设置
# =======================
include(FetchContent)

set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

# =======================
# 依赖版本
# =======================
set(SDL3_VERSION        "release-3.2.0")
set(SDL_IMAGE_VERSION   "release-3.2.0")
set(SDL_MIXER_VERSION   "release-3.2.0")
set(SDL_NET_VERSION     "release-3.2.0")
set(SDL_TTF_VERSION     "release-3.2.0")
set(GLM_VERSION         "1.1.0")
set(JSON_VERSION        "v3.11.3")
set(SPDLOG_VERSION      "v1.17.0")

# =======================
# 代理控制
# =======================
option(USE_GH_PROXY "Use gh-proxy.com for GitHub downloads" OFF)

if(USE_GH_PROXY)
    set(GH_URL "https://gh-proxy.com/https://github.com")
    message(STATUS "[INFO] GitHub proxy ENABLED")
else()
    set(GH_URL "https://github.com")
    message(STATUS "[INFO] GitHub proxy DISABLED, using direct download")
endif()

# =======================
# 依赖处理宏
# =======================
macro(add_dependency NAME PATH_IN_EXTERNAL GIT_REPO GIT_TAG)
    math(EXPR _IDX "${_IDX} + 1")

    set(_LOCAL_PATH "${EXTERNAL_DIR}/${PATH_IN_EXTERNAL}")
    set(_FETCH_VAR "FETCHCONTENT_SOURCE_DIR_${NAME}")

    if(USE_LOCAL_DEPS AND EXISTS "${_LOCAL_PATH}/CMakeLists.txt")
        print_progress(${_IDX} ${_TOTAL_DEPS} "${NAME}: using local")
        add_subdirectory("${_LOCAL_PATH}")
    elseif(DEFINED ${_FETCH_VAR})
        print_progress(${_IDX} ${_TOTAL_DEPS} "${NAME}: using FETCHCONTENT_SOURCE_DIR")
        add_subdirectory(${${_FETCH_VAR}})
    else()
        print_progress(${_IDX} ${_TOTAL_DEPS} "${NAME}: downloading...")

        FetchContent_Declare(
            ${NAME}
            GIT_REPOSITORY "${GH_URL}/${GIT_REPO}"
            GIT_TAG         ${GIT_TAG}
            GIT_PROGRESS    ON
            USES_TERMINAL_DOWNLOAD ON
        )
        FetchContent_MakeAvailable(${NAME})
    endif()
endmacro()

set(_IDX 0)

# =======================
# SDL3
# =======================
add_dependency(
    SDL3
    SDL
    libsdl-org/SDL
    ${SDL3_VERSION}
)

# =======================
# SDL_image
# =======================
set(SDLIMAGE_INSTALL OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_VENDORED OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_AVIF OFF)
set(SDLIMAGE_JXL OFF)
set(SDLIMAGE_WEBP OFF)
set(SDLIMAGE_TIF OFF)

add_dependency(
    SDL3_image
    SDL_image
    libsdl-org/SDL_image
    ${SDL_IMAGE_VERSION}
)

# =======================
# SDL_mixer
# =======================
set(SDLMIXER_INSTALL OFF CACHE BOOL "" FORCE)
set(SDLMIXER_VENDORED OFF CACHE BOOL "" FORCE)
set(SDLMIXER_OPUS OFF)
set(SDLMIXER_FLAC_LIBFLAC OFF)
set(SDLMIXER_MP3_MPG123 OFF)
set(SDLMIXER_MIDI_FLUIDSYNTH OFF)

add_dependency(
    SDL3_mixer
    SDL_mixer
    libsdl-org/SDL_mixer
    ${SDL_MIXER_VERSION}
)

# =======================
# SDL_net
# =======================
set(SDLNET_INSTALL OFF CACHE BOOL "" FORCE)

add_dependency(
    SDL3_net
    SDL_net
    libsdl-org/SDL_net
    ${SDL_NET_VERSION}
)

# =======================
# SDL_ttf
# =======================
set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)
set(SDLTTF_USE_SYSTEM_FREETYPE OFF CACHE BOOL "" FORCE)
set(SDLTTF_INSTALL OFF CACHE BOOL "" FORCE)
set(SDLTTF_HARFBUZZ OFF)
set(SDLTTF_PLUTOSVG OFF)

add_dependency(
    SDL3_ttf
    SDL_ttf
    libsdl-org/SDL_ttf
    ${SDL_TTF_VERSION}
)

# =======================
# glm
# =======================
add_dependency(
    glm
    glm
    g-truc/glm
    ${GLM_VERSION}
)

# =======================
# nlohmann::json
# =======================
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(JSON_Install OFF CACHE BOOL "" FORCE)

add_dependency(
    json
    json
    nlohmann/json
    ${JSON_VERSION}
)

# =======================
# spdlog
# =======================
set(SPDLOG_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SPDLOG_INSTALL OFF CACHE BOOL "" FORCE)
set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "" FORCE)

add_dependency(
    spdlog
    spdlog
    gabime/spdlog
    ${SPDLOG_VERSION}
)

# =======================
# 主程序
# =======================
file(GLOB_RECURSE SOURCES "${SOURCE_DIR}/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_mixer::SDL3_mixer
        SDL3_net::SDL3_net
        SDL3_ttf::SDL3_ttf
        glm::glm
        nlohmann_json::nlohmann_json
        spdlog::spdlog
)

# =======================
# DLL 复制（MinGW）
# =======================
if(WIN32 AND MINGW)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3_image::SDL3_image>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3_mixer::SDL3_mixer>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3_net::SDL3_net>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3_ttf::SDL3_ttf>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "[SunnyLand] Copying DLLs to output directory"
    )
endif()

# =======================
# Assets 复制（稳定版）
# =======================
if(EXISTS "${ASSETS_DIR}")
    set(ASSETS_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${ASSETS_OUTPUT_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ASSETS_DIR}"
            "${ASSETS_OUTPUT_DIR}"
        COMMENT "[SunnyLand] Copying assets to output directory"
    )

    message(STATUS "[OK] Assets copy enabled")
    message(STATUS "     Source : ${ASSETS_DIR}")
    message(STATUS "     Target : ${ASSETS_OUTPUT_DIR}")
endif()