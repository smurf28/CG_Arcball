#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include "qopengl.h"
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include<QString>
#include<vector>
using namespace std;
class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	GLWidget(QWidget *parent = 0);
	~GLWidget();

	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int width, int height) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	QSize minimumSizeHint() const override;
	void setupVertexAttribs();
	double dot(QVector<double> v1, QVector<double> v2);
	double angle(QVector<double> v1, QVector<double> v2);
	QVector<double> crossProduct(QVector<double> v1, QVector<double> v2);
	void arcBall(QPoint last_pnt, QPoint curr_pnt);

	void process_line(QByteArray line);
	QOpenGLVertexArrayObject m_vao;
	QOpenGLBuffer m_vbo;
	QOpenGLBuffer m_vertexBuf;
	QOpenGLBuffer m_indexBuf;
	QOpenGLShaderProgram *m_program;
	QMatrix4x4 m_proj;
	QMatrix4x4 m_camera;
	QMatrix4x4 m_world;
	QMatrix4x4 m_world_tmp;
	int m_mvMatrixLoc;
	int m_projMatrixLoc;
	QPoint m_lastPos;
	QPoint m_currPos;
	QVector3D rotateAxis;
	float rotateAngle;

	vector<QVector3D> vertex;
	vector<int> face;
	QVector3D *vArr_c;
	int *fSets_c;
	bool isShowNormal;
	public slots:
		void loadFile(QString);
		void controlNormal();
};

#endif
