#include "glad.h"
#include "glfw3.h"
#include "iostream"
#include "sstream"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Light.h"

float SCREEN_WIDTH = 1280.0f;
float SCREEN_HRIGHT = 720.0f;

float delta_time = 0.0f;
float last_frame = 0.0f;

GLfloat near_plane = 1.0f, far_plane = 20.0f;
float last_x = SCREEN_WIDTH / 2.0f, last_y = SCREEN_HRIGHT / 2.0f;

const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

Camera camera = Camera(glm::vec3(5.0f, 5.0f, -4.0f),glm::vec3(0,1,0),135,-30);

void DrawScene(Shader shader, float current_frame);
void ProcessInput(GLFWwindow *window)
{
    float speed = camera.mouse_speed * delta_time;
    // �ƶ�
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.Move(camera.forward * speed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.Move(-camera.forward * speed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.Move(-camera.right * speed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.Move(camera.right * speed);
}

void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE: 
			glfwSetWindowShouldClose(window, true); 
			break;
        default:
            break;
        }
    }
}

void MouseCallback(GLFWwindow* window, double x_pos, double y_pos)
{
    float x_offset = x_pos - last_x;
    float y_offset = y_pos - last_y;
    last_x = x_pos;
    last_y = y_pos;

    float sensitivity = 0.1;
    x_offset *= sensitivity;
    y_offset *= sensitivity;

    camera.Rotate(glm::vec3(x_offset, y_offset, 0));
}

void ScrollCallback(GLFWwindow* window, double x_offset, double y_offset)
{
    camera.Zoom(y_offset);
}

int main()
{

    // ---------------------gl��ʼ��--------------------------
    // glfw��ʼ�������õ�GL�汾Ϊ3.3Core��
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // ����GL����
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HRIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // ָ��������Ϊ��ǰ����
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetKeyCallback(window, KeyCallback);

    // ��ʼ��glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed!" << std::endl;
        return -1;
    }

	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // ������Ȳ���
    glEnable(GL_DEPTH_TEST);
    camera.flip_y = true;
	Shader cube_shader = Shader("res/shader/DrawCube.vs", "res/shader/DrawCube.fs");
	Shader shadowMap_shader = Shader("res/shader/ShadowMap.vs", "res/shader/ShadowMap.fs");

    // ��������
    float cubeVertices[] = {
        // positions           // normals         // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
                                     
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
                                           
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
                                            
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
                                          
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f
    };

    DirectLight dirLight = DirectLight(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.2f), glm::vec3(0.5f), glm::vec3(0.8f));

    // ---------------------�󶨶����������----------------------
    unsigned int vbo, cube_vao;
    glGenVertexArrays(1, &cube_vao);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindVertexArray(cube_vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    cube_shader.Use();
    unsigned int diffuse_map = Texture::LoadTextureFromFile("res/texture/container2.png");
    unsigned int specular_map = Texture::LoadTextureFromFile("res/texture/container2_specular.png");
    cube_shader.SetInt("material.diffuse", 0);
    cube_shader.SetInt("material.specular", 1);
	cube_shader.SetInt("depthMap", 2);

	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(glm::vec3(6, 6, 6), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 lightPV = lightProjection * lightView;

	glEnable(GL_CULL_FACE);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    last_frame = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
        ProcessInput(window);

		glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), SCREEN_WIDTH / SCREEN_HRIGHT, 0.1f, 100.0f);

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glClear(GL_DEPTH_BUFFER_BIT);
			shadowMap_shader.Use();
			shadowMap_shader.SetMat4("lightPV", lightPV);
			DrawScene(shadowMap_shader, current_frame);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HRIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cube_shader.Use();
        cube_shader.SetVec3("viewPos", camera.position);
        cube_shader.SetFloat("material.shininess", 32.0f);
		cube_shader.SetMat4("lightPV", lightPV);

        // ƽ�й��Դ
        dirLight.Draw(cube_shader, "dirLight");
		glm::mat4 view = camera.GetViewMatrix();
        cube_shader.SetMat4("projection", projection);
        cube_shader.SetMat4("view", view);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse_map);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specular_map);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, depthMap);
        glBindVertexArray(cube_vao);
		DrawScene(cube_shader, current_frame);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &cube_vao);
    glDeleteBuffers(1, &vbo);

    glfwTerminate();
    return 0;
}


void DrawScene(Shader shader,float current_frame)
{
	//���ذ�
	glCullFace(GL_FRONT);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0, 4.5f, 0));
	model = glm::scale(model, glm::vec3(10));
	shader.SetMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 24, 6);

	//����ֹ�ڵذ��ϵķ���
	glCullFace(GL_BACK);
	model = glm::mat4(1.0f);
	shader.SetMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//����ֹ�����ķ���
	model = glm::translate(model, glm::vec3(1.8f, 1.5f, 2.3f));
	shader.SetMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	model = glm::translate(model, glm::vec3(-2.8f, -1.0f, 1.1f));
	shader.SetMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	//����ת�ķ���
	model = glm::translate(model, glm::vec3(2.8f, 1.2f, -4.1f));
	model = glm::rotate(model, current_frame, glm::vec3(1.0f, 0.3f, 0.5f));
	shader.SetMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}