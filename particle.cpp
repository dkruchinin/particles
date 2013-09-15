#include <iomanip>
#include <cmath>
#include "particle.hpp"

Particle::Particle(double x, double y, double vx, double vy, double radius, int mass,
                   double vbound, double hbound, int r, int g, int b)
{
    // remember relative coordinates for clarity
    rel_x = x;
    rel_y = y;
    rel_vx = vx;
    rel_vy = vy;
    rel_radius = radius;

    double mid = (vbound + hbound) / 2;

    // translate relative coordinates to absolute
    this->x = vbound * rel_x;
    this->y = hbound * rel_y;
    this->vx = mid * rel_vx;
    this->vy = mid * rel_vy;
    this->radius = std::round(mid * rel_radius);
    this->mass = mass;

    // we need to know vertical and horisontal
    // bounds to detect collisions with walls
    this->vbound = vbound;
    this->hbound = hbound;
    this->r = r;
    this->g = g;
    this->b = b;
    rev = 0;
}

bool Particle::overlaps(const Particle &p) const
{
    double dx = std::abs(x - p.x), dy = std::abs(y - p.y);

    // round the results before we try to compare them
    // oh, I hate floating point numbers comparison so much...
    double rdist = round(radius + p.radius, 4);
    double hipotenusa = round(std::sqrt(dx * dx + dy * dy), 4);

    return (hipotenusa < rdist);
}

void Particle::bounceWall(WallType wtype)
{
    if (wtype == WallType::Vertical)
        vx = -vx;
    else
        vy = -vy;

    rev++;
}

void Particle::bounceParticle(Particle &p)
{
    double dx = p.x - x, dy = p.y - y;
    double dvx = p.vx - vx, dvy = p.vy - vy;
    double dvdr = dvx * dx + dvy * dy;
    int distance = radius + p.radius;

    // calculate the impulse
    double J = (2 * mass * p.mass * dvdr) / (distance * (mass + p.mass));
    double Jx = J * dx / distance;
    double Jy = J * dy / distance;

    /*
     * And apply the Newton's second law to compute velocities
     * after collision.
     *
     * Note:
     * Limit the precision of calculation results rounding
     * them to 8 digits after the point. This helps to avoid
     * floating point error accumulating in less significant
     * digits of mantissa.
     */
    vx = round(vx + Jx / mass, 8);
    vy = round(vy + Jy / mass, 8);
    p.vx = round(p.vx - Jx / p.mass, 8);
    p.vy = round(p.vy - Jy / p.mass, 8);

    rev++;
    p.rev++;
}

double Particle::collidesWall(WallType wtype) const
{
    switch (wtype) {
    case WallType::Vertical:
        return predictWallCollision(x, vx, vbound);
    case WallType::Horisontal:
        return predictWallCollision(y, vy, hbound);
    }
}

double Particle::collidesParticle(const Particle &p) const
{
    if (this == &p)
        return -1.0;

    int distance = radius + p.radius;
    double dx = p.x - x, dy = p.y - y;
    double dvx = p.vx - vx, dvy = p.vy - vy;
    double drdr = dx * dx + dy * dy;
    double dvdv = dvx * dvx + dvy * dvy;
    double dvdr = dvx * dx + dvy * dy;
    double d = dvdr * dvdr - dvdv * (drdr - distance * distance);

    if (dvdr >= 0 || d < 0)
        return -1.0;

    return -(dvdr + sqrt(d)) / dvdv;
}

void Particle::move(double dt)
{
    x += vx * dt;
    y += vy * dt;
}

double Particle::predictWallCollision(double coord, double velocity, int bound) const
{
    if (velocity > 0.0)
        return (bound - radius - coord) / velocity;
    else if (velocity < 0.0)
        return (radius - coord) / velocity;
    else
        return -1.0;
}

double Particle::round(double num, int precision) const
{
    int mult = std::pow(10, precision);
    return std::floor((num * mult + 0.5) / mult);
}

std::ostream& operator<<(std::ostream &os, const Particle &p)
{
    os << "(x: " << p.rel_x << " (" << p.x << "), y: " << p.rel_y
       << " (" << p.y << "), vx: " << p.rel_vx << " (" << p.vx
       << "), vy: " << p.rel_vy << " (" << p.vy << "), mass: "
       << p.mass << ", radius: " << p.rel_radius << " ("
       << p.radius << "), rgb: [" << p.r << ", " << p.g << ", "
       << p.b << "]" << ", rev: " << p.rev << ")";
    return os;
}
