#include <iostream>
#include <vector>
#include <unordered_map>
#include <utility>
#include <fstream>
#include <random>

// 生成随机双精度浮点数
double generateRandomDouble(double low, double high) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(low, high);
    return dis(gen);
}
struct dronePoint{
    std::pair<double, double> point;
    bool flag;
    unsigned char status;
};

int main() {
    std::unordered_map<int, std::vector<dronePoint>> dronePaths;

    // 填充数据，模拟15架无人机及其航迹点
    for (int i = 1; i <= 15; ++i) {
        std::vector<dronePoint>path;
        int numPoints = 5; // 每架无人机5个航迹点，这个数值可以调整
        for (int j = 0; j < numPoints; ++j) {
            double lat = generateRandomDouble(-90.0, 90.0);  // 纬度
            double lon = generateRandomDouble(-180.0, 180.0); // 经度
            unsigned char status = 0xff;
            path.push_back({std::make_pair(lat, lon),true, status});
        }
        dronePaths[i] = path;
    }

    // 输出到XML文件
    std::ofstream xmlFile("drones.xml");
    xmlFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xmlFile << "<Drones>\n";

    for (const auto& drone : dronePaths) {
        xmlFile << "  <Drone id=\"" << drone.first << "\">\n";
        for (const auto& point : drone.second) {
//            xmlFile << "    <Point latitude=\"" << point.first << "\" longitude=\"" << point.second << "\"/>\n";
            xmlFile << "    <Point flag=\"" << point.flag << "\" latitude=\"" << point.point.first << "\" longitude=\""
            << point.point.second << "\" status=\"" << std::hex << static_cast<int>(point.status) << std::dec << "\"/>\n";
        }
        xmlFile << "  </Drone>\n";
    }
    xmlFile << "</Drones>\n";
    xmlFile.close();

    return 0;
}
