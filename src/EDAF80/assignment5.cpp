#include "assignment5.hpp"

#include "config.hpp"
#include "core/Bonobo.h"
#include "core/FPSCamera.h"
#include "core/helpers.hpp"
#include "core/ShaderProgramManager.hpp"
#include "core/node.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <tinyfiledialogs.h>

#include <clocale>
#include <stdexcept>

edaf80::Assignment5::Assignment5(WindowManager& windowManager) :
	mCamera(0.5f * glm::half_pi<float>(),
	        static_cast<float>(config::resolution_x) / static_cast<float>(config::resolution_y),
	        0.01f, 1000.0f),
	inputHandler(), mWindowManager(windowManager), window(nullptr)
{
	WindowManager::WindowDatum window_datum{inputHandler, mCamera, config::resolution_x, config::resolution_y, 0, 0, 0, 0};

	window = mWindowManager.CreateGLFWWindow("EDAF80: Assignment 5", window_datum, config::msaa_rate);
	if (window == nullptr) {
		throw std::runtime_error("Failed to get a window: aborting!");
	}

	bonobo::init();
}

edaf80::Assignment5::~Assignment5()
{
	bonobo::deinit();
}

void
edaf80::Assignment5::run()
{
	// Set up the camera
	mCamera.mWorld.SetTranslate(glm::vec3(0.0f, 0.0f, 6.0f));
	mCamera.mMouseSensitivity = glm::vec2(0.003f);
	mCamera.mMovementSpeed = glm::vec3(3.0f); // 3 m/s => 10.8 km/h

	// Create the shader programs
	ShaderProgramManager program_manager;
	GLuint fallback_shader = 0u;
	program_manager.CreateAndRegisterProgram("Fallback",
	                                         { { ShaderType::vertex, "common/fallback.vert" },
	                                           { ShaderType::fragment, "common/fallback.frag" } },
	                                         fallback_shader);
	if (fallback_shader == 0u) {
		LogError("Failed to load fallback shader");
		return;
	}
    GLuint water_shader = 0u;
    program_manager.CreateAndRegisterProgram("Water",
                                             { { ShaderType::vertex, "EDAF80/water.vert" },
                                               { ShaderType::fragment, "EDAF80/water.frag" } },
                                             water_shader);
    if (water_shader == 0u) {
        LogError("Failed to load water shader");
        return;
    }
    GLuint diffuse_shader = 0u;
    program_manager.CreateAndRegisterProgram("Diffuse",
                                             { { ShaderType::vertex, "EDAF80/diffuse.vert" },
                                               { ShaderType::fragment, "EDAF80/diffuse.frag" } },
                                             diffuse_shader);
    if (diffuse_shader == 0u) {
        LogError("Failed to load diffuse shader");
        return;
    }
    GLuint phong_shader = 0u;
    program_manager.CreateAndRegisterProgram("Phong",
                                             { { ShaderType::vertex, "EDAF80/phong.vert" },
                                               { ShaderType::fragment, "EDAF80/phong.frag" } },
                                             phong_shader);
    if (phong_shader == 0u) {
        LogError("Failed to load phong shader");
        return;
    }
    GLuint skybox_shader = 0u;
    program_manager.CreateAndRegisterProgram("Skybox",
                                             { { ShaderType::vertex, "EDAF80/skybox.vert" },
                                               { ShaderType::fragment, "EDAF80/skybox.frag" } },
                                             skybox_shader);
    if (skybox_shader == 0u) {
        LogError("Failed to load skybox shader");
        return;
    }
    
    auto light_position = glm::vec3(-16.0f, 4.0f, 16.0f);
    bool shader_reload_failed = false;
    auto camera_position = mCamera.mWorld.GetTranslation();
    float elapsed_time_s = 0.0f;
    
    auto const sand_uniforms = [&light_position,&camera_position](GLuint program){
        glUniform1i(glGetUniformLocation(program, "use_normal_mapping"), 1);
        glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
        glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));
        glUniform3fv(glGetUniformLocation(program, "ambient"), 1, glm::value_ptr(glm::vec3(0.1f, 0.1f, 0.1f)));
        glUniform3fv(glGetUniformLocation(program, "diffuse"), 1, glm::value_ptr(glm::vec3(0.7f, 0.2f, 0.4f)));
        glUniform3fv(glGetUniformLocation(program, "specular"), 1, glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.4f)));
        glUniform1f(glGetUniformLocation(program, "shininess"), 1.0f);
    };
    
    auto const ocean_uniforms = [&light_position,&camera_position,&elapsed_time_s](GLuint program) {
        glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
        glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));
        glUniform1f(glGetUniformLocation(program, "t"), elapsed_time_s);
    };
    auto const skybox_uniforms = [&light_position,&camera_position,&elapsed_time_s](GLuint program) {
        glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
        glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));
        glUniform1f(glGetUniformLocation(program, "t"), elapsed_time_s);
    };
    
    GLuint const skybox_texture = bonobo::loadTextureCubeMap(config::game_res_path("cubemaps/Ocean/posx.jpg"),
                                                             config::game_res_path("cubemaps/Ocean/negx.jpg"),
                                                             config::game_res_path("cubemaps/Ocean/posy.jpg"),
                                                             config::game_res_path("cubemaps/Ocean/negy.jpg"),
                                                             config::game_res_path("cubemaps/Ocean/posz.jpg"),
                                                             config::game_res_path("cubemaps/Ocean/negz.jpg"), true);
    GLuint const waterbox_texture = bonobo::loadTextureCubeMap(config::game_res_path("cubemaps/Underwater/posx.jpg"),
                                                             config::game_res_path("cubemaps/Underwater/negx.jpg"),
                                                             config::game_res_path("cubemaps/Underwater/posy.jpg"),
                                                             config::game_res_path("cubemaps/Underwater/negy.jpg"),
                                                             config::game_res_path("cubemaps/Underwater/posz.jpg"),
                                                             config::game_res_path("cubemaps/Underwater/negz.jpg"), true);
    
    Node skybox;
    skybox.set_geometry(parametric_shapes::createSphere(500.0f, 100u, 100u));
    skybox.add_texture("skybox_texture", skybox_texture, GL_TEXTURE_CUBE_MAP);
    skybox.set_program(&skybox_shader, skybox_uniforms);
    
    Node waterbox;
    waterbox.set_geometry(parametric_shapes::createSphere(500.0f, 100u, 100u));
    waterbox.add_texture("waterbox_texture", waterbox_texture, GL_TEXTURE_CUBE_MAP);
    waterbox.set_program(&skybox_shader, skybox_uniforms);
    
    Node ocean;
    ocean.set_geometry(parametric_shapes::createQuad(100.0f, 100.0f, 1000u, 1000u));
    ocean.add_texture("cube_map", skybox_texture, GL_TEXTURE_CUBE_MAP);
    ocean.add_texture("normal_map", bonobo::loadTexture2D(config::resources_path("textures/waves.png")), GL_TEXTURE_2D);
    ocean.get_transform().SetTranslate(glm::vec3(-50.0f,-5.0f,-50.0f));
    ocean.set_program(&water_shader, ocean_uniforms);
    
    Node sand;
    sand.set_geometry(parametric_shapes::createQuad(50.0f, 50.0f, 100u, 100u));
    sand.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("textures/floor_diffuse.png")), GL_TEXTURE_2D);
    sand.add_texture("normal_map", bonobo::loadTexture2D(config::game_res_path("textures/floor_normal.png")), GL_TEXTURE_2D);
    sand.get_transform().SetTranslate(glm::vec3(-50.0f,-50.0f,-50.0f));
    sand.set_program(&phong_shader, sand_uniforms);
    
    

    std::vector<Node> objects;
    objects.push_back(sand);
    objects.push_back(ocean);
    objects.push_back(skybox);
    objects.push_back(waterbox);
    
	glClearDepthf(1.0f);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);

    bonafide b(objects, &mCamera);
    auto lastTime = std::chrono::high_resolution_clock::now();

	while (!glfwWindowShouldClose(window)) {
        auto const nowTime = std::chrono::high_resolution_clock::now();
		auto const deltaTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(nowTime - lastTime);
		lastTime = nowTime;
        
        /*if (!pause_animation) {
            elapsed_time_s += std::chrono::duration<float>(deltaTimeUs).count();
        }*/
		//inputHandler.SetUICapture(io.WantCaptureMouse, io.WantCaptureKeyboard);

        
        
		glfwPollEvents();
		inputHandler.Advance();
		mCamera.Update(deltaTimeUs, inputHandler);

		if (inputHandler.GetKeycodeState(GLFW_KEY_W) & JUST_PRESSED)
            b.keydown.keydown_W = true;
        if (inputHandler.GetKeycodeState(GLFW_KEY_W) & JUST_RELEASED)
            b.keydown.keydown_W = false;
        if (inputHandler.GetKeycodeState(GLFW_KEY_A) & JUST_PRESSED)
            b.keydown.keydown_A = true;
        if (inputHandler.GetKeycodeState(GLFW_KEY_A) & JUST_RELEASED)
            b.keydown.keydown_A = false;
        if (inputHandler.GetKeycodeState(GLFW_KEY_S) & JUST_PRESSED)
            b.keydown.keydown_S = true;
        if (inputHandler.GetKeycodeState(GLFW_KEY_S) & JUST_RELEASED)
            b.keydown.keydown_S = false;
        if (inputHandler.GetKeycodeState(GLFW_KEY_D) & JUST_PRESSED)
            b.keydown.keydown_D = true;
        if (inputHandler.GetKeycodeState(GLFW_KEY_D) & JUST_RELEASED)
            b.keydown.keydown_D = false;
        if (inputHandler.GetKeycodeState(GLFW_KEY_UP) & JUST_PRESSED)
            b.keydown.keydown_UP = true;
        if (inputHandler.GetKeycodeState(GLFW_KEY_UP) & JUST_RELEASED)
            b.keydown.keydown_UP = false;
        if (inputHandler.GetKeycodeState(GLFW_KEY_DOWN) & JUST_PRESSED)
            b.keydown.keydown_DOWN = true;
        if (inputHandler.GetKeycodeState(GLFW_KEY_DOWN) & JUST_RELEASED)
            b.keydown.keydown_DOWN = false;
        if (inputHandler.GetKeycodeState(GLFW_KEY_RIGHT) & JUST_PRESSED)
            b.keydown.keydown_RIGHT = true;
        if (inputHandler.GetKeycodeState(GLFW_KEY_RIGHT) & JUST_RELEASED)
            b.keydown.keydown_RIGHT = false;
        if (inputHandler.GetKeycodeState(GLFW_KEY_LEFT) & JUST_PRESSED)
            b.keydown.keydown_LEFT = true;
        if (inputHandler.GetKeycodeState(GLFW_KEY_LEFT) & JUST_RELEASED)
            b.keydown.keydown_LEFT = false;
        if (inputHandler.GetKeycodeState(GLFW_KEY_SPACE) & JUST_PRESSED)
            b.keydown.keydown_SPACE = true;
        if (inputHandler.GetKeycodeState(GLFW_KEY_SPACE) & JUST_RELEASED)
            b.keydown.keydown_SPACE = false;
        if (inputHandler.GetKeycodeState(GLFW_KEY_ENTER) & JUST_PRESSED)
            b.keydown.keydown_ENTER = true;
		if (inputHandler.GetKeycodeState(GLFW_KEY_F11) & JUST_RELEASED)
			mWindowManager.ToggleFullscreenStatusForWindow(window);

		int framebuffer_width, framebuffer_height;
		glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
		glViewport(0, 0, framebuffer_width, framebuffer_height);

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		if (!shader_reload_failed) {

		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glfwSwapBuffers(window);
	}
}

int main()
{
	std::setlocale(LC_ALL, "");

	Bonobo framework;

	try {
		edaf80::Assignment5 assignment5(framework.GetWindowManager());
		assignment5.run();
	} catch (std::runtime_error const& e) {
		LogError(e.what());
	}
}
