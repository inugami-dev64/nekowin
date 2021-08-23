#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include <glad/glad.h>
#include <vulkan/vulkan.h>
#include <nwin.h>
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


void check_shader_compile_status(const uint32_t id) {
    int success;
    char log[_LOG_SIZE] = { 0 };
    
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(id, _LOG_SIZE, NULL, log);
        std::cout << "Shader compilation error\n";
        std::cout << std::string(log) << std::endl;
        exit(-1);
    }
}


void check_shader_link_status(const uint32_t id) {
    int success;
    char log[_LOG_SIZE] = { 0 };
    int32_t len = _LOG_SIZE;
    
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(id, len, &len, log);
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
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (char*) 0 + 4 * sizeof(GLfloat));
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


void run(neko_Window win) {
    bool allow_toggle = true;
    neko_Hint hints;
    int32_t x, y;

    while(neko_IsRunning(win)) {
        neko_UpdateWindow(win);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        err_check("glClearColor");
        glClear(GL_COLOR_BUFFER_BIT);

        neko_GetWindowSize(win, &x, &y);
        glViewport(0, 0, x, y);

        err_check("glClear");
        glUseProgram(sh_program);
        err_check("glUseProgram");
        glBindVertexArray(vao);
        err_check("glBindVertexArray");
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        err_check("glDrawElements");

        neko_GetWindowHints(win, &hints);

        int64_t x, y;
        neko_GetMousePos(win, &x, &y);

        if(neko_FindKeyStatus(NEKO_KEY_F, NEKO_INPUT_EVENT_TYPE_ACTIVE) && allow_toggle) {
            allow_toggle = false;
            if(hints & NEKO_HINT_FULL_SCREEN) {
                neko_UpdateSizeMode(win, NEKO_HINT_NO_FULL_SCREEN);
                printf("Switching to resizeable format\n");
            }
            else if(hints & NEKO_HINT_NO_FULL_SCREEN) {
                neko_UpdateSizeMode(win, NEKO_HINT_FULL_SCREEN);
                printf("Switching to fullscreen format\n");
            }
        }

        else if(neko_FindKeyStatus(NEKO_KEY_Q, NEKO_INPUT_EVENT_TYPE_ACTIVE))
            break;

        else if(neko_FindKeyStatus(NEKO_KEY_H, NEKO_INPUT_EVENT_TYPE_ACTIVE) && allow_toggle) {
            allow_toggle = false;
            neko_SetMouseCursorMode(win, NEKO_CURSOR_MODE_HIDDEN);
        }

        else if(neko_FindKeyStatus(NEKO_KEY_S, NEKO_INPUT_EVENT_TYPE_ACTIVE) && allow_toggle) {
            allow_toggle = false;
            neko_SetMouseCursorMode(win, NEKO_CURSOR_MODE_STANDARD);
        }

        else if(neko_FindKeyStatus(NEKO_KEY_P, NEKO_INPUT_EVENT_TYPE_ACTIVE) && allow_toggle) {
            allow_toggle = false;
            neko_SetMouseCursorMode(win, NEKO_CURSOR_MODE_POINTER);
        }

        else if(neko_FindKeyStatus(NEKO_KEY_W, NEKO_INPUT_EVENT_TYPE_ACTIVE) && allow_toggle) {
            allow_toggle = false;
            neko_SetMouseCursorMode(win, NEKO_CURSOR_MODE_WAITING);
        }


        if(neko_FindKeyStatus(NEKO_KEY_F, NEKO_INPUT_EVENT_TYPE_RELEASED) ||
           neko_FindKeyStatus(NEKO_KEY_H, NEKO_INPUT_EVENT_TYPE_RELEASED) ||
           neko_FindKeyStatus(NEKO_KEY_S, NEKO_INPUT_EVENT_TYPE_RELEASED) ||
           neko_FindKeyStatus(NEKO_KEY_P, NEKO_INPUT_EVENT_TYPE_RELEASED) ||
           neko_FindKeyStatus(NEKO_KEY_W, NEKO_INPUT_EVENT_TYPE_RELEASED)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            allow_toggle = true;
        }
    }

    std::cout << "Done" << std::endl;
}


void cleanup(neko_Window win) {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteProgram(sh_program);

    neko_DestroyWindow(win);
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
    neko_Window win = neko_NewWindow(width, height, NEKO_HINT_API_OPENGL | NEKO_HINT_RESIZEABLE, "GLTest");

    int status = neko_LoadGL();
    if(!status) {
        std::cerr << "Failed to load OpenGL functions" << std::endl;
        std::exit(-1);
    }

    const unsigned char *ver = glGetString(GL_VERSION);
    compile_shaders();
    create_buffer_handles();
    run(win);
    cleanup(win);

    return 0;
}
