#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <ctime>

#include "device_scan.hpp"
#include "yaml-cpp/yaml.h"
#include "cameraVision.h"
#include <fstream>

using namespace std;

cv::VideoCapture cap;
cv::Mat frame;
std::string camera_omni = "HD_Camera_Manufacturer";
cameraVision *camProp = NULL;

int brightness_ = 0, saturation_ = 0, contrast_ = 0;

void bscTrackbarChange(int, void *){ 

}

void loadYAML(const std::string &filename, YAML::Node &omniConfig)
{
    omniConfig = YAML::LoadFile(filename);
    brightness_ = omniConfig["brightness"].as<int>();
    contrast_ = omniConfig["contrast"].as<int>();
    saturation_ = omniConfig["saturation"].as<int>();
}

void saveYAML(const std::string &filename, const YAML::Node &omniConfig)
{
    YAML::Node config = omniConfig;

    config["brightness"] = brightness_;
    config["saturation"] = saturation_;
    config["contrast"] = contrast_;

    IC(brightness_, saturation_, contrast_);

    std::ofstream out(filename);
    out << config;
    out.close();
}

string Filename(const string& directory) {
    time_t now = time(0);
    tm *ltm = localtime(&now);

    char filename[100];
    sprintf(filename, "omni_%02d-%02d-%02d.avi", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
    
    return directory + "/" + string(filename);
}


int main(int argc, char **argv)
{
    string save_path = "../videos";
    std::string filename = "/opt/offline/config/omni_config.yaml";

    YAML::Node omniConfig;

    loadYAML(filename, omniConfig);

    bool recording = false;
    cv::VideoWriter video;

    // BRIGHTNESS, SATURATION, AND CONTRAST TRACKBAR //
    cv::namedWindow("Brightness, Saturation, and Contrast", cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("Brightness", "Brightness, Saturation, and Contrast", &brightness_, 100, bscTrackbarChange);
    cv::createTrackbar("Saturation", "Brightness, Saturation, and Contrast", &saturation_, 100, bscTrackbarChange);
    cv::createTrackbar("Contrast", "Brightness, Saturation, and Contrast", &contrast_, 100, bscTrackbarChange);
    
    camProp = new cameraVision(0, 0, &frame, &cap);

    std::cout << "################Start capturing################" << std::endl;
    while (true)
    {
        if (!cap.isOpened())
        {
            auto scanned_device = device_scan::devlinks_list(camera_omni.c_str(), "video4linux");
            for (auto &device : scanned_device)
            {

                if (!cap.open(device, cv::CAP_V4L2))
                {
                    std::cerr << "Error: Could not open device " << device << std::endl;
                    continue;
                }

                if (cap.isOpened())
                {
                    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
                    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
                    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
                    break;
                }
            }
        }

        camProp->setBrigtness(brightness_);
        camProp->setSaturation(saturation_);
        camProp->setContrast(contrast_);
        cap.read(frame);
        double fps = cap.get(cv::CAP_PROP_FPS);

        if (frame.empty())
        {
            std::cerr << "Error: Blank frame grabbed" << std::endl;
            break;
        }

        frame = cv::Mat(frame, cv::Rect(522, 280, 720, 720));
    
        cv::resize(frame, frame, cv::Size(480, 480));
        cv::flip(frame, frame, 1);

        cv::imshow("CAMERA", frame);

        char c = (char)cv::waitKey(1);

        if (c == 27) { 
                break;
            }

        if (c == 's' && !recording)
        {
            string path = Filename(save_path); 
            std::cout << "Recording started for " << path << "\n";
            video.open(path, cv::VideoWriter::fourcc('M','J','P','G'), fps, cv::Size(frame.cols, frame.rows));

            if (!video.isOpened()) {
                cerr << "Error: Could not open video writer." << endl;
                return EXIT_FAILURE;
            }

            recording = true;
        }  

        if (recording)
            video.write(frame);

        if (c == 'x')
        {
            std::cout << "Recording finished.\n";
            recording = false;
            video.release();
        }
    }

    std::cout << "################DONE################" << std::endl;

    // Release the camera and close all windows
    cap.release();
    cv::destroyAllWindows();

    return 0;
}
