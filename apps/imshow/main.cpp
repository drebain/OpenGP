#include <OpenGP/Image/Image.h>
using namespace OpenGP;


int main(int, char**){

    Image<Vec3> I;
    imread("dice.png", I);

    mLogger() << I(240, 300);

    for (int i = 0;i < 200;i++) {
        for (int j = 0;j < 200;j++) {
            I(i, j) = Vec3(0.9,0,0);
        }
    }

    imwrite("dice2.png", I);

    return EXIT_SUCCESS;
}
