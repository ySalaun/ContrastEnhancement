#include "main/parameters.h"
#include "libIO/io_png.h"
#include "libIO/cmdLine.h"

#include <algorithm>
#include <limits>
#include <iostream>
#include <sstream>

static bool loadImage(const char* name, LWImage<float>& im) {
    size_t nx, ny;
    im.data = io_png_read_f32_rgb(name, &nx, &ny);
    im.w = nx; im.h = ny;
    if(! im.data)
        std::cerr << "Unable to load image file " << name << std::endl;
    return (im.data!=0);
}

int main (int argc, char** argv)
{
    if(argc != 4) {
        std::cerr << "Usage: " << argv[0]
                  << "im.png method"
                  << std::endl;
        return 1;
    }

    // Read image input
    LWImage<float> i_picture(0,0,0);
    if(! (loadImage(argv[1],i_picture)))
        return 1;

    // Read method
    int method;
    std::istringstream f(argv[2]);
    if(! ((f>>method).eof())) {
        std::cerr << "Error reading method" << std::endl;
        return 1;
    }

	// computationnal process
	std::cerr << "open picture " << argv[1] << " and use method " << method << " and save in " << argv[3] << std::endl;

    free(i_picture.data);

    return 0;
}
