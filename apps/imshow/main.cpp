#include <OpenGP/Image/Image.h>
using namespace OpenGP;


int main(int, char**){

    Image<Vec3> I;
    imread("planet.png", I);

    mLogger() << I(240, 300);

    return EXIT_SUCCESS;
}
