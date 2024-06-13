#include "Core/Application.h"

int main(int argc, char** argv)
{
    crychic::ApplicationDesc config = {1200, 600, "Crychic"};
    crychic::Application     app(config);
    app.Run();
    return 0;
}
