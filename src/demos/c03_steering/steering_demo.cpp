/*
 * The steering behaviour demo.
 *
 * Part of the Artificial Intelligence for Games system.
 *
 * Copyright (c) Ian Millington 2003-2006. All Rights Reserved.
 *
 * This software is distributed under licence. Use of this software
 * implies agreement with all terms and conditions of the accompanying
 * software licence.
 */
#include <cstdlib>
#include <cstdio>

#include <gl/glut.h>
#include <aicore/aicore.h>

#include "../common/gl/app.h"

// This is the size of the world in both directions from 0 (i.e. from
// -WORLD_SIZE to +WORLD_SIZE)
#define WORLD_SIZE 50

// This is the gap between grid lines
#define GRID_SIZE 5

/**
 * The main demo class.
 */
class SteeringDemo : public Application
{
    /** Holds the kinematic of the two agents. */
    aicore::Kinematic *kinematic;

    /** Holds the steering behaviours. */
    aicore::Seek *seek;
    aicore::Flee *flee;
	aicore::Wander *wander;
	aicore::Arrive *arrive;
	aicore::Align *align;

	/* DELEGATED BEHAVIORS */
	aicore::Pursue *pursue;
	aicore::Evade *evade;

    /** Holds the currently used behaviours. */
    aicore::SteeringBehaviour **current;
    const char **status;

public:
    SteeringDemo();
    virtual ~SteeringDemo();

    virtual bool key(unsigned char key);
    virtual void update();
    virtual void display();
    virtual const char* getTitle();
    virtual void getHelpText(const char*** lines, unsigned *lineCount);
    virtual unsigned getStatusCount();
    virtual const char* getStatusText(unsigned slot);
};


SteeringDemo::SteeringDemo()
    :
    Application()
{
    static const aicore::real accel = (aicore::real)20.0;
	static const aicore::real prediction = (aicore::real) 10.0;

    kinematic = new aicore::Kinematic[2];

    kinematic[0].position.x = (aicore::real)10.0;
    kinematic[0].position.z = (aicore::real)10.0;
    kinematic[0].orientation = (aicore::real)2.1;
    kinematic[0].velocity.clear();
    kinematic[0].rotation = (aicore::real)0;

    kinematic[1].position.x = (aicore::real)-10.0;
    kinematic[1].position.z = (aicore::real)-20.0;
    kinematic[1].orientation = (aicore::real)0.75;
    kinematic[1].velocity.clear();
    kinematic[1].rotation = (aicore::real)0;

    // Set up all possible behaviours
    seek = new aicore::Seek[2];
    flee = new aicore::Flee[2];
    wander = new aicore::Wander[2];
	arrive = new aicore::Arrive[2];
	align = new aicore::Align[2];
	pursue = new aicore::Pursue[2];
	evade = new aicore::Evade[2];

    for (unsigned i = 0; i < 2; i++) {
        seek[i].character = &kinematic[i];
        seek[i].target = &kinematic[1-i].position;
        seek[i].maxAcceleration = accel;

        flee[i].character = &kinematic[i];
        flee[i].target = &kinematic[1-i].position;
        flee[i].maxAcceleration = accel;

        wander[i].character = &kinematic[i];
        wander[i].maxAcceleration = accel;
		wander[i].volatility = (aicore::real)20.0;
		wander[i].turnSpeed = (aicore::real)2.0;

		arrive[i].character = &kinematic[i];
		arrive[i].target = &kinematic[1 - i].position;
		arrive[i].maxAcceleration = accel;
		arrive[i].targetRadius = 5.0f;
		arrive[i].slowRadius = 10.0f;

		align[i].character = &kinematic[i];
		align[i].targetOrientation = &kinematic[1 - i].orientation;
		align[i].maxAngularAcceleration = aicore::M_PI_4;
		align[i].targetRadius = aicore::M_PI / 10.0;
		align[i].slowRadius = aicore::M_PI/ 5.0;

		pursue[i].character = &kinematic[i];
		pursue[i].target = &kinematic[1 - i].position;
		pursue[i].targetVelocity = &kinematic[1 - i].velocity;
		pursue[i].maxPrediction = prediction;
		pursue[i].maxAcceleration = accel;

		evade[i].character = &kinematic[i];
		evade[i].target = &kinematic[1 - i].position;
		evade[i].targetVelocity = &kinematic[1 - i].velocity;
		evade[i].maxPrediction = prediction;
		evade[i].maxAcceleration = accel;

    }

    // Set the current behaviours
    current = new aicore::SteeringBehaviour*[2];
    status = new const char*[2];

    current[0] = &seek[0];
    status[0] = "Seek";
    current[1] = &flee[1];
    status[1] = "Flee";
}

SteeringDemo::~SteeringDemo()
{
    delete[] kinematic;

    delete[] seek;
    delete[] flee;
    delete[] wander;
	delete[] arrive;

    delete[] current;
    delete[] status;
}

