#ifndef _EVENT_HPP_
#define _EVENT_HPP_

#include "particle.hpp"

/*
 * There're three events we use in the simulation:
 * - Refresh event: stands for refreshing the screen
 * - Wall collision event: represents the moment a
 *   particle collides with wall
 * - Particle collision event: represents the moment
 *   a particle collides another particle
 */
enum class EventType {Refresh, WallCollision, ParticleCollision};

// Basic abstract class for all events
class Event {
protected:
    double time; // the time the event will happen
    EventType ev_type; // the type of the event

public:
    explicit Event(double time, EventType type) : time(time), ev_type(type)  {};
    virtual ~Event() {};

    virtual double getTime() const {
        return time;
    }

    virtual EventType getType() const {
        return ev_type;
    }

    // returns true if the event is outdated/cancelled
    virtual bool isStale() const = 0;
};

class RefreshEvent : public Event {
public:
    explicit RefreshEvent(double time) : Event(time, EventType::Refresh) {};
    virtual ~RefreshEvent() {};

    // this event can not be cancelled
    bool isStale() const { return false; }
};

class WallCollisionEvent : public Event {
protected:
    Particle *p; // a particle
    WallType wtype; // the type of wall particle will collide
    int p_rev;

public:
    WallCollisionEvent(double time, Particle &p, WallType wtype)
        : Event(time, EventType::WallCollision) {
        this->p = &p;
        this->wtype = wtype;
        p_rev = p.getRevision();
    };

    virtual ~WallCollisionEvent() {};

    // the event is considered to be stale if
    // the particle collides something before
    bool isStale() const {
        return (p_rev != p->getRevision());
    }

    WallType getWallType() const {
        return wtype;
    }

    Particle& getParticle() const {
        return *p;
    }
};

class ParticleCollisionEvent : public Event {
protected:
    Particle *pa;
    Particle *pb;
    int pa_rev;
    int pb_rev;

public:
    ParticleCollisionEvent(double time, Particle &pa, Particle &pb)
        : Event(time, EventType::ParticleCollision) {
        this->pa = &pa;
        this->pb = &pb;
        pa_rev = pa.getRevision();
        pb_rev = pb.getRevision();
    }

    virtual ~ParticleCollisionEvent() {};

    // the event is considered to be stale if
    // one of particles collides somethign
    // before.
    bool isStale() const {
        return ((pa_rev != pa->getRevision()) ||
                (pb_rev != pb->getRevision()));
    }

    Particle &getFirstParticle() const {
        return *pa;
    }

    Particle &getSecondParticle() const {
        return *pb;
    }
};

#endif /* _EVENT_HPP_ */
