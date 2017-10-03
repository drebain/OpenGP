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

    ImguiRenderer imrenderer;

    Scene scene;

    app.set_update_callback([&](){scene.update();});

    auto &bunny = scene.create_entity_with<WorldRenderComponent>();
    auto &renderer = bunny.set_renderer<SurfaceMeshRenderer>();
    renderer.upload_mesh(mesh);

    auto &cam = scene.create_entity_with<CameraComponent>();
    cam.get<TransformComponent>().position = Vec3(3, 3, -10);
    //cam.get<TransformComponent>().position = Vec3(0, 0, -3);

    auto &canvas = scene.create_entity_with<GUICanvasComponent>();

    auto &window = app.create_window([&](Window &window){

        cam.draw();

        int width, height;
        std::tie(width, height) = window.get_size();

        imrenderer.begin_frame(width, height);

        Mat4x4 proj = cam.get_projection();
        Mat4x4 view = cam.get_view();
        Mat4x4 model = Mat4x4::Identity();

        //ImGuizmo::Manipulate(view.data(), proj.data(), ImGuizmo::ROTATE, ImGuizmo::WORLD, model.data());
        //ImGuizmo::DrawCube(view.data(), proj.data(), model.data());

        ImGui::BeginMainMenuBar();
        ImGui::MenuItem("File");
        ImGui::MenuItem("Edit");
        ImGui::MenuItem("View");
        ImGui::MenuItem("Help");
        ImGui::EndMainMenuBar();

        ImGui::Begin("Test Window 1");
        ImGui::Text("This is a test imgui window");
        ImGui::End();

        imrenderer.end_frame();

    });

    cam.set_window(window);
    canvas.set_camera(cam);

/*
    std::unique_ptr<EventSentinel> sentinel(new EventSentinel());

    window.add_listener<MouseMoveEvent>([](const MouseMoveEvent &e){
        mLogger() << e.position(0) << e.position(1);
    }, *sentinel);

    window.add_listener<MouseButtonEvent>([&](const MouseButtonEvent &e){
        mLogger() << "reset";
        sentinel.reset();
    });*/

    window.set_title("Test Window");

    return app.run();
}
