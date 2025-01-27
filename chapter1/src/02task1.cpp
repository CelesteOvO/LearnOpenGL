#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

///task1:添加更多顶点到数据中，使用glDrawArrays，尝试绘制两个彼此相连的三角形：

void init_glfw();
auto init_glad() -> bool;
auto create_window(int width, int height) -> GLFWwindow *;
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void process_input(GLFWwindow *window);

const char *my_vertex_shader_source = "#version 330 core\n"
                                      "layout (location = 0) in vec3 aPos;\n"
                                      "void main()\n"
                                      "{\n"
                                      "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                      "}\0";
const char *my_fragment_shader_source = "#version 330 core\n"
                                        "out vec4 FragColor;\n"
                                        "void main()\n"
                                        "{\n"
                                        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                        "}\n\0";

auto compile_shader(const char *vertex_shader_source, const char *fragment_shader_source) -> unsigned int
{
    // 编译Vertex Shader
    unsigned int vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_id, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader_id);

    // 编译Fragment Shader
    unsigned int fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_id, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader_id);

    // 创建Shader Program
    unsigned int shader_program_id = glCreateProgram();
    glAttachShader(shader_program_id, vertex_shader_id);
    glAttachShader(shader_program_id, fragment_shader_id);
    glLinkProgram(shader_program_id);

    /**
     * 可选项：检查上面三个编译是否成功
     */
    int success;
    char infoLog[512];
    // 检查Vertex Shader是否编译成功
    glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader_id, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED" << std::endl;
    }
    // 检查Fragment Shader是否编译成功
    glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_shader_id, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED" << std::endl;
    }
    // 检查Shader Program是否链接成功
    glGetProgramiv(shader_program_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader_program_id, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::LINK_FAILED" << std::endl;
    }

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    return shader_program_id;
}

/// 使用glDrawArrays，所以先不使用EBO了
float vertices[] = {
        // first triangle
        -0.9f, -0.5f, 0.0f,  // left
        -0.0f, -0.5f, 0.0f,  // right
        -0.45f, 0.5f, 0.0f,  // top
        // second triangle
        0.0f, -0.5f, 0.0f,  // left
        0.9f, -0.5f, 0.0f,  // right
        0.45f, 0.5f, 0.0f   // top
};

auto pass_geometry_data_to_GPU(float vertices_array[], int vertices_array_size) -> unsigned int
{
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO); // 生成一个VAO
    glGenBuffers(1, &VBO); // 在GPU中开辟一块内存，返回一个值到VBO中代表这块内存的编号。

    glBindVertexArray(VAO); // 绑定VAO，这样后面的操作都会作用在这块内存上。

    glBindBuffer(GL_ARRAY_BUFFER, VBO); // 绑定VBO，这样后面的操作都会作用在这块内存上。
    glBufferData(GL_ARRAY_BUFFER, vertices_array_size, vertices_array, GL_STATIC_DRAW); // 将数据传递到VBO中。

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0); // 设置顶点属性指针，告诉OpenGL如何解析顶点数据。
    glEnableVertexAttribArray(0); // 启用顶点属性指针。

    glBindBuffer(GL_ARRAY_BUFFER, 0); // 解绑VBO
    glBindVertexArray(0); // 解绑VAO

    return VAO;
}

auto main() -> int
{
    init_glfw();
    auto *window = create_window(800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    init_glad();

    // -------------------- NEW START --------------------
    /**
     * 在初始化OpenGL之后，我们可以做一些在渲染之前的准备工作。
     * 比如预加载数据到GPU以及预编译Shader，都是在这里进行的。
     */
    unsigned int shader_program_id = compile_shader(my_vertex_shader_source, my_fragment_shader_source);
    unsigned int triangle_VAO = pass_geometry_data_to_GPU(vertices, sizeof(vertices));
    // -------------------- NEW END --------------------

    while (!glfwWindowShouldClose(window))
    {
        process_input(window);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // -------------------- NEW START --------------------
        /**
         * 在渲染循环中绘制想要的图形
         */
        glUseProgram(shader_program_id); // 绘制之前一定要指定使用哪个shader program
        glBindVertexArray(triangle_VAO); // 指定想要绘制的图形的数据（这里是两个三角形）
        //        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 在绘制之前，我们可以选择指定线框模式绘制（如果要回到原来的模式，一定要在设置一遍glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)给它设置回来）
        glDrawArrays(GL_TRIANGLES, 0, 6); // 绘制两个三角形
        // -------------------- NEW END --------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

void init_glfw()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

auto init_glad() -> bool
{
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    return true;
}

auto create_window(int width, int height) -> GLFWwindow *
{
    GLFWwindow *window = glfwCreateWindow(width, height, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    return window;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
