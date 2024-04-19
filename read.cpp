//
// Created by dmj on 24-4-11.
//
#include <iostream>
#include <unordered_map>
#include <vector>
#include <utility>
#include <tinyxml2.h>

using namespace tinyxml2;
using std::pair;
using std::unordered_map;
using std::vector;
using std::cout;
using std::endl;

struct dronePoint{
    std::pair<double, double> point;
    bool flag;
    unsigned char status;
};

int main() {
    XMLDocument doc;
    doc.LoadFile("drones.xml");  // 确保此文件路径正确

    std::unordered_map<int, std::vector<dronePoint>> dronePaths;

    XMLElement* root = doc.FirstChildElement("Drones");
    if (root == nullptr) {
        cout << "Failed to load file: No 'Drones' element." << endl;
        return -1;
    }

    for (XMLElement* drone = root->FirstChildElement("Drone"); drone != nullptr; drone = drone->NextSiblingElement("Drone")) {
        int id = atoi(drone->Attribute("id"));
        vector<dronePoint> path;

        for (XMLElement* point = drone->FirstChildElement("Point"); point != nullptr; point = point->NextSiblingElement("Point")) {
            double latitude = atof(point->Attribute("latitude"));
            double longitude = atof(point->Attribute("longitude"));
            int id = atoi(point->Attribute("flag"));
            path.push_back({{latitude,longitude},(bool)id});
        }

        dronePaths[id] = path;
    }

    // 打印读取的数据，确认是否正确
    for (const auto& drone : dronePaths) {
        cout << "Drone ID: " << drone.first << endl;
        for (const auto& point : drone.second) {
            cout << "  Point: id = " << point.flag << ", latitude = " << point.point.first << ", longitude = " << point.point.second<<endl;
        }
    }

    return 0;
}
