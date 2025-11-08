/*#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ray.h"
#include "body.h"
#include "shader.h"
#include "camera.h"
#include "maths.h"

class Engine
{
    public:
        Body bodies[3];

        Camera cam = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        int raysPerPixel = 1;

        Engine() 
        {
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            window = glfwCreateWindow(1440, 900, "OpenGL Window", NULL, NULL);

            if (window == NULL) 
            {
                glfwTerminate();
                return;
            }

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            glfwMakeContextCurrent(window);

            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
            {
                glfwTerminate();
                return;
            }

            shaderProgram = new Shader("../src/vertex.glsl", "../src/frag.glsl");

            float quadVertices[] {
                // positions    // tex coords
                0.0f, 1.0f,     0.0f, 1.0f,
                1.0f, 0.0f,     1.0f, 0.0f,
                0.0f, 0.0f,     0.0f, 0.0f,

                0.0f, 1.0f,     0.0f, 1.0f,
                1.0f, 1.0f,     1.0f, 1.0f,
                1.0f, 0.0f,     1.0f, 0.0f
            };

            height = static_cast<int>(width / aspectRatio);

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

            glBindVertexArray(0);

            pixels = std::vector<unsigned char>(width * height * 3, 0);

            glGenTextures(1, &pixelTex);
            glBindTexture(GL_TEXTURE_2D, pixelTex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            bodies[0] = {glm::vec3(0.0f, 0.0f, -10.0f), 2.0f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 0.1f, 0.0f, false};
            bodies[1] = {glm::vec3(-8.0f, 0.0f, -10.0f), 2.0f, glm::vec4(10.0f, 10.0f, 10.0f, 1.0f), 0.0f, 0.0f, true};
            bodies[2] = {glm::vec3(0.0f, 10.0f, -10.0f), 5.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 0.4f, 1.0f, false};
        }

        ~Engine() 
        {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            if (window)
            {
                glfwDestroyWindow(window);
            }
            glfwTerminate();
        }

        void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
        {
            unsigned char color[3] = {r, g, b};
            glBindTexture(GL_TEXTURE_2D, pixelTex);
            glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, color);
        }

        void run()
        {
            while (!glfwWindowShouldClose(window))
            {
                float currentFrame = static_cast<float>(glfwGetTime());
                deltaTime = currentFrame - lastFrame;
                lastFrame = currentFrame;

                const char* fps = ("FPS: " + std::to_string(1.0f/deltaTime)).c_str();

                glfwSetWindowTitle(window, fps);

                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                processInput(window);
                cam.processInput(window, deltaTime);

                float fov = 90.0f;
                float fov_scale = 1.0f;

                shaderProgram->use();
                shaderProgram->setFloat("aspectRatio", aspectRatio);

                for (int x = 0; x < width; x++)
                {
                    for (int y = 0; y < height; y++)
                    {
                        glm::vec3 finalColor = glm::vec3(0.0f);
                        for(int r = 0; r < raysPerPixel; r++)
                        {
                            float u_off = (2.0f * x) / (float)width - 1.0f;
                            float v_off = 1.0f - (2.0f * y) / (float)height;

                            u_off *= aspectRatio * fov_scale;
                            v_off *= fov_scale;

                            Ray ray;
                            ray.origin = cam.position;
                            ray.direction = cam.forward + (u_off * cam.right) + (v_off * cam.up);

                            finalColor = ray.direction;
                        }
                        
                        finalColor /= (float)raysPerPixel;

                        int i = (y * width + x) * 3;

                        pixels[i + 0] = (unsigned char)(glm::clamp(finalColor.r, 0.0f, 1.0f) * 255);
                        pixels[i + 1] = (unsigned char)(glm::clamp(finalColor.g, 0.0f, 1.0f) * 255);
                        pixels[i + 2] = (unsigned char)(glm::clamp(finalColor.b, 0.0f, 1.0f) * 255);
                    }
                }

                glBindTexture(GL_TEXTURE_2D, pixelTex);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, pixelTex);
                shaderProgram->setInt("pixelTex", 0);

                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);

                glfwSwapBuffers(window);
                glfwPollEvents();
            }
        }

    private:
        int width = 1440;
        int height;

        const int screenWidth = 1440;
        const int screenHeight = 900;

        float aspectRatio = (float)screenWidth / (float)screenHeight;

        float lastFrame;
        float deltaTime;

        std::vector<unsigned char> pixels;

        unsigned int VAO, VBO, pixelTex;
        GLFWwindow* window;
        Shader* shaderProgram;

        void processInput(GLFWwindow * window)
        {
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);
        }
};

int main() 
{
    Engine engine;
    engine.run();
}*/
