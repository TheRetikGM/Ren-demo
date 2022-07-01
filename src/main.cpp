#include <Ren/Engine.h>
#include <vector>
#include <Ren/Renderer/Renderer.h>
#include <Ren/ecs/ecs.hpp>
#include "stb_image_write.h"
#include "GUILogger.hpp"

unsigned int SCREEN_WIDTH = 800;
unsigned int SCREEN_HEIGHT = 600;

using namespace Ren;

void imgui_frame_handler();
GUILogger logger;

class Game : public GameCore
{
	Ref<VertexArray> vao;
	Ref<TextureBatch> batch;
	int32_t tex_ids[7];
	Ref<Framebuffer> game_view = nullptr;
	glm::ivec2 mViewSize = {1600, 800};
	bool mWireframeRender = false;
	Ref<Scene> mScene;
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

		game_view = Framebuffer::Create2DBasicFramebuffer(mViewSize.x, mViewSize.y);

		std::srand(std::time(0));

		mScene = Scene::Create();
		EntityID e1 = mScene->NewEntity();
		mScene->AssignMultiple<Transform, Material>(e1);
		EntityID e2 = mScene->NewEntity();
		mScene->Assign<Material>(e2);
		EntityID e3 = mScene->NewEntity();
		mScene->Assign<glm::vec3>(e3);
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

		if (Input->Pressed(Key::NUM_1))
		{
			std::string str = "";
			for (auto&& ent : SceneView<Material>(*mScene))
				str += " " + std::to_string(Utils::GetEntityIndex(ent));
			LOG_I("Material entity ids: " + str);
		}
		if (Input->Pressed(Key::NUM_2))
		{
			std::string str = "";
			for (auto&& ent : SceneView<Transform>(*mScene))
				str += " " + std::to_string(Utils::GetEntityIndex(ent));
			LOG_I("Transform entity ids: " + str);
		}
		if (Input->Pressed(Key::NUM_3))
		{
			std::string str = "";
			for (auto&& ent : SceneView<Material, Transform>(*mScene))
				str += " " + std::to_string(Utils::GetEntityIndex(ent));
			LOG_I("Materal and Transform entity ids: " + str);
		}
		if (Input->Pressed(Key::NUM_4))
		{
			std::string str = "";
			for (auto&& ent : SceneView<glm::vec3>(*mScene))
				str += " " + std::to_string(Utils::GetEntityIndex(ent));
			LOG_I("glm::vec3 entity ids: " + str);
		}
	}
	void Update(float dt)
	{
		this->dt = dt;
	}
	float dt = 0.0f;
	void Render()
	{
		game_view->Bind();
		game_view->Clear({0.2f, 0.2f, 0.2f, 1.0f});
		RenderAPI::WireframeRender(mWireframeRender);
		glm::mat4 projection = glm::ortho(0.0f, (float)mViewSize.x, (float)mViewSize.y, 0.0f, 0.0f, -1.0f);
		renderer_2d->BeginScene(projection, glm::mat4(1.0f));
		{
			renderer_2d->SubmitQuad({{20.0f, 100.0f}, {200.0f, 200.0f}, GetTimeFromStart() * 10.0f}, {{1.0f, 0.0f, 1.0f}});
			text_renderer->RenderText("Gaming text.\nLorem ipsum dolor sit amet...", 300.0f, 300.0f, 1.0f, {0.0f, 1.0f, 1.0f});
			text_renderer->RenderText("FPS: " + std::to_string(1.0f / dt), 10.0f, 10.0f, 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		renderer_2d->EndScene();
		renderer_2d->Render();
		game_view->Unbind();
		// sprite_renderer->RenderGLTexture(game_view->GetColorAttachmentID(0), {0.0f, 0.0f}, game_view->Size, 0.0f, glm::vec3(1.0f), false, true);
	}
	void OnResize() override 
	{
		//game_view->Resize(Width, Height);
	}

	friend void imgui_frame_handler();
};

Game* game;

void imgui_frame_handler()
{
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	ImGui::Begin("Game View");
	// ImVec2 a_space = ImGui::GetWindowContentRegionMax();
	static ImVec2 game_view_size(game->game_view->Width, game->game_view->Height);
	//game_view_size.y = game_view_size.x / (game->game_view->Width / game->game_view->Height);
	ImGui::Image(ImTextureID(intptr_t(game->game_view->GetColorAttachmentID(0))), game_view_size, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();

	logger.Draw();

	ImGui::Begin("Settings");
	ImGui::DragFloat("View width", &game_view_size.x);
	ImGui::Checkbox("Wireframe", &game->mWireframeRender);
	ImGui::Separator();
	if (ImGui::Button("Exit"))
		game->Exit();
	ImGui::End();

	ImGui::Begin("Info");
	if (ImGui::BeginTable("table_info", 2, ImGuiTableFlags_RowBg))
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::Text("%ix%i", game->game_view->Width, game->game_view->Height);
		ImGui::TableNextColumn(); ImGui::Text("Resolution");
		ImGui::TableNextColumn(); ImGui::Text("%i", game->renderer_2d->GetPrimitiveCount());
		ImGui::TableNextColumn(); ImGui::Text("Primitives");
		ImGui::TableNextColumn(); ImGui::Text("%i", game->renderer_2d->GetVertexCount());
		ImGui::TableNextColumn(); ImGui::Text("Vertices");
		ImGui::TableNextColumn(); ImGui::Text("%i", game->renderer_2d->GetIndexCount());
		ImGui::TableNextColumn(); ImGui::Text("Indices");
		ImGui::TableNextColumn(); ImGui::Text("%i", game->renderer_2d->GetTextureCount());
		ImGui::TableNextColumn(); ImGui::Text("Textures");
		ImGui::TableNextColumn(); ImGui::Text("%i", game->renderer_2d->GetBatchCount());
		ImGui::TableNextColumn(); ImGui::Text("Batches");
		ImGui::EndTable();
	}
	ImGui::End();
}

int main()
{
	Logger::EntryHandler = &logger;

	game = new Game(SCREEN_WIDTH, SCREEN_HEIGHT);
	GameLauncher launcher(game);
	launcher.GuiTheme = ImGuiTheme::dark;
	launcher.ImGuiFrameHandler = imgui_frame_handler;
	launcher.Init().Launch();

	delete game;
	return 0;
}
