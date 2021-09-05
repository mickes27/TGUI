#ifdef TGUI_NEXT
    #error This file was split and moved to TGUI/Backend/Renderer/OpenGL3/BackendRenderTargetOpenGL3.hpp and TGUI/Backend/Renderer/GLES2/BackendRenderTargetGLES2.hpp
#else
    #include <TGUI/Config.hpp>

    #if TGUI_USE_GLES
        #pragma message("Warning: This file was deprecated and moved to TGUI/Backend/Renderer/GLES2/BackendRenderTargetGLES2.hpp")
        #include <TGUI/Backend/Renderer/GLES2/BackendRenderTargetGLES2.hpp>
    #else
        #pragma message("Warning: This file was deprecated and moved to TGUI/Backend/Renderer/OpenGL3/BackendRenderTargetOpenGL3.hpp")
        #include <TGUI/Backend/Renderer/OpenGL3/BackendRenderTargetOpenGL3.hpp>
    #endif
#endif
