#include <Arduino.h>

struct Point {
    double lat;
    double lng;
};

struct Line {
    Point A;
    Point B;
};

// Helper function to find orientation of ordered triplet (p, q, r).
// Returns:
// 0 --> p, q and r are collinear
// 1 --> Clockwise
// 2 --> Counterclockwise
static int orientation(Point p, Point q, Point r) {
    // Using lat as y-coordinate and lng as x-coordinate for calculations.
    double val =
        (q.lat - p.lat) * (r.lng - q.lng) - (q.lng - p.lng) * (r.lat - q.lat);

    if (val == 0) return 0;    // Collinear
    return (val > 0) ? 1 : 2;  // Clockwise or Counterclockwise
}

// Given three collinear points p, q, r, the function checks if
// point q lies on line segment 'pr'.
static bool onSegment(Point p, Point q, Point r) {
    if (q.lng <= max(p.lng, r.lng) && q.lng >= min(p.lng, r.lng) &&
        q.lat <= max(p.lat, r.lat) && q.lat >= min(p.lat, r.lat))
        return true;
    return false;
}

/**
 * @brief Check if two line segments cross each other.
 */
inline bool linesCrossed(const Line& line1, const Line& line2) {
    Point p1 = line1.A;
    Point q1 = line1.B;
    Point p2 = line2.A;
    Point q2 = line2.B;

    // Find the four orientations needed for general and special cases
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    // General case:
    // Segments intersect if orientations (p1,q1,p2) and (p1,q1,q2) are
    // different, AND orientations (p2,q2,p1) and (p2,q2,q1) are different.
    if (o1 != o2 && o3 != o4) {
        return true;
    }

    // Special Cases for collinear points:
    // These cases handle when segments are collinear and overlap.

    // o1 == 0 means p1, q1, p2 are collinear.
    // If p2 lies on segment p1q1, then segments intersect.
    if (o1 == 0 && onSegment(p1, p2, q1)) return true;

    // o2 == 0 means p1, q1, q2 are collinear.
    // If q2 lies on segment p1q1, then segments intersect.
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;

    // o3 == 0 means p2, q2, p1 are collinear.
    // If p1 lies on segment p2q2, then segments intersect.
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;

    // o4 == 0 means p2, q2, q1 are collinear.
    // If q1 lies on segment p2q2, then segments intersect.
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;

    return false;  // Segments do not intersect in any other case
}