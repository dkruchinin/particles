#include <iostream>
#include <sstream>
#include <SDL2/SDL.h>

#include "simulation.hpp"
#include "particle.hpp"
#include "event.hpp"

static const Uint8 background_r = 255;
static const Uint8 background_g = 255;
static const Uint8 background_b = 255;

static const int SPEED_MIN = 1;
static const int SPEED_MAX = 3;

Simulation::Simulation(int width, int height, int fps)
{
    this->width = width;
    this->height = height;
    this->fps = fps;
    is_paused = false;
    speed = SPEED_MIN;
    now = 0.0;
    delay_ms = 1000 / fps;
    window = nullptr;
    renderer = nullptr;

    if (SDL_CreateWindowAndRenderer(width, height, 0,
            &window, &renderer) != 0) {
        std::ostringstream oss;

        oss << "Failed to create window (SDL: "
            << SDL_GetError() << ")";
        throw SimulationError(oss.str());
    }

    resetBackgroundColor();
}

Simulation::~Simulation()
{
    if (renderer != nullptr)
        SDL_DestroyRenderer(renderer);
    if (window != nullptr)
        SDL_DestroyWindow(window);

    while (!events.empty()) {
        Event *ev = events.top();
        events.pop();
        delete ev;
    }
    while (!particles.empty()) {
        Particle *p = particles.back();
        particles.pop_back();
        delete p;
    }
}

void Simulation::addParticle(double x, double y, double vx, double vy,
                             double radius, int mass, int r, int g, int b)
{
    Particle *new_p = new Particle(x, y, vx, vy, radius, mass,
                                   width, height, r, g, b);

    // ensure that new particle does not overlap
    // with existing ones before adding it to the
    // simulation.
    for (Particle *p : particles) {
        if (new_p->overlaps(*p)) {
            std::ostringstream oss;

            oss << "Particle " << *new_p << " overlaps with "
                << "existing particle " << *p;
            throw SimulationError(oss.str());
        }
    }

    particles.push_back(new_p);
}

void Simulation::tick()
{
    // The idea behind event driven simulation is quite
    // ingenious: we determine the time of all collisions
    // happening between all particles and walls assuming
    // that particles move by straight lines at constant
    // speed without any resistance.
    //
    // We keep the collision events arranged by time in priority
    // queue, so that we always know when and what collisions
    // are going to happen.
    //
    // The expensive calculations have to be done only
    // once when the priority queue is initialised. By the expensive
    // calculations I mean the calculation of all collisions between
    // all available particles O(n^2). Then the event driven model
    // requires to recalculate new events only after some event (collision)
    // happens, which requires no more than O(N). That's why this
    // model is so swift.
    //
    // Of course some of the events in the queue have to be cancelled after
    // the collision event happens (since particle's trajectories change),
    // that's why the system allows to detect whether the event
    // is stale/cancelled.

    if (events.empty()) {
        if (particles.size() == 0) {
            throw SimulationError("Simulation can not be launched "
                                  "with 0 particles");
        }

        initializeEvents();
    }
    if (is_paused) {
        SDL_Delay(delay_ms);
        return;
    }

    bool enough = false;
    while (!enough) {
        Event *ev = events.top();

        events.pop();
        if (ev->isStale()) {
            delete ev;
            continue;
        }

        // simulation system does time related calculations
        // in relative time, not absolute. So we have to
        // translate relative time time to absolute one
        // and vice versa.
        moveParticles(ev->getTime() - now);
        SDL_Delay(simulationTimeToMS(ev->getTime() - now));
        now = ev->getTime();

        switch (ev->getType()) {
        case EventType::WallCollision:
        {
            // Particle collides a wall. This requires to calculate
            // the collisions of this particle with all other particles
            // and walls.
            WallCollisionEvent *wc_ev = dynamic_cast<WallCollisionEvent*>(ev);
            wc_ev->getParticle().bounceWall(wc_ev->getWallType());
            predictCollisions(wc_ev->getParticle());
            break;
        }

        case EventType::ParticleCollision:
        {
            // Two particles collide each other. This requires to calculate
            // the collisions of these two particles with all other particles
            // and walls.
            ParticleCollisionEvent *pc_ev = dynamic_cast<ParticleCollisionEvent*>(ev);
            pc_ev->getFirstParticle().bounceParticle(pc_ev->getSecondParticle());
            predictCollisions(pc_ev->getFirstParticle());
            predictCollisions(pc_ev->getSecondParticle());
            break;
        }

        case EventType::Refresh:
            refresh();
            enough = true;
            events.push(new RefreshEvent(now + MSToSimulationTime(delay_ms)));
            break;
        }

        delete ev;
    }
}

