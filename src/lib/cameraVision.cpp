#include "cameraVision.h"

cameraVision::cameraVision(bool cal, int robot_id, cv::Mat *matFrame, cv::VideoCapture *videoCapture)
{
    calibrate = cal;
    rob_id = robot_id;
    cap = videoCapture;

    if (!initPidValues(rob_id))
    {
        std::string error_1 = "Error Reading Properties PID controler gains";
        IC(error_1);
    }
    else
    {
        std::string error_2 = "Properties PID controler configurations Ready.";
        IC(error_2);
    }

    imageready = false;
    newimage = false;
    // framebuffer = new cv::Mat(480,480,CV_8UC3,cv::Scalar(0,0,0));

    // Parameters for calibration
    minError_Lumi = lumiTarget * 0.08;
    minError_Sat = satTarget * 0.08;
    minError_RGB = 127 * 0.1;
    // setNewFrame(NewFrame);
    framebuffer = matFrame;

    firsttime = true;
}

cameraVision::~cameraVision()
{
    // destroyAllWindows();
    closeCamera();
    Mutex.unlock();
}

bool cameraVision::closeCamera()
{
    return 1;
}

bool cameraVision::spinCapture()
{
    cap->read(frame);
    if (frame.empty())
    {
        return 0;
    }
    // frame = cv::Mat(frame, cv::Rect(168, 95, 250, 250));
    // cv::resize(frame, frame,cv::Size(480,480));
    setNewFrame(&frame);
    return 1;
}

bool cameraVision::stopCapture()
{
    return 1;
}
void cameraVision::printCameraInfo()
{
    return;
}

void cameraVision::setNewFrame(cv::Mat *newframe)
{

    static int count_conf = 0;
    static bool do_calibration = false;
    // IC(count_conf);
    if (count_conf > 32 && calibrate)
    {
        count_conf = 0;
        do_calibration = true;
        std::string calb = "Do Calibration";
        IC(calb);
    }
    else
        count_conf++;

    {
        boost::lock_guard<boost::mutex> locker(this->Mutex);

        framebuffer = newframe;

        if (do_calibration)
            framebuffer->copyTo(image);
    }
    newimage = true;
    // IC(do_calibration);
    if (do_calibration)
    {
        do_calibration = cameraCalibrate();
    }
}
bool cameraVision::frameAvailable() { return newimage; }
bool cameraVision::isCameraReady() { return 1; }
boost::mutex *cameraVision::getLockingMutex()
{
    return &Mutex;
}

cv::Mat *cameraVision::getImage()
{
    cv::Mat tmp;
    buffer = NULL;
    if (newimage == true)
    {
        newimage = false;
        boost::lock_guard<boost::mutex> locker(Mutex);
        buffer = new cv::Mat(framebuffer->rows, framebuffer->cols, CV_8UC3, framebuffer->data, framebuffer->step);
        buffer->copyTo(tmp);
        // tmp = changeContrast(tmp, prop_[contrast]);
        // tmp = changeBrightness(tmp, prop_[brightness]);
        // tmp = Image_Sharpening(tmp, 0, prop_[sharp_kernel], prop_[sharp_k]);
        // tmp = cv::Mat(tmp, cv::Rect(0, 0, 480, 480));
        // *buffer = cv::Mat(tmp);
    }

    return buffer;
}

bool cameraVision::connect()
{
    cap->open(0, cv::CAP_V4L2);
    if (!cap->isOpened())
        return false;
    return true;
}

float cameraVision::getFPS() { return fps; }

/*---------------- SET/GET CAMERA PROPERTIES -------------------------------*/
/*------------------------------------------------------------------------*/

void cameraVision::setSharpening(int unsharp_mask_and_high_boosting, uint8_t sharp_kernel_size, double k)
{
    if (sharp_kernel_size % 2 == 0)
        sharp_kernel_size++;
    prop_[sharp_kernel] = sharp_kernel_size;
    prop_[sharp_k] = k;
}
void cameraVision::setContrast(double alpha)
{
    prop_[contrast] = alpha;
}

