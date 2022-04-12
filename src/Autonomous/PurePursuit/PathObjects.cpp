#include "robokauz/Autonomous/PurePursuit/PathObjects.hpp"

//Raw path
RawPath::RawPath(std::vector<RawPoint> pts) { points = pts; }
int RawPath::size() { return points.size(); }
RawPoint& RawPath::at(int index) { return points.at(index); } 
void RawPath::add(RawPoint point) { points.push_back(point); }

//Full path
void Path::setName(std::string new_name) { name = new_name; }
std::string Path::getName() { return name; }
int Path::size() { return points.size(); }
PathPoint& Path::at(int index) { return points.at(index); } 
void Path::add(PathPoint point) { points.push_back(point); }