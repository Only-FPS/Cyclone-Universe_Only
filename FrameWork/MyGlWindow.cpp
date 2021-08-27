

#include "MyGlWindow.h"



#include <iostream>
#include "drawUtils.h"


#include "timing.h"


static double DEFAULT_VIEW_POINT[3] = { 600, 600, 600 };
static double DEFAULT_VIEW_CENTER[3] = { 0, 0, 0 };
static double DEFAULT_UP_VECTOR[3] = { 0, 1, 0 };

MyGlWindow::MyGlWindow(int x, int y, int w, int h) :
	Fl_Gl_Window(x, y, w, h)
	//==========================================================================
{

	mode(FL_RGB | FL_ALPHA | FL_DOUBLE | FL_STENCIL);

	fieldOfView = 45;

	glm::vec3 viewPoint(DEFAULT_VIEW_POINT[0], DEFAULT_VIEW_POINT[1], DEFAULT_VIEW_POINT[2]);
	glm::vec3 viewCenter(DEFAULT_VIEW_CENTER[0], DEFAULT_VIEW_CENTER[1], DEFAULT_VIEW_CENTER[2]);
	glm::vec3 upVector(DEFAULT_UP_VECTOR[0], DEFAULT_UP_VECTOR[1], DEFAULT_UP_VECTOR[2]);

	float aspect = (w / (float)h);
	m_viewer = new Viewer(viewPoint, viewCenter, upVector, 45.0f, aspect);

	for (int i = 0; i < 2500; i++) {
		m_mover.push_back(new Mover());
	}

	TimingData::init();
	run = 0;

}


void MyGlWindow::setupLight(float x, float y, float z)
{

	// set up the lighting
	GLfloat lightPosition[] = { 500, 900.0, 500, 1.0 };
	GLfloat lightPosition2[] = { 1, 0, 0, 0 };
	GLfloat lightPosition3[] = { 0, -1, 0, 0 };

	GLfloat violetLight[] = { 0.5f, 0.1f, .5f, 1.0 };
	GLfloat whiteLight[] = { 1, 1, 1, 1.0 };
	GLfloat whiteLight2[] = { .3, .3, .3, 1.0 };
	GLfloat blueLight[] = { .1f,.1f,.3f,1.0 };


	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_DEPTH);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);


	lightPosition[0] = x;
	lightPosition[1] = y;
	lightPosition[2] = z;

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_AMBIENT, whiteLight2);
	// 
	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, whiteLight);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}





void MyGlWindow::drawStuff()
{
	polygonf(4, 20., 0., -25., 20., 0., 25., 20., 30., 25., 20., 30., -25.);
}

//==========================================================================
void MyGlWindow::draw()
//==========================================================================
{

	glViewport(0, 0, w(), h());

	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0.2f, 0.2f, .2f, 0);		// background should be blue


	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);



	// now draw the ground plane
	setProjection();
	//setupFloor();

	//glPushMatrix();
	//drawFloor(200, 20);
	//glPopMatrix();


	setupLight(m_viewer->getViewPoint().x, m_viewer->getViewPoint().y, m_viewer->getViewPoint().z);


	// Add a sphere to the scene.
   //Draw axises
	//glLineWidth(3.0f);
	//glBegin(GL_LINES);
	//glColor3f(1, 0, 0);

	//glVertex3f(0, 0.1, 0);
	//glVertex3f(0, 100, 0);

	//glColor3f(0, 1, 0);

	//glVertex3f(0, 0.1, 0);
	//glVertex3f(100, 0.1, 0);

	//glColor3f(0, 0, 1);

	//glVertex3f(0, 0.1, 0);
	//glVertex3f(0, 0.1, 100);
	//glEnd();
	//glLineWidth(1.0f);




	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	//draw shadow
	setupShadows();
	for (unsigned int i = 0; i < m_mover.size(); i++) {
		if (m_mover[i]) m_mover[i]->draw(1);
	}
	unsetupShadows();


	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);

	//draw objects
	glPushMatrix();
	for (unsigned int i = 0; i < m_mover.size(); i++) {
		if (m_mover[i]) m_mover[i]->draw(0);
	}
	glPopMatrix();



	glEnable(GL_COLOR_MATERIAL);

}

void MyGlWindow::test()  //is called when you press test button.
{
	cyclone::Vector3 u(1, 1, 1);
	cyclone::Vector3 v(1, 0, 2);

	cyclone::Vector3 t = v.cross(u);
	cyclone::real a = t.magnitude();


}

