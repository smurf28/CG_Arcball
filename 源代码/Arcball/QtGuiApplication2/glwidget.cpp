#include "glwidget.h"
#include <QMouseEvent>
#include <iostream>
using namespace std;
static const char *myVertexShaderSource =
"in vec4 vPosition;\n"
"in vec3 normal;\n"
"out vec3 N;\n"
"out vec3 L;\n"
"uniform mat3 normalMatrix;\n"
"uniform vec3 lightPos;\n"
"uniform mat4 projMatrix;\n"
"uniform mat4 mvMatrix;\n"
"void main(){\n"
"   gl_Position = projMatrix * mvMatrix * vPosition;\n"
"   N = normalize(normalMatrix * normal);\n"
"	L = normalize(lightPos - vPosition.xyz);\n"
"}\n";

/*
static const char *myVertexShaderSource =
"in vec4 vPosition;\n"
"in vec3 normal;\n"
"uniform mat4 projMatrix;\n"
"uniform mat4 mvMatrix;\n"
"uniform mat3 normalMatrix;\n"
"uniform vec3 lightPos;\n"
"out vec4 veryingColor;\n"
"void main(){\n"
"    gl_Position = projMatrix * mvMatrix * vPosition;\n"
"	 vec4 objectColor = vec4(0.5, 0.4, 0.8, 1.0);\n"
"	 veryingColor = dot(normalize(normalMatrix*normal), normalize(lightPos-vPosition.xyz)) * objectColor;\n"
"}\n";

*/

static const char *myFragmentShaderSource =
"in vec3 N;\n"
"in vec3 L;\n"
"out vec4 fColor;\n"
"void main(){\n"
"   vec4 aColor = vec4(0.1 ,0.1 , 0.1 , 0.0);\n"
"   vec4 dColor = vec4(0.5 ,0.4 , 0.8 , 1.0);\n"
"   vec4 sColor = vec4(1.0 ,1.0 , 1.0 , 0.0);\n"
"	vec3 r = normalize(reflect(-L, N));\n"
"	fColor = aColor + dColor * dot(N, L) + sColor * pow(dot(r,N), 128.0);\n"
"}\n";

GLWidget::GLWidget(QWidget *parent)
	: QOpenGLWidget(parent), m_indexBuf(QOpenGLBuffer::IndexBuffer),isShowNormal(false)
{
	
}
GLWidget::~GLWidget() {}

QSize GLWidget::minimumSizeHint() const
{
	return QSize(500, 500);
}

void GLWidget::initializeGL()
{
	initializeOpenGLFunctions();
	int m_transparent = 0;
	glClearColor(0.8, 0.8, 0.8, 1);

	m_program = new QOpenGLShaderProgram;
	m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, myVertexShaderSource);
	m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, myFragmentShaderSource);
	m_program->bindAttributeLocation("vertex", 0);
	m_program->bindAttributeLocation("normal", 1);

	m_program->link();
	m_program->bind();

	m_projMatrixLoc = m_program->uniformLocation("projMatrix");
	m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");
	m_program->setUniformValue(m_program->uniformLocation("lightPos"), QVector3D(35, 35, 70));

	m_vao.create();
	QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

	// Setup our vertex buffer object.
	m_vertexBuf.create();
	m_vertexBuf.bind();
	m_vertexBuf.allocate(this->vArr_c, this->vertex.size() * sizeof(QVector3D));


	m_indexBuf.create();
	m_indexBuf.bind();
	m_indexBuf.allocate(fSets_c, this->face.size() * sizeof(int));
	// Store the vertex attribute bindings for the program.
	//  setupVertexAttribs();

	m_program->enableAttributeArray(0);
	m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 2 * sizeof(QVector3D));
	m_program->enableAttributeArray(1);
	m_program->setAttributeBuffer(1, GL_FLOAT, sizeof(QVector3D), 3, 2 * sizeof(QVector3D));
	// Our camera never changes in this example.
	m_camera.setToIdentity();
	m_camera.translate(0, 0, -4);

	m_program->release();

	m_world.setToIdentity();
	rotateAngle = 0;
	rotateAxis = QVector3D(1, 0, 0);
}

void GLWidget::loadFile(QString fileName) {
	qDebug() << fileName;
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Can't open the file!" << endl;
		return;
	}
	while (!file.atEnd()) {
		QByteArray line = file.readLine();
		process_line(line);
	}
	file.close();

	vArr_c = new QVector3D[vertex.size()];
	fSets_c = new int[face.size()];

	vArr_c = &vertex[0];
	fSets_c = &face[0];

	initializeGL();
}