double cameraVision::getContrast()
{
    return prop_[contrast];
}
void cameraVision::setBrigtness(float value)
{
    cap->set(cv::CAP_PROP_BRIGHTNESS, value);
}
float cameraVision::getBrigtness()
{
    return cap->get(cv::CAP_PROP_BRIGHTNESS);
}

bool cameraVision::getBrigtnessState()
{
    return true;
}

void cameraVision::setBrigtnessState(bool state)
{
}

void cameraVision::setShutter(float value)
{
    cap->set(cv::CAP_PROP_EXPOSURE, value);
}
float cameraVision::getShuttertime()
{
    return cap->get(cv::CAP_PROP_EXPOSURE);
}
void cameraVision::setShutterState(bool state)
{
}
bool cameraVision::getShutterState()
{
    return 1;
}

void cameraVision::setGain(float value)
{
    cap->set(cv::CAP_PROP_GAIN, value);
}
float cameraVision::getGain()
{
    return cap->get(cv::CAP_PROP_GAIN);
}

bool cameraVision::getGainState()
{
    return 1;
}

void cameraVision::setGainState(bool state)
{
}

void cameraVision::setExposure(float value)
{
}

float cameraVision::getExposure()
{
    return 1;
}

bool cameraVision::getExposureState()
{
    return 1;
}

void cameraVision::setExposureState(bool state)
{
}

void cameraVision::setGamma(float value)
{
    cap->set(cv::CAP_PROP_GAMMA, value);
}

float cameraVision::getGamma()
{
    return cap->get(cv::CAP_PROP_GAMMA);
}

bool cameraVision::getGammaState()
{
    return 1;
}

void cameraVision::setGammaState(bool state)
{
}

void cameraVision::setSaturation(float value)
{
    cap->set(cv::CAP_PROP_SATURATION, value);
}

float cameraVision::getSaturation()
{
    return cap->get(cv::CAP_PROP_SATURATION);
}

bool cameraVision::getSaturationState()
{
    return 1;
}

void cameraVision::setSaturationState(bool state)
{
}

void cameraVision::setWhite_Balance(float value, float value2)
{
}

void cameraVision::setWhite_Balance_valueA(float value)
{
}

void cameraVision::setWhite_Balance_valueB(float value)
{
}

float cameraVision::getWhite_Balance_valueA()
{
    return cap->get(cv::CAP_PROP_WHITE_BALANCE_RED_V);
}

float cameraVision::getWhite_Balance_valueB()
{
    return cap->get(cv::CAP_PROP_WHITE_BALANCE_BLUE_U);
}

bool cameraVision::getWhite_BalanceState()
{
    return 1;
}

void cameraVision::setWhite_BalanceState(bool state)
{
}

void cameraVision::setWhite_BalanceAuto(bool state)
{
}

void cameraVision::setProps(int num)
{
}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

bool cameraVision::initProps(int robot_id)
{
    return true;
}

/*----------------CALIBRATION CAMERA PROPERTIES FUNCTIONS -------------------------------*/
/*------------------------------------------------------------------------*/

///
/// \brief Calculates Luminosity histogram
///
void cameraVision::calcLumiHistogram()
{
    std::string info_Lumi = "Do CalcLumiHistogram";
    // IC(info_Lumi);
    static bool first = true;
    cv::Mat gray;

    cvtColor(image, gray, cv::COLOR_RGB2GRAY);

    histvalue.clear();
    histvalue.resize(256);

    if (first == true)
    {
        for (int i = 0; i < gray.cols * image.rows; i++)
            histvalue[(int)gray.ptr()[i]]++;
        first = false;
    }

    else
    {
        for (int i = 0; i < gray.cols * image.rows; i = i + 10)
            histvalue[(int)gray.ptr()[i]]++;
    }
    std::string done_Lumi = "done CalcLumiHistogram";
    IC(done_Lumi);
}

