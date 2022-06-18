#include <Ren/Engine.h>
#include <vector>
#include <Ren/Renderer/Renderer.h>
#include "stb_image_write.h"

unsigned int SCREEN_WIDTH = 800;
unsigned int SCREEN_HEIGHT = 600;

using namespace Ren;

class Game : public GameCore
{
	Ref<VertexArray> vao;
	Ref<TextureBatch> batch;

public:
	Game(unsigned int width, unsigned int height) : GameCore(width, height) {}

	void Init()
	{
		ResourceManager::LoadTexture(ASSETS_DIR "awesomeface.png", true, "face");
		ResourceManager::LoadShader(SHADERS_DIR "vertex.vert", SHADERS_DIR "fragment.frag", nullptr, "shader");

		Renderer2D::Init();
	}
	void Delete()
	{
		Renderer2D::Destroy();
	}
	void ProcessInput()
	{
		if (Input->Pressed(Key::ESCAPE))
			Exit();
	}
	void Update(float dt)
	{
	}
	void Render()
	{
		Renderer2D::BeginScene(pixel_projection, glm::mat4(1.0f));
		{
			Transform t;
			Material m;

			glm::vec2 offset(100.0f);
			glm::vec2 quad_size(75.0f);
			glm::vec2 spacing(5.0f);

			for (int i = 0; i < 100; i++)
			{
				int x = i % 10;
				int y = i / 10;

				t.position = offset + glm::vec2(x, y) * (quad_size + spacing);
				t.size = quad_size;
				t.rotation = GetTimeFromStart() * 40;
				glm::vec4 colors[7] = {
					{1.0f, 0.0f, 0.0f, 1.0f},
					{0.0f, 1.0f, 0.0f, 1.0f},
					{0.0f, 0.0f, 1.0f, 1.0f},
					{1.0f, 1.0f, 0.0f, 1.0f},
					{1.0f, 0.0f, 1.0f, 1.0f},
					{0.0f, 1.0f, 1.0f, 1.0f},
					{1.0f, 1.0f, 1.0f, 1.0f},
				};
				m.color = colors[i % 7];

				Renderer2D::SubmitQuad(t, m);
			}
		}
		Renderer2D::EndScene();
		Renderer2D::Render();

		// ImGui::Begin("Hello, World!", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		// ImGui::Text("Hello, this is OpenGL template demo!");
		// if (ImGui::Button("Exit"))
		// 	Exit();
		// ImGui::End();
	}
	void OnResize() override 
	{
	}
};

int main()
{
	GameCore* game = new Game(SCREEN_WIDTH, SCREEN_HEIGHT);
	GameLauncher launcher(game);
	launcher.GuiTheme = ImGuiTheme::dark;
	launcher.Init().Launch();

	delete game;
	return 0;
}
