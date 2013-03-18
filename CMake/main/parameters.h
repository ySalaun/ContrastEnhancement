#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "libLWImage/LWImage.h"

class PARAMETERS{
	public:
	/*--------------PICTURES---------------*/
	// left and right pictures for disparity computation
	LWImage<float> im;

	// height, width of pictures and their product
	int h, w, wh;

	/*--------------METHOD---------------*/
	int m;

	/*---INITIALIZATION & DEFAULT VALUES---*/
	// initialization of parameters need the 2 images and the disparity range
	PARAMETERS(LWImage<float> image, int method){
		// images
		im = image;

		// width and height of images
		h = image.h;
		w = image.w;
		wh = w*h;

		// method
		m = method;
	}
    ~PARAMETERS() {
    }
};

#endif
