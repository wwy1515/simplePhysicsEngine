//
// Created by Wenyou Wang on 12/31/20.
//

#include "draw.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#define BUFFER_OFFSET(x)  ((const void*) (x))


////////////////////////////////////////////////////////
Vector2f Camera::ConvertScreenToWorld(const Vector2f& ps)
{
    float w = float(m_width);
    float h = float(m_height);
    float u = ps.x / w;
    float v = (h - ps.y) / h;

    float ratio = w / h;
    Vector2f extents(ratio * 25.0f, 25.0f);
    extents *= m_zoom;

    Vector2f lower = m_center - extents;
    Vector2f upper = m_center + extents;

    Vector2f pw;
    pw.x = (1.0f - u) * lower.x + u * upper.x;
    pw.y = (1.0f - v) * lower.y + v * upper.y;
    return pw;
}

//
Vector2f Camera::ConvertWorldToScreen(const Vector2f& pw)
{
    float w = float(m_width);
    float h = float(m_height);
    float ratio = w / h;
    Vector2f extents(ratio * 25.0f, 25.0f);
    extents *= m_zoom;

    Vector2f lower = m_center - extents;
    Vector2f upper = m_center + extents;

    float u = (pw.x - lower.x) / (upper.x - lower.x);
    float v = (pw.y - lower.y) / (upper.y - lower.y);

    Vector2f ps;
    ps.x = u * w;
    ps.y = (1.0f - v) * h;
    return ps;
}

// Convert from world coordinates to normalized device coordinates.
// http://www.songho.ca/opengl/gl_projectionmatrix.html
void Camera::BuildProjectionMatrix(float* m, float zBias)
{
    float w = float(m_width);
    float h = float(m_height);
    float ratio = w / h;
    Vector2f extents(ratio * 40.0f, 40.0f);
    extents *= m_zoom;

    Vector2f lower = m_center - extents;
    Vector2f upper = m_center + extents;

    m[0] = 2.0f / (upper.x - lower.x);
    m[1] = 0.0f;
    m[2] = 0.0f;
    m[3] = 0.0f;

    m[4] = 0.0f;
    m[5] = 2.0f / (upper.y - lower.y);
    m[6] = 0.0f;
    m[7] = 0.0f;

    m[8] = 0.0f;
    m[9] = 0.0f;
    m[10] = 1.0f;
    m[11] = 0.0f;

    m[12] = -(upper.x + lower.x) / (upper.x - lower.x);
    m[13] = -(upper.y + lower.y) / (upper.y - lower.y);
    m[14] = zBias;
    m[15] = 1.0f;
}

//////////////////////////////////////////////////////////////////////

static void sCheckGLError()
{
    GLenum errCode = glGetError();
    if (errCode != GL_NO_ERROR)
    {
        fprintf(stderr, "OpenGL error = %d\n", errCode);
        assert(false);
    }
}

// Prints shader compilation errors
static void sPrintLog(GLuint object)
{
    GLint log_length = 0;
    if (glIsShader(object))
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
    else if (glIsProgram(object))
        glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
    else
    {
        fprintf(stderr, "printlog: Not a shader or a program\n");
        return;
    }

    char* log = (char*)malloc(log_length);

    if (glIsShader(object))
        glGetShaderInfoLog(object, log_length, NULL, log);
    else if (glIsProgram(object))
        glGetProgramInfoLog(object, log_length, NULL, log);

    fprintf(stderr, "%s", log);
    free(log);
}


//
static GLuint sCreateShaderFromString(const char* source, GLenum type)
{
    GLuint res = glCreateShader(type);
    const char* sources[] = { source };
    glShaderSource(res, 1, sources, NULL);
    glCompileShader(res);
    GLint compile_ok = GL_FALSE;
    glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
    if (compile_ok == GL_FALSE)
    {
        fprintf(stderr, "Error compiling shader of type %d!\n", type);
        sPrintLog(res);
        glDeleteShader(res);
        return 0;
    }

    return res;
}

