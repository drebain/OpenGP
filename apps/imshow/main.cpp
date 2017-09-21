#include <OpenGP/Image/Image.h>
using namespace OpenGP;


int main(int, char**){

    Image<Vec4> I;
    imread("planet.png", I);

    mLogger() << I(240, 300);

    I(240, 300) = Vec4(1,0,0,1);

    imwrite("planet2.png", I);

    return EXIT_SUCCESS;
}
