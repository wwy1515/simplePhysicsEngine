//
// Created by 王文优 on 12/30/20.
//

#ifndef SIMPLEPHYSICSENGINE_RENDER_H
#define SIMPLEPHYSICSENGINE_RENDER_H

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "../common/common.h"


const uint SCR_WIDTH = 1280;
const uint SCR_HEIGHT = 720;

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

#endif //SIMPLEPHYSICSENGINE_RENDER_H
