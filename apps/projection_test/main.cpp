#include <iostream>


#include <OpenGP/GL/Application.h>
#include <OpenGP/GL/Components/WorldRenderComponent.h>
#include <OpenGP/GL/Components/TrackballComponent.h>
#include <OpenGP/GL/Components/GUICanvasComponent.h>
#include <OpenGP/GL/Components/GizmoComponent.h>
#include <OpenGP/SphereMesh/GL/SphereMeshRenderer.h>
#include <OpenGP/SurfaceMesh/GL/SurfaceMeshRenderer.h>
#include <OpenGP/SphereMesh/helpers.h>

#define OPENGP_IMPLEMENT_ALL_IN_THIS_FILE
#include <OpenGP/util/implementations.h>


using namespace OpenGP;

Vec3 sphere_mesh_project(const Vec3 &p, const SphereMesh &mesh, Vec4& s0_out, Vec4& s1_out, Vec4& s_out) {

    auto vpoint = mesh.get_vertex_property<Vec4>("v:point");

    Vec3 best_proj;
    float best_dist = FLT_MAX;

    for (const auto &face : mesh.faces()) {
        Vec4 s0 = vpoint[mesh.vertex(face, 0)];
        Vec4 s1 = vpoint[mesh.vertex(face, 1)];
        Vec4 s2 = vpoint[mesh.vertex(face, 2)];

        Vec3 proj = wedge_project(p, s0, s1, s2);
        float dist = (proj - p).norm();
        if (dist < best_dist) {
            best_dist = dist;
            best_proj = proj;
            s0_out = s0;
            s1_out = wedge_s3(p, s0, s1, s2);
            s_out = pill_s2(p, s0_out, s1_out);
        }
    }

    for (const auto &edge : mesh.edges()) {
        Vec4 s0 = vpoint[mesh.vertex(edge, 0)];
        Vec4 s1 = vpoint[mesh.vertex(edge, 1)];

        Vec3 proj = pill_project(p, s0, s1);
        float dist = (proj - p).norm();
        if (dist < best_dist) {
            best_dist = dist;
            best_proj = proj;
            s0_out = s0;
            s1_out = s1;
            s_out = pill_s2(p, s0_out, s1_out);
        }
    }

    return best_proj;

}

int main(int argc, char **argv) {

    std::string filename = (argc > 1) ? argv[1] : "example.smo";

    SphereMesh mesh;
    mesh.read(filename);

    SurfaceMesh bmesh;
    bmesh.read("sphere.obj");

    bmesh.triangulate();
    bmesh.update_face_normals();
    bmesh.update_vertex_normals();

    Application app;
    Scene scene;

    auto &main_camera = scene.create_entity_with<TrackballComponent>().get<CameraComponent>();

    auto &smesh = scene.create_entity_with<WorldRenderComponent>();
    smesh.set_renderer<SphereMeshRenderer>().upload_mesh(mesh);

    auto &pill_mesh = scene.create_entity_with<WorldRenderComponent>();
    pill_mesh.set_renderer<SphereMeshRenderer>().get_material().set_property("base_color", Vec3(1, 0, 0));

    auto &sphere_mesh = scene.create_entity_with<WorldRenderComponent>();
    sphere_mesh.set_renderer<SphereMeshRenderer>().get_material().set_property("base_color", Vec3(0, 1, 0));

    auto &target = scene.create_entity_with<WorldRenderComponent>();
    target.set_renderer<SurfaceMeshRenderer>().upload_mesh(bmesh);
    auto &gizmo = target.require<GizmoComponent>();
    target.get<TransformComponent>().scale = Vec3(1, 1, 1) * 0.2;

    auto &projection = scene.create_entity_with<WorldRenderComponent>();
    projection.set_renderer<SurfaceMeshRenderer>().upload_mesh(bmesh);
    projection.get_renderer<SurfaceMeshRenderer>().get_material().set_property("base_color", Vec3(0, 0, 1));
    projection.get<TransformComponent>().scale = Vec3(1, 1, 1) * 0.05;

    auto &canvas = scene.create_entity_with<GUICanvasComponent>();
    canvas.add_listener<GUIElementDrawEvent>([&gizmo](const GUIElementDrawEvent &e){ gizmo.on_gui(e); });

    auto &scene_window = app.create_window([&](Window &window){
        main_camera.draw();
    });
    main_camera.set_window(scene_window);
    canvas.set_camera(main_camera);

    app.add_listener<ApplicationUpdateEvent>([&](const ApplicationUpdateEvent&){

        Vec4 s0, s1, s;

        Vec3 pos = sphere_mesh_project(target.get<TransformComponent>().position, mesh, s0, s1, s);;

        projection.get<TransformComponent>().position = pos;

        s0(3) *= 1.01;
        s1(3) *= 1.01;
        s(3) *= 1.02;

        SphereMesh pill;
        auto v0 = pill.add_vertex(s0);
        auto v1 = pill.add_vertex(s1);
        pill.add_edge(v0, v1);

        pill_mesh.get_renderer<SphereMeshRenderer>().upload_mesh(pill);

        SphereMesh sphere;
        auto v = sphere.add_vertex(s);
        sphere.add_sphere(v);

        sphere_mesh.get_renderer<SphereMeshRenderer>().upload_mesh(sphere);

        scene.update();
    });

    app.run();

}
