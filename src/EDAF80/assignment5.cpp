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

#define key(k) inputHandler.GetKeycodeState(k) & PRESSED

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
    GLuint sand_shader = 0u;
    program_manager.CreateAndRegisterProgram("Sand",
                                             { { ShaderType::vertex, "EDAF80/sand.vert" },
                                               { ShaderType::fragment, "EDAF80/sand.frag" } },
                                             sand_shader);
    if (sand_shader == 0u) {
        LogError("Failed to load sand shader");
        return;
    }
    GLuint diffuse_shader = 0u;
    program_manager.CreateAndRegisterProgram("Diffuse",
                                             { { ShaderType::vertex, "EDAF80/texture.vert" },
                                               { ShaderType::fragment, "EDAF80/texture.frag" } },
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
    
    bonobo::material_data water_material;
    water_material.ambient = glm::vec3(0.0f, 0.0f, 0.3f);
    water_material.diffuse = glm::vec3(0.0f, 0.0f, 0.2f);
    water_material.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    water_material.shininess = 6.0f;
    
    bonobo::material_data sand_material;
    sand_material.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
    sand_material.diffuse = glm::vec3(0.7f, 0.2f, 0.4f);
    sand_material.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    sand_material.shininess = 5.0f;
    
    bonobo::material_data submarine_material;
    submarine_material.ambient = glm::vec3(0.3f, 0.3f, 0.3f);
    submarine_material.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    submarine_material.specular = glm::vec3(0.5f, 0.5f, 0.5f);
    submarine_material.shininess = 10.0f;
    
    bonobo::material_data tuna_material;
    tuna_material.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    tuna_material.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    tuna_material.specular = glm::vec3(0.5f, 0.5f, 0.5f);
    tuna_material.shininess = 10.0f;
    
    bonobo::material_data shark_material;
    shark_material.ambient = glm::vec3(0.5f, 0.5f, 0.5f);
    shark_material.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    shark_material.specular = glm::vec3(0.5f, 0.5f, 0.5f);
    shark_material.shininess = 10.0f;
    
    bonobo::material_data rock_material;
    shark_material.ambient = glm::vec3(0.9f, 0.9f, 0.9f);
    shark_material.diffuse = glm::vec3(0.6f, 0.6f, 0.6f);
    shark_material.specular = glm::vec3(0.5f, 0.5f, 0.5f);
    shark_material.shininess = 2.0f;
    
    auto water_uniforms = [&elapsed_time_s, &light_position, &camera_position](GLuint program) {
        glUniform1f(glGetUniformLocation(program, "elapsed_time_s"), elapsed_time_s);
        glUniform1i(glGetUniformLocation(program, "use_normal_mapping"), 1);
        glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
        glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));
    };
    auto const sand_uniforms = [&light_position, &camera_position](GLuint program) {
        glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
        glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));
    };
    auto const diffuse_uniforms = [&light_position,&camera_position,&elapsed_time_s](GLuint program) {
        glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
        glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));
        glUniform1f(glGetUniformLocation(program, "t"), elapsed_time_s);
    };
    auto const phong_uniforms = [&light_position, &camera_position, &submarine_material, &elapsed_time_s](GLuint program) {
        glUniform1i(glGetUniformLocation(program, "use_normal_mapping"), 1);
        glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
        glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));
        glUniform3fv(glGetUniformLocation(program, "ambient_colour"), 1, glm::value_ptr(submarine_material.ambient));
        glUniform3fv(glGetUniformLocation(program, "diffuse_colour"), 1, glm::value_ptr(submarine_material.diffuse));
        glUniform3fv(glGetUniformLocation(program, "specular_colour"), 1, glm::value_ptr(submarine_material.specular));
        glUniform1f(glGetUniformLocation(program, "shininess"), submarine_material.shininess);
        glUniform1f(glGetUniformLocation(program, "elapsed_time_s"), elapsed_time_s);
    };
    
    
    GLuint const skybox_texture = bonobo::loadTextureCubeMap(config::game_res_path("cubemaps/Ocean/posx.png"),
                                                             config::game_res_path("cubemaps/Ocean/negx.png"),
                                                             config::game_res_path("cubemaps/Ocean/posy.png"),
                                                             config::game_res_path("cubemaps/Ocean/negy.png"),
                                                             config::game_res_path("cubemaps/Ocean/posz.png"),
                                                             config::game_res_path("cubemaps/Ocean/negz.png"), true);
    GLuint const waterbox_texture = bonobo::loadTextureCubeMap(config::game_res_path("cubemaps/Underwater/posx.jpg"),
                                                             config::game_res_path("cubemaps/Underwater/negx.jpg"),
                                                             config::game_res_path("cubemaps/Underwater/posy.jpg"),
                                                             config::game_res_path("cubemaps/Underwater/negy.jpg"),
                                                             config::game_res_path("cubemaps/Underwater/posz.jpg"),
                                                             config::game_res_path("cubemaps/Underwater/negz.jpg"), true);

    
    Node skybox;
    skybox.set_geometry(parametric_shapes::createSphere(500.0f, 100u, 100u));
    skybox.add_texture("diffuse_map", skybox_texture, GL_TEXTURE_CUBE_MAP);
    skybox.set_program(&skybox_shader, diffuse_uniforms);
    
    Node waterbox;
    waterbox.set_geometry(parametric_shapes::createSphere(500.0f, 100u, 100u));
    waterbox.add_texture("diffuse_map", waterbox_texture, GL_TEXTURE_CUBE_MAP);
    waterbox.set_program(&skybox_shader, diffuse_uniforms);
    
    Node ocean;
    ocean.set_geometry(parametric_shapes::createQuad(1000.0f, 1000.0f, 1000u, 1000u));
    ocean.add_texture("normal_map", bonobo::loadTexture2D(config::resources_path("textures/waves.png"), true), GL_TEXTURE_2D);
    ocean.add_texture("cube_map", skybox_texture, GL_TEXTURE_CUBE_MAP);
    ocean.set_material_constants(water_material);
    ocean.set_program(&water_shader, water_uniforms);
    
    Node sand;
    sand.set_geometry(parametric_shapes::createQuad(50.0f, 50.0f, 100u, 100u));
    sand.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("textures/floor_diffuse.png")), GL_TEXTURE_2D);
    sand.add_texture("normal_map", bonobo::loadTexture2D(config::game_res_path("textures/floor_normal.png")), GL_TEXTURE_2D);
    sand.add_texture("specular_map", bonobo::loadTexture2D(config::game_res_path("textures/floor_specular.png")), GL_TEXTURE_2D);
    sand.set_material_constants(sand_material);
    sand.set_program(&sand_shader, sand_uniforms);

    Node submarine_body;
    std::vector<bonobo::mesh_data> submarine_body_mesh = bonobo::loadObjects(config::game_res_path("objects/submarine.obj"));;
    submarine_body.set_geometry(submarine_body_mesh.front());
    submarine_body.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("textures/submarine_diff.png")), GL_TEXTURE_2D);
    submarine_body.add_texture("normal_map", bonobo::loadTexture2D(config::game_res_path("textures/submarine_normal.png")), GL_TEXTURE_2D);
    submarine_body.add_texture("specular_map", bonobo::loadTexture2D(config::game_res_path("textures/submarine_spec.png")), GL_TEXTURE_2D);
    submarine_body.set_material_constants(submarine_material);
    submarine_body.get_transform().SetScale(0.01f);
    submarine_body.set_program(&phong_shader, phong_uniforms);
    
    Node submarine_propeller;
    std::vector<bonobo::mesh_data> submarine_propeller_mesh = bonobo::loadObjects(config::game_res_path("objects/propeller.obj"));
    submarine_propeller.set_geometry(submarine_propeller_mesh.front());
    submarine_propeller.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("textures/propeller.jpg")), GL_TEXTURE_2D);
    submarine_propeller.get_transform().SetScale(0.13f);
    submarine_propeller.get_transform().SetRotateZ(glm::half_pi<float>());
    submarine_propeller.set_program(&diffuse_shader, diffuse_uniforms);

    Node whale_skeleton;
    std::vector<bonobo::mesh_data> whale_skeleton_mesh = bonobo::loadObjects(config::game_res_path("objects/whale-skeleton.obj"));;
    whale_skeleton.set_geometry(whale_skeleton_mesh.front());
    whale_skeleton.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("textures/bone.jpg")), GL_TEXTURE_2D);
    whale_skeleton.set_program(&diffuse_shader, diffuse_uniforms);
    
    Node dino_skeleton;
    std::vector<bonobo::mesh_data> dino_skeleton_mesh = bonobo::loadObjects(config::game_res_path("objects/dino-skeleton.obj"));;
    dino_skeleton.set_geometry(dino_skeleton_mesh.front());
    dino_skeleton.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("textures/bone.jpg")), GL_TEXTURE_2D);
    dino_skeleton.set_program(&diffuse_shader, diffuse_uniforms);
    
    Node tuna;
    std::vector<bonobo::mesh_data> tuna_mesh = bonobo::loadObjects(config::game_res_path("objects/tuna_fish.obj"));;
    tuna.set_geometry(tuna_mesh.front());
    tuna.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("textures/tuna_body_diff.png")), GL_TEXTURE_2D);
    tuna.add_texture("normal_map", bonobo::loadTexture2D(config::game_res_path("textures/tuna_body_normal.png")), GL_TEXTURE_2D);
    tuna.add_texture("specular_map", bonobo::loadTexture2D(config::game_res_path("textures/tuna_body_rough.png")), GL_TEXTURE_2D);
    tuna.set_material_constants(tuna_material);
    tuna.set_program(&phong_shader, phong_uniforms);
    
    Node shark;
    std::vector<bonobo::mesh_data> shark_mesh = bonobo::loadObjects(config::game_res_path("objects/shark.obj"));;
    shark.set_geometry(shark_mesh.front());
    shark.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("textures/shark_diff.png")), GL_TEXTURE_2D);
    shark.add_texture("normal_map", bonobo::loadTexture2D(config::game_res_path("textures/shark_normal.png")), GL_TEXTURE_2D);
    shark.add_texture("specular_map", bonobo::loadTexture2D(config::game_res_path("textures/shark_rough.png")), GL_TEXTURE_2D);
    shark.set_material_constants(shark_material);
    shark.set_program(&phong_shader, phong_uniforms);
    
    Node hammer_shark;
    std::vector<bonobo::mesh_data> hammershark_mesh = bonobo::loadObjects(config::game_res_path("objects/hammershark.obj"));;
    hammer_shark.set_geometry(hammershark_mesh.front());
    hammer_shark.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("textures/hammershark_diff.png")), GL_TEXTURE_2D);
    hammer_shark.add_texture("normal_map", bonobo::loadTexture2D(config::game_res_path("textures/hammershark_normal.png")), GL_TEXTURE_2D);
    hammer_shark.add_texture("specular_map", bonobo::loadTexture2D(config::game_res_path("textures/hammershark_spec.png")), GL_TEXTURE_2D);
    hammer_shark.set_material_constants(shark_material);
    hammer_shark.set_program(&phong_shader, phong_uniforms);
    
    Node seaweed;
    std::vector<bonobo::mesh_data> seaweed_mesh = bonobo::loadObjects(config::game_res_path("objects/seaweed.obj"));;
    seaweed.set_geometry(seaweed_mesh.front());
    seaweed.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("textures/seaweed_diffuse.png")), GL_TEXTURE_2D);
    seaweed.set_program(&diffuse_shader, diffuse_uniforms);
    
    Node rock;
    std::vector<bonobo::mesh_data> rock_mesh = bonobo::loadObjects(config::game_res_path("objects/rock.obj"));;
    rock.set_geometry(rock_mesh.front());
    rock.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("textures/rock_diff.png")), GL_TEXTURE_2D);
    rock.add_texture("normal_map", bonobo::loadTexture2D(config::game_res_path("textures/rock_normal.png")), GL_TEXTURE_2D);
    rock.add_texture("specular_map", bonobo::loadTexture2D(config::game_res_path("textures/rock_rough.png")), GL_TEXTURE_2D);
    rock.set_material_constants(rock_material);
    rock.set_program(&phong_shader, phong_uniforms);
    
    Node ammonite;
    std::vector<bonobo::mesh_data> ammonite_mesh = bonobo::loadObjects(config::game_res_path("objects/ammonite.obj"));;
    ammonite.set_geometry(ammonite_mesh.front());
    ammonite.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("textures/ammonite_diff.jpg")), GL_TEXTURE_2D);
    ammonite.set_program(&diffuse_shader, diffuse_uniforms);
    
    Node coin;
    std::vector<bonobo::mesh_data> coin_mesh = bonobo::loadObjects(config::game_res_path("objects/coin.obj"));;
    coin.set_geometry(coin_mesh.front());
    coin.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("textures/coin_diff.png")), GL_TEXTURE_2D);
    coin.set_program(&diffuse_shader, diffuse_uniforms);
    
    Node treasure;
    std::vector<bonobo::mesh_data> treasure_mesh = bonobo::loadObjects(config::game_res_path("objects/treasure.obj"));;
    treasure.set_geometry(treasure_mesh.front());
    treasure.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("textures/treasure_diff.png")), GL_TEXTURE_2D);
    treasure.add_texture("normal_map", bonobo::loadTexture2D(config::game_res_path("textures/treasure_normal.png")), GL_TEXTURE_2D);
    treasure.add_texture("specular_map", bonobo::loadTexture2D(config::game_res_path("textures/treasure_spec.png")), GL_TEXTURE_2D);
    treasure.get_transform().SetScale(0.63f);
    treasure.set_material_constants(submarine_material);
    treasure.set_program(&phong_shader, phong_uniforms);
    
    Node pane1;
    pane1.set_geometry(parametric_shapes::createPane(1.0f, 1.0f));
    pane1.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("splash/title.png")), GL_TEXTURE_2D);
    pane1.get_transform().SetScale(glm::vec3(1, 0.4, 1.1));
    pane1.set_program(&diffuse_shader,diffuse_uniforms);
    
    Node pane2;
    pane2.set_geometry(parametric_shapes::createPane(1.0f, 1.0f));
    pane2.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("splash/start.png")), GL_TEXTURE_2D);
    pane2.get_transform().SetScale(glm::vec3(1, 0.2, 0.6));
    pane2.set_program(&diffuse_shader,diffuse_uniforms);
    
    Node pane3;
    pane3.set_geometry(parametric_shapes::createPane(1.0f, 1.0f));
    pane3.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("splash/quit.png")), GL_TEXTURE_2D);
    pane3.get_transform().SetScale(glm::vec3(1, 0.2, 0.6));
    pane3.set_program(&diffuse_shader,diffuse_uniforms);
    
    Node pane4;
    pane4.set_geometry(parametric_shapes::createPane(1.0f, 1.0f));
    pane4.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("splash/cursor.png")), GL_TEXTURE_2D);
    pane4.get_transform().SetScale(glm::vec3(0.2, 0.2, 0.2));
    pane4.get_transform().SetTranslate(glm::vec3(0.258907-1.5,-148.511444-0.2,6.213686-2.2));
    pane4.set_program(&diffuse_shader,diffuse_uniforms);
    
    Node sonar;
    sonar.set_geometry(parametric_shapes::createPane(1.0f, 1.0f));
    sonar.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("sprites/sonar.png")), GL_TEXTURE_2D);
    sonar.get_transform().SetScale(glm::vec3(1, 0.6, 0.6));
    sonar.set_program(&diffuse_shader,diffuse_uniforms);
    
    Node sonarping;
    sonarping.set_geometry(parametric_shapes::createPane(1.0f, 1.0f));
    sonarping.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("sprites/sonar_ping1.png")), GL_TEXTURE_2D);
    sonarping.get_transform().SetScale(glm::vec3(1, 0.4, 0.4));
    sonarping.set_program(&diffuse_shader,diffuse_uniforms);
    
    Node cicon;
    cicon.set_geometry(parametric_shapes::createPane(1.0f, 1.0f));
    cicon.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("sprites/coin.png")), GL_TEXTURE_2D);
    cicon.get_transform().SetScale(glm::vec3(1, 0.3, 0.3));
    cicon.set_program(&diffuse_shader,diffuse_uniforms);
    
    Node picon;
    picon.set_geometry(parametric_shapes::createPane(1.0f, 1.0f));
    picon.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("sprites/propeller.png")), GL_TEXTURE_2D);
    picon.get_transform().SetScale(glm::vec3(1, 0.3, 0.3));
    picon.set_program(&diffuse_shader,diffuse_uniforms);
    
    Node health;
    health.set_geometry(parametric_shapes::createPane(1.0f, 1.0f));
    health.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("sprites/health.png")), GL_TEXTURE_2D);
    health.get_transform().SetScale(glm::vec3(1, 0.4, 0.4));
    health.set_program(&diffuse_shader,diffuse_uniforms);
    
    Node bar;
    bar.set_geometry(parametric_shapes::createPane(1.0f, 1.0f));
    bar.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("sprites/health_bar.png")), GL_TEXTURE_2D);
    bar.get_transform().SetScale(glm::vec3(1, 0.4, 0.0));
    bar.set_program(&diffuse_shader,diffuse_uniforms);
    
    Node win;
    win.set_geometry(parametric_shapes::createPane(1.0f, 1.0f));
    win.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("sprites/win.png")), GL_TEXTURE_2D);
    win.get_transform().SetScale(glm::vec3(8, 8, 8));
    submarine_propeller.get_transform().SetRotateY(glm::pi<float>());
    win.set_program(&diffuse_shader,diffuse_uniforms);
    
    Node lose;
    lose.set_geometry(parametric_shapes::createPane(1.0f, 1.0f));
    lose.add_texture("diffuse_map", bonobo::loadTexture2D(config::game_res_path("sprites/lose.png")), GL_TEXTURE_2D);
    win.get_transform().SetScale(glm::vec3(8, 8, 8));
    submarine_propeller.get_transform().SetRotateY(glm::pi<float>());
    lose.set_program(&diffuse_shader,diffuse_uniforms);
    
    std::vector<Node*> objects;
    objects.emplace_back(&sand);
    objects.emplace_back(&ocean);
    objects.emplace_back(&skybox);
    objects.emplace_back(&waterbox);
    objects.emplace_back(&submarine_body);
    objects.emplace_back(&submarine_propeller);
    objects.emplace_back(&whale_skeleton);
    objects.emplace_back(&dino_skeleton);
    objects.emplace_back(&tuna);
    objects.emplace_back(&shark);
    objects.emplace_back(&hammer_shark);
    objects.emplace_back(&seaweed);
    objects.emplace_back(&rock);
    objects.emplace_back(&ammonite);
    objects.emplace_back(&coin);
    objects.emplace_back(&treasure);
    
    std::vector<Node> screens;
    screens.emplace_back(pane1);
    screens.emplace_back(pane2);
    screens.emplace_back(pane3);
    screens.emplace_back(pane4);
    screens.emplace_back(sonar);
    screens.emplace_back(sonarping);
    screens.emplace_back(cicon);
    screens.emplace_back(picon);
    screens.emplace_back(health);
    screens.emplace_back(bar);
    screens.emplace_back(win);
    screens.emplace_back(lose);
    
	glClearDepthf(1.0f);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);

    auto lastTime = std::chrono::high_resolution_clock::now();
    std::chrono::time_point<std::chrono::high_resolution_clock> now_Us;
    bonafide b(objects, screens, &mCamera, &elapsed_time_s, &light_position);
    
    b.keydown.W.key = 'W';
    b.keydown.A.key = 'A';
    b.keydown.S.key = 'S';
    b.keydown.D.key = 'D';
    b.keydown.E.key = 'E';
    b.keydown.Q.key = 'Q';
    b.keydown.UP.key = 'U'; //up
    b.keydown.DOWN.key = 'N'; //down
    b.keydown.RIGHT.key = 'R'; //right
    b.keydown.LEFT.key = 'L'; //left
    b.keydown.ENTER.key = 'T'; //enter
    b.keydown.SPACE.key = 'P'; //space

    int ret;
    
	while (!glfwWindowShouldClose(window)) {
        auto const nowTime = std::chrono::high_resolution_clock::now();
		auto const deltaTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(nowTime - lastTime);
		lastTime = nowTime;
        /*auto& io = ImGui::GetIO();
        inputHandler.SetUICapture(io.WantCaptureMouse, io.WantCaptureKeyboard);*/
        
		glfwPollEvents();
		inputHandler.Advance();
		mCamera.Update(deltaTimeUs * 20, inputHandler);

        if (inputHandler.GetKeycodeState(GLFW_KEY_ESCAPE) & JUST_PRESSED) {
            if(state == GAME) {
                b.mute();
                state = PAUSE;
            }
            else if(state == PAUSE)
                state = GAME;
        }
        /*if (inputHandler.GetKeycodeState(GLFW_KEY_ENTER) & JUST_PRESSED) {
            if(state == GAME) {
                mCamera.mWorld.SetTranslate(glm::vec3(0.258907,-148.511444,6.213686));
                mCamera.mWorld.RotateY(glm::pi<float>());
                state = MENU;
            }
            else if(state == MENU) {
                submarine_body.get_transform().SetTranslate(glm::vec3(0.0f, -250.0f, 0.0f));
                state = GAME;
            }
        }*/
        
        b.keydown.W.val = key(GLFW_KEY_W) ? true : false;
        b.keydown.A.val = key(GLFW_KEY_A) ? true : false;
        b.keydown.S.val = key(GLFW_KEY_S) ? true : false;
        b.keydown.D.val = key(GLFW_KEY_D) ? true : false;
        b.keydown.E.val = key(GLFW_KEY_E) ? true : false;
        b.keydown.Q.val = key(GLFW_KEY_Q) ? true : false;
        b.keydown.UP.val = key(GLFW_KEY_UP) ? true : false;
        b.keydown.DOWN.val = key(GLFW_KEY_DOWN) ? true : false;
        b.keydown.RIGHT.val = key(GLFW_KEY_RIGHT) ? true : false;
        b.keydown.LEFT.val = key(GLFW_KEY_LEFT) ? true : false;
        b.keydown.SPACE.val = key(GLFW_KEY_SPACE) ? true : false;
        b.keydown.ENTER.val = key(GLFW_KEY_ENTER) ? true : false;
            
		if (inputHandler.GetKeycodeState(GLFW_KEY_F11) & JUST_RELEASED)
			mWindowManager.ToggleFullscreenStatusForWindow(window);

		int framebuffer_width, framebuffer_height;
		glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
		glViewport(0, 0, framebuffer_width, framebuffer_height);

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        
        switch(state) {
            case(MENU):
                elapsed_time_s += std::chrono::duration<float>(deltaTimeUs).count();
                ret = b.mainMenu();
                if(ret)
                    state = GAME;
                break;
            case(GAME):
                elapsed_time_s += std::chrono::duration<float>(deltaTimeUs).count();
                b.gameframe();
                break;
            case(PAUSE):
                b.gameframe();
                break;
            case(EXIT):
                glfwSetWindowShouldClose(window, true);
                break;
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