///
/// \brief Calculates Saturation histogram
///
void cameraVision::calcSatHistogram()
{
    std::string info_Sat = "Do CalcSatHistogram";
    IC(info_Sat);
    bool static first = true;
    cv::Mat Hsv;

    cvtColor(image, Hsv, cv::COLOR_RGB2HSV);

    histvalue.clear();
    histvalue.resize(256);

    if (first)
    {
        for (int i = 0; i < Hsv.cols * Hsv.rows; i++)
            histvalue[(int)Hsv.ptr()[i]]++;
        first = false;
    }
    else
    {
        for (int i = 0; i < Hsv.cols * Hsv.rows; i++)
            histvalue[(int)Hsv.ptr()[i]]++;
    }
    std::string done_Sat = "done CalcSatHistogram";
    IC(done_Sat);
}

///
/// \brief Given a region of interest calculates the mean values of RGB
/// \return
///
cv::Scalar cameraVision::averageRGB()
{
    std::string info_AvRGB = "Start AvRGB";
    IC(info_AvRGB);
    int meanR = 0, meanG = 0, meanB = 0, count = 0;

    for (int i = 0; i < image_roi_black.cols; i++)
    {
        for (int j = 0; j < image_roi_black.rows; j++)
        {
            meanR = meanR + image_roi_black.ptr()[(i * image_roi_black.cols + j) * 3];
            meanG = meanG + image_roi_black.ptr()[(i * image_roi_black.cols + j) * 3 + 1];
            meanB = meanB + image_roi_black.ptr()[(i * image_roi_black.cols + j) * 3 + 2];

            count++;
        }
    }

    meanR = meanR / count;
    meanG = meanG / count;
    meanB = meanB / count;
    IC(meanR, meanG, meanB);

    cv::Scalar s = cv::Scalar(meanR, meanG, meanB);

    return s;
}

///
/// \brief Calculates mean given a vector with histogram values
/// \return
///
float cameraVision::calcMean()
{
    std::string info_CalMean = "Start CalcMean";
    IC(info_CalMean);
    float sum = 0, temp = 0;

    for (int i = 0; i < 256; i++)
    {
        sum = sum + i * histvalue[i];
        temp = temp + histvalue[i];
    }

    sum = sum / temp;
    IC(sum);
    return sum;
}

///
/// \brief Given a region of interest calculates the mean values of UV
/// \return
///
cv::Scalar cameraVision::averageUV()
{
    std::string info_AvgUV = "Start AvgUV";
    IC(info_AvgUV);
    int meanU = 0, meanV = 0, count = 0;
    cv::Mat temp;

    cvtColor(image_roi_white, temp, cv::COLOR_BGR2YUV);

    for (int i = 0; i < temp.cols; i++)
    {
        for (int j = 0; j < temp.rows; j++)
        {

            meanU = meanU + temp.ptr()[(i * temp.cols + j) * 3 + 1];
            meanV = meanV + temp.ptr()[(i * temp.cols + j) * 3 + 2];
            count++;
        }
    }
    meanU = meanU / count;
    meanV = meanV / count;
    IC(meanU, meanV);

    cv::Scalar s = cv::Scalar(meanU, meanV);
    return s;
}

///
/// \brief Defines regions of interest from our image
///
void cameraVision::setRegions()
{

    // image_roi_white = image(roi_white);
    image_roi_black = image(roi_black);
}

