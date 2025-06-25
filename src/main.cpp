#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <cmath>

const unsigned int SCR_WIDTH = 1400;
const unsigned int SCR_HEIGHT = 1200;

// Шейдерные программы
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 model;
    uniform mat4 projection;
    void main() {
        gl_Position = projection * model * vec4(aPos, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    uniform vec3 objectColor;
    void main() {
        FragColor = vec4(objectColor, 1.0);
    }
)";

// Константа PI
const float PI = 3.14159265358979323846f;

struct Ball {
    glm::vec2 position;
    glm::vec2 velocity;
    float radius;
    glm::vec3 color;
    bool isMoving;
};

struct Cue {
    glm::vec2 position;
    float angle;
    float power;
    bool isAiming;
};

std::vector<Ball> balls;
Cue cue;
Ball* selectedBall = nullptr;
float tableWidth = 1.8f;
float tableHeight = 0.9f;
float friction = 0.98f;

void initGame() {
    balls.clear();
    
    // Белый шар (биток)
    balls.push_back({glm::vec2(-0.6f, 0.0f), glm::vec2(0.0f), 0.05f, glm::vec3(1.0f), false});
    
    // Цветные шары
    float startX = 0.6f;
    float startY = 0.0f;
    float spacing = 0.06f;
    
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j <= i; ++j) {
            float x = startX + i * spacing * 0.866f;
            float y = startY + (j - i/2.0f) * spacing;
            glm::vec3 color = glm::vec3(
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX
            );
            balls.push_back({glm::vec2(x, y), glm::vec2(0.0f), 0.05f, color, false});
        }
    }
    
    cue.position = balls[0].position;
    cue.angle = 0.0f;
    cue.power = 0.0f;
    cue.isAiming = false;
    
    selectedBall = &balls[0];
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    bool anyBallMoving = false;
    for (auto& ball : balls) {
        if (ball.isMoving) {
            anyBallMoving = true;
            break;
        }
    }

    if (!anyBallMoving) {
        selectedBall = &balls[0];
        
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cue.angle += 0.05f;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cue.angle -= 0.05f;
            
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            cue.isAiming = true;
            cue.power = glm::min(cue.power + 0.001f, 0.05f);
        }
        
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && cue.isAiming) {
            selectedBall->velocity.x = -cos(cue.angle) * cue.power * 10.0f;
            selectedBall->velocity.y = -sin(cue.angle) * cue.power * 10.0f;
            selectedBall->isMoving = true;
            cue.power = 0.0f;
            cue.isAiming = false;
        }
    }
}

void updatePhysics() {
    for (auto& ball : balls) {
        if (!ball.isMoving) continue;
        
        ball.velocity *= friction;
        ball.position += ball.velocity;
        
        if (glm::length(ball.velocity) < 0.0001f) {
            ball.velocity = glm::vec2(0.0f);
            ball.isMoving = false;
        }
        
        if (ball.position.x - ball.radius < -tableWidth/2) {
            ball.position.x = -tableWidth/2 + ball.radius;
            ball.velocity.x *= -0.9f;
        }
        if (ball.position.x + ball.radius > tableWidth/2) {
            ball.position.x = tableWidth/2 - ball.radius;
            ball.velocity.x *= -0.9f;
        }
        if (ball.position.y - ball.radius < -tableHeight/2) {
            ball.position.y = -tableHeight/2 + ball.radius;
            ball.velocity.y *= -0.9f;
        }
        if (ball.position.y + ball.radius > tableHeight/2) {
            ball.position.y = tableHeight/2 - ball.radius;
            ball.velocity.y *= -0.9f;
        }
    }
    
    for (size_t i = 0; i < balls.size(); ++i) {
        for (size_t j = i + 1; j < balls.size(); ++j) {
            glm::vec2 delta = balls[j].position - balls[i].position;
            float distance = glm::length(delta);
            float minDistance = balls[i].radius + balls[j].radius;
            
            if (distance < minDistance) {
                glm::vec2 normal = delta / distance;
                glm::vec2 relativeVelocity = balls[j].velocity - balls[i].velocity;
                float velocityAlongNormal = glm::dot(relativeVelocity, normal);
                
                if (velocityAlongNormal > 0) continue;
                
                float restitution = 0.9f;
                float impulse = -(1.0f + restitution) * velocityAlongNormal;
                impulse /= 2.0f;
                
                glm::vec2 impulseVec = impulse * normal;
                balls[i].velocity -= impulseVec;
                balls[j].velocity += impulseVec;
                
                float overlap = minDistance - distance;
                glm::vec2 correction = overlap * 0.5f * normal;
                balls[i].position -= correction;
                balls[j].position += correction;
                
                balls[i].isMoving = true;
                balls[j].isMoving = true;
            }
        }
    }
    
    if (!balls[0].isMoving) {
        cue.position = balls[0].position;
    }
}

