#include <icecream.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core/persistence.hpp>
#include <opencv2/core.hpp>
#include "device_scan.hpp"
#include "yaml-cpp/yaml.h"
#include "barelangTrackbar.h"
#include "cameraVision.h"
#include <iostream>
#include <string>
#include <fstream>

cv::VideoCapture cap;
cv::Mat frame;
std::string video_path = "../videos/omni_11-08-04.avi"; 

std::vector<int> greenVec;
std::vector<int> whiteVec;
std::vector<int> blackVec;

barelang::TrackbarColour *Lapangan{nullptr};
barelang::TrackbarColour *Obstacle{nullptr};
barelang::TrackbarColour *Garis{nullptr};

void loadYAML(const std::string &filename, YAML::Node &omniConfig)
{
    omniConfig = YAML::LoadFile(filename);
}

void saveYAML(const std::string &filename, const YAML::Node &omniConfig)
{
    YAML::Node config = omniConfig;

    config["whiteBit"] = whiteVec;
    config["greenBit"] = greenVec;
    config["blackBit"] = blackVec;

    IC(whiteVec, greenVec, blackVec);

    std::ofstream out(filename);
    out << config;
    out.close();
}

int main(int argc, char **argv)
{
    if (argc<=0 && argc > 1){
        std::cerr << "Usage: " << argv[0] << " <mode>\n"<< std::endl;
        std::cerr << "Mode 0: Open Garis\n";
        std::cerr << "Mode 1: Open Lapangan\n";
        std::cerr << "Mode 2: Open Obstacle\n";
        return -1;
    }
      int mode = std::atoi(argv[1]);
      IC(mode);

    std::string filename = "/opt/offline/config/omni_config.yaml";
    YAML::Node omniConfig;
    loadYAML(filename, omniConfig);

    whiteVec = omniConfig["whiteBit"].as<std::vector<int>>();
    greenVec = omniConfig["greenBit"].as<std::vector<int>>();
    blackVec = omniConfig["blackBit"].as<std::vector<int>>();

    bool lapangan_  =0 , obstacle_ = 0, garis_ = 0;

    switch (mode)
    {
    case 0:
        garis_ = true;
        break;
    case 1:
        lapangan_ = true;
        break;
    case 2:
        obstacle_ = true;
        break;

    default:
        garis_ = false;
        obstacle_ = false;
        lapangan_ = false;
        break;
    }

    IC(lapangan_, garis_, obstacle_);

    // COLOR SEGMENTATION TRACKBAR //
    Garis = new barelang::TrackbarColour(
            "GARIS", "COLOR-SEGMENTATION", barelang::ColorChannel::HSV, 
            whiteVec[1], whiteVec[0], whiteVec[3], 
            whiteVec[2], whiteVec[5], whiteVec[4], 
            255, garis_);

    Lapangan = new barelang::TrackbarColour("LAPANGAN", "COLOR-SEGMENTATION", barelang::ColorChannel::YUV, 
               greenVec[1], greenVec[0], greenVec[3], 
               greenVec[2], greenVec[5], greenVec[4], 
               255, lapangan_);

    Obstacle = new barelang::TrackbarColour(
        "OBSTACLE", "OBJEK", barelang::ColorChannel::HSV, 
        blackVec[1], blackVec[0], blackVec[3], 
        blackVec[2], blackVec[5], blackVec[4], 
        255, obstacle_);

    // Open the video file
    if (!cap.open(video_path))
    {
        std::cerr << "Error: Could not open video file " << video_path << std::endl;
        return -1;
    }

    while (true)
    {
        cap.read(frame);

        if (frame.empty())
        {
            std::cerr << "Error: Blank frame grabbed or end of video" << std::endl;
            break;
        }

        cv::imshow("REAL", frame);

        cv::Mat hsv_mat, yuv_mat, lapangan_cnt, obstacle_cnt, garis_cnt;
        cv::cvtColor(frame, hsv_mat, cv::COLOR_BGR2HSV);
        cv::cvtColor(frame, yuv_mat, cv::COLOR_BGR2YUV);

        if (lapangan_)
        {
            cv::inRange(yuv_mat, Lapangan->getLower(), Lapangan->getUpper(), lapangan_cnt);
            cv::imshow("masking lapangan", lapangan_cnt);
        }

        if (garis_)
        {
            cv::inRange(hsv_mat, Garis->getLower(), Garis->getUpper(), garis_cnt);
            cv::imshow("masking garis", garis_cnt);
        }

        if (obstacle_)
        {
            cv::inRange(hsv_mat, Obstacle->getLower(), Obstacle->getUpper(), obstacle_cnt);
            cv::imshow("masking obstacle", obstacle_cnt);
        }

        int key = cv::waitKey(50);
        if (key == 's')
        {
            IC("Save");
            saveYAML(filename, omniConfig);
        }
        else if (key == 27) // ESC key
        {
            break;
        }
    }

    // Release the camera and close all windows
    cap.release();
    cv::destroyAllWindows();

    return 0;
}