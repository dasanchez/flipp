#ifndef GLPlotterWidget_H
#define GLPlotterWidget_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QObject>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <QFile>

#define ACTIVE_VERTICES 400
#define ANIMSPEED 25

static const char *vPath = "../flipp/plotter/shader2d.vert";
static const char *fPath = "../flipp/plotter/shader2d.frag";

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GLPlotterWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLPlotterWidget(QWidget *parent = 0);
    ~GLPlotterWidget();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    QTimer *plotTimer;

public slots:
    void cleanup();
    void moveCamera();
    void addDataPoint(double);
    void setXWindow(quint16);

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
//    void resizeGL(int width, int height) Q_DECL_OVERRIDE;
private:
    bool m_core;
    QOpenGLShaderProgram *shaderProgram;
    QOpenGLVertexArrayObject *vao;
    QOpenGLBuffer *vbo;

    QMatrix4x4 m_world;
    QMatrix4x4 m_camera;
    QMatrix4x4 m_proj;

    int matrixLocation;
    int projMatrixLoc;
    int mvMatrixLoc;
    int vColorLocation;

    void prepareXPos(quint16);
    GLfloat verts[2000];
    quint16 vertCount;
    quint16 vertPointer;
    GLfloat color[3];
};

#endif // GLPlotterWidget_H
