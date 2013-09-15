#ifndef _PARTICLE_HPP_
#define _PARTICLE_HPP_

#include <cmath>

enum class WallType {Vertical, Horisontal};

class Particle {
private:
    // coordinates in relative form
    double rel_x, rel_y, rel_vx, rel_vy, rel_radius;

    // coordinates in absolute form
    double x, y;
    double vx, vy;
    int radius, mass;

    // vertical and horisontal bounds
    int vbound, hbound;

    // color
    int r, g, b;

    // revision of the particle,
    // it's incremented every time particle collides
    // with either wall or another particle
    int rev;

    double predictWallCollision(double coord, double velocity, int bound) const;
    double round(double num, int precision) const;

public:
    Particle(double x, double y, double vx, double vy, double radius, int mass,
             double vbound, double hbound, int r, int g, int b);
    ~Particle() {};

    int getRevision() const {
        return rev;
    }

    int getX() const {
        return std::round(x);
    }

    int getY() const {
        return std::round(y);
    }

    int getRadius() const {
        return radius;
    }

    int getR() const {
        return r;
    }

    int getG() const {
        return g;
    }

    int getB() const {
        return b;
    }

    // returns true if this particle overlaps with another one
    bool overlaps(const Particle &p) const;

    // bounce this particle of a wall
    void bounceWall(WallType wtype);

    // bounce this particle of another particle
    void bounceParticle(Particle &p);

    // get the time after which this particle collides a wall
    double collidesWall(WallType wtype) const;

    // get the time after which this particle collides another one
    double collidesParticle(const Particle &p) const;

    // move this particle to a position it should be after
    // expiration of dt time
    void move(double dt);
    friend std::ostream& operator<<(std::ostream &os, const Particle &p);
};

#endif /* _PARTICLE_HPP_ */
