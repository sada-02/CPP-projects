#include<iostream>
#include<vector>
#include<deque>
#include<cmath>
#include<cstdlib>
#include<ctime>
#include<GL/glew.h>
#include<GLFW/glfw3.h>

float PI = 3.14159265358979323846f;
float DT = 0.016f;
int WINDOW_WIDTH = 1200;
int WINDOW_HEIGHT = 900;
const float G_SCALED = 1.0f; 

Vector3D cameraPosition(0.0f, 20.0f, 80.0f);
Vector3D cameraTarget(0.0f, 0.0f, 0.0f);
float cameraYaw = 0.0f;
float cameraPitch = 0.0f;
float cameraSpeed = 2.0f;
bool mousePressed = false;
double lastMouseX, lastMouseY;
bool wPressed = false, sPressed = false, aPressed = false, dPressed = false;
bool spacePressed = false, shiftPressed = false;

struct Vector3D {
    float x , y , z;
    Vector3D() : x(0.0f) , y(0.0f) , z(0.0f) {}
    Vector3D(float xVal , float yVal , float zVal) : x(xVal) , y(yVal) , z(zVal) {}

    Vector3D operator+(const Vector3D& v) const {
        return Vector3D(x + v.x , y + v.y , z + v.z);
    }
    Vector3D operator-(const Vector3D& v) const {
        return Vector3D(x - v.x , y - v.y , z - v.z);
    }
    Vector3D operator*(float scalar) const {
        return Vector3D(x * scalar , y * scalar , z * scalar);
    }
    Vector3D operator/(float scalar) const {
        if(scalar == 0.0f) return Vector3D(0.0f, 0.0f, 0.0f);
        return Vector3D(x / scalar , y / scalar , z / scalar);
    }
};

float dotProduct(const Vector3D& v1 , const Vector3D& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
float magnitude(const Vector3D& v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}
Vector3D normalize(const Vector3D& v) {
    float mag = magnitude(v);
    if(mag == 0.0f) return Vector3D(0.0f , 0.0f , 0.0f);
    return v / mag ;
}

Vector3D crossProduct(const Vector3D& a, const Vector3D& b) {
    return Vector3D(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

void keyCallback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_W) wPressed = true;
        if (key == GLFW_KEY_S) sPressed = true;
        if (key == GLFW_KEY_A) aPressed = true;
        if (key == GLFW_KEY_D) dPressed = true;
        if (key == GLFW_KEY_SPACE) spacePressed = true;
        if (key == GLFW_KEY_LEFT_SHIFT) shiftPressed = true;
        if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GLFW_TRUE);
    } else if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_W) wPressed = false;
        if (key == GLFW_KEY_S) sPressed = false;
        if (key == GLFW_KEY_A) aPressed = false;
        if (key == GLFW_KEY_D) dPressed = false;
        if (key == GLFW_KEY_SPACE) spacePressed = false;
        if (key == GLFW_KEY_LEFT_SHIFT) shiftPressed = false;
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int /*mods*/) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            mousePressed = true;
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
        } else if (action == GLFW_RELEASE) {
            mousePressed = false;
        }
    }
}

void mouseCursorCallback(GLFWwindow* /*window*/, double xpos, double ypos) {
    if (mousePressed) {
        float deltaX = (float)(xpos - lastMouseX);
        float deltaY = (float)(ypos - lastMouseY);
        
        cameraYaw += deltaX * 0.003f;
        cameraPitch -= deltaY * 0.003f;
        
        // Clamp pitch
        if (cameraPitch > PI/2 - 0.1f) cameraPitch = PI/2 - 0.1f;
        if (cameraPitch < -PI/2 + 0.1f) cameraPitch = -PI/2 + 0.1f;
        
        lastMouseX = xpos;
        lastMouseY = ypos;
    }
}

void scrollCallback(GLFWwindow* /*window*/, double /*xoffset*/, double yoffset) {
    cameraSpeed += (float)yoffset * 0.5f;
    if (cameraSpeed < 0.5f) cameraSpeed = 0.5f;
    if (cameraSpeed > 20.0f) cameraSpeed = 20.0f;
}

