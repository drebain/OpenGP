#include <OpenGP/types.h>
#include <OpenGP/MLogger.h>
#include <OpenGP/GL/GlfwWindow.h>

///--- Mesh rendering
#include <OpenGP/GL/Application.h>
#include <OpenGP/GL/Scene.h>
#include <OpenGP/GL/Components/WorldRenderComponent.h>
#include <OpenGP/GL/Components/CameraComponent.h>
#include <OpenGP/SurfaceMesh/SurfaceMesh.h>
#include <OpenGP/SurfaceMesh/GL/SurfaceMeshRenderer.h>

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
    auto &window = app.create_window();

    window.set_title("Test Window");

    Scene scene;

    app.set_update_callback([&](){scene.update();});

    auto &bunny = scene.create_entity_with<WorldRenderComponent>();
    auto &renderer = bunny.set_renderer<SurfaceMeshRenderer>();
    renderer.upload_mesh(mesh);

    auto &cam = scene.create_entity_with<CameraComponent>();
    cam.get<TransformComponent>().position = Vec3(4, 4, -10);

    cam.set_window(window);

    return app.run();
}
