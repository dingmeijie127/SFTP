cmake_minimum_required(VERSION 3.10)
project(testxml2)


option (USE_CROSS "Use cross compile " ON)
if(USE_CROSS)
    # 添加库目录
    link_directories(./lib)
    # 添加头文件
    include_directories(inc)
    # 设置使用的编译器
    set(CMAKE_CXX_COMPILER "/opt/linux/x86-arm/aarch64-mix210-linux/bin/aarch64-mix210-linux-g++")
    # 指定生成目标
    add_executable(SFTPClientARM mytest.cpp testSFTP.cpp testSFTP.h)
    # 添加链接库
    target_link_libraries(SFTPClientARM  sslm crypto ssh2)

else ()
    # Find PkgConfig to manage dependencies
    find_package(PkgConfig REQUIRED)

    # Check for tinyxml2
    pkg_check_modules(TINYXML2 REQUIRED IMPORTED_TARGET tinyxml2)

    # Check for libssh2 using pkg-config
    pkg_check_modules(LIBSSH2 REQUIRED IMPORTED_TARGET libssh2)

    # Create executables
    add_executable(${PROJECT_NAME} main.cpp)
    add_executable(Read read.cpp)
    add_executable(SFTPClient mytest.cpp testSFTP.cpp testSFTP.h)
    add_executable(testmm testmm.cpp)


    # Link tinyxml2 with the first two executables
    target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::TINYXML2)
    target_link_libraries(Read PRIVATE PkgConfig::TINYXML2)

    # Link libssh2 with the SFTPClient executable
    target_link_libraries(SFTPClient PRIVATE PkgConfig::LIBSSH2)
    target_link_libraries(testmm PRIVATE PkgConfig::LIBSSH2)
endif()



