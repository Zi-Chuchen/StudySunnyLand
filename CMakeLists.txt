cmake_minimum_required(VERSION 3.16)
project(SunnyLand LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)

# ==================== 代理设置 ====================
# 询问用户是否使用 GitHub 代理加速下载
if(NOT DEFINED USE_GH_PROXY)
    # 在非脚本/CI 环境下，给出交互提示
    if(CMAKE_INTERACTIVE)
        message(STATUS "==========================================")
        message(STATUS "  是否使用 GitHub 代理加速下载依赖库？")
        message(STATUS "  代理地址: https://gh-proxy.com/")
        message(STATUS "  设置 USE_GH_PROXY=ON  启用代理")
        message(STATUS "  设置 USE_GH_PROXY=OFF 直连 GitHub")
        message(STATUS "==========================================")
        set(USE_GH_PROXY OFF CACHE BOOL "Use gh-proxy.com to accelerate GitHub downloads")
    else()
        # 非交互环境（CI/脚本）默认关闭代理
        set(USE_GH_PROXY OFF CACHE BOOL "Use gh-proxy.com to accelerate GitHub downloads")
    endif()
endif()

if(USE_GH_PROXY)
    set(GH_PROXY_URL "https://gh-proxy.com/")
    message(STATUS "✅ GitHub 代理已启用: ${GH_PROXY_URL}")
else()
    set(GH_PROXY_URL "")
    message(STATUS "ℹ️  GitHub 代理未启用，直连下载")
endif()

# ==================== 辅助函数 ====================
# 下载并添加依赖库：优先使用本地 external/，缺失时自动 FetchContent 下载
# 参数:
#   LIB_NAME     - 库名称（对应 external/ 下的子目录名）
#   REPO_URL     - GitHub 仓库 URL（不含 .git 后缀）
#   GIT_TAG      - Git 标签/分支/commit
#   TARGET_NAME  - 预期的 CMake target 名称（用于检测是否可用）
function(add_dependency LIB_NAME REPO_URL GIT_TAG TARGET_NAME)
    set(LIB_DIR "${EXTERNAL_DIR}/${LIB_NAME}")

    if(EXISTS "${LIB_DIR}/CMakeLists.txt")
        # 本地已存在，直接使用
        message(STATUS "📁 [${LIB_NAME}] 使用本地版本: ${LIB_DIR}")
        add_subdirectory("${LIB_DIR}")
    else()
        # 本地不存在，通过 FetchContent 下载
        message(STATUS "🌐 [${LIB_NAME}] 本地未找到，开始自动下载...")

        # 确保 FetchContent 可用
        include(FetchContent)
        include(CMakeDependentOption)

        # 构造带代理前缀的 URL
        set(FULL_URL "${GH_PROXY_URL}${REPO_URL}")

        # 设置安装相关缓存变量（避免子项目安装）
        string(TOUPPER "${LIB_NAME}" LIB_NAME_UPPER)

        FetchContent_Declare(
            ${LIB_NAME}
            GIT_REPOSITORY "${FULL_URL}"
            GIT_TAG        "${GIT_TAG}"
            GIT_SHALLOW    TRUE
            GIT_PROGRESS   TRUE
        )

        # 在获取前设置子项目的缓存变量
        # SDL3
        if(LIB_NAME STREQUAL "SDL")
            set(FETCHCONTENT_${LIB_NAME}_SOURCE_DIR "" CACHE PATH "" FORCE)
        endif()

        FetchContent_MakeAvailable(${LIB_NAME})

        message(STATUS "✅ [${LIB_NAME}] 下载并构建完成")
    endif()
endfunction()

