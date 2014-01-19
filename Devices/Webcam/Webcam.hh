#ifndef WEBCAM_HH
#define WEBCAM_HH

class Webcam:
{
public:
    Webcam();
    ~Webcam();

    bool init();
    bool setOn();
    bool setOff();
    bool isOn();

    std::string getIP();
    
private:
    bool status_;
};

#endif // WEBCAM_HH