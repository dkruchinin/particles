#ifndef _SIMULATION_HPP_
#define _SIMULATION_HPP_

#include <queue>
#include <vector>
#include <memory>
#include <exception>
#include <SDL2/SDL.h>
#include "event.hpp"

class SimulationError : public std::runtime_error {
public:
    explicit SimulationError(const std::string msg) :
        std::runtime_error(msg) {};
    virtual ~SimulationError() {};
};

class Simulation {
private:
    int width;
    int height;
    int fps;
    int speed;
    int delay_ms;
    double now;
    SDL_Window *window;
    SDL_Renderer *renderer;
    std::vector<Particle *> particles;
    bool is_paused;

    // Comparator for priority queue making it a min queue.
    class EventsCompare {
    public:
        bool operator() (const Event *ev1, const Event *ev2) const {
            return std::less<double>()(ev2->getTime(), ev1->getTime());
        };
    };

    // priority queue in C++ is so bloody insane
    std::priority_queue<Event*, std::vector<Event*>, EventsCompare> events;

    void moveParticles(double dt);
    void refresh();
    void drawLine(int x0, int y0, int x1, int y1);
    void drawDisk(int x0, int y0, int radius);
    void resetBackgroundColor();
    void initializeEvents();
    void addWallCollisionEvent(Particle &p, WallType wtype);
    void predictCollisions(Particle &p);
    int simulationTimeToMS(double sim_time) const;
    double MSToSimulationTime(int ms) const;

public:
    Simulation(int width, int height, int tick_time_ms);
    virtual ~Simulation();
    void addParticle(double x, double y, double vx, double vy,
                     double radius, int mass, int r, int g, int b);
    bool paused() const;
    void pause();
    void resume();
    void tick();
};

#endif /* _SIMULATION_HPP_ */
