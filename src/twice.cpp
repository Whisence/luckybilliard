
/*{
    #define GLAD_GL_IMPLEMENTATION
    //#include <glad/gl.h> ошибку выдает (-_-)
    #include <glad/glad.h>
    #define GLFW_INCLUDE_NONE
    #include <GLFW/glfw3.h>

    #include <linmath.h>
    #include <iostream>
    #include <stdlib.h>
    #include <stddef.h>
    #include <stdio.h>

    typedef struct Vertex
    {
        vec2 pos;
        vec3 col;
    } Vertex;

    static const Vertex vertices[3] =
    {
        { { -0.6f, -0.4f }, { 1.f, 0.f, 0.f } },
        { {  0.6f, -0.4f }, { 0.f, 1.f, 0.f } },
        { {   0.f,  0.6f }, { 0.f, 0.f, 1.f } }
    };

    static const char* vertex_shader_text =
    "#version 330\n"
    "uniform mat4 MVP;\n"
    "in vec3 vCol;\n"
    "in vec2 vPos;\n"
    "out vec3 color;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
    "    color = vCol;\n"
    "}\n";

    static const char* fragment_shader_text =
    "#version 330\n"
    "in vec3 color;\n"
    "out vec4 fragment;\n"
    "void main()\n"
    "{\n"
    "    fragment = vec4(color, 1.0);\n"
    "}\n";

    static void error_callback(int error, const char* description)
    {
        fprintf(stderr, "Error: %s\n", description);
    }

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    int main(void)
    {
        glfwSetErrorCallback(error_callback);

        if (!glfwInit())
            exit(EXIT_FAILURE);
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL Triangle", NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        glfwSetKeyCallback(window, key_callback);
    
        glfwMakeContextCurrent(window);
        gladLoadGL(glfwGetProcAddress);
        glfwSwapInterval(1);

        // NOTE: OpenGL error checks have been omitted for brevity

        GLuint vertex_buffer;
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
        glCompileShader(vertex_shader);

        const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
        glCompileShader(fragment_shader);

        const GLuint program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);

        const GLint mvp_location = glGetUniformLocation(program, "MVP");
        const GLint vpos_location = glGetAttribLocation(program, "vPos");
        const GLint vcol_location = glGetAttribLocation(program, "vCol");
        GLuint vertex_array;
        glGenVertexArrays(1, &vertex_array);
        glBindVertexArray(vertex_array);
        glEnableVertexAttribArray(vpos_location);
        glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                            sizeof(Vertex), (void*) offsetof(Vertex, pos));
        glEnableVertexAttribArray(vcol_location);
        glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                            sizeof(Vertex), (void*) offsetof(Vertex, col));
        while (!glfwWindowShouldClose(window))
        {
            int width, height;
            glfwGetFrameBufferSize(window, &width, &height);
            const float ratio = width / (float) height;

            glViewport(0, 0, width, height);
            glClear(GL_COLOR_BUFFER_BIT);

            mat4x4 m, p, mvp;
            mat4x4_identity(m);
            mat4x4_rotate_Z(m, m, (float) glfwGetTime());
            mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
            mat4x4_mul(mvp, p, m);

            glUseProgram(program);
            glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp);
            glBindVertexArray(vertex_array);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        glfwDestroyWindow(window);

        glfwTerminate();
        exit(EXIT_SUCCESS);
    }
    }*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cstdlib>
#include <cmath>

#include <vector>
//cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake     cmakelists типо
//cmake --build . --config Debug                                                компиляция
//.\Debug\game.exe                                                              запуск игры

// Структура вершины
struct Vertex {
    glm::vec2 pos;
    glm::vec3 col;
};

float starX = 0.5f;
float starY = 0.5f;

float ballX = -0.5f;
float ballY = -0.5f;

struct Vertex1 {  
    float x, y;   // Позиция (X, Y)  
    float r, g, b; // Цвет (R, G, B)  
};

// Вершины треугольника
static const Vertex vertices[3] = {
    { { -0.6f, -0.4f }, { 1.f, 1.f, 0.f } },
    { {  0.6f, -0.4f }, { 0.f, 1.f, 1.f } },
    { {   0.f,  0.6f }, { 1.f, 0.f, 1.f } }
};

// Вершинный шейдер
static const char* vertex_shader_text =
R"(#version 330 core
uniform mat4 MVP;
in vec2 vPos;
in vec3 vCol;
out vec3 color;
void main() {
    gl_Position = MVP * vec4(vPos, 0.0, 1.0);
    color = vCol;
})";

// Фрагментный шейдер
static const char* fragment_shader_text =
R"(#version 330 core
in vec3 color;
out vec4 FragColor;
void main() {
    FragColor = vec4(color, 1.0);
})";


// Вершины квадрата (X, Y, R, G, B)  
std::vector<Vertex1> square = {  
    { -0.5f, -0.5f,  1.0f, 0.0f, 0.0f }, // Левый нижний (красный)  
    {  0.5f, -0.5f,  0.0f, 1.0f, 0.0f }, // Правый нижний (зелёный)  
    {  0.5f,  0.5f,  0.0f, 0.0f, 1.0f }, // Правый верхний (синий)  
    { -0.5f,  0.5f,  1.0f, 1.0f, 0.0f }  // Левый верхний (жёлтый)  
};  


std::vector<Vertex1> createStar(float centerX, float centerY, 
                              float outerRadius, float innerRadius, 
                              int rays, float r, float g, float b) {
    std::vector<Vertex1> star;
    for (int i = 1; i <= rays*2; i++) {
        float angle = 3.14159f * 2 * i / (rays*2);
        float radius = (i % 2 == 0) ? outerRadius : innerRadius;
        star.push_back({
            centerX + radius * cos(angle), // X
            centerY + radius * sin(angle), // Y
            r, g, b                        // цвет
        });
    }
    return star;
}


std::vector<Vertex1> createCircle(float centerX, float centerY, float radius, int segments,
                                 float r, float g, float b)
{
    std::vector<Vertex1> ball1;
    ball1.push_back({centerX, centerY, r , g, b});
    for (int i = 0; i <=segments; ++i)
    {
        float angle = 2.0f * 3.14159f * i / segments;

        ball1.push_back({
            centerX + radius * cos(angle),
            centerY + radius * sin(angle),
            r, g, b});

    }

    return ball1;


}

std::vector<Vertex1> star = createStar(0.5f, 0.5f,  // позиция центра
                                         0.1f, 0.05f,  // внешний/внутренний радиус
                                         5,            // лучей
                                         1.0f, 0.0f, 0.5f); // фиолет

std::vector<Vertex1> ball1=createCircle(-0.5f, -0.5f, 0.1f, 30, 0.5f, 0.1f, 0.5f);

// Обработчик ошибок GLFW
static void error_callback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}
// Обработчик клавиатуры
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    float speed = 0.001f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        if (starX>-1)  starX -= speed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        if (starX<1)  starX += speed;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        if  (starY<1) starY += speed;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        if (starY>-1)  starY -= speed;
}

// Проверка компиляции шейдера
void check_shader_compile(GLuint shader) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
    }
}

int main() {
//начало main----------------------------------------------------------------------------------------------
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return EXIT_FAILURE;
    }

// Настройка GLFW для OpenGL 3.3 Core Profile---------------------------------------------------------------
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(640, 480, "Lucky billiard", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);


// Инициализация GLAD--------------------------------------------------------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl; 
        return EXIT_FAILURE;
    }

// Создание и компиляция шейдеров------------------------------------------------------------------------
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);
    check_shader_compile(vertex_shader);

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
    check_shader_compile(fragment_shader);

// Создание шейдерной программы-------------------------------------------------------------------------
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    

// Удаляем шейдеры после линковки-------------------------------------------------------------------------
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

// Создание VBO и VAO ------------------------------------------------------------------------------------
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

// Устанавливаем указатели на атрибуты треугольника-------------------------------------------------------------------------
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, col));
    glEnableVertexAttribArray(1);
    

// 4. Говорим OpenGL, как читать вершины:  
    // - Позиция (первые 2 числа — X, Y)  
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);  
    glEnableVertexAttribArray(0);  

    // - Цвет (последние 3 числа — R, G, B)  
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));  
    glEnableVertexAttribArray(1); 



//создание квадрата-----------------------------------------

    GLuint squareVAO, squareVBO;
    glGenVertexArrays(1, &squareVAO);
    glGenBuffers(1, &squareVBO);

    glBindVertexArray(squareVAO);
    glBindBuffer(GL_ARRAY_BUFFER, squareVBO);
    glBufferData(GL_ARRAY_BUFFER, square.size() * sizeof(Vertex1), square.data(), GL_STATIC_DRAW);

    //Настройка атрибутов (как читать буфер)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); //позиция
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2*sizeof(float))); //цвет
    glEnableVertexAttribArray(1);

///////////////////////////
    GLuint starVAO, starVBO;
    glGenVertexArrays(1,&starVAO);
    glGenBuffers(1, &starVBO);

    glBindVertexArray(starVAO);
    glBindBuffer(GL_ARRAY_BUFFER, starVBO);
    glBufferData(GL_ARRAY_BUFFER, star.size() * sizeof(Vertex1), star.data(), GL_DYNAMIC_DRAW);


    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
/////////////////////////////////////////////////////////////////////
    GLuint ballVAO, ballVBO;
    glGenVertexArrays(1,&ballVAO);
    glGenBuffers(1, &ballVBO);

    glBindVertexArray(ballVAO);
    glBindBuffer(GL_ARRAY_BUFFER, ballVBO);
    glBufferData(GL_ARRAY_BUFFER, ball1.size() * sizeof(Vertex1), ball1.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);


     














    // Получаем location uniform-переменной--------------------------------------------------------------------------
    GLint mvp_location = glGetUniformLocation(program, "MVP");













// Главный цикл рендеринга---------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        const float ratio = width / (float)height;
        glViewport(0, 0, width, height);
        
        glClear(GL_COLOR_BUFFER_BIT);
        
         

// Создаем матрицы с помощью GLM

        glm::mat4 projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, -1.0f, 1.0f);
        
        glm::mat4 TRImodel = glm::mat4(1.0f);
        TRImodel = glm::rotate(TRImodel, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 mvp = projection * TRImodel;

        //КВАДРАТ-----------------------
        glm::mat4 squareModel = glm::mat4(1.0f);
        glm::mat4 squareMVP = projection * squareModel;
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(squareMVP));
        glBindVertexArray(squareVAO); // Выбираем наш квадрат  
        glDrawArrays(GL_TRIANGLE_FAN, 0, square.size()); // Рисуем как "веер" из треугольников
        

    // Рендерим треугольник
        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        
    





    //ЗВЕЗДА-----------------------------------
        glm::mat4 starModel = glm::mat4(1.0f);
        glm::mat4 starMVP = projection * starModel;
        glUniformMatrix4fv(mvp_location, 1,GL_FALSE, glm::value_ptr(starMVP));
        star = createStar(starX, starY, 0.1f, 0.05f, 5, 1.0f, 0.0f, 0.5f);
        glBindBuffer(GL_ARRAY_BUFFER, starVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, star.size() * sizeof(Vertex1), star.data());

        
        glBindVertexArray(starVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, star.size());
         
        

    //МЯЧ-------------------------------------
        glm::mat4 ballModel = glm::mat4(1.0f);
        glm::mat4 ballMVP = projection * ballModel;
        glUniformMatrix4fv(mvp_location, 1,GL_FALSE, glm::value_ptr(ballMVP));
        ball1 = createCircle(ballX, ballY, 0.1f, 30, 0.5f, 0.1f, 0.5f);
        glBindBuffer(GL_ARRAY_BUFFER, starVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, ball1.size() * sizeof(Vertex1), ball1.data());

        
        glBindVertexArray(ballVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, ball1.size());






    //свап буферов-----------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

// Очистка ресурсов
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &squareVAO);
    glDeleteBuffers(1, &squareVBO);
    glDeleteVertexArrays(1, &starVAO);
    glDeleteBuffers(1, &starVBO);
    
    glDeleteProgram(program);
    glDeleteProgram(program);

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}