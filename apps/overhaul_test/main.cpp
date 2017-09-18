#include <OpenGP/types.h>
#include <OpenGP/MLogger.h>
#include <OpenGP/GL/GlfwWindow.h>

///--- Mesh rendering
#include <OpenGP/GL/Application.h>
#include <OpenGP/GL/Scene.h>
#include <OpenGP/SurfaceMesh/SurfaceMesh.h>
#include <OpenGP/SurfaceMesh/GL/SurfaceMeshRenderer.h>

using namespace OpenGP;
int main(int argc, char** argv){
    /*std::string file = (argc==2) ? argv[1] : "bunny.obj";

    SurfaceMesh mesh;
    bool success = mesh.read(file);
    if(!success) mFatal() << "File not found";
    mesh.triangulate();
    mesh.update_face_normals();
    mesh.update_vertex_normals();*/

    Application app;
    auto &window = app.create_window();

    window.set_title("Test Window");

    Scene scene;
    auto &renderer = scene.create_entity_with<SurfaceMeshRenderer>();

    return app.run();
}
