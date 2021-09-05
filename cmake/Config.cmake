# Detect the OS
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(TGUI_OS_WINDOWS 1)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    set(TGUI_OS_ANDROID 1)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    if(ANDROID)  # TODO TGUI_NEXT: This should no longer be checked, we use CMAKE_SYSTEM_NAME now
        message(WARNING "CMAKE_SYSTEM_NAME is supposed to match 'Android' when cross-compiling for Android")
        set(TGUI_OS_ANDROID 1)
    else()
        set(TGUI_OS_LINUX 1)
    endif()
elseif(CMAKE_SYSTEM_NAME MATCHES "^k?FreeBSD$" OR CMAKE_SYSTEM_NAME MATCHES "^OpenBSD$")
    # We will pretend that BSD is Linux because the two are practically indistinguishable on the
    # high level that TGUI uses to access the OS. The presence or absence of the TGUI_OS_BSD variable
    # can still be used to differentiate between the two where necessary.
    set(TGUI_OS_LINUX 1)
    set(TGUI_OS_BSD 1)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    if(IOS)  # TODO TGUI_NEXT: This should no longer be checked, we use CMAKE_SYSTEM_NAME now
        message(WARNING "CMAKE_SYSTEM_NAME is supposed to match 'iOS' when cross-compiling for iOS")
        set(TGUI_OS_IOS 1)
    else()
        set(TGUI_OS_MACOS 1)
    endif()
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
    set(TGUI_OS_IOS 1)
else()
    message(FATAL_ERROR "Unsupported operating system or environment")
    return()
endif()

# Detect the compiler.
# Note: The detection is order is important because:
# - Visual Studio can both use MSVC and Clang
# - GNUCXX can still be set on macOS when using Clang
if(MSVC)
    set(TGUI_COMPILER_MSVC 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(TGUI_COMPILER_CLANG 1)
elseif(CMAKE_COMPILER_IS_GNUCXX)
    set(TGUI_COMPILER_GCC 1)

    # Check if this is the TDM-GCC version.
    # The TGUI_COMPILER_GCC_TDM variable will contain a text if TDM and will be empty otherwise.
    execute_process(COMMAND "${CMAKE_CXX_COMPILER}" "--version" OUTPUT_VARIABLE GCC_COMPILER_VERSION)
    string(REGEX MATCHALL ".*(tdm[64]*-[1-9]).*" TGUI_COMPILER_GCC_TDM "${GCC_COMPILER_VERSION}")
else()
    message(FATAL_ERROR "Unsupported compiler")
    return()
endif()

# Set pkgconfig install directory
if (TGUI_OS_BSD)
    set(TGUI_PKGCONFIG_DIR "/libdata/pkgconfig")
else()
    set(TGUI_PKGCONFIG_DIR "/${CMAKE_INSTALL_LIBDIR}/pkgconfig")
endif()
