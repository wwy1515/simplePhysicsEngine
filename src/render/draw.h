//
// Created by Wenyou Wang on 12/31/20.
//

#ifndef SIMPLEPHYSICSENGINE_DRAW_H
#define SIMPLEPHYSICSENGINE_DRAW_H

#include "render.h"

struct GLRenderPoints;
struct GLRenderLines;
struct GLRenderTriangles;
struct GLFWwindow;


struct sPE_Color
{
    sPE_Color() { r = 0.0f; g = 0.0f; b = 0.0f; a = 1.0f;}
    sPE_Color(float rIn, float gIn, float bIn, float aIn = 1.0f)
    {
        r = rIn; g = gIn; b = bIn; a = aIn;
    }
    void Set(float rIn, float gIn, float bIn, float aIn = 1.0f)
    {
        r = rIn; g = gIn; b = bIn; a = aIn;
    }
    float r, g, b, a;
};
//
struct Camera
{
    Camera()
    {
        m_center.Set(0.0f, 0.0f);
        m_zoom = 1.0f;
        m_width = SCR_WIDTH;
        m_height = SCR_HEIGHT;
    }

    Vector2f ConvertScreenToWorld(const Vector2f& screenPoint);
    Vector2f ConvertWorldToScreen(const Vector2f& worldPoint);
    void BuildProjectionMatrix(float* m, float zBias);

    Vector2f m_center;
    float m_zoom;
    uint m_width;
    uint m_height;
};

class sPE_renderTarget
{
public:
    sPE_renderTarget();
    void Bind();
    void Unbind();
    void Destroy();
    inline uint getTargetTexture() { return texture;};


private:
    unsigned int fbo,rbo,texture;
    uint m_width;
    uint m_height;

};




class sPE_Draw
{
public:
    sPE_Draw();
    ~sPE_Draw();

    void Create();
    void Destroy();

    void DrawPolygon(const Vector2f* vertices, uint vertexCount, const sPE_Color& color);

    void DrawPoint(const Vector2f& p, float size, const sPE_Color& color);

    void Flush();

    GLRenderPoints* m_points;
    GLRenderLines* m_lines;
    GLRenderTriangles* m_triangles;
    sPE_renderTarget* renderTarget;

    Camera *camera;
private:
    uint m_drawFlags;
};

extern sPE_Draw g_Draw;
extern Camera g_camera;
extern GLFWwindow* g_mainWindow;



#endif //SIMPLEPHYSICSENGINE_DRAW_H