void GLWidget::process_line(QByteArray array) {
	QVector3D Vntmp, Vtmp;
	QString string = QString(array);
	QStringList line = string.split(" ");
	if (line[0] == "vn")
	{
		Vntmp.setX(line[1].toFloat());
		Vntmp.setY(line[2].toFloat());
		Vntmp.setZ(line[3].toFloat());
		this->vertex.push_back(Vntmp);
	}
	if (line[0] == "v") {
		Vtmp.setX(line[1].toFloat());
		Vtmp.setY(line[2].toFloat());
		Vtmp.setZ(line[3].toFloat());
		Vntmp = vertex[vertex.size() - 1];
		vertex.pop_back();
		vertex.push_back(Vtmp);
		vertex.push_back(Vntmp);
	}
	if (line[0] == "f")
	{
		for (int i = 1; i<line.size(); i++)
		{
			QStringList t = line[i].split("//");
			face.push_back(t[0].toInt() - 1);
		}
	}

}

void GLWidget::setupVertexAttribs()
{
	//    m_vbo.bind();
	m_vertexBuf.bind();
	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
	f->glEnableVertexAttribArray(0);
	f->glEnableVertexAttribArray(1);
	f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
	f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
	//    m_vbo.release();
	m_vertexBuf.release();
}

void GLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	if (this->isShowNormal) {
		QVector3D v, vn;
		for (int i = 0; i < this->vertex.size(); i += 2) {

			glBegin(GL_LINES);
			glColor3f(1, 0, 0);
			v = vertex[i];
			vn = vertex[i + 1];

			v.normalize();
			v = v * 0.6;
			glVertex3f(v.x(), v.y(), v.z());

			vn.normalize();
			vn = v + vn * 0.2;
			glVertex3f(vn.x(), vn.y(), vn.z());

			glEnd();

		}
	}

	QMatrix4x4 tmp;
	tmp.rotate(rotateAngle*180.0 / 3.14, rotateAxis);
	m_world = tmp * m_world;

	QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
	m_program->bind();

	m_program->setUniformValue(m_projMatrixLoc, m_proj);
	m_program->setUniformValue(m_mvMatrixLoc, m_camera * m_world);
	m_program->setUniformValue(m_program->uniformLocation("normalMatrix"), m_world.normalMatrix());

	glDrawElements(GL_TRIANGLES, this->face.size(), GL_UNSIGNED_INT, 0);

	m_program->release();
}

void GLWidget::controlNormal() {
	if (this->isShowNormal)
		this->isShowNormal = false;
	else
		this->isShowNormal = true;
	update();
}

void GLWidget::resizeGL(int w, int h)
{
	m_proj.setToIdentity();
	m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
	m_lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	m_currPos = event->pos();
	if (event->buttons() & Qt::LeftButton) {
		arcBall(m_lastPos, m_currPos);
		update();
	}
	m_lastPos = m_currPos;
}
double GLWidget::dot(QVector<double> v1, QVector<double> v2) {
	double cross = 0;
	for (int i = 0; i<v1.size(); i++) {
		cross += v1[i] * v2[i];
	}
	return cross;
}

double GLWidget::angle(QVector<double> v1, QVector<double> v2) {
	return acos(dot(v1, v2) / (sqrt(dot(v1, v1))  * sqrt(dot(v2, v2))));
}

QVector<double> GLWidget::crossProduct(QVector<double> v1, QVector<double> v2) {
	QVector<double> v(3);
	v[0] = v1[1] * v2[2] - v1[2] * v2[1];
	v[1] = v1[2] * v2[0] - v1[0] * v2[2];
	v[2] = v1[0] * v2[1] - v1[1] * v2[0];
	return v;
}

void GLWidget::arcBall(QPoint last_pnt, QPoint curr_pnt) {

	int x = last_pnt.x();
	int y = last_pnt.y();
	double z1, z2;
	QVector<double> vec1(3);
	QVector<double> vec2(3);
	QVector<double> vec3(3);
	vec3[0] = 1.0;
	vec3[1] = 1.0;
	vec3[2] = 1.0;
	double temp1 = 2.0*x / 500 - 1;
	double temp2 = -(2.0*y / 500 - 1);
	double temp = x ^ 2 + y ^ 2;
	if (temp <= 1) {
		z1 = sqrt(1 - temp);
	}
	else {
		z1 = 0;
	}
	vec1[0] = temp1 - 0;
	vec1[1] = temp2 - 0;
	vec1[2] = z1;
	x = curr_pnt.x();
	y = curr_pnt.y();
	temp1 = 2.0*x / 500 - 1;
	temp2 = -(2.0*y / 500 - 1);
	temp = x ^ 2 + y ^ 2;
	if (temp <= 1) {
		z2 = sqrt(1 - temp);
	}
	else {
		z2 = 0;
	}
	vec2[0] = temp1 - 0;
	vec2[1] = temp2 - 0;
	vec2[2] = z2;

	rotateAngle = angle(vec1, vec2);

	QVector<double> v = crossProduct(vec1, vec2);
	double distance = sqrt(dot(v, v));
	for (int i = 0; i < v.size(); i++)
		v[i] /= distance;

	rotateAxis.setX(v[0]);
	rotateAxis.setY(v[1]);
	rotateAxis.setZ(v[2]);
}

