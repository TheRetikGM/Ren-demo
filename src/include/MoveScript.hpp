#pragma once
#include <Ren/ecs/ecs.hpp>

using namespace Ren;
using namespace Ren::ecs::components;

class MoveScript : public ecs::ScriptBehavior
{
    Transform2D* transform;
    float move_speed = 500.0f;
    InputInterface* input{ nullptr };
public:

    void Init() override
    {
        transform = get<Transform2D>();
        transform->layer = 0;
        LOG_I("Movement initialized. Use WSAD to move around!");
    }
    void ProcessInput(InputInterface* input) override
    {
        if (!this->input)
            this->input = input;
    }
    void Update(float dt) override
    {
        if (input->Held(Key::W))
            transform->position.y -= dt * move_speed;
        if (input->Held(Key::S))
            transform->position.y += dt * move_speed;
        if (input->Held(Key::A))
            transform->position.x -= dt * move_speed;
        if (input->Held(Key::D))
            transform->position.x += dt * move_speed;
        
        transform->rotation += 180.0f * dt;
        if (transform->rotation >= 360.0f)
            transform->rotation -= 360.0f;
    }
};