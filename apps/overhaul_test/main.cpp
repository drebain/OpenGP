#include <OpenGP/types.h>
#include <OpenGP/MLogger.h>
#include <OpenGP/GL/GlfwWindow.h>

///--- Mesh rendering
#include <OpenGP/GL/Application.h>
#include <OpenGP/GL/Scene.h>
#include <OpenGP/GL/Components/WorldRenderComponent.h>
#include <OpenGP/GL/Components/CameraComponent.h>
#include <OpenGP/GL/Components/GUICanvasComponent.h>
#include <OpenGP/SurfaceMesh/SurfaceMesh.h>
#include <OpenGP/SurfaceMesh/GL/SurfaceMeshRenderer.h>
#include <OpenGP/GL/ImguiRenderer.h>

#define OPENGP_IMPLEMENT_ALL_IN_THIS_FILE
#include <OpenGP/util/implementations.h>


using namespace OpenGP;
int main(int argc, char** argv){
    std::string file = (argc==2) ? argv[1] : "bunny.obj";

    SurfaceMesh mesh;
    bool success = mesh.read(file);
    if(!success) mFatal() << "File not found";
    mesh.triangulate();
    mesh.update_face_normals();
    mesh.update_vertex_normals();

    Application app;

    Scene scene;

    app.set_update_callback([&](){scene.update();});

    auto &canvas = scene.create_entity_with<GUICanvasComponent>();

    canvas.set_action([](){

        //Mat4x4 proj = cam.get_projection();
        //Mat4x4 view = cam.get_view();
        //Mat4x4 model = Mat4x4::Identity();

        //ImGuizmo::Manipulate(view.data(), proj.data(), ImGuizmo::ROTATE, ImGuizmo::WORLD, model.data());
        //ImGuizmo::DrawCube(view.data(), proj.data(), model.data());

        ImGui::BeginMainMenuBar();
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("New", "ctrl + n");
            ImGui::MenuItem("Open");
            ImGui::MenuItem("Save");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();

        ImGui::Begin("Test Window 42");
        ImGui::Text("This is the answer to Life, The Universe, and Everything");
        ImGui::End();

    });

    auto &obj = scene.create_entity_with<WorldRenderComponent>();
    auto &renderer = obj.set_renderer<SurfaceMeshRenderer>();
    renderer.upload_mesh(mesh);

    auto &cam = scene.create_entity_with<CameraComponent>();
    cam.get<TransformComponent>().position = Vec3(3, 3, -10);

    auto &window = app.create_window([&](Window &window){
        cam.draw();
    });

    cam.set_window(window);
    canvas.set_camera(cam);

    window.set_title("Test Window");

    return app.run();
}
