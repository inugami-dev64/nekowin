/*    ____         ________     __      _        ______  
 *   |  _ \_      |  ______|   |  \    | |     _/  ____\
 *   | | \_ \_    |  |         |   \   | |    /  _/   
 *   | |   \  |   |  |_____    | |\ \  | |   |  |   _____ 
 *   | |    | |   |  ______|   | | \ \ | |   |  |  |___  | 
 *   | |  _/ _|   |  |         | |  \ \| |    \  \    / /
 *   | |_/ _/     |  |_____    | |   \   |     \_ \__/ /
 *   |____/       |________|   |_|    \__|       \____/
 *               
 * __________________________________________________________
 * __________________________________________________________
 *                       Project DENG 
 *
 *
 * Copyright (C) 2020 - 2021
 * This Software is licensed under Apache License as described 
 * in the LICENSE.md file, which you should have recieved with 
 * this distribution.
 * 
 * You may reproduce and distribute copies of the
 * Work or Derivative Works thereof in any medium, with or without
 * modifications, and in Source or Object form, provided that You
 * meet the following conditions:
 *
 * (a) You must give any other recipients of the Work or
 *     Derivative Works a copy of this License; and
 *
 * (b) You must cause any modified files to carry prominent notices
 *     stating that You changed the files; and
 *
 * (c) You must retain, in the Source form of any Derivative Works
 *     that You distribute, all copyright, patent, trademark, and
 *     attribution notices from the Source form of the Work,
 *     excluding those notices that do not pertain to any part of
 *     the Derivative Works; and
 *
 * (d) If the Work includes a "NOTICE" text file as part of its
 *     distribution, then any Derivative Works that You distribute must
 *     include a readable copy of the attribution notices contained
 *     within such NOTICE file, excluding those notices that do not
 *     pertain to any part of the Derivative Works, in at least one
 *     of the following places: within a NOTICE text file distributed
 *     as part of the Derivative Works; within the Source form or
 *     documentation, if provided along with the Derivative Works; or,
 *     within a display generated by the Derivative Works, if and
 *     wherever such third-party notices normally appear. The contents
 *     of the NOTICE file are for informational purposes only and
 *     do not modify the License. You may add Your own attribution
 *     notices within Derivative Works that You distribute, alongside
 *     or as an addendum to the NOTICE text from the Work, provided
 *     that such additional attribution notices cannot be construed
 *     as modifying the License.
 *
 * You may add Your own copyright statement to Your modifications and
 * may provide additional or different license terms and conditions
 * for use, reproduction, or distribution of Your modifications, or
 * for any such Derivative Works as a whole, provided Your use,
 * reproduction, and distribution of the Work otherwise complies with
 * the conditions stated in this License.
 */ 


#ifndef __NWIN_HPP
#define __NWIN_HPP

#ifdef __NWIN_CPP
    #include <stdlib.h>
    #include <stdint.h>
    #include <vector>
    #include <utility>
    #include <string>

    #include <vulkan/vulkan.h>
    #include <nwin.h>
#endif


#define NOMINMAX
#include <surface_window.h>


namespace neko {   
    
    /// Main window handling class for DENG
    class Window {
    private:
        neko_Window *m_p_surface;
        neko_Hint m_hints;
        std::string m_title;
        bool m_is_vc = false;
        std::pair<uint64_t, uint64_t> m_prev_vc_pos;

    public:
        Window(int32_t width, int32_t height, neko_Hint api_hints, const char *title);
        ~Window();


        /// Toggle virtual cursor mode
        void toggleVCMode();


        /// Force set virtual cursor mode
        void changeVCMode(bool is_vc);

        
        /// Hide the cursor's visbility
        void hideCursor();


        /// Make the cursor visible
        /// NOTE: There should be a special cursor struct for DENG called neko_Cursor in the future
        /// but for now it is ignored
        void showCursor();


        /// Check if virtual cursor mode is enabled
        bool isVCP();


        /// Update window and input devices data
        void update();


        /// Force specified VCP position to virtual mouse cursor instance
        void forceVCPPos(const std::pair<uint64_t, uint64_t> &pos);


        /// Create new vulkan surface instance
        VkResult initVkSurface(VkInstance &instance,
            VkSurfaceKHR &surface);


        /// Search for all required vulkan extensions
        char **findVulkanSurfaceExtensions(size_t *p_ext_c, bool enable_vl);



        /// Get the current mouse position
        std::pair<uint64_t, uint64_t> getMPos() const;


        /// Get the mouse delta compared to previous frame mouse position
        std::pair<uint64_t, uint64_t> getMDelta() const;


        /// Get the title of the window
        const std::string &getTitle() const;

        
        /// Get the size of the window
        std::pair<uint32_t, uint32_t> getSize() const;


        /// Get the vector size for one pixel in surface
        std::pair<float, float> getPixelSize() const;
    };
}

#endif
