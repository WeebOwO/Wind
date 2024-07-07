#include "Editor.h"

#include <iostream>

void SandBox::Init()
{
    Application::Init();
    LoadScene("");
}

void SandBox::Quit()
{
    Application::Quit();
}

void SandBox::Update()
{
    
}

crychic::Application* crychic::CreateApplication(int argc, char** argv) { return new SandBox(); }