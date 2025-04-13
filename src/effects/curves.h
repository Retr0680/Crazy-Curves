#ifndef CURVES_H
#define CURVES_H

class Curves {
public:
    Curves();
    ~Curves();

    void applyCurveAdjustment(float* input, float* output, int length);
    void setCurvePoints(const std::vector<float>& points);
    
private:
    std::vector<float> curvePoints;
};

#endif // CURVES_H