//
static GLuint sCreateShaderProgram(const char* vs, const char* fs)
{
    GLuint vsId = sCreateShaderFromString(vs, GL_VERTEX_SHADER);
    GLuint fsId = sCreateShaderFromString(fs, GL_FRAGMENT_SHADER);
    assert(vsId != 0 && fsId != 0);

    GLuint programId = glCreateProgram();
    glAttachShader(programId, vsId);
    glAttachShader(programId, fsId);
    glBindFragDataLocation(programId, 0, "color");
    glLinkProgram(programId);

    glDeleteShader(vsId);
    glDeleteShader(fsId);

    GLint status = GL_FALSE;
    glGetProgramiv(programId, GL_LINK_STATUS, &status);
    assert(status != GL_FALSE);

    return programId;
}

//////////////////////////////////////////////////////////////////

//
struct GLRenderPoints
{
    void Create(Camera* gCamera)
    {
        const char* vs = \
			"#version 330\n"
            "uniform mat4 projectionMatrix;\n"
            "layout(location = 0) in vec2 v_position;\n"
            "layout(location = 1) in vec4 v_color;\n"
            "layout(location = 2) in float v_size;\n"
            "out vec4 f_color;\n"
            "void main(void)\n"
            "{\n"
            "	f_color = v_color;\n"
            "	gl_Position = projectionMatrix * vec4(v_position, 0.0f, 1.0f);\n"
            "   gl_PointSize = v_size;\n"
            "}\n";

        const char* fs = \
			"#version 330\n"
            "in vec4 f_color;\n"
            "out vec4 color;\n"
            "void main(void)\n"
            "{\n"
            "	color = f_color;\n"
            "}\n";

        m_programId = sCreateShaderProgram(vs, fs);
        m_projectionUniform = glGetUniformLocation(m_programId, "projectionMatrix");
        m_vertexAttribute = 0;
        m_colorAttribute = 1;
        m_sizeAttribute = 2;
        this->gCamera = gCamera;

        // Generate
        glGenVertexArrays(1, &m_vaoId);
        glGenBuffers(3, m_vboIds);

        glBindVertexArray(m_vaoId);
        glEnableVertexAttribArray(m_vertexAttribute);
        glEnableVertexAttribArray(m_colorAttribute);
        glEnableVertexAttribArray(m_sizeAttribute);

        // Vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
        glVertexAttribPointer(m_vertexAttribute, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
        glVertexAttribPointer(m_colorAttribute, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[2]);
        glVertexAttribPointer(m_sizeAttribute, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_sizes), m_sizes, GL_DYNAMIC_DRAW);

        sCheckGLError();

        // Cleanup
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        m_count = 0;
    }

    void Destroy()
    {
        if (m_vaoId)
        {
            glDeleteVertexArrays(1, &m_vaoId);
            glDeleteBuffers(3, m_vboIds);
            m_vaoId = 0;
        }

        if (m_programId)
        {
            glDeleteProgram(m_programId);
            m_programId = 0;
        }
    }

    void Vertex(const Vector2f& v, const sPE_Color& c, float size)
    {
        if (m_count == e_maxVertices)
            Flush();

        m_vertices[m_count] = v;
        m_colors[m_count] = c;
        m_sizes[m_count] = size;
        ++m_count;
    }

    void Flush()
    {
        if (m_count == 0)
            return;
        glUseProgram(m_programId);

        float proj[16] = { 0.0f };
        gCamera->BuildProjectionMatrix(proj, 0.0f);

        glUniformMatrix4fv(m_projectionUniform, 1, GL_FALSE, proj);
        glBindVertexArray(m_vaoId);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(Vector2f), m_vertices);

        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(sPE_Color), m_colors);

        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[2]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(float), m_sizes);

        glEnable(GL_PROGRAM_POINT_SIZE);
        glDrawArrays(GL_POINTS, 0, m_count);
        glDisable(GL_PROGRAM_POINT_SIZE);

        sCheckGLError();

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);

        m_count = 0;
    }

    enum { e_maxVertices = 512 };
    Vector2f m_vertices[e_maxVertices];
    sPE_Color m_colors[e_maxVertices];
    float m_sizes[e_maxVertices];

    uint m_count;

    GLuint m_vaoId;
    GLuint m_vboIds[3];
    GLuint m_programId;
    GLint m_projectionUniform;
    GLint m_vertexAttribute;
    GLint m_colorAttribute;
    GLint m_sizeAttribute;

    Camera* gCamera;
};