void Simulation::pause()
{
    is_paused = true;
}

void Simulation::resume()
{
    is_paused = false;
}

bool Simulation::paused() const
{
    return is_paused;
}

void Simulation::incSpeed()
{
    if (speed < SPEED_MAX)
        speed++;
}

void Simulation::decSpeed()
{
    if (speed > SPEED_MIN)
        speed--;
}

int Simulation::getSpeed() const
{
    return speed;
}

void Simulation::moveParticles(double dt)
{
    for (Particle *p: particles)
        p->move(dt);
}

void Simulation::refresh()
{
    SDL_RenderClear(renderer);
    for (Particle *p : particles) {
        SDL_SetRenderDrawColor(renderer, p->getR(), p->getG(),
                               p->getB(), 255);
        drawDisk(p->getX(), p->getY(), p->getRadius());
        resetBackgroundColor();
    }

    SDL_RenderPresent(renderer);
}

// SDL does not provide primitives for drawing lines
// and circles, so we use good old Bresenham's black
// magic to cast these shapes.
void Simulation::drawDisk(int x0, int y0, int radius)
{
    int x = 0, y = radius, d = 3 - 2 * radius;

    while (x <= y) {
        drawLine(x0 + x, y0 + y, x0 + x, y0 - y);
        drawLine(x0 - x, y0 + y, x0 - x, y0 - y);
        drawLine(x0 + y, y0 + x, x0 + y, y0 - x);
        drawLine(x0 - y, y0 + x, x0 - y, y0 - x);

        if (d <= 0)
            d += 4 * x + 6;
        else {
            d += 4 * (x - y) + 10;
            y--;
        }

        x++;
    }
}

// Expecto Patronum!
void Simulation::drawLine(int x0, int y0, int x1, int y1)
{
    int delta_x = abs(x1 - x0);
    int delta_y = -abs(y1 - y0);
    int sx = (x1 > x0) ? 1 : -1;
    int sy = (y1 > y0) ? 1 : -1;
    int error = delta_x + delta_y;

    for (int x = x0, y = y0; x != x1 || y != y1;) {
        SDL_RenderDrawPoint(renderer, x, y);

        int err = error * 2;
        if (err >= delta_y) {
            error += delta_y;
            x += sx;
        }
        if (err <= delta_x) {
            error += delta_x;
            y += sy;
        }
    }
}

void Simulation::resetBackgroundColor()
{
    SDL_SetRenderDrawColor(renderer, background_r, background_g,
                           background_b, 255);
}

void Simulation::initializeEvents()
{
    for (Particle *p : particles)
        predictCollisions(*p);

    events.push(new RefreshEvent(now));
}

void Simulation::predictCollisions(Particle &particle)
{
    for (Particle *p : particles) {
        double dt = particle.collidesParticle(*p);

        if (dt < 0)
            continue;

        events.push(new ParticleCollisionEvent(now + dt, particle, *p));
    }

    addWallCollisionEvent(particle, WallType::Vertical);
    addWallCollisionEvent(particle, WallType::Horisontal);    
}

void Simulation::addWallCollisionEvent(Particle &p, WallType wtype)
{
    double dt;

    dt = p.collidesWall(wtype);
    if (dt < 0)
        return;

    events.push(new WallCollisionEvent(now + dt, p, wtype));
}

int Simulation::simulationTimeToMS(double sim_time) const
{
    return 60 / speed * sim_time;
}

double Simulation::MSToSimulationTime(int ms) const
{
    return (double)ms / 60 * speed;
}
