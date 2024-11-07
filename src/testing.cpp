#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/persistence.hpp>
#include <iostream>
#include <string>
#include "yaml-cpp/yaml.h"
#include "icecream.hpp"
#include <fstream>
#include "barelangTrackbar.h"

barelang::TrackbarColour *Lapangan{nullptr};
std::vector<int> greenVec;
YAML::Node omniconfig;

void saveYAML(const std::string& filename) {
    YAML::Node omniconfig = YAML::LoadFile(filename);

    omniconfig["greenBit"] = greenVec;

    IC(greenVec);

    std::ofstream out(filename);
    out << omniconfig;
    out.close();
}

int main() {
    std::string filename = "/opt/offline/config/omni_config.yaml";
    omniconfig = YAML::LoadFile(filename);

    greenVec = omniconfig["greenBit"].as<std::vector<int>>();

    Lapangan = new barelang::TrackbarColour("LAPANGAN", "COLOR-SEGMENTATION", barelang::ColorChannel::YUV, 
               greenVec[1], greenVec[0], greenVec[3], 
               greenVec[2], greenVec[5], greenVec[4], 
               255, true);

    while (true) 
    {
        int key = cv::waitKey(1);
        if (key == 27) {  // Press 'q' to quit the program
            break;
        } else if (key == 's') {  // Press 's' to save current settings
            saveYAML(filename);
            std::cout << "Settings saved to " << filename << std::endl;
        }
    }

    cv::destroyAllWindows();
    return 0;
}
