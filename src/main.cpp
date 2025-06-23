/*#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main() 
{ 
    // 1. Инициализация GLFW
    if(!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 2. Настройка окна
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 3. Создание окна
    GLFWwindow* window = glfwCreateWindow(800, 600, "Lucky Billiard", NULL, NULL);
    if(!window) 
    {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // 4. Делаем окно текущим контекстом
    glfwMakeContextCurrent(window);

    // 5. Инициализация GLAD
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

     // 6. Главный цикл игры
    while(!glfwWindowShouldClose(window)) {
        // Очистка экрана
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f); // Красный
        glVertex2f(-0.5f, -0.5f);
        glColor3f(0.0f, 1.0f, 0.0f); // Зелёный
        glVertex2f(0.5f, -0.5f);
        glColor3f(0.0f, 0.0f, 1.0f); // Синий
        glVertex2f(0.0f, 0.5f);
        glEnd();

        // Обмен буферов и обработка событий
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 7. Очистка
    glfwTerminate();
    return 0;
}
    */


    //cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
    //cmake --build . --config Debug
    //.\Debug\game.exe
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

// Размеры окна
const unsigned int SCR_WIDTH = 1400;
const unsigned int SCR_HEIGHT = 1200;

// Шейдерные программы
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform vec2 offset;
    void main() {
        gl_Position = vec4(aPos.x + offset.x, aPos.y + offset.y, aPos.z, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    uniform vec3 triangleColor;
    void main() {
        FragColor = vec4(triangleColor, 1.0);
    }
)";

// Позиция треугольника
glm::vec2 trianglePos(0.0f);
float triangleSpeed = 0.005f;
glm::vec3 triangleColor(1.0f, 0.5f, 0.2f); // Оранжевый

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Управление WASD
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        trianglePos.y += triangleSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        trianglePos.y -= triangleSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        trianglePos.x -= triangleSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        trianglePos.x += triangleSpeed;


    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        triangleColor.
}

int main() {
    // Инициализация GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Настройка GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Создание окна
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "My OpenGL Game", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Инициализация GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Компиляция шейдеров
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Вершины треугольника
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // левый угол
         0.5f, -0.5f, 0.0f, // правый угол
         0.0f,  0.5f, 0.0f  // верхний угол
    };

    // Буферы
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);



    // Главный цикл
    while (!glfwWindowShouldClose(window)) {
        // Обработка ввода
        processInput(window);

        // Рендеринг
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Активация шейдера
        glUseProgram(shaderProgram);
        
        // Передача параметров в шейдер
        glUniform2f(glGetUniformLocation(shaderProgram, "offset"), trianglePos.x, trianglePos.y);
        glUniform3f(glGetUniformLocation(shaderProgram, "triangleColor"), 
                   triangleColor.r, triangleColor.g, triangleColor.b);

        // Отрисовка треугольника
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Обмен буферов
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Очистка
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}