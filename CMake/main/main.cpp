#include "main/parameters.h"
#include "libIO/io_png.h"
#include "libIO/cmdLine.h"

#include <algorithm>
#include <limits>
#include <iostream>
#include <sstream>

void test(float n = 0){
	std::cerr << "test" << n << std::endl;
}

void message(const char* m){
	std::cerr << "-------------" << m << "-------------" << std::endl;
}

static bool loadImage(const char* name, LWImage<float>& im) {
    size_t nx, ny;
    im.data = io_png_read_f32_rgb(name, &nx, &ny);
    im.w = nx; im.h = ny;
    if(! im.data)
        std::cerr << "Unable to load image file " << name << std::endl;
    return (im.data!=0);
}

static bool saveImage(const char* name, const LWImage<float>& im){
	float* data = new float[im.h*im.w*3];
	int w =im.w, h= im.h, wh = h*w;
	for(int c=0; c<3; ++c){
		for(int x=0; x<w; ++x){
			for(int y=0; y<h; ++y){
				data[x+im.w*y+c*wh] = (*(im.pixel(x,y)+c*wh));
			}
		}
	}
	
	if(io_png_write_f32(name, data, im.w, im.h, 3) != 0) {
        std::cerr << "Error writing file " << name << std::endl;
        return 1;
	}
	return 0;
}

float intensity(const LWImage<float>& I, const int i, const int j, const int col_offset){
	if(i>0 && i<I.w && j>0 && j<I.h){
		return *(I.pixel(i,j)+col_offset);
	}
	return 0.0f;
}

// computes a gaussian 1D kernel of standard deviation sd
float* gaussianKernel(const float sd){
	// parameters
	int i, n;
	float value, sum;

	// kernel half-size
	n = floor(3*sd);

	// output kernel
	float* kernel = new float[2*n+1];

	// kernel filling
	const float invs2 = -1/(2*sd*sd);
	sum = 0;
	for(int i=0; i<n; ++i){
		value = exp(i*i*invs2);
		sum += 2*value;
		kernel[n-1-i] = value;
		kernel[n+1+i] = value;
	}
	sum += 1;
	kernel[n] = 1;

	// normalize kernel
	for(int i=0; i<2*n+1; ++i){
		kernel[i] /= sum;
	}

	return kernel;
}

LWImage<float> bilat(const LWImage<float>& I, const float* kernel, const float kernel_size, const float sd){
	// parameters
	int i, j, ii, jj, c;
	float sum, color;
	int w = I.w, h = I.h, wh=w*h;
	int ks = kernel_size/2;
	float* data = new float[wh*3];

	// loop for convolution
	for(c =0; c<3; ++c){
		for(i=0; i<w; ++i){
			for(j=0; j<h; ++j){
				sum = 0;
				for(ii=-ks; ii<=ks; ++ii){
					for(ii=-ks; ii<=ks; ++ii){
						color = intensity(I, i+ii, j+jj, c*wh);
						sum += color*kernel[ii+ks];
				}
				data[i+w*j+c*wh] = sum;
			}
		}
	}
	LWImage<float> convolved(data, w, h);
	return convolved;
}

LWImage<float> convol1D(const LWImage<float>& I, const float* kernel, const float kernel_size, bool horizontal){
	// parameters
	int i, j, ii, c;
	float sum, color;
	int w = I.w, h = I.h, wh=w*h;
	int ks = kernel_size/2;
	float* data = new float[wh*3];

	// loop for convolution
	for(c =0; c<3; ++c){
		for(i=0; i<w; ++i){
			for(j=0; j<h; ++j){
				sum = 0;
				for(ii=-ks; ii<=ks; ++ii){
					if(horizontal){
						color = intensity(I, i+ii, j, c*wh);
					}
					else{
						color = intensity(I, i, j+ii, c*wh);
					}
					sum += color*kernel[ii+ks];
				}
				data[i+w*j+c*wh] = sum;
			}
		}
	}
	LWImage<float> convolved(data, w, h);
	return convolved;
}

LWImage<float> usm(const LWImage<float> u, const LWImage<float> Mu, float s){
	// parameters
	int i, j, c, w = u.w, h = u.h, wh=w*h;
	float* data = new float[wh*3];

	// computing v = u + s*(u-Mu) on the whole picture
	for(c =0; c<3; ++c){
		for(i=0; i<w; ++i){
			for(j=0; j<h; ++j){
				data[i+w*j+c*wh] = (1+s)*(*(u.pixel(i,j)+c*wh)) - s*(*(Mu.pixel(i,j)+c*wh));
			}
		}
	}
	LWImage<float> v(data, w, h);
	return v;
}


int main (int argc, char** argv)
{
	if(argc != 6) {
        std::cerr << "Usage: " << argv[0]
                  << " im.png method output_path sd s"
                  << std::endl;
        return 1;
    }

    // Read image input
    LWImage<float> i_picture(0,0,0);
    if(! (loadImage(argv[1],i_picture)))
        return 1;

    // Read method
    int method;
	float s, sd;		// sd = standard deviation of gaussian kernel
						// s = parameter for usm
    std::istringstream f(argv[2]), g(argv[4]), h(argv[5]);
    if(! ((f>>method).eof()) || !((g>>sd).eof()) || !((h>>s).eof())) {
        std::cerr << "Error reading method" << std::endl;
        return 1;
    }

	// computationnal process
	
	// USM method
	message("USM method");
	
	message("kernel computation");								
	float size = 2*floor(3*sd)+1;								// size of the kernel
	float* kernel = gaussianKernel(sd);							// kernel
	
	message("convolution process");
	LWImage<float> M = convol1D(i_picture, kernel, size, true);	// M is the less resolute version of the input picture
	M = convol1D(M, kernel, size, false);

	message("contrast enhancement");
	LWImage<float> o_usm = usm(i_picture, M, s);
	
	message("saving process");
	saveImage(argv[3], o_usm);
	
    free(i_picture.data);

    return 0;
}
