#include "heart.h"
#include <vector>
#include <unordered_set>
using namespace cv;


int main(void) {
    Mat img = Mat::zeros(HEIGHT, WIDTH, CV_8UC3);
    Heart heart;
    heart.build(2000);
    heart.draw(img, 150);
    return(0);
}



