// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <OpenGP/GL/Application.h>
#include <OpenGP/GL/Scene.h>
#include <OpenGP/GL/Window.h>
#include <OpenGP/SurfaceMesh/GL/SurfaceMeshRenderer.h>

#include <OpenGP/GL/Components/TrackballComponent.h>
#include <OpenGP/GL/Components/GUICanvasComponent.h>
#include <OpenGP/GL/Components/GizmoComponent.h>

//=============================================================================
namespace OpenGP {
//=============================================================================

class DepthSensorComponent : public Component {
private:

    //const char *shader = R"GLSL( vec4 shade() { return vec4(1, 1, 1, 1); } )GLSL";

public:

    void init() {
        require<GizmoComponent>();
        auto &renderer = require<WorldRenderComponent>().set_renderer<SurfaceMeshRenderer>();
        SurfaceMesh mesh;
        auto v0 = mesh.add_vertex(Vec3(0, 0, 0));
        auto v1 = mesh.add_vertex(Vec3(1, 1, 1));
        auto v2 = mesh.add_vertex(Vec3(1, -1, 1));
        auto v3 = mesh.add_vertex(Vec3(-1, 1, 1));
        auto v4 = mesh.add_vertex(Vec3(-1, -1, 1));

        mesh.add_triangle(v0, v1, v3);
        mesh.add_triangle(v0, v1, v2);
        mesh.add_triangle(v0, v2, v4);
        mesh.add_triangle(v0, v3, v4);

        mesh.update_vertex_normals();

        renderer.upload_mesh(mesh);
        //renderer.set_material(Material(shader));
        renderer.wirecolor = Vec3(1, 1, 1);
        renderer.wireframe_mode = WireframeMode::WiresOnly;
    }

};

class VirtualDepthSensor {
protected:

    // Must be first
    Application app;

    Scene scene;

    CameraComponent *main_camera;

    DepthSensorComponent *sensor;

    GUICanvasComponent *canvas;

    Window *scene_window;
    Window *depthmap_window;

public:

    VirtualDepthSensor() {

        app.set_update_callback([&](){ scene.update(); });

        main_camera = &scene.create_entity_with<TrackballComponent>().get<CameraComponent>();

        sensor = &scene.create_entity_with<DepthSensorComponent>();
        auto *gizmo = &sensor->get<GizmoComponent>();

        canvas = &scene.create_entity_with<GUICanvasComponent>();
        canvas->add_listener<GUIElementDrawEvent>([this](const GUIElementDrawEvent &e){ on_gui(e); });
        canvas->add_listener<GUIElementDrawEvent>([gizmo](const GUIElementDrawEvent &e){ gizmo->on_gui(e); });

        scene_window = &app.create_window([&](Window &window){
            main_camera->draw();
        });

        main_camera->set_window(*scene_window);
        canvas->set_camera(*main_camera);

    }

    ~VirtualDepthSensor() {}

    void on_gui(const GUIElementDrawEvent &event) {

        ImGui::BeginMainMenuBar();
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Quit"))
                app.close();
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();

    }

    int run() {
        return app.run();
    }

};

//=============================================================================
} // OpenGP::
//=============================================================================