///
/// \brief this function is responsible for controling cameraVision parameters when ambient conditions change
/// \return true/false to give information if some parameters was changed
///
bool cameraVision::cameraCalibrate()
{
    framebuffer->copyTo(image);
    std::string info_Calib = "Start Camera Calibration";
    IC(info_Calib);
    setRegions();
    changed = false;

    float gain = getGain();
    float sat = getSaturation();
    float brig = getBrigtness();
    float shu = getShuttertime();
    IC(gain, sat, brig, shu);

    if (firsttime)
    {
        GainPID->reset();
        SatPID->reset();
        BrigPID->reset();
        ShuPID->reset();
        firsttime = false;
    }

    // Initialization of some variables
    calcLumiHistogram();
    msv = calcMean();
    msvError = lumiTarget - msv;
    IC(lumiTarget, msv, msvError);
    IC(msv);
    // std::cerr << msv << endl;
    if (msvError > minError_Lumi || msvError < -(minError_Lumi))
    {
        // changed = true;
        if (gain >= (getGainMax() - (getGainMax() / 4)))
        {
            shu = ShuPID->calc_pid(shu, msvError);
            setShutter(shu);
            gain = 0;

            setGain(gain);
            std::string info_1 = "Done 1";
            IC(info_1, gain, shu);
        }
        else
        {
            gain = GainPID->calc_pid(gain, msvError);
            setGain(gain);
            CamP.gain = gain;
            // setProps(GAI);
            std::string info_2 = "Done 2";
            IC(info_2, gain);
        }
    }

    if (!changed)
    {
        calcSatHistogram();
        msv = calcMean();
        msvError = satTarget - msv;
        IC(msv, msvError);
        IC(msvError);

        if (msvError > minError_Sat || msvError < (-minError_Sat))
        {
            changed = true;
            sat = SatPID->calc_pid(sat, msvError);
            setSaturation(sat);
            CamP.sat = sat;
            // setProps(SAT);
            std::string info_3 = "Done 3";
            IC(info_3, sat);
        }

        cv::Scalar rgbMean = averageRGB();
        if ((rgbMean[0] > minError_RGB || rgbMean[1] > minError_RGB || rgbMean[2] > minError_RGB) && brig <= (getBrigtnessMax() / 2))
        {
            // std::cerr << "Entra sempre na calibração do brightness" << endl;
            changed = true;
            float rgbError = 0;
            for (int i = 0; i < 3; i++)
                rgbError += rgbMean[i];
            brig = BrigPID->calc_pid(brig, rgbError / 3);
            if (brig > (getBrigtnessMax() - getBrigtnessMax() / 4))
                brig = (getBrigtnessMax() / 2);
            setBrigtness(brig);
            CamP.brig = brig;
            // setProps(BRI);
            std::string info_4 = "Done 4";
            IC(info_4, brig);
        }
    }
    IC(changed);

    return changed;
}

bool cameraVision::initPidValues(int robot_id)
{
    // //LEITURA DOS ROIS
    for (int i = 0; i < 2; i++)
    {
        // 	line = in.readLine();
        // 	pid_list = line.right(line.size()-line.indexOf('=')-1).split(",");
        if (i == 1)
            roi_black = cv::Rect(232, 467, 9, 9);
        else
            roi_white = cv::Rect(252, 239, 6, 6);
    }

    lumiTarget = 127;
    satTarget = 117;

    std::cerr << "Targets: " << lumiTarget << "  " << satTarget << endl;

    // Initialization of PID's
    BrigPID = new PID(4.54, 0, 2.1, getBrigtnessMin(), getBrigtnessMax());
    GainPID = new PID(0.02, 0, 0, getGainMin(), getGainMax());
    ShuPID = new PID(0.0135, 0.001, 0, getExposureMin(), getExposureMax());
    GammaPID = new PID(0.0033, 0.0062, 0.0036, getGammaMin(), getGainMax());
    SatPID = new PID(0.21, 0.18, 0.17, getSaturationMin(), getSaturationMax());
    wBluePID = new PID(1.04, 0.04, 0.04, getWhite_BalanceMin(), getWhite_BalanceMax());
    wRedPID = new PID(1.04, 0.04, 0.04, getWhite_BalanceMin(), getWhite_BalanceMax());

    return true;
}

