#ifndef CAMERA_H
#define CAMERA_H
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/video.hpp"
#include "thread"
#include "mutex"
#include "time.h"
#include "PID.h"
// #include "QFile"
// #include "QString"
// #include "QTextStream"
// #include "QStringList"
// #include "QPoint"
#include "icecream.hpp"
#include "definisi.h"
// ####################################################################################################################
// ####################################################################################################################
#include "boost/thread.hpp"
#include "boost/chrono.hpp"
// ####################################################################################################################
// ####################################################################################################################
struct{
    float gain;
    float sat;
    float brig;
    float shu;
} CamP;
enum property_cam
{
    BRI,
    GAI,
    SHU,
    GAM,
    SAT,
    WB,
    EXP
};
// ####################################################################################################################
// ####################################################################################################################
enum prop_cam
{
    brightness,
    contrast,
    sharp_kernel,
    sharp_k
};
// ####################################################################################################################
// ####################################################################################################################
class cameraVision
{
private:
    bool imageready, newimage, calibrate, firsttime;
    cv::Mat *framebuffer, *buffer;
    cv::Mat holder, frame;
    cv::VideoCapture *cap;
    float rowBytes;
    int frameCounter;
    float fps;
    boost::thread spin_th_;
    boost::mutex Mutex;
    struct timespec past, present;
    int rob_id;
    bool changed;
    double prop_[10];
    //####################################################################################################################
    //####################################################################################################################
    // #### CAMERA PARAMETERS DATA ####
    // ################################
    // QString pidPath, propertyPath;
    bool initPidValues(int robot_id);
    // Property props[7];
    PID *ShuPID, *BrigPID, *SatPID, *GammaPID, *GainPID, *wRedPID, *wBluePID;
    //####################################################################################################################
    //####################################################################################################################
    // #### CALIBRATION DATA ####
    // ##########################
    cv::Rect roi_black, roi_white;
    std::vector<int> histvalue;
    cv::Mat image_roi_white, image_roi_black, image;
    float msvError;
    int msv, satTarget, lumiTarget;
    float minError_Lumi, minError_Sat, minError_UV_1, minError_UV_2, minError_RGB;
    //####################################################################################################################
    //####################################################################################################################
public:
    cameraVision(bool cal, int robot_id , cv::Mat *matFrame, cv::VideoCapture* videCapture);
    ~cameraVision();
    // ####################################################################################################################
    // ####################################################################################################################
    bool connect();
    bool closeCamera();
    bool spinCapture();
    bool stopCapture();
    void printCameraInfo();
    // ####################################################################################################################
    // ####################################################################################################################
    //  pake cv mat
    void setNewFrame(cv::Mat *frame);
    bool frameAvailable();
    bool isCameraReady();
    boost::mutex *getLockingMutex();
    cv::Mat *getImage();
    float getFPS();
    inline void toggleCalibrate() { calibrate = !calibrate; }
    inline void toggleFirsttime() { firsttime = (!firsttime); }
    // ####################################################################################################################
    // ####################################################################################################################
    void setContrast(double alpha);
    double getContrast();
    // ####################################################################################################################
    // ####################################################################################################################
    void setSharpening(int unsharp_mask_and_high_boosting, uint8_t sharp_kernel_size, double k);
    // ####################################################################################################################
    // ####################################################################################################################
    //  Brigtness(5.469->29.688)
    void setBrigtness(float value);
    float getBrigtness();
    bool getBrigtnessState();
    void setBrigtnessState(bool state);
    inline float getBrigtnessMax() { return 29.688; }
    inline float getBrigtnessMin() { return 5.469; }
    // ####################################################################################################################
    // ####################################################################################################################
    //  Shutter(0.046->32.754)
    void setShutter(float value);
    float getShuttertime();
    bool getShutterState();
    void setShutterState(bool state);
    inline float getShuttertimeMax() { return 32.754; }
    inline float getShuttertimeMin() { return 0.046; }
    // ####################################################################################################################
    // ####################################################################################################################
    //  Gain(0->23.991)
    void setGain(float value);
    float getGain();
    bool getGainState();
    void setGainState(bool state);
    inline float getGainMax() { return 23.991; }
    inline float getGainMin() { return 0; }
    // ####################################################################################################################
    // ####################################################################################################################
    //  Auto-Exposure(-7.585->2.414)
    void setExposure(float value);
    float getExposure();
    bool getExposureState();
    void setExposureState(bool state);
    inline float getExposureMax() { return 2.414; }
    inline float getExposureMin() { return -7.585; }
    // ####################################################################################################################
    // ####################################################################################################################
    //  Gamma(0.5->3.999)
    void setGamma(float value);
    float getGamma();
    bool getGammaState();
    void setGammaState(bool state);
    inline float getGammaMax() { return 3.999; }
    inline float getGammaMin() { return 0.5; }
    // ####################################################################################################################
    // ####################################################################################################################
    //  Saturation(0->399.902)
    void setSaturation(float value);
    float getSaturation();
    bool getSaturationState();
    void setSaturationState(bool state);
    inline float getSaturationMax() { return 128; }
    inline float getSaturationMin() { return 0; }
    // ####################################################################################################################
    // ####################################################################################################################
    //  White_Balance(0->1023) both
    void setWhite_Balance(float value, float value2);
    void setWhite_Balance_valueA(float value);
    void setWhite_Balance_valueB(float value);
    float getWhite_Balance_valueA();
    float getWhite_Balance_valueB();
    bool getWhite_BalanceState();
    void setWhite_BalanceState(bool state);
    inline float getWhite_BalanceMax() { return 1023; }
    inline float getWhite_BalanceMin() { return 0; }
    void setWhite_BalanceAuto(bool state);
    // ####################################################################################################################
    // ####################################################################################################################
    void setProps(int num);
    bool initProps(int robot_id);
    bool writePIDConfig();
    bool writePropConfig();
    void setPropControlPID(int id, float p, float i, float d, bool blue);
    cv::Point2d getError(int prop_in_use);
    // ####################################################################################################################
    // ####################################################################################################################
    //  #### CALIBRATION FUNCTIONS ####
    //  ###############################
    void calcLumiHistogram();
    void calcSatHistogram();
    cv::Scalar averageRGB();
    float calcMean();
    cv::Scalar averageUV();
    bool cameraCalibrate();
    void setRegions();
    void setWhiteROI(int x, int y, int h);
    void setBlackROI(int x, int y, int h);
    // ####################################################################################################################
    // ####################################################################################################################
    void setSatTarget(int val);
    void setLumiTarget(int val);
    int getSatTarget();
    int getLumiTarget();
    cv::Mat changeContrast(cv::Mat image, double alpha);
    cv::Mat changeBrightness(cv::Mat image, double beta);
    cv::Mat Image_Sharpening(cv::Mat image, int sharpening_type, uint8_t kernel_size, double k);
};
#endif