# ==================== 依赖版本配置 ====================
# 在此集中管理各依赖的版本/分支，方便统一升级
set(SDL3_VERSION        "release-3.2.0")
set(SDL_IMAGE_VERSION   "release-3.2.0")
set(SDL_MIXER_VERSION   "release-3.2.0")
set(SDL_NET_VERSION     "release-3.2.0")
set(SDL_TTF_VERSION     "release-3.2.0")
set(GLM_VERSION          "1.0.1")
set(JSON_VERSION         "v3.11.3")
set(SPDLOG_VERSION       "v1.15.0")

# ==================== SDL3 ====================
add_dependency(SDL "https://github.com/libsdl-org/SDL.git" ${SDL3_VERSION} SDL3)

# ==================== SDL_image ====================
set(SDLIMAGE_INSTALL OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_VENDORED OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_AVIF OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_JXL OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_WEBP OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_TIF OFF CACHE BOOL "" FORCE)
add_dependency(SDL_image "https://github.com/libsdl-org/SDL_image.git" ${SDL_IMAGE_VERSION} SDL3_image)

# ==================== SDL_mixer ====================
set(SDLMIXER_INSTALL OFF CACHE BOOL "" FORCE)
set(SDLMIXER_VENDORED OFF CACHE BOOL "" FORCE)
set(SDLMIXER_OPUS OFF CACHE BOOL "" FORCE)
set(SDLMIXER_FLAC_LIBFLAC OFF CACHE BOOL "" FORCE)
set(SDLMIXER_MP3_MPG123 OFF CACHE BOOL "" FORCE)
set(SDLMIXER_MIDI_FLUIDSYNTH OFF CACHE BOOL "" FORCE)
add_dependency(SDL_mixer "https://github.com/libsdl-org/SDL_mixer.git" ${SDL_MIXER_VERSION} SDL3_mixer)

# ==================== SDL_net ====================
set(SDLNET_INSTALL OFF CACHE BOOL "" FORCE)
add_dependency(SDL_net "https://github.com/libsdl-org/SDL_net.git" ${SDL_NET_VERSION} SDL3_net)

# ==================== SDL_ttf ====================
set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)
set(SDLTTF_USE_SYSTEM_FREETYPE OFF CACHE BOOL "" FORCE)
set(SDLTTF_INSTALL OFF CACHE BOOL "" FORCE)
set(SDLTTF_HARFBUZZ OFF CACHE BOOL "" FORCE)
set(SDLTTF_FREETYPE ON CACHE BOOL "" FORCE)
set(SDLTTF_PLUTOSVG OFF CACHE BOOL "" FORCE)
add_dependency(SDL_ttf "https://github.com/libsdl-org/SDL_ttf.git" ${SDL_TTF_VERSION} SDL3_ttf)

# ==================== GLM ====================
add_dependency(glm "https://github.com/g-truc/glm.git" ${GLM_VERSION} glm)

# ==================== nlohmann::json ====================
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(JSON_Install OFF CACHE BOOL "" FORCE)
add_dependency(json "https://github.com/nlohmann/json.git" ${JSON_VERSION} nlohmann_json)

# ==================== spdlog ====================
set(SPDLOG_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SPDLOG_INSTALL OFF CACHE BOOL "" FORCE)
set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "" FORCE)
add_dependency(spdlog "https://github.com/gabime/spdlog.git" ${SPDLOG_VERSION} spdlog)

# ==================== 主程序 ====================
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

# ==================== DLL 复制（Windows + MinGW） ====================
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
        COMMENT "Copying DLLs to output directory..."
    )
endif()

# ==================== Assets 资源复制 ====================
# 复制到可执行文件同目录下的 assets/ 文件夹
# 使用 copy_directory 支持增量更新（仅复制变化的文件）
if(EXISTS ${ASSETS_DIR})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${ASSETS_DIR}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
        COMMENT "📦 Copying assets to output directory..."
    )
    message(STATUS "📦 Assets 复制已启用: ${ASSETS_DIR} → $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets")
else()
    message(WARNING "⚠️  Assets 目录不存在: ${ASSETS_DIR}（跳过资源复制）")
endif()