void updateCamera(float dt) {
    Vector3D forward(
        cosf(cameraPitch) * sinf(cameraYaw),
        sinf(cameraPitch),
        cosf(cameraPitch) * cosf(cameraYaw)
    );
    forward = normalize(forward);
    
    Vector3D worldUp(0.0f, 1.0f, 0.0f);
    Vector3D right = normalize(crossProduct(forward, worldUp));
    Vector3D up = normalize(crossProduct(right, forward));
    
    float speed = cameraSpeed * dt * 60.0f; 
    
    if (wPressed) cameraPosition = cameraPosition + forward * speed;
    if (sPressed) cameraPosition = cameraPosition - forward * speed;
    if (aPressed) cameraPosition = cameraPosition - right * speed;
    if (dPressed) cameraPosition = cameraPosition + right * speed;
    if (spacePressed) cameraPosition = cameraPosition + up * speed;
    if (shiftPressed) cameraPosition = cameraPosition - up * speed;
    
    cameraTarget = cameraPosition + forward;
}

void setupCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    float aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
    float fov = 60.0f * PI / 180.0f;
    float nearPlane = 0.1f;
    float farPlane = 500.0f;
    
    float f = 1.0f / tanf(fov / 2.0f);
    float rangeInv = 1.0f / (nearPlane - farPlane);
    
    float projMatrix[16] = {
        f / aspectRatio, 0.0f, 0.0f, 0.0f,
        0.0f, f, 0.0f, 0.0f,
        0.0f, 0.0f, (nearPlane + farPlane) * rangeInv, -1.0f,
        0.0f, 0.0f, nearPlane * farPlane * rangeInv * 2.0f, 0.0f
    };
    glMultMatrixf(projMatrix);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    Vector3D forward = cameraTarget - cameraPosition;
    forward = normalize(forward);
    
    Vector3D worldUp(0.0f, 1.0f, 0.0f);
    Vector3D right = normalize(crossProduct(forward, worldUp));
    Vector3D up = normalize(crossProduct(right, forward));
    
    float viewMatrix[16] = {
        right.x, up.x, -forward.x, 0.0f,
        right.y, up.y, -forward.y, 0.0f,
        right.z, up.z, -forward.z, 0.0f,
        -dotProduct(right, cameraPosition), -dotProduct(up, cameraPosition), dotProduct(forward, cameraPosition), 1.0f
    };
    glMultMatrixf(viewMatrix);
}

struct Planet {
    Vector3D position;
    float radius;
    float redColor , greenColor , blueColor; 
    float mass;
    Vector3D velocity;
    Vector3D acceleration;
    bool isSun;
    std::deque<Vector3D> trail;
    int maxTrailLength;

    Planet() : position(0.0f, 0.0f, 0.0f), radius(0.05f), redColor(1.0f), greenColor(1.0f), blueColor(1.0f), 
               mass(1.0f), velocity(0.0f, 0.0f, 0.0f), acceleration(0.0f, 0.0f, 0.0f), isSun(false), maxTrailLength(1000) {}

    Planet(float x, float y, float z, float r, float red, float green, float blue, float massPlanet, float vx, float vy, float vz, bool sun = false) 
        : position(x, y, z), radius(r), redColor(red), greenColor(green), blueColor(blue), mass(massPlanet), 
          velocity(vx, vy, vz), acceleration(0.0f, 0.0f, 0.0f), isSun(sun), maxTrailLength(1000) {}

    void applyForce(const Vector3D& force) {
        if (!isSun) {
            acceleration = acceleration + force / mass;
        }
    }

    void updatePosition(float dt) {
        if (isSun) return;
        
        velocity = velocity + acceleration * dt;
        position = position + velocity * dt;
        
        static int frameCount = 0;
        if (++frameCount % 2 == 0) {
            trail.push_back(position);
            if ((int)trail.size() > maxTrailLength) {
                trail.pop_front();
            }
        }
        
        acceleration = Vector3D(0.0f, 0.0f, 0.0f);
    }
};

struct Moon {
    Vector3D relativePosition;
    float radius;
    float redColor, greenColor, blueColor;
    float orbitRadius;
    float orbitSpeed;
    float orbitAngle;
    Planet* parentPlanet;
    std::deque<Vector3D> trail;
    int maxTrailLength;

    Moon(Planet* parent, float orbRadius, float speed, float r, float red, float green, float blue)
        : parentPlanet(parent), orbitRadius(orbRadius), orbitSpeed(speed), orbitAngle(0.0f),
          radius(r), redColor(red), greenColor(green), blueColor(blue), maxTrailLength(500) {}

    void update(float dt) {
        orbitAngle += orbitSpeed * dt;
        if (orbitAngle > 2 * PI) orbitAngle -= 2 * PI;
        
        relativePosition.x = orbitRadius * cosf(orbitAngle);
        relativePosition.y = 0.0f;
        relativePosition.z = orbitRadius * sinf(orbitAngle);
        
        static int frameCount = 0;
        if (++frameCount % 3 == 0) {
            Vector3D worldPos = parentPlanet->position + relativePosition;
            trail.push_back(worldPos);
            if ((int)trail.size() > maxTrailLength) {
                trail.pop_front();
            }
        }
    }

    Vector3D getWorldPosition() const {
        return parentPlanet->position + relativePosition;
    }
};

struct AsteroidBelt {
    std::vector<Vector3D> asteroids;
    float innerRadius;
    float outerRadius;
    float redColor, greenColor, blueColor;

    AsteroidBelt(float inner, float outer, int count, float r, float g, float b)
        : innerRadius(inner), outerRadius(outer), redColor(r), greenColor(g), blueColor(b) {
        
        for (int i = 0; i < count; i++) {
            float angle = (rand() / (float)RAND_MAX) * 2 * PI;
            float radius = innerRadius + (rand() / (float)RAND_MAX) * (outerRadius - innerRadius);
            float offsetY = ((rand() / (float)RAND_MAX) - 0.5f) * 0.5f;
            
            asteroids.push_back(Vector3D(
                radius * cosf(angle),
                offsetY,
                radius * sinf(angle)
            ));
        }
    }

    void render() {
        glColor3f(redColor, greenColor, blueColor);
        glPointSize(2.0f);
        glBegin(GL_POINTS);
        for (const auto& pos : asteroids) {
            glVertex3f(pos.x, pos.y, pos.z);
        }
        glEnd();
    }
};

Vector3D calculateGravitationalForce(const Planet& p1, const Planet& p2) {
    Vector3D delta = p2.position - p1.position;
    float distance = magnitude(delta);
    
    const float minDistance = 0.5f;
    if (distance < minDistance) distance = minDistance;

    float forceMagnitude = G_SCALED * p1.mass * p2.mass / (distance * distance);
    
    Vector3D direction = normalize(delta);
    return direction * forceMagnitude;
}

void createPlanet(const Planet& currPlanet, int latSegments = 20, int lonSegments = 20) {
    glColor3f(currPlanet.redColor, currPlanet.greenColor, currPlanet.blueColor);
    
    glPushMatrix();
    glTranslatef(currPlanet.position.x, currPlanet.position.y, currPlanet.position.z);
    
    for (int lat = 0; lat < latSegments; lat++) {
        float lat0 = PI * (-0.5f + (float)lat / latSegments);
        float lat1 = PI * (-0.5f + (float)(lat + 1) / latSegments);
        float y0 = sinf(lat0);
        float y1 = sinf(lat1);
        float cosLat0 = cosf(lat0);
        float cosLat1 = cosf(lat1);
        
        glBegin(GL_TRIANGLE_STRIP);
        for (int lon = 0; lon <= lonSegments; lon++) {
            float lon0 = 2 * PI * (float)lon / lonSegments;
            float x0 = cosf(lon0);
            float z0 = sinf(lon0);
            
            glVertex3f(currPlanet.radius * cosLat0 * x0, currPlanet.radius * y0, currPlanet.radius * cosLat0 * z0);
            glVertex3f(currPlanet.radius * cosLat1 * x0, currPlanet.radius * y1, currPlanet.radius * cosLat1 * z0);
        }
        glEnd();
    }
    
    glPopMatrix();
}

void createMoon(const Moon& moon) {
    Vector3D worldPos = moon.getWorldPosition();
    
    glColor3f(moon.redColor, moon.greenColor, moon.blueColor);
    glPushMatrix();
    glTranslatef(worldPos.x, worldPos.y, worldPos.z);
    
    int segments = 10;
    for (int lat = 0; lat < segments; lat++) {
        float lat0 = PI * (-0.5f + (float)lat / segments);
        float lat1 = PI * (-0.5f + (float)(lat + 1) / segments);
        float y0 = sinf(lat0);
        float y1 = sinf(lat1);
        float cosLat0 = cosf(lat0);
        float cosLat1 = cosf(lat1);
        
        glBegin(GL_TRIANGLE_STRIP);
        for (int lon = 0; lon <= segments; lon++) {
            float lon0 = 2 * PI * (float)lon / segments;
            float x0 = cosf(lon0);
            float z0 = sinf(lon0);
            
            glVertex3f(moon.radius * cosLat0 * x0, moon.radius * y0, moon.radius * cosLat0 * z0);
            glVertex3f(moon.radius * cosLat1 * x0, moon.radius * y1, moon.radius * cosLat1 * z0);
        }
        glEnd();
    }
    
    glPopMatrix();
}

void drawTrail(const std::deque<Vector3D>& trail, float r, float g, float b) {
    if (trail.size() < 2) return;
    
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    
    for (size_t i = 0; i < trail.size(); i++) {
        float alpha = (float)i / trail.size();
        glColor4f(r, g, b, alpha * 0.7f);
        glVertex3f(trail[i].x, trail[i].y, trail[i].z);
    }
    
    glEnd();
}

int main() {
    srand((unsigned int)time(NULL));
    
    if(!glfwInit()) {
        std::cout<<"Failed to initialise GLFW"<<std::endl;
        return 0;
    }

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH , WINDOW_HEIGHT , "Solar System - WASD/Space/Shift to move, Right-click drag to look, Scroll to change speed" , NULL , NULL);
    if(!window) {
        std::cout<<"Failed to create window"<<std::endl;
        glfwTerminate();
        return 0;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK) {
        std::cout<<"Failed to initialise GLEW"<<std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
    }

    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mouseCursorCallback);
    glfwSetScrollCallback(window, scrollCallback);
    
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(0.0f, 0.0f, 0.05f, 1.0f);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    std::vector<Planet> planets;
    
    planets.push_back(Planet(0.0f, 0.0f, 0.0f, 2.5f, 1.0f, 0.85f, 0.0f, 50000.0f, 0.0f, 0.0f, 0.0f, true));
    
    float mercuryDist = 15.0f;
    float mercurySpeed = sqrtf(G_SCALED * planets[0].mass / mercuryDist) * 0.98f;
    planets.push_back(Planet(mercuryDist, 0.0f, 0.0f, 0.2f, 0.7f, 0.7f, 0.7f, 0.03f, 0.0f, 0.0f, mercurySpeed));
    
    float venusDist = 25.0f;
    float venusSpeed = sqrtf(G_SCALED * planets[0].mass / venusDist) * 0.98f;
    planets.push_back(Planet(venusDist, 0.0f, 0.0f, 0.28f, 0.95f, 0.8f, 0.4f, 0.05f, 0.0f, 0.0f, venusSpeed));
    
    float earthDist = 35.0f;
    float earthSpeed = sqrtf(G_SCALED * planets[0].mass / earthDist) * 0.98f;
    planets.push_back(Planet(earthDist, 0.0f, 0.0f, 0.3f, 0.2f, 0.5f, 1.0f, 0.06f, 0.0f, 0.0f, earthSpeed));
    
    float marsDist = 50.0f;
    float marsSpeed = sqrtf(G_SCALED * planets[0].mass / marsDist) * 0.98f;
    planets.push_back(Planet(marsDist, 0.0f, 0.0f, 0.22f, 0.95f, 0.4f, 0.2f, 0.04f, 0.0f, 0.0f, marsSpeed));
    
    float jupiterDist = 80.0f;
    float jupiterSpeed = sqrtf(G_SCALED * planets[0].mass / jupiterDist) * 0.98f;
    planets.push_back(Planet(jupiterDist, 0.0f, 0.0f, 1.2f, 0.85f, 0.65f, 0.45f, 2.0f, 0.0f, 0.0f, jupiterSpeed));
    
    float saturnDist = 120.0f;
    float saturnSpeed = sqrtf(G_SCALED * planets[0].mass / saturnDist) * 0.98f;
    planets.push_back(Planet(saturnDist, 0.0f, 0.0f, 1.0f, 0.95f, 0.85f, 0.6f, 1.8f, 0.0f, 0.0f, saturnSpeed));
    
    float uranusDist = 160.0f;
    float uranusSpeed = sqrtf(G_SCALED * planets[0].mass / uranusDist) * 0.98f;
    planets.push_back(Planet(uranusDist, 0.0f, 0.0f, 0.7f, 0.4f, 0.8f, 0.95f, 1.2f, 0.0f, 0.0f, uranusSpeed));
    
    float neptuneDist = 200.0f;
    float neptuneSpeed = sqrtf(G_SCALED * planets[0].mass / neptuneDist) * 0.98f;
    planets.push_back(Planet(neptuneDist, 0.0f, 0.0f, 0.65f, 0.25f, 0.4f, 0.95f, 1.1f, 0.0f, 0.0f, neptuneSpeed));

    std::vector<Moon> moons;
    moons.push_back(Moon(&planets[3], 0.8f, 3.0f, 0.1f, 0.85f, 0.85f, 0.85f)); // Earth's moon
    moons.push_back(Moon(&planets[5], 2.0f, 2.0f, 0.18f, 0.9f, 0.75f, 0.6f)); // Jupiter moon 1
    moons.push_back(Moon(&planets[5], 2.8f, 1.5f, 0.15f, 0.75f, 0.65f, 0.5f)); // Jupiter moon 2
    moons.push_back(Moon(&planets[5], 3.6f, 1.1f, 0.13f, 0.85f, 0.7f, 0.55f)); // Jupiter moon 3
    moons.push_back(Moon(&planets[6], 2.2f, 1.7f, 0.16f, 0.88f, 0.78f, 0.58f)); // Saturn moon 1
    moons.push_back(Moon(&planets[6], 3.0f, 1.3f, 0.13f, 0.8f, 0.75f, 0.6f)); // Saturn moon 2

    AsteroidBelt asteroidBelt(60.0f, 75.0f, 3000, 0.65f, 0.65f, 0.65f);

    double lastTime = glfwGetTime();
    while(!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        float deltaTime = (float)(currentTime - lastTime);
        lastTime = currentTime;
        
        if (deltaTime > 0.05f) deltaTime = 0.05f;

        updateCamera(deltaTime);

        for(size_t i = 0; i < planets.size(); i++) {
            for(size_t j = i + 1; j < planets.size(); j++) {
                Vector3D force = calculateGravitationalForce(planets[i], planets[j]);
                planets[i].applyForce(force);
                planets[j].applyForce(force * -1.0f);
            }
        }

        for(auto& planet : planets) {
            planet.updatePosition(DT);
        }

        for(auto& moon : moons) {
            moon.update(DT);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        setupCamera();

        for(size_t i = 1; i < planets.size(); i++) {
            drawTrail(planets[i].trail, planets[i].redColor, planets[i].greenColor, planets[i].blueColor);
        }

        for(const auto& moon : moons) {
            drawTrail(moon.trail, moon.redColor * 0.7f, moon.greenColor * 0.7f, moon.blueColor * 0.7f);
        }

        asteroidBelt.render();

        glPushMatrix();
        glTranslatef(planets[6].position.x, planets[6].position.y, planets[6].position.z);
        glRotatef(15.0f, 1.0f, 0.0f, 0.0f);
        glColor4f(0.85f, 0.75f, 0.55f, 0.6f);
        glLineWidth(1.5f);
        for(int ring = 0; ring < 5; ring++) {
            float ringRadius = 1.4f + ring * 0.15f;
            glBegin(GL_LINE_LOOP);
            for(int i = 0; i < 60; i++) {
                float angle = (i / 60.0f) * 2 * PI;
                glVertex3f(ringRadius * cosf(angle), 0.0f, ringRadius * sinf(angle));
            }
            glEnd();
        }
        glPopMatrix();

        for(const auto& planet : planets) {
            createPlanet(planet);
        }

        for(const auto& moon : moons) {
            createMoon(moon);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}