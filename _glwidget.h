#ifndef _GLWIDGET_H
#define _GLWIDGET_H

#include <QOpenGLWidget>
#include <QMouseEvent>
#include "_scene.h"

class _GLWidget :  public QOpenGLWidget
{
public:
    _GLWidget();
    explicit _GLWidget(QWidget *parent = 0);
    ~_GLWidget();
protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h)Q_DECL_OVERRIDE;
    void paintGL()Q_DECL_OVERRIDE;
    //
    virtual void mousePressEvent(QMouseEvent *e) override;
    virtual void mouseReleaseEvent(QMouseEvent *e) override;
    virtual void mouseMoveEvent(QMouseEvent *e) override;
    virtual void wheelEvent(QWheelEvent *e) override;
    virtual void  keyPressEvent(QKeyEvent *event) override;

private:
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;
    unsigned int shaderProgram;

    _Scene* scene;//Scene include renderer , camera and sceneentity classes, so no need to reinclude header
    _SceneEntity s, s1,s2;
    _SceneEntity background_quad;
    _AssetLoader assetLoader;
    _Camera cam;
    QVector2D mousePressPosition;

    //Varible for InputCallbackFunctions
    unsigned int id;
    float scroolScale;
    QVector2D rotRads;
};

#endif // _GLWIDGET_H
