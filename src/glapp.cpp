#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <csignal>

#include <vulkan/vulkan.h>

#define STB_IMAGE_IMPLEMENTATION
#include "nwin/stb_image.h"
#include "nwin/nwin.h"
#define _LOG_SIZE   512


static GLuint sh_program;
static GLuint vao, vbo, ibo;

struct shaders {
    const std::string vert = 
        "#version 460\n"
        "layout(location = 0) in vec4 pos;\n"
        "layout(location = 1) in vec4 color;\n"
        "out vec4 o_color;\n"
        "void main() {\n"
        "   gl_Position = pos;\n"
        "   o_color = color;\n"
        "}\n";

    const std::string frag = 
        "#version 460\n"
        "in vec4 o_color;\n"
        "layout(location = 0) out vec4 FragColor;\n"
        "void main() { \n"
        "   FragColor = o_color;\n"
        "}\n";
};


static GLfloat vertices[] = {
//  X     Y     Z     A           R     G     B     A
  -1.0f, 1.0f, 0.0f, 1.0f,       1.0f, 0.0f, 0.0f, 1.0f, // vertex 0
   1.0f, 1.0f, 0.0f, 1.0f,       0.0f, 1.0f, 0.0f, 1.0f, // vertex 1
   1.0f,-1.0f, 0.0f, 1.0f,       0.0f, 0.0f, 1.0f, 1.0f, // vertex 2
  -1.0f,-1.0f, 0.0f, 1.0f,       1.0f, 0.0f, 0.0f, 1.0f, // vertex 3
}; // 4 vertices with 6 components (floats) each


static GLuint indices[] = {
    0, 1, 2,
    2, 3, 0
};


void check_shader_compile_status(const uint32_t _id) {
    int success;
    char log[_LOG_SIZE] = { 0 };
    
    glGetShaderiv(_id, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(_id, _LOG_SIZE, NULL, log);
        std::cout << "Shader compilation error\n";
        std::cout << std::string(log) << std::endl;
        exit(-1);
    }
}


void check_shader_link_status(const uint32_t _id) {
    int success;
    char log[_LOG_SIZE] = { 0 };
    int32_t len = _LOG_SIZE;
    
    glGetProgramiv(_id, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(_id, len, &len, log);
        std::cout << "Shader linking failed" << std::endl;
        std::cout << std::string(log) << std::endl;
        exit(-1);
    }
}


void compile_shaders() {
    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

    shaders sh;

    // Create vertex shader source object
    const char *data = sh.vert.data();
    int32_t len = static_cast<int32_t>(sh.vert.size());
    glShaderSource(vert_shader, 1, &data, &len);

    // Create fragment shader source object
    data = sh.frag.data();
    len = static_cast<int32_t>(sh.frag.size());
    glShaderSource(frag_shader, 1, &data, &len);

    // Compile shaders into bytecode
    glCompileShader(vert_shader);
    check_shader_compile_status(vert_shader);
    glCompileShader(frag_shader);
    check_shader_compile_status(frag_shader);

    // Create a new shader program instance
    sh_program = glCreateProgram();
    glAttachShader(sh_program, vert_shader);
    glAttachShader(sh_program, frag_shader);

    // Link shaders together
    glLinkProgram(sh_program);
    check_shader_link_status(sh_program);

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
}


void create_buffer_handles() {
    // Create and bind OpenGL buffers
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    
    std::cout << "Vertices size: " << sizeof(vertices) << std::endl;
    glBufferData(GL_ARRAY_BUFFER, 32 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);

    // Enable vertex attribute arrays
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (char*) (4 * sizeof(GLfloat)));
}


void err_check(const std::string &func_name) {
    GLenum error;
    while((error = glGetError()) != GL_NO_ERROR) {
        switch(error) {
        case GL_INVALID_ENUM:
            std::cerr << func_name << "(): GL_INVALID_ENUM " << std::endl;
            break;

        case GL_INVALID_VALUE:
            std::cerr << func_name << "(): GL_INVALID_VALUE " << std::endl;
            break;

        case GL_INVALID_OPERATION:
            std::cerr << func_name << "(): GL_INVALID_OPERATION " << std::endl;
            break;

        case GL_STACK_OVERFLOW:
            std::cerr << func_name << "(): GL_STACK_OVERFLOW " << std::endl;
            break;

        case GL_STACK_UNDERFLOW:
            std::cerr << func_name << "(): GL_STACK_UNDERFLOW " << std::endl;
            break;

        case GL_OUT_OF_MEMORY:
            std::cerr << func_name << "(): GL_OUT_OF_MEMORY " << std::endl;
            break;

        default:
            break;
        }

        exit(-1);
    }
}