static const char *defaultHelp[] = {
    "AI4G: Steering Demo1",
    "H - Toggle help.",
    "",
    "Red character:",
    "Q - Stationary",
    "W - Seek",
    "E - Flee",
    "R - Wander",
	"T - Arrive",
	"Y - Align",
	"U - Pursue",
	"I - Evade",
    "",
    "Green character:",
    "A - Stationary",
    "S - Seek",
    "D - Flee",
	"F - Wander",
	"G - Arrive",
	"J - Align",
	"K - Pursue",
	"L - Evade"
};

void SteeringDemo::getHelpText(const char*** lines,
                                        unsigned *lineCount)
{
    *lines = defaultHelp;
    *lineCount = sizeof(defaultHelp) / sizeof(const char*);
}

const char* SteeringDemo::getTitle()
{
    return defaultHelp[0];
}

bool SteeringDemo::key(unsigned char key)
{
    if (Application::key(key)) return true;

    switch(key) {
    case 'Q': case 'q':
        current[0] = NULL;
        status[0] = "Stationary";
        return true;
    case 'W': case 'w':
        current[0] = &seek[0];
        status[0] = "Seek";
        return true;
    case 'E': case 'e':
        current[0] = &flee[0];
        status[0] = "Flee";
        return true;
	case 'R': case 'r':
		current[0] = &wander[0];
		status[0] = "Wander";
		return true;
	case 'T': case 't':
		current[0] = &arrive[0];
		status[0] = "Arrive";
		return true;
	case 'Y': case 'y':
		current[0] = &align[0];
		status[0] = "Align";
		return true;
	case 'U': case 'u':
		current[0] = &pursue[0];
		status[0] = "Pursue";
		return true;
	case 'I': case 'i':
		current[0] = &evade[0];
		status[0] = "Evade";
		return true;

    case 'A': case 'a':
        current[1] = NULL;
        status[1] = "Stationary";
        return true;
    case 'S': case 's':
        current[1] = &seek[1];
        status[1] = "Seek";
        return true;
    case 'D': case 'd':
        current[1] = &flee[1];
        status[1] = "Flee";
        return true;
	case 'F': case 'f':
		current[1] = &wander[1];
		status[1] = "Wander";
		return true;
	case 'G': case 'g':
		current[1] = &arrive[1];
		status[1] = "Arrive";
		return true;
	case 'J': case 'j':
		current[1] = &align[1];
		status[1] = "Align";
		return true;
	case 'K': case 'k':
		current[1] = &pursue[1];
		status[1] = "Pursue";
		return true;
	case 'L': case 'l':
		current[1] = &evade[1];
		status[1] = "Evade";
		return true;
    }

    return false;
}

void SteeringDemo::display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(-53.0f, 53.0f, 0.0f,
              0.0f, -30.0f, 0.0f,
              0.0f, 1.0f, 0.0f);

    // Draw the characters.
    glColor3f(0.6f, 0.0f, 0.0f);
    renderAgent(kinematic[0]);
    glColor3f(0.0f, 0.6f, 0.0f);
    renderAgent(kinematic[1]);

    // Draw some scale lines
    glColor3f(0.8, 0.8, 0.8);
    glBegin(GL_LINES);
    for (int i = -WORLD_SIZE; i <= WORLD_SIZE; i += GRID_SIZE) {

        glVertex3i(-WORLD_SIZE, -1, i);
        glVertex3i(WORLD_SIZE, -1, i);

        glVertex3i(i, -1, WORLD_SIZE);
        glVertex3i(i, -1, -WORLD_SIZE);
    }
    glEnd();

    // Draw the help (the method decides if it should be displayed)
    displayHelp();
}

#define TRIM_WORLD(var) \
    if (var < -WORLD_SIZE) var = WORLD_SIZE; \
    if (var > WORLD_SIZE) var = -WORLD_SIZE;

void SteeringDemo::update()
{
    float duration = aicore::TimingData::get().lastFrameDuration * 0.001f;
    aicore::SteeringOutput steer;

    for (unsigned i = 0; i < 2; i++) {

        // Update the steering if we have a movement algorithm
        if (current[i]) current[i]->getSteering(&steer);
        else steer.clear();

		if (current[i])
		{
			// Update the kinematic
			kinematic[i].integrate(steer, (aicore::real)0.95, duration);
			if (current[i] != &align[i])
				kinematic[i].setOrientationFromVelocity();

			// Check for maximum speed
			kinematic[i].trimMaxSpeed((aicore::real)20.0);
		}
		
        // Keep in bounds of the world
        TRIM_WORLD(kinematic[i].position.x);
        TRIM_WORLD(kinematic[i].position.z);
    }

    glutPostRedisplay();
}

unsigned SteeringDemo::getStatusCount()
{
    return 2;
}

const char* SteeringDemo::getStatusText(unsigned slot)
{
    // Set the color
    switch(slot)
    {
    case 0:
        glColor3f(0.6f, 0.0f, 0.0f);
        break;
    case 1:
        glColor3f(0.0f, 0.6f, 0.0f);
        break;
    }

    // Return the text
    return status[slot];
}


Application* getApplication()
{
    return new SteeringDemo();
}