void MyGlWindow::update()
{

	TimingData::get().update();

	if (!run) return;

	float duration = (float)TimingData::get().lastFrameDuration * 0.003;

	for (unsigned int i = 0; i < m_mover.size(); i++) {
		glm::vec3 _velocity(0.0f, 0.0f, 0.0f);

		// 모든 공 루프 (상대)
		for (unsigned int j = 0; j < m_mover.size(); j++) {
			if (m_mover[i] && i != j && m_mover[j]) { // 자신이 아닐경우

				cyclone::Vector3 _vec = m_mover[j]->m_particle->getPosition() - m_mover[i]->m_particle->getPosition();
				glm::vec3 _glmvec(_vec.x, _vec.y, _vec.z);

				// 거리 r 구하기
				cyclone::real distance = glm::length(_glmvec);
				if (distance > m_mover[i]->size || distance > m_mover[j]->size)
				{
					// F = G * M1 * M2 / r^2
					cyclone::real F = (0.01f * m_mover[i]->m_particle->getMass() * m_mover[j]->m_particle->getMass()) / (distance * distance);

					// 방향 * F
					_velocity = glm::vec3(_velocity.x + glm::normalize(_glmvec).x * F, _velocity.y + glm::normalize(_glmvec).y * F, _velocity.z + glm::normalize(_glmvec).z * F);
					m_mover[i]->addAccel(_velocity.x, _velocity.y, _velocity.z);
					//_velocity += (glm::normalize(_glmvec) * F);
				}
				//서로의 권역 안에 들어갔을 때 흡수되도록 하는 기능
				if (distance < m_mover[i]->size || distance < m_mover[j]->size)
				{
					//두 행성 간 벡터의 합에 쓰일 비율
					cyclone::real MassRatio1 = (m_mover[i]->m_particle->getMass()) / (m_mover[i]->m_particle->getMass() + m_mover[j]->m_particle->getMass());
					cyclone::real MassRatio2 = ABS(1.0 - MassRatio1);

					cyclone::Vector3 tempVelo(((m_mover[i]->m_particle->getVelocity().x * MassRatio1) + (m_mover[j]->m_particle->getVelocity().x * MassRatio2)) * 0.99f,
						((m_mover[i]->m_particle->getVelocity().y * MassRatio1) + (m_mover[j]->m_particle->getVelocity().y * MassRatio2)) * 0.99f,
						((m_mover[i]->m_particle->getVelocity().z * MassRatio1) + (m_mover[j]->m_particle->getVelocity().z * MassRatio2)) * 0.99f);

					//cyclone::Vector3 tempPosition(m_mover[i]->m_particle->getPosition() + m_mover[j]->m_particle->getPosition());
					//tempPosition = cyclone::Vector3(tempPosition.x / 2.0, tempPosition.y / 2.0, tempPosition.z / 2.0);

					//cyclone::real tempMass = (m_mover[i]->m_particle->getMass() + m_mover[j]->m_particle->getMass()) / 3.0;

					//크기가 큰 공에 작은 공이 흡수되도록
					if (MassRatio1 >= MassRatio2)
					{

						/*if (m_mover[i]->m_particle->getMass() > 300 && m_mover[j]->m_particle->getMass() > 300)
						{
							m_mover[i]->m_particle->setVelocity((m_mover[i]->m_particle->getVelocity() * -1));
							m_mover[j]->m_particle->setVelocity((m_mover[j]->m_particle->getVelocity() * -1));
							m_mover[i]->m_particle->setMass(tempMass);
							m_mover[j]->m_particle->setMass(tempMass);
							
							m_mover.push_back(new Mover(tempPosition * 1.1, tempVelo, tempMass));
							break;
						}
						else
						{*/
							//무게 비율별로 두 벡터를 합함
							m_mover[i]->m_particle->setVelocity(tempVelo);

							m_mover[i]->m_particle->setMass(m_mover[i]->m_particle->getMass() + m_mover[j]->m_particle->getMass());

							//합쳐졌으니 하나 제거
							m_mover.erase(m_mover.begin() + j);
						//}
					}
					else
					{
						//if (m_mover[i]->m_particle->getMass() > 300 && m_mover[j]->m_particle->getMass() > 300)
						//{
						//	m_mover[i]->m_particle->setVelocity((m_mover[i]->m_particle->getVelocity() * -1));
						//	m_mover[j]->m_particle->setVelocity((m_mover[j]->m_particle->getVelocity() * -1));
						//	m_mover[i]->m_particle->setMass(tempMass);
						//	m_mover[j]->m_particle->setMass(tempMass);

						//	m_mover.push_back(new Mover(tempPosition * 1.1, tempVelo, tempMass));
						//	break;
						//}
						//else
						//{
							//무게 비율별로 두 벡터를 합함
							m_mover[j]->m_particle->setVelocity(tempVelo);

							m_mover[j]->m_particle->setMass(m_mover[i]->m_particle->getMass() + m_mover[j]->m_particle->getMass());

							//합쳐졌으니 하나 제거
							m_mover.erase(m_mover.begin() + i);
						//}
					}
				}
			}
		}
		// 가속 적용
		//if(m_mover[i]) m_mover[i]->addAccel(_velocity.x, _velocity.y, _velocity.z);
	}

	for (unsigned int i = 0; i < m_mover.size(); i++) {
		if (m_mover[i]) m_mover[i]->update(duration);
	}
}


