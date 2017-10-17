// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <OpenGP/GL/Application.h>
#include <OpenGP/GL/Scene.h>
#include <OpenGP/GL/Window.h>
#include <OpenGP/GL/SyntheticDepthmap.h>
#include <OpenGP/SurfaceMesh/GL/SurfaceMeshRenderer.h>

#include <OpenGP/GL/Components/TrackballComponent.h>
#include <OpenGP/GL/Components/GUICanvasComponent.h>
#include <OpenGP/GL/Components/GizmoComponent.h>

//=============================================================================
namespace OpenGP {
//=============================================================================

class DepthSensorComponent : public Component {
private:

    std::unique_ptr<SyntheticDepthmap> depthmap;

    FullscreenQuad fsq;

    R32FTexture test_texture;

public:

    void init() {
        require<GizmoComponent>();
        auto &renderer = require<WorldRenderComponent>().set_renderer<SurfaceMeshRenderer>();
        GPUMesh &mesh = renderer.get_gpu_mesh();

        std::vector<Vec3> vpoint;

        vpoint.push_back(Vec3(0, 0, 0));
        vpoint.push_back(Vec3(1, 1, 1));
        vpoint.push_back(Vec3(1, -1, 1));
        vpoint.push_back(Vec3(-1, -1, 1));
        vpoint.push_back(Vec3(-1, 1, 1));

        std::vector<unsigned int> triangles;

        triangles.insert(triangles.end(), { 0, 1, 2 });
        triangles.insert(triangles.end(), { 0, 2, 3 });
        triangles.insert(triangles.end(), { 0, 3, 4 });
        triangles.insert(triangles.end(), { 0, 4, 1 });

        mesh.set_vpoint(vpoint);
        mesh.set_triangles(triangles);

        renderer.wirecolor = Vec3(1, 1, 1);
        renderer.wireframe_mode = WireframeMode::WiresOnly;

        depthmap = std::unique_ptr<SyntheticDepthmap>(new SyntheticDepthmap(512, 512));

        test_texture.allocate(10, 10);
        Image<float> i = Image<float>::Random(10, 10);
        test_texture.upload(i);
    }

    void draw_depthmap() {
        fsq.draw_texture(test_texture);
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
        scene_window->add_listener<WindowCloseEvent>([&](const WindowCloseEvent &){ app.close(); });

        main_camera->set_window(*scene_window);
        canvas->set_camera(*main_camera);

        depthmap_window = &app.create_window([&](Window &window){
            sensor->draw_depthmap();
        });
        depthmap_window->add_listener<WindowCloseEvent>([&](const WindowCloseEvent &){ app.close(); });

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
