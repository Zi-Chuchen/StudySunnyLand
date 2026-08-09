# StudySunnyLand - CMake 构建说明

## 快速开始

```bash
# 1. 克隆项目
git clone https://github.com/Zi-Chuchen/StudySunnyLand
# (国内可使用代理)
# git clone https://gh-proxy.com/https://github.com/Zi-Chuchen/StudySunnyLand
cd StudySunnyLand

# 2. 创建构建目录
mkdir build && cd build

# 3. 配置（CMake 会提示是否使用代理）
cmake ..

# 4. 编译
cmake --build . -j
```

## GitHub 代理加速

如果在国内访问 GitHub 较慢，可以在配置时启用代理：

```bash
# 启用代理（会在每个 GitHub URL 前加上 https://gh-proxy.com/）
cmake -DUSE_GH_PROXY=ON ..

# 关闭代理（直连 GitHub）
cmake -DUSE_GH_PROXY=OFF ..
```

> 代理地址固定为 `https://gh-proxy.com/`，如需更换可在 `CMakeLists.txt` 顶部修改 `GH_PROXY_URL`。

## 依赖管理策略

项目采用 **本地优先 + 自动下载** 的混合策略：

| 情况 | 行为 |
|------|------|
| `external/<库名>/` 存在 | ✅ 使用本地版本（`add_subdirectory`） |
| `external/<库名>/` 不存在 | 🌐 自动通过 `FetchContent` 从 GitHub 下载 |

### 当前管理的依赖

| 库 | 版本/分支 | 用途 |
|----|-----------|------|
| SDL3 | release-3.2.0 | 核心窗口/输入/渲染 |
| SDL_image | release-3.2.0 | 图片加载 |
| SDL_mixer | release-3.2.0 | 音频播放 |
| SDL_net | release-3.2.0 | 网络通信 |
| SDL_ttf | release-3.2.0 | 字体渲染 |
| glm | 1.0.1 | 数学库 |
| nlohmann/json | v3.11.3 | JSON 解析 |
| spdlog | v1.15.0 | 日志库 |

> 版本统一在 `CMakeLists.txt` 顶部的 `set(..._VERSION)` 处管理，升级时只需改一处。

## 手动预下载所有依赖

如果想提前把所有依赖下载到 `external/` 目录（之后离线构建）：

```bash
# Linux/macOS
./scripts/fetch_deps.sh

# Windows (PowerShell)
.\scripts\fetch_deps.ps1
```

## Assets 资源复制

构建完成后，`assets/` 目录会自动复制到可执行文件同目录下的 `assets/` 文件夹：

```
build/bin/
├── SunnyLand.exe (或 SunnyLand)
├── SDL3.dll
├── SDL3_image.dll
├── ...
└── assets/
    ├── textures/
    ├── sounds/
    └── ...
```

## 常见问题

### Q: 代理下载失败怎么办？
A: 尝试关闭代理重新配置：`cmake -DUSE_GH_PROXY=OFF ..`，或检查 `https://gh-proxy.com/` 是否可访问。

### Q: 如何强制重新下载某个库？
A: 删除 `external/` 下对应的子目录，或删除 `build/_deps/` 下对应的缓存目录，重新运行 cmake。

### Q: CI/CD 中如何避免交互提示？
A: 始终显式传入 `-DUSE_GH_PROXY=ON` 或 `OFF`，CMake 检测到变量已定义就不会再提示。
