#include <Ren/Engine.h>
#include <vector>
#include <Ren/Renderer/Renderer.h>
#include <Ren/ecs/ecs.hpp>
#include "stb_image_write.h"
#include "GUILogger.hpp"
#include "MoveScript.hpp"

unsigned int SCREEN_WIDTH = 800;
unsigned int SCREEN_HEIGHT = 600;

using namespace Ren;

void imgui_frame_handler();
GUILogger logger;
bool g_showGui = false;

class Game : public GameCore
{
	Ref<VertexArray> vao;
	Ref<TextureBatch> batch;
	Ref<Framebuffer> game_view = nullptr;
	glm::ivec2 mViewSize = {1600, 800};
	bool mWireframeRender = false;
	PixelCamera m_camera;

	// ecs
	Ref<ecs::Scene> mScene;
	ecs::SystemsManager mSystems;
	MoveScript move_script;
public:
	Game(unsigned int width, unsigned int height) : GameCore(width, height) {}

	void Init()
	{
		//ResourceManager::LoadTexture(ASSETS_DIR "awesomeface.png", true, "face");
		ResourceManager::LoadShader(SHADERS_DIR "vertex.vert", SHADERS_DIR "fragment.frag", nullptr, "shader");

		mScene = ecs::Scene::Create();
		mSystems.AddSystem<ecs::RenderSystem>();
		mSystems.AddSystem<ecs::ScriptSystem>();
		
		renderer_2d->BeginPrepare();
		{
			auto ent = mScene->NewEntity();
			
			auto trans = mScene->Assign<ecs::components::Transform2D>(ent);
			*trans = { {20.0f, 100.0f}, {200.0f, 200.0f} };

			auto spr = mScene->Assign<ecs::components::SpriteRenderer>(ent);
			spr->color = {1.0f, 0.0f, 1.0f};
			spr->image_path = ASSETS_DIR "awesomeface.png";

			mScene->Assign<ecs::components::Script>(ent)->Add(&move_script);

			mSystems.SetScene(mScene.get());
			mSystems.Init();
			text_renderer->Load(ASSETS_DIR "fonts/DejaVuSansCondensed.ttf", 64);
		}
		renderer_2d->EndPrepare();

		game_view = Framebuffer::Create2DBasicFramebuffer(mViewSize.x, mViewSize.y);
		m_camera.Init(game_view->Size);
		std::srand(std::time(0));
	}
	void Delete()
	{
		mSystems.Destroy();
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
		if (Input->Pressed(Key::SPACE))
		{
			m_camera.SetPosition({ 0.0f, 0.0f });
			m_camera.SetZoom(1.0f);
		}
		static bool imgui_show_demo = false;
		if (Input->Pressed(Key::I))
			imgui_show_demo = !imgui_show_demo;
		if (imgui_show_demo)
			ImGui::ShowDemoWindow();
		if (Input->Held(Key::LEFT_CONTROL) && Input->Pressed(Key::G)) {
			g_showGui = !g_showGui;
			if (g_showGui)
				Logger::SetHandler(&logger);
			else
				Logger::SetDefaultHandler();
		}

		mSystems.ProcessInput(Input);
	}
	void Update(float dt)
	{
		m_camera.Update(Input, dt);
		mSystems.Update(dt);
		this->dt = dt;
	}
	void ProcessScroll(float yoffset)
	{
		m_camera.AddToZoom(yoffset * 0.1f);
	};
	float dt = 0.0f;
	void Render()
	{
		game_view->Bind();
		game_view->Clear({0.2f, 0.2f, 0.2f, 1.0f});
		RenderAPI::WireframeRender(mWireframeRender);
		renderer_2d->BeginScene(&m_camera);
		{
			mSystems.Render();
			text_renderer->RenderText("Gaming text.\nLorem ipsum dolor sit amet...", 300.0f, 300.0f, 1.0f, {0.0f, 1.0f, 1.0f});
			text_renderer->RenderText("FPS: " + std::to_string(1.0f / dt) + "\nGui: " + (g_showGui ? "yes" : "no"), 10.0f, 10.0f, 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		renderer_2d->EndScene();
		renderer_2d->Render();
		game_view->Unbind();

		if (!g_showGui)
		{
			RenderAPI::WireframeRender(false);
			sprite_renderer->RenderGLTexture(game_view->GetColorAttachmentID(0), {0.0f, 0.0f}, game_view->Size, 0.0f, glm::vec3(1.0f), false, true);
		}
	}
	void OnResize() override 
	{
		game_view->Resize(Width, Height);
		m_camera.OnResize(game_view->Size);
	}

	friend void imgui_frame_handler();
};

Game* game;

void imgui_frame_handler()
{
	if (!g_showGui)
		return;
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