//
struct GLRenderLines
{
    void Create(Camera* gCamera)
    {
        const char* vs = \
			"#version 330\n"
            "uniform mat4 projectionMatrix;\n"
            "layout(location = 0) in vec2 v_position;\n"
            "layout(location = 1) in vec4 v_color;\n"
            "out vec4 f_color;\n"
            "void main(void)\n"
            "{\n"
            "	f_color = v_color;\n"
            "	gl_Position = projectionMatrix * vec4(v_position, 0.0f, 1.0f);\n"
            "}\n";

        const char* fs = \
			"#version 330\n"
            "in vec4 f_color;\n"
            "out vec4 color;\n"
            "void main(void)\n"
            "{\n"
            "	color = f_color;\n"
            "}\n";

        m_programId = sCreateShaderProgram(vs, fs);
        m_projectionUniform = glGetUniformLocation(m_programId, "projectionMatrix");
        m_vertexAttribute = 0;
        m_colorAttribute = 1;
        this->gCamera = gCamera;

        // Generate
        glGenVertexArrays(1, &m_vaoId);
        glGenBuffers(2, m_vboIds);

        glBindVertexArray(m_vaoId);
        glEnableVertexAttribArray(m_vertexAttribute);
        glEnableVertexAttribArray(m_colorAttribute);

        // Vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
        glVertexAttribPointer(m_vertexAttribute, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
        glVertexAttribPointer(m_colorAttribute, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors, GL_DYNAMIC_DRAW);

        sCheckGLError();

        // Cleanup
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        m_count = 0;
    }

    void Destroy()
    {
        if (m_vaoId)
        {
            glDeleteVertexArrays(1, &m_vaoId);
            glDeleteBuffers(2, m_vboIds);
            m_vaoId = 0;
        }

        if (m_programId)
        {
            glDeleteProgram(m_programId);
            m_programId = 0;
        }
    }

    void Vertex(const Vector2f& v, const sPE_Color& c)
    {
        if (m_count == e_maxVertices)
            Flush();

        m_vertices[m_count] = v;
        m_colors[m_count] = c;
        ++m_count;
    }

    void Flush()
    {
        if (m_count == 0)
            return;

        glUseProgram(m_programId);

        float proj[16] = { 0.0f };
        gCamera->BuildProjectionMatrix(proj, 0.1f);

        glUniformMatrix4fv(m_projectionUniform, 1, GL_FALSE, proj);

        glBindVertexArray(m_vaoId);

        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(Vector2f), m_vertices);

        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(sPE_Color), m_colors);

        glDrawArrays(GL_LINES, 0, m_count);

        sCheckGLError();

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);

        m_count = 0;
    }

    enum { e_maxVertices = 2 * 512 };
    Vector2f m_vertices[e_maxVertices];
    sPE_Color m_colors[e_maxVertices];

    uint m_count;

    GLuint m_vaoId;
    GLuint m_vboIds[2];
    GLuint m_programId;
    GLint m_projectionUniform;
    GLint m_vertexAttribute;
    GLint m_colorAttribute;

    Camera* gCamera;
};

//
struct GLRenderTriangles
{
    void Create(Camera* gCamera)
    {
        const char* vs = \
			"#version 330\n"
            "uniform mat4 projectionMatrix;\n"
            "layout(location = 0) in vec2 v_position;\n"
            "layout(location = 1) in vec4 v_color;\n"
            "out vec4 f_color;\n"
            "void main(void)\n"
            "{\n"
            "	f_color = v_color;\n"
            "	gl_Position = projectionMatrix * vec4(v_position, 0.0f, 1.0f);\n"
            "}\n";

        const char* fs = \
			"#version 330\n"
            "in vec4 f_color;\n"
            "out vec4 color;\n"
            "void main(void)\n"
            "{\n"
            "	color = f_color;\n"
            "}\n";

        m_programId = sCreateShaderProgram(vs, fs);
        m_projectionUniform = glGetUniformLocation(m_programId, "projectionMatrix");
        m_vertexAttribute = 0;
        m_colorAttribute = 1;
        this->gCamera = gCamera;

        // Generate
        glGenVertexArrays(1, &m_vaoId);
        glGenBuffers(2, m_vboIds);

        glBindVertexArray(m_vaoId);
        glEnableVertexAttribArray(m_vertexAttribute);
        glEnableVertexAttribArray(m_colorAttribute);

        // Vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
        glVertexAttribPointer(m_vertexAttribute, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
        glVertexAttribPointer(m_colorAttribute, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors, GL_DYNAMIC_DRAW);

        sCheckGLError();

        // Cleanup
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        m_count = 0;
    }

    void Destroy()
    {
        if (m_vaoId)
        {
            glDeleteVertexArrays(1, &m_vaoId);
            glDeleteBuffers(2, m_vboIds);
            m_vaoId = 0;
        }

        if (m_programId)
        {
            glDeleteProgram(m_programId);
            m_programId = 0;
        }
    }

    void Vertex(const Vector2f& v, const sPE_Color& c)
    {
        if (m_count == e_maxVertices)
            Flush();

        m_vertices[m_count] = v;
        m_colors[m_count] = c;
        ++m_count;
    }

    void Flush()
    {
        if (m_count == 0)
            return;

        glUseProgram(m_programId);

        float proj[16] = { 0.0f };
        gCamera->BuildProjectionMatrix(proj, 0.2f);

        glUniformMatrix4fv(m_projectionUniform, 1, GL_FALSE, proj);

        glBindVertexArray(m_vaoId);

        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(Vector2f), m_vertices);

        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(sPE_Color), m_colors);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDrawArrays(GL_TRIANGLES, 0, m_count);
        glDisable(GL_BLEND);

        sCheckGLError();

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);

        m_count = 0;
    }

    enum { e_maxVertices = 3 * 512 };
    Vector2f m_vertices[e_maxVertices];
    sPE_Color m_colors[e_maxVertices];

    uint m_count;

    GLuint m_vaoId;
    GLuint m_vboIds[2];
    GLuint m_programId;
    GLint m_projectionUniform;
    GLint m_vertexAttribute;
    GLint m_colorAttribute;

    Camera* gCamera;
};


sPE_renderTarget::sPE_renderTarget()
{
    m_width = SCR_WIDTH;
    m_height = SCR_HEIGHT;
    //init FrameBuffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    sCheckGLError();

}
void sPE_renderTarget::Destroy()
{
    if(fbo)
    {
        glDeleteFramebuffers(1, &fbo);
    }
    if(rbo)
    {
        glDeleteRenderbuffers(1, &rbo);
    }
    if(texture)
    {
        glDeleteTextures(1, &texture);
    }
    return;


}

void sPE_renderTarget::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

}

void sPE_renderTarget::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//
sPE_Draw::sPE_Draw()
{
    m_points = NULL;
    m_lines = NULL;
    m_triangles = NULL;
}

//
sPE_Draw::~sPE_Draw()
{
    sPE_Assert(m_points == NULL);
    sPE_Assert(m_lines == NULL);
    sPE_Assert(m_triangles == NULL);
}

//
void sPE_Draw::Create()
{
    camera = new Camera();
    m_points = new GLRenderPoints;
    m_points->Create(camera);
    m_lines = new GLRenderLines;
    m_lines->Create(camera);
    m_triangles = new GLRenderTriangles;
    m_triangles->Create(camera);
    renderTarget = new sPE_renderTarget();

}

//
void sPE_Draw::Destroy()
{
    m_points->Destroy();
    delete m_points;
    m_points = NULL;

    m_lines->Destroy();
    delete m_lines;
    m_lines = NULL;

    m_triangles->Destroy();
    delete m_triangles;
    m_triangles = NULL;

    renderTarget->Destroy();
    delete renderTarget;
    renderTarget = NULL;

    delete camera;
    return;
}

void sPE_Draw::DrawPolygon(const Vector2f* vertices, uint vertexCount, const sPE_Color& color)
{
    Vector2f p1 = vertices[vertexCount - 1];
    for (uint i = 0; i < vertexCount; ++i)
    {
        Vector2f p2 = vertices[i];
        m_lines->Vertex(p1, color);
        m_lines->Vertex(p2, color);
        p1 = p2;
    }
}

//
void sPE_Draw::DrawPoint(const Vector2f& p, float size, const sPE_Color& color)
{
    m_points->Vertex(p, color, size);
}

//
void sPE_Draw::Flush()
{

    renderTarget->Bind();
    m_triangles->Flush();
    m_lines->Flush();
    m_points->Flush();
    renderTarget->Unbind();
}



