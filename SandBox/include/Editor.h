#pragma once

#include "Application.h"

class SandBox : public crychic::Application
{
public:
    SandBox() : Application({1280, 720, "SandBox", crychic::ApplicationType::Editor}) {}

    void Init() override;
    void Quit() override;
    void Update() override;

private:
    std::string m_scenePath;
};