void drawCircle(unsigned int shaderProgram, unsigned int VAO, float x, float y, float radius, glm::vec3 color) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(radius, radius, 1.0f));
    
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
    glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), color.r, color.g, color.b);
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 32);
}

void drawRectangle(unsigned int shaderProgram, unsigned int VAO, float x, float y, float width, float height, glm::vec3 color) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));
    
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
    glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), color.r, color.g, color.b);
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void drawLine(unsigned int shaderProgram, unsigned int VAO, float x1, float y1, float x2, float y2, float width, glm::vec3 color) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float length = sqrt(dx*dx + dy*dy);
    float angle = atan2(dy, dx);
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3((x1+x2)/2, (y1+y2)/2, 0.0f));
    model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(length/2, width/2, 1.0f));
    
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
    glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), color.r, color.g, color.b);
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

int main() {
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lucky Billiard", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

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

    // Вершины для круга (32 сегмента)
    std::vector<float> circleVertices;
    circleVertices.push_back(0.0f);
    circleVertices.push_back(0.0f);
    circleVertices.push_back(0.0f);
    
    for (int i = 0; i <= 32; ++i) {
        float angle = 2.0f * PI * i / 32;
        circleVertices.push_back(cos(angle));
        circleVertices.push_back(sin(angle));
        circleVertices.push_back(0.0f);
    }

    float rectangleVertices[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f
    };

    glm::mat4 projection = glm::ortho(
        -static_cast<float>(SCR_WIDTH)/SCR_HEIGHT, 
        static_cast<float>(SCR_WIDTH)/SCR_HEIGHT, 
        -1.0f, 1.0f, -1.0f, 1.0f);

    unsigned int VAO[2], VBO[2];
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);

    // Круг
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(float), circleVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Прямоугольник
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    initGame();

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        updatePhysics();

        glClearColor(0.1f, 0.3f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

        // Стол
        drawRectangle(shaderProgram, VAO[1], 0.0f, 0.0f, tableWidth, tableHeight, glm::vec3(0.0f, 0.3f, 0.0f));
        
        // Борта
        float borderWidth = 0.05f;
        drawRectangle(shaderProgram, VAO[1], 0.0f, tableHeight/2 + borderWidth/2, tableWidth + 2*borderWidth, borderWidth, glm::vec3(0.5f, 0.3f, 0.1f));
        drawRectangle(shaderProgram, VAO[1], 0.0f, -tableHeight/2 - borderWidth/2, tableWidth + 2*borderWidth, borderWidth, glm::vec3(0.5f, 0.3f, 0.1f));
        drawRectangle(shaderProgram, VAO[1], tableWidth/2 + borderWidth/2, 0.0f, borderWidth, tableHeight, glm::vec3(0.5f, 0.3f, 0.1f));
        drawRectangle(shaderProgram, VAO[1], -tableWidth/2 - borderWidth/2, 0.0f, borderWidth, tableHeight, glm::vec3(0.5f, 0.3f, 0.1f));

        // Шары
        for (const auto& ball : balls) {
            drawCircle(shaderProgram, VAO[0], ball.position.x, ball.position.y, ball.radius, ball.color);
        }

        // Кий
        bool anyBallMoving = false;
        for (const auto& ball : balls) {
            if (ball.isMoving) {
                anyBallMoving = true;
                break;
            }
        }

        if (!anyBallMoving && selectedBall) {
            float cueLength = 0.5f;
            float cueX = selectedBall->position.x - cos(cue.angle) * (selectedBall->radius + cue.power * 10.0f + cueLength/2);
            float cueY = selectedBall->position.y - sin(cue.angle) * (selectedBall->radius + cue.power * 10.0f + cueLength/2);
            
            drawLine(shaderProgram, VAO[1], 
                   cueX, cueY,
                   cueX - cos(cue.angle) * cueLength, 
                   cueY - sin(cue.angle) * cueLength,
                   0.02f, glm::vec3(0.8f, 0.8f, 0.8f));
            
            if (cue.isAiming) {
                drawCircle(shaderProgram, VAO[0],
                          selectedBall->position.x - cos(cue.angle) * (selectedBall->radius + 0.02f),
                          selectedBall->position.y - sin(cue.angle) * (selectedBall->radius + 0.02f),
                          cue.power * 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(2, VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}