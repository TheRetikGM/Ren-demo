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
	int32_t tex_ids[7];
public:
	Game(unsigned int width, unsigned int height) : GameCore(width, height) {}

	void Init()
	{
		//ResourceManager::LoadTexture(ASSETS_DIR "awesomeface.png", true, "face");
		ResourceManager::LoadShader(SHADERS_DIR "vertex.vert", SHADERS_DIR "fragment.frag", nullptr, "shader");

		auto face = RawTexture::Load(ASSETS_DIR "awesomeface.png");
		renderer_2d->BeginPrepare();
		{
			for (int i = 0; i < 6; i++)
			{
				auto tex = RawTexture::Load((ASSETS_DIR + std::string(1, 'a' + i) + ".png").c_str());
				tex_ids[i] = renderer_2d->PrepareTexture(tex);
				tex.Delete();
			}
			tex_ids[6] = renderer_2d->PrepareTexture(face);

			text_renderer->Load(ASSETS_DIR "fonts/DejaVuSansCondensed.ttf", 64);
		}
		renderer_2d->EndPrepare();
		face.Delete();

		std::srand(std::time(0));
	}
	void Delete()
	{
	}
	void ProcessInput()
	{
		if (Input->Pressed(Key::ESCAPE))
			Exit();
		if (Input->Pressed(Key::P))
		{
			auto tex_desc = renderer_2d->GetTextureDescriptor(0);
			RawTexture batch = Utils::GLToRawTexture(*tex_desc.pTexture);
			Utils::SaveTexturePNG("/home/kuba/projects/git/Ren-demo/builddir/test.png", batch, false);
			batch.Delete();
		}
		static bool imgui_show_demo = false;
		if (Input->Pressed(Key::I))
			imgui_show_demo = !imgui_show_demo;
		if (imgui_show_demo)
			ImGui::ShowDemoWindow();
	}
	void Update(float dt)
	{
		this->dt = dt;
	}
	float dt;
	void Render()
	{
		renderer_2d->BeginScene(pixel_projection, glm::mat4(1.0f));
		{
			Transform t;
			Material m;

			glm::vec2 offset(50.0f);
			glm::vec2 quad_size(15.0f);
			glm::vec2 spacing(5.0f);

			int w = 90;
			int h = 40;

			static glm::vec3 colors[4] = {
				{ 1.0f, 0.0f, 0.0f },
				{ 0.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f },
				{ 0.0f, 1.0f, 1.0f }
			};

			static glm::vec3 rand_rot(Helper::RandomFloat_0_1(), Helper::RandomFloat_0_1(), Helper::RandomFloat_0_1());

			for (int i = 0; i < 4; i++)
				colors[i] = glm::vec3(glm::rotate(glm::mat4(1.0f), -dt * 1.0f, rand_rot) * glm::vec4(colors[i], 0.0f));

			for (int i = 0, layer = 0; i < w * h; i++, layer += (i % 50 == 0) ? 1 : 0)
			{
				int x = i % w;
				int y = i / w;

				t.position = offset + glm::vec2(x, y) * (quad_size + spacing);
				t.scale = quad_size;
				t.rotation = GetTimeFromStart() * 40;

				// Bilinear interpolation
				float w11 = (w - x) * (h - y);
				float w12 = (w - x) * y;
				float w21 = x * (h - y);
				float w22 = x * y;
				m.color = glm::vec4((w11 * colors[0] + w21 * colors[1] + w22 * colors[2] + w12 * colors[3]) / float(w * h), i % 7 == 5 ? 0.65f : 1.0f);

				m.texture_id = tex_ids[i % 7];
				renderer_2d->SubmitQuad(t, m);
			}

			text_renderer->RenderText("FPS: " + std::to_string(1.0f / dt), 10.0f, 10.0f, 0.5f, glm::vec3(1.0f, 1.0f, 0.0f));
		}
		renderer_2d->EndScene();
		renderer_2d->Render();

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
