#include <OpenGP/types.h>
#include <OpenGP/MLogger.h>

///--- Mesh rendering
#include <OpenGP/GL/Application.h>
#include <OpenGP/GL/ImguiRenderer.h>


using namespace OpenGP;
int main(int argc, char** argv){

    Application app;
    auto &window = app.create_window();

    window.set_title("imgui Test");

    ImguiRenderer imrenderer;

    window.set_display_callback([&](Window &window){

        int width, height;
        std::tie(width, height) = window.get_size();

        imrenderer.begin_frame(width, height);

        ImGui::BeginMainMenuBar();
        ImGui::MenuItem("File");
        ImGui::MenuItem("Edit");
        ImGui::MenuItem("View");
        ImGui::MenuItem("Help");
        ImGui::EndMainMenuBar();

        ImGui::Begin("Test Window 1");
        ImGui::Text("This is a test imgui window");
        ImGui::End();

        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        imrenderer.end_frame();

    });

    return app.run();
}
