#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

void init_glfw();
auto init_glad() -> bool;
auto create_window(int width, int height) -> GLFWwindow *;
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void process_input(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

#include "../include/shader.h"
#include "../include/texture.h"
#include "../include/object.h"
#include "../include/camera.h"

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 800.f / 2.0f;
float lastY = 600.f / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

auto main() -> int
{
    init_glfw();
    auto *window = create_window(800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    init_glad();

    Shader shader(MY_SHADER_DIR + std::string("04shader_vertex.glsl"), MY_SHADER_DIR + std::string("07shader_fragment.glsl"));
    Shader lightShader(MY_SHADER_DIR + std::string("01light_shader_vertex.glsl"), MY_SHADER_DIR + std::string("07light_shader_fragment.glsl"));

    std::array<float, 288> vertices = {
            // positions         // normal         // texture coords
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    glm::vec3 pointLightPositions[] = {
            glm::vec3( 0.7f,  0.2f,  2.0f),
            glm::vec3( 2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    glm::vec3 pointLightColors[] = {
            glm::vec3(1.0f, 0.6f, 0.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 1.0, 0.0),
            glm::vec3(0.2f, 0.2f, 1.0f)
    };

    std::array<unsigned int, 36> indices = {0, 1, 2,
                                            3, 4, 5,
                                            6, 7, 8,
                                            9, 10, 11,
                                            12, 13, 14,
                                            15, 16, 17,
                                            18, 19, 20,
                                            21, 22, 23,
                                            24, 25, 26,
                                            27, 28, 29,
                                            30, 31, 32,
                                            33, 34, 35,
    };

    Object cube(vertices.data(), sizeof(vertices), indices.data(), sizeof(indices));
    Object light(vertices.data(), sizeof(vertices), indices.data(), sizeof(indices));
    Texture texture1(MY_TEXTURE_DIR + std::string("container2.png"));
    Texture texture2(MY_TEXTURE_DIR + std::string("container2_specular.png"));
    ///container2_specular.png这张图片应该是4通道的，在原来的texture类里面只能读取3通道，所以在这一次稍微更改了texture类，包括之前如果出现图片无法读取的情况也可能和通道相关

    shader.use();
    shader.set_int("material.diffuse", 0);
    shader.set_int("material.specular", 1);

    while (!glfwWindowShouldClose(window))
    {
        process_input(window);
        glClearColor(0.75f, 0.52f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        shader.use();
        shader.set_vec3("viewPos", camera.Position);
        shader.set_float("material.shininess", 32.0f);

        // directional light
        shader.set_vec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        shader.set_vec3("dirLight.ambient", 0.3f, 0.24f, 0.14f);
        shader.set_vec3("dirLight.diffuse", 0.7f, 0.42f, 0.26f);
        shader.set_vec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        // point light 1
        shader.set_vec3("pointLights[0].position", pointLightPositions[0]);
        shader.set_vec3("pointLights[0].ambient", pointLightColors[0].x * 0.1,  pointLightColors[0].y * 0.1,  pointLightColors[0].z * 0.1);
        shader.set_vec3("pointLights[0].diffuse", pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z);
        shader.set_vec3("pointLights[0].specular", pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z);
        shader.set_float("pointLights[0].constant", 1.0f);
        shader.set_float("pointLights[0].linear", 0.09f);
        shader.set_float("pointLights[0].quadratic", 0.032f);
        // point light 2
        shader.set_vec3("pointLights[1].position", pointLightPositions[1]);
        shader.set_vec3("pointLights[1].ambient", pointLightColors[1].x * 0.1,  pointLightColors[1].y * 0.1,  pointLightColors[1].z * 0.1);
        shader.set_vec3("pointLights[1].diffuse", pointLightColors[1].x,  pointLightColors[1].y,  pointLightColors[1].z);
        shader.set_vec3("pointLights[1].specular", pointLightColors[1].x,  pointLightColors[1].y,  pointLightColors[1].z);
        shader.set_float("pointLights[1].constant", 1.0f);
        shader.set_float("pointLights[1].linear", 0.09f);
        shader.set_float("pointLights[1].quadratic", 0.032f);
        // point light 3
        shader.set_vec3("pointLights[2].position", pointLightPositions[2]);
        shader.set_vec3("pointLights[2].ambient", pointLightColors[2].x * 0.1,  pointLightColors[2].y * 0.1,  pointLightColors[2].z * 0.1);
        shader.set_vec3("pointLights[2].diffuse", pointLightColors[2].x,  pointLightColors[2].y,  pointLightColors[2].z);
        shader.set_vec3("pointLights[2].specular", pointLightColors[2].x,  pointLightColors[2].y,  pointLightColors[2].z);
        shader.set_float("pointLights[2].constant", 1.0f);
        shader.set_float("pointLights[2].linear", 0.09f);
        shader.set_float("pointLights[2].quadratic", 0.032f);
        // point light 4
        shader.set_vec3("pointLights[3].position", pointLightPositions[3]);
        shader.set_vec3("pointLights[3].ambient", pointLightColors[3].x * 0.1,  pointLightColors[3].y * 0.1,  pointLightColors[3].z * 0.1);
        shader.set_vec3("pointLights[3].diffuse", pointLightColors[3].x,  pointLightColors[3].y,  pointLightColors[3].z);
        shader.set_vec3("pointLights[3].specular", pointLightColors[3].x,  pointLightColors[3].y,  pointLightColors[3].z);
        shader.set_float("pointLights[3].constant", 1.0f);
        shader.set_float("pointLights[3].linear", 0.09f);
        shader.set_float("pointLights[3].quadratic", 0.032f);
        // spotLight
        shader.set_vec3("spotLight.position", camera.Position);
        shader.set_vec3("spotLight.direction", camera.Front);
        shader.set_vec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        shader.set_vec3("spotLight.diffuse", 0.8f, 0.8f, 0.0f);
        shader.set_vec3("spotLight.specular", 0.8f, 0.8f, 0.0f);
        shader.set_float("spotLight.constant", 1.0f);
        shader.set_float("spotLight.linear", 0.09f);
        shader.set_float("spotLight.quadratic", 0.032f);
        shader.set_float("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        shader.set_float("spotLight.outerCutOff", glm::cos(glm::radians(13.0f)));

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.f / 600.f, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.set_mat4("projection", projection);
        shader.set_mat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        shader.set_mat4("model", model);

        glActiveTexture(GL_TEXTURE0);
        texture1.bind();
        cube.renderCube(shader);
        glActiveTexture(GL_TEXTURE1);
        texture2.bind();
        cube.renderCube(shader);

        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

            shader.set_mat4("model", model);

            cube.renderCube(shader);
        }



        //////

        lightShader.use();
        lightShader.set_mat4("projection", projection);
        lightShader.set_mat4("view", view);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            lightShader.set_mat4("model", model);
            lightShader.set_vec3("LightColor", pointLightColors[i]);
            light.renderCube(lightShader);
        }
        //////

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