void MyGlWindow::doPick()
{
	make_current();		// since we'll need to do some GL stuff

	int mx = Fl::event_x(); // where is the mouse?
	int my = Fl::event_y();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	// set up the pick matrix on the stack - remember, FlTk is
	// upside down!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix((double)mx, (double)(viewport[3] - my), 5, 5, viewport);

	// now set up the projection
	setProjection();

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100, buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);

	// draw the cubes, loading the names as we go
	//for (size_t i = 0; i < world->points.size(); ++i) {
	//	glLoadName((GLuint)(i + 1));
	//	draw();
	//}

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		//selectedCube = buf[3] - 1;
	}
	else {// nothing hit, nothing selected
		//selectedCube = -1;
	}
	//printf("Selected Cube %d\n", selectedCube);
}



void MyGlWindow::setProjection(int clearProjection)
//==========================================================================
{
	glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, w(), h());
	if (clearProjection)
		glLoadIdentity();
	// compute the aspect ratio so we don't distort things
	double aspect = ((double)w()) / ((double)h());
	gluPerspective(fieldOfView, aspect, 1, 3000);

	// put the camera where we want it to be
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// use the transformation in the ArcBall

	gluLookAt(
		m_viewer->getViewPoint().x, m_viewer->getViewPoint().y, m_viewer->getViewPoint().z,
		m_viewer->getViewCenter().x, m_viewer->getViewCenter().y, m_viewer->getViewCenter().z,
		m_viewer->getUpVector().x, m_viewer->getUpVector().y, m_viewer->getUpVector().z
	);

	glDisable(GL_BLEND);
}

static int last_push;
int m_pressedMouseButton;
int m_lastMouseX;
int m_lastMouseY;

int MyGlWindow::handle(int e)
//==========================================================================
{

	switch (e) {
	case FL_SHOW:		// you must handle this, or not be seen!
		show();
		return 1;
	case FL_PUSH:
	{

		m_pressedMouseButton = Fl::event_button();
		m_lastMouseX = Fl::event_x();
		m_lastMouseY = Fl::event_y();


	}
	damage(1);
	return 1;
	case FL_RELEASE:
		m_pressedMouseButton = -1;
		damage(1);
		return 1;
	case FL_DRAG: // if the user drags the mouse
	{

		double fractionChangeX = static_cast<double>(Fl::event_x() - m_lastMouseX) / static_cast<double>(this->w());
		double fractionChangeY = static_cast<double>(m_lastMouseY - Fl::event_y()) / static_cast<double>(this->h());

		if (m_pressedMouseButton == 1) {
			m_viewer->rotate(fractionChangeX, fractionChangeY);
		}
		else if (m_pressedMouseButton == 2) {
			m_viewer->zoom(fractionChangeY);
		}
		else if (m_pressedMouseButton == 3) {
			m_viewer->translate(-fractionChangeX, -fractionChangeY, (Fl::event_key(FL_Shift_L) == 0) || (Fl::event_key(FL_Shift_R) == 0));
		}
		else {
			std::cout << "Warning: dragging with unknown mouse button!  Nothing will be done" << std::endl;
		}

		m_lastMouseX = Fl::event_x();
		m_lastMouseY = Fl::event_y();
		redraw();
	}

	return 1;

	case FL_KEYBOARD:
		return 0;

	default:
		return 0;
	}
}


//
// get the mouse in NDC
//==========================================================================
void MyGlWindow::getMouseNDC(float& x, float& y)
//==========================================================================
{
	// notice, we put everything into doubles so we can do the math
	float mx = (float)Fl::event_x();	// remeber where the mouse went down
	float my = (float)Fl::event_y();

	// we will assume that the viewport is the same as the window size
	float wd = (float)w();
	float hd = (float)h();

	// remember that FlTk has Y going the wrong way!
	my = hd - my;

	x = (mx / wd) * 2.0f - 1.f;
	y = (my / hd) * 2.0f - 1.f;
}