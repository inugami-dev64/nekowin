#include <iostream>
#include <string>

#include <glad/glad.h>
#include <vulkan/vulkan.h>
#include <nwin.hpp>
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

    // Enable vertex attribute arrays
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (char*) 0 + 4 * sizeof(GLfloat));

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);
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


void run(neko::Window &win) {
    while(neko_IsRunning()) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(sh_program);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        win.update();

        // Check for any errors during the frame generation
        err_check("glDrawElements");
    }

    std::cout << "Done" << std::endl;
}


void cleanup() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteProgram(sh_program);
}


int main() {
    int width, height;
    std::cout << "Enter window width: ";
    std::cin >> width;
    std::cout << "Enter widow height: ";
    std::cin >> height;

    
    // Create a new window
    neko::Window win(width, height, NEKO_HINT_API_OPENGL, "GLtest");

    int status = gladLoadGL();
    if(!status) {
        std::cerr << "Failed to load OpenGL functions" << std::endl;
        std::exit(-1);
    }

    compile_shaders();
    create_buffer_handles();
    run(win);

    cleanup();

    return 0;
}
