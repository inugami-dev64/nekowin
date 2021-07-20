/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: nwim.cpp - nwin library C++ binding source file
/// author: Karl-Mihkel Ott

#define __NWIN_CPP
#include <nwin.hpp>

namespace neko {
    Window::Window(int32_t x, int32_t y, neko_Hint hints, const char *title) {
        m_title = title;
        m_p_surface = neko_InitSurfaceWindow(x, y, hints, title);
        m_hints = hints;
    }

    Window::~Window() {
        neko_DestroyWindow(m_p_surface);
    }


    /// Set virtual cursor mode that locks real cursor to the center of the window
    void Window::toggleVCMode() {    
        if(!m_p_surface->vc_data.is_enabled) {
            neko_SetMouseCursorMode(m_p_surface, NEKO_MOUSE_MODE_INVISIBLE);
            m_p_surface->vc_data.is_enabled = true;
        } 

        else { 
            neko_SetMouseCursorMode(m_p_surface, NEKO_MOUSE_MODE_CURSOR_VISIBLE);
            m_p_surface->vc_data.is_enabled = false;
        }
        m_is_vc = !m_is_vc;
    }

    
    /// Force set virtual cursor mode
    void Window::changeVCMode(bool is_vc) {
        if(m_is_vc == is_vc) return;
        m_is_vc = is_vc;
        if(is_vc) neko_SetMouseCursorMode(m_p_surface, NEKO_MOUSE_MODE_INVISIBLE);
        else neko_SetMouseCursorMode(m_p_surface, NEKO_MOUSE_MODE_CURSOR_VISIBLE);
    }
    

    /// Hide the cursor's visbility
    void Window::hideCursor() {
        neko_SetMouseCursorMode(m_p_surface, NEKO_MOUSE_MODE_INVISIBLE);
    }


    /// Make the cursor visible
    /// NOTE: There should be a special cursor struct for DENG called neko_Cursor in the future
    /// but for now it is ignored
    void Window::showCursor() {
        neko_SetMouseCursorMode(m_p_surface, NEKO_MOUSE_MODE_CURSOR_VISIBLE);
    }


    /// Update window and input devices data
    void Window::update() {
        m_prev_vc_pos.first = m_p_surface->vc_data.x;
        m_prev_vc_pos.second = m_p_surface->vc_data.y;

        neko_UpdateWindow(m_p_surface);

        if(m_hints & NEKO_HINT_API_OPENGL)
            glViewport(0, 0, m_p_surface->width, m_p_surface->height);
    }


    /// Force specified VCP position to virtual mouse cursor instance
    void Window::forceVCPPos(const std::pair<uint64_t, uint64_t> &pos) {
        m_p_surface->vc_data.x = pos.first;
        m_p_surface->vc_data.y = pos.second;
    }


    /// Create new vulkan surface instance
    VkResult Window::initVkSurface (
        VkInstance &instance,
        VkSurfaceKHR &surface
    ) {
        return neko_InitVKSurface(m_p_surface, &instance, &surface);
    }


    /// Find all required surface extensions
    char **Window::findVulkanSurfaceExtensions (
        uint32_t *p_ext_c, 
        bool enable_vl
    ) {
        char **exts;
        neko_GetRequiredVKSurfaceExt(m_p_surface, &exts, p_ext_c, enable_vl);

        return exts;
    }


    /// Get the current mouse position
    std::pair<uint64_t, uint64_t> Window::getMPos() const {
        neko_GetMousePos(m_p_surface, false);

        // Return either virtual cursor position or real cursor position
        if(m_is_vc) {
            return std::pair<uint64_t, uint64_t>{
                static_cast<uint64_t>(m_p_surface->vc_data.x), 
                static_cast<uint64_t>(m_p_surface->vc_data.y)
            };
        }

        else {
            return std::pair<uint64_t, uint64_t>{
                static_cast<uint64_t>(m_p_surface->mx),
                static_cast<uint64_t>(m_p_surface->my)
            };
        }
    }


    /// Get the mouse delta compared to previous frame mouse position
    std::pair<uint64_t, uint64_t> Window::getMDelta() const {
        if(m_p_surface->vc_data.is_enabled) {
            return std::pair<uint64_t, uint64_t>{
                static_cast<uint64_t>(m_p_surface->vc_data.x - m_prev_vc_pos.first),
                static_cast<uint64_t>(m_p_surface->vc_data.y - m_prev_vc_pos.second)
            };
        }

        else {
            return std::pair<uint64_t, uint64_t>{
                static_cast<uint64_t>(m_p_surface->mx - m_prev_vc_pos.first),
                static_cast<uint64_t>(m_p_surface->my - m_prev_vc_pos.second)
            };
        }
    }


    /// Get the vector size for one pixel in surface
    std::pair<float, float> Window::getPixelSize() const { 
        return std::pair<float, float> {
            2.0f / static_cast<float>(m_p_surface->width),
            2.0f / static_cast<float>(m_p_surface->height)
        }; 
    }

    /// Get the title of the window
    const std::string &Window::getTitle() const { return m_title; }

    /// Get the size of the window
    std::pair<uint32_t, uint32_t> Window::getSize() const { 
        return std::pair<uint32_t, uint32_t> {
            static_cast<uint32_t>(m_p_surface->width),
            static_cast<uint32_t>(m_p_surface->height)
        }; 
    }

    /// Check if virtual cursor mode is enabled
    bool Window::isVCP() { return m_is_vc; }
}