///
/// brief Function responsible for setting PID controler of Properties
/// to the respective property object
///
void cameraVision::setPropControlPID(int id, float p, float i, float d, bool blue)
{
    if (id <= 5 && id >= 0)
    {
        if (id == 0)
            BrigPID->setPIDValues(p, i, d);
        if (id == 1)
            GainPID->setPIDValues(p, i, d);
        if (id == 2)
            ShuPID->setPIDValues(p, i, d);
        if (id == 3)
            GammaPID->setPIDValues(p, i, d);
        if (id == 4)
            SatPID->setPIDValues(p, i, d);
        if (id == 5 && blue == true)
            wBluePID->setPIDValues(p, i, d);
        else if (id == 5)
            wRedPID->setPIDValues(p, i, d);
        firsttime = true;
        writePIDConfig();
    }
    else
        printf("Property ID Out Of Range\n");
}

///
/// brief Initializes PID controler of Properties readed from a file
///
// bool cameraVision::initPidValues(int robot_id)
// {
//     return true;
// }
bool cameraVision::writePIDConfig()
{
    return true;
}

void cameraVision::setWhiteROI(int x, int y, int h)
{
    roi_white.x = x;
    roi_white.y = y;
    roi_white.height = h;
    roi_white.width = h;
    writePIDConfig();
}
void cameraVision::setBlackROI(int x, int y, int h)
{
    roi_black.x = x;
    roi_black.y = y;
    roi_black.height = roi_white.width = h;
    writePIDConfig();
}

bool cameraVision::writePropConfig()
{
    return true;
}

cv::Point2d cameraVision::getError(int prop_in_use)
{
    cv::Point2d point;
    point.x = 0.00;
    point.y = 0.00;
    if (prop_in_use <= 6 && prop_in_use >= 0 && calibrate)
    {
        if (prop_in_use == 0)
        {
            point.x = BrigPID->getError();
            point.y = getBrigtness();
        }
        if (prop_in_use == 1)
        {
            point.x = GainPID->getError();
            point.y = getGain();
        }
        if (prop_in_use == 2)
        {
            point.x = GainPID->getError();
            point.y = getShuttertime();
        }
        if (prop_in_use == 3)
        {
            point.x = GainPID->getError();
            point.y = getGamma();
        }
        if (prop_in_use == 4)
        {
            point.x = SatPID->getError();
            point.y = getSaturation();
        }
        if (prop_in_use == 5)
        {
            point.x = wRedPID->getError();
            point.y = getWhite_Balance_valueA();
        }
        if (prop_in_use == 6)
        {
            point.x = wBluePID->getError();
            point.y = getWhite_Balance_valueB();
        }
        return point;
    }
    else
    {
        // ROS_ERROR("Property ID Out Of Range %d",prop_in_use);point.x=0.00;
        return point;
    }
}

void cameraVision::setSatTarget(int val)
{
    satTarget = val;
    minError_Sat = satTarget * 0.08;
    writePIDConfig();
}

void cameraVision::setLumiTarget(int val)
{
    lumiTarget = val;
    minError_Lumi = lumiTarget * 0.08;
    writePIDConfig();
}

int cameraVision::getSatTarget()
{
    return satTarget;
}

int cameraVision::getLumiTarget()
{
    return lumiTarget;
}

cv::Mat cameraVision::changeContrast(cv::Mat image, double alpha)
{
    if (alpha == 0)
        return image;

    cv::Mat output_image;
    image.convertTo(output_image, -1, alpha, 0);
    return output_image;
}

cv::Mat cameraVision::changeBrightness(cv::Mat image, double beta)
{
    cv::Mat output_image;
    image.convertTo(output_image, -1, 1, beta);
    return output_image;
}

cv::Mat cameraVision::Image_Sharpening(cv::Mat image, int sharpening_type, uint8_t kernel_size, double k)
{
    cv::Mat image_sharpened;

    switch (sharpening_type)
    {
    case 0:
    {
        cv::Mat unsharp_mask, image_blurred;
        cv::GaussianBlur(image, image_blurred, cv::Size(kernel_size, kernel_size), 0);
        cv::subtract(image, image_blurred, unsharp_mask);
        add(image, k * unsharp_mask, image_sharpened);
        break;
        // addWeighted(image, 1 + amount, unsharp_mask, -amount, 0, sharp);
    }
    }
    return image_sharpened;
}