#include "min_circle.h"
#include <iostream>



bool Point2D::operator==(const Point2D& other) const {
    return std::abs(x - other.x) < EPS && std::abs(y - other.y) < EPS;
}

bool Point2D::operator!=(const Point2D& other) const {
    return !(*this == other);
}



bool Circle::isValid() const {
    return radius >= 0;
}

bool Circle::contains(const Point2D& point) const {
    double dx = center.x - point.x;
    double dy = center.y - point.y;
    return (dx*dx + dy*dy) <= radius*radius + EPS;
}

bool Circle::contains(const LineSegment& segment) const {
    return contains(segment.start) && contains(segment.end) && containsEntireSegment(segment);
}

bool Circle::containsEntireSegment(const LineSegment& segment) const {
    Point2D mid = findMidPoint(segment);
    return contains(mid);
}



double distanceSquared(const Point2D& p1, const Point2D& p2) {
    double dx = p1.x - p2.x;
    double dy = p1.y - p2.y;
    return dx*dx + dy*dy;
}

double distance(const Point2D& p1, const Point2D& p2) {
    return std::sqrt(distanceSquared(p1, p2));
}

Circle circleFromDiameter(const Point2D& p1, const Point2D& p2) {
    Circle c;
    c.center.x = (p1.x + p2.x) / 2;
    c.center.y = (p1.y + p2.y) / 2;
    c.radius = distance(p1, p2) / 2;
    return c;
}

Circle circleFrom3Points(const Point2D& p1, const Point2D& p2, const Point2D& p3) {
    double d = 2 * (p1.x*(p2.y - p3.y) + p2.x*(p3.y - p1.y) + p3.x*(p1.y - p2.y));
    
    if (std::abs(d) < EPS) {
        double d12 = distanceSquared(p1, p2);
        double d13 = distanceSquared(p1, p3);
        double d23 = distanceSquared(p2, p3);
        
        if (d12 >= d13 && d12 >= d23) return circleFromDiameter(p1, p2);
        if (d13 >= d12 && d13 >= d23) return circleFromDiameter(p1, p3);
        return circleFromDiameter(p2, p3);
    }
    
    Circle c;
    double ux = ((p1.x*p1.x + p1.y*p1.y)*(p2.y - p3.y) + 
                 (p2.x*p2.x + p2.y*p2.y)*(p3.y - p1.y) + 
                 (p3.x*p3.x + p3.y*p3.y)*(p1.y - p2.y)) / d;
    
    double uy = ((p1.x*p1.x + p1.y*p1.y)*(p3.x - p2.x) + 
                 (p2.x*p2.x + p2.y*p2.y)*(p1.x - p3.x) + 
                 (p3.x*p3.x + p3.y*p3.y)*(p2.x - p1.x)) / d;
    
    c.center = {ux, uy};
    c.radius = distance(c.center, p1);
    return c;
}

Point2D findMidPoint(const LineSegment& segment) {
    return {(segment.start.x + segment.end.x) / 2, 
            (segment.start.y + segment.end.y) / 2};
}

std::vector<Point2D> extractPointsFromSegments(const std::vector<LineSegment>& segments) {
    std::vector<Point2D> points;
    
    for (const auto& seg : segments) {
        points.push_back(seg.start);
        points.push_back(seg.end);
        points.push_back(findMidPoint(seg));
    }
    
    for (size_t i = 0; i < segments.size(); i++) {
        for (size_t j = i+1; j < segments.size(); j++) {
            const auto& s1 = segments[i];
            const auto& s2 = segments[j];
            
            double x1 = s1.start.x, y1 = s1.start.y;
            double x2 = s1.end.x, y2 = s1.end.y;
            double x3 = s2.start.x, y3 = s2.start.y;
            double x4 = s2.end.x, y4 = s2.end.y;
            
            double denom = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4);
            if (std::abs(denom) < EPS) continue;
            
            double t = ((x1-x3)*(y3-y4) - (y1-y3)*(x3-x4)) / denom;
            double u = -((x1-x2)*(y1-y3) - (y1-y2)*(x1-x3)) / denom;
            
            if (t >= -EPS && t <= 1+EPS && u >= -EPS && u <= 1+EPS) {
                Point2D inter = {x1 + t*(x2-x1), y1 + t*(y2-y1)};
                points.push_back(inter);
            }
        }
    }
    
    return points;
}



Circle minCircleHelper(std::vector<Point2D>& points, std::vector<Point2D> boundary, size_t n) {
    if (n == 0 || boundary.size() == 3) {
        if (boundary.empty()) return {{0,0}, -1};
        if (boundary.size() == 1) return {boundary[0], 0};
        if (boundary.size() == 2) return circleFromDiameter(boundary[0], boundary[1]);
        return circleFrom3Points(boundary[0], boundary[1], boundary[2]);
    }
    
    size_t idx = rand() % n;
    Point2D p = points[idx];
    std::swap(points[idx], points[n-1]);
    
    Circle c = minCircleHelper(points, boundary, n-1);
    
    if (c.isValid() && c.contains(p)) return c;
    
    boundary.push_back(p);
    return minCircleHelper(points, boundary, n-1);
}

Circle minCircle(std::vector<Point2D> points) {
    if (points.empty()) return {{0,0}, -1};
    std::shuffle(points.begin(), points.end(), std::mt19937(std::random_device()()));
    return minCircleHelper(points, {}, points.size());
}



Circle MinimumEnclosingCircleForSegments(const std::vector<LineSegment>& segments) {
    if (segments.empty()) return {{0,0}, -1};
    if (segments.size() == 1) {
        return circleFromDiameter(segments[0].start, segments[0].end);
    }
    
    std::vector<Point2D> points = extractPointsFromSegments(segments);
    return minCircle(points);
}