int draw(neko_Window &_win, bool &_allow_toggle) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    err_check("glClearColor");
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    err_check("glClear");
    glViewport(0, 0, _win.cwidth, _win.cheight);
    err_check("glViewport");

    glUseProgram(sh_program);
    err_check("glUseProgram");
    glBindVertexArray(vao);
    err_check("glBindVertexArray");
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    err_check("glDrawElements");

    if(!_win.input.use_text_mode) {
        if(_win.input.raw.active_table[NEKO_KEY_F] && _allow_toggle) {
            _allow_toggle = false;
            if (_win.hints & NEKO_HINT_FULL_SCREEN) {
                std::cout << "glapp: Disabling full-screen mode" << std::endl;
                neko_UpdateSizeMode(&_win, NEKO_HINT_NO_FULL_SCREEN);
            }
            else if (_win.hints & NEKO_HINT_NO_FULL_SCREEN) {
                std::cout << "glapp: Enabling full-screen mode" << std::endl;
                neko_UpdateSizeMode(&_win, NEKO_HINT_FULL_SCREEN);
            }
        } else if (_win.input.raw.active_table[NEKO_KEY_ESCAPE] && _allow_toggle) {
            std::cout << "glapp: Unicode input mode is enabled. Text will be outputted to the console" << std::endl;
            _allow_toggle = false;
            _win.input.use_text_mode = true;
        } else if (_win.input.raw.active_table[NEKO_KEY_Q] && _allow_toggle) {
            std::cout << "glapp: Quitting, bye!" << std::endl;
            return 0;
        } else if (_win.input.raw.active_table[NEKO_KEY_H] && _allow_toggle) {
            std::cout << "glapp: Cursor is now in hidden mode" << std::endl;
            _allow_toggle = false;
            neko_SetMouseCursorMode(&_win, NEKO_CURSOR_MODE_HIDDEN);
        } else if (_win.input.raw.active_table[NEKO_KEY_S] && _allow_toggle) {
            std::cout << "glapp: Cursor is now in standard mode" << std::endl;
            _allow_toggle = false;
            neko_SetMouseCursorMode(&_win, NEKO_CURSOR_MODE_STANDARD);
        } else if (_win.input.raw.active_table[NEKO_KEY_P] && _allow_toggle) {
            std::cout << "glapp: Cursor is now in pointer mode" << std::endl;
            _allow_toggle = false;
            neko_SetMouseCursorMode(&_win, NEKO_CURSOR_MODE_POINTER);
        } else if (_win.input.raw.active_table[NEKO_KEY_W] && _allow_toggle) {
            std::cout << "glapp: Cursor is now in waiting mode" << std::endl;
            _allow_toggle = false;
            neko_SetMouseCursorMode(&_win, NEKO_CURSOR_MODE_WAITING);
        }

        if (_win.input.raw.released_table[NEKO_KEY_F] ||
            _win.input.raw.released_table[NEKO_KEY_ESCAPE] ||
            _win.input.raw.released_table[NEKO_KEY_H] ||
            _win.input.raw.released_table[NEKO_KEY_S] ||
            _win.input.raw.released_table[NEKO_KEY_P] ||
            _win.input.raw.released_table[NEKO_KEY_W]) 
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            _allow_toggle = true;
        }
    } else {
        // check if disabling text mode is required
        if(_win.input.raw.active_table[NEKO_KEY_ESCAPE] && _allow_toggle) {
            std::cout << "glapp: Unicode input mode is disabled." << std::endl;
            _allow_toggle = false;
            _win.input.use_text_mode = false;
        } 
        
        if(_win.input.raw.released_table[NEKO_KEY_ESCAPE]) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            _allow_toggle = true;
        }

        for(uint32_t i = 0; i < _win.input.text.size; i++) {
            std::cout << _win.input.text.ucs[i] << std::flush;
        }
    }

    return 1;
}


void run(neko_Window &parent) {
    bool allow_toggle = true;

    while(parent.is_running) {
        neko_UpdateWindow(&parent);
        if (!draw(parent, allow_toggle))
            break;
    }

    std::cout << "Done" << std::endl;
}


void cleanup(neko_Window &_parent) {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteProgram(sh_program);

    neko_DestroyWindow(&_parent);
    neko_DeinitAPI();
}


int main() {
    int width, height;
    std::cout << "Enter window width: ";
    std::cin >> width;
    std::cout << "Enter widow height: ";
    std::cin >> height;

    
    // Create a new window
    neko_InitAPI();
    neko_Window parent_win = neko_NewWindow(width, height, NEKO_HINT_API_OPENGL | NEKO_HINT_RESIZEABLE, 0, 0, "GLTest");
    neko_glMakeCurrent(&parent_win);

    int status = neko_LoadGL();
    if(!status) {
        std::cerr << "Failed to load OpenGL functions" << std::endl;
        std::exit(-1);
    }

    neko_Icon icons[2];
    icons[0].pixels = stbi_load("../icon/32x32.png", &icons[0].width, &icons[0].height, 0, 4);
    icons[1].pixels = stbi_load("../icon/16x16/png", &icons[0].width, &icons[0].height, 0, 4);

    neko_SetIcons(&parent_win, 2, icons);
    free(icons[0].pixels);
    free(icons[1].pixels);

    neko_SetMouseCursorMode(&parent_win, NEKO_CURSOR_MODE_STANDARD);
    compile_shaders();
    create_buffer_handles();
    run(parent_win);
    cleanup(parent_win);

    return 0;
}
