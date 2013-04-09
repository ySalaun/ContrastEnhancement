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
				float temp = (*(im.pixel(x,y)+c*wh));
				data[x+im.w*y+c*wh] = (temp>255)?255:(temp<0)?0:temp;
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
float* gaussianKernel(const float sd, const bool normalized){
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
	if(normalized){
		for(int i=0; i<2*n+1; ++i){
			kernel[i] /= sum;
		}
	}

	return kernel;
}

LWImage<float> bilat(const LWImage<float>& I, const float* kernel, const float kernel_size, const float sd){
	// parameters
	int i, j, ii, jj, c;
	float sum, norm_sum, color, color_center, gaussian_color, coeff;
	int w = I.w, h = I.h, wh=w*h;
	int ks = kernel_size/2;
	float* data = new float[wh*3];

	// loop for convolution
	for(c =0; c<3; ++c){
		for(i=0; i<w; ++i){
			for(j=0; j<h; ++j){
				sum = 0;
				norm_sum= 0;
				color_center = *(I.pixel(i,j)+c*wh);
				for(ii=-ks; ii<=ks; ++ii){
					for(jj=-ks; jj<=ks; ++jj){
						color = intensity(I, i+ii, j+jj, c*wh);
						gaussian_color = (color-color_center)/sd;
						gaussian_color = exp(-0.5*gaussian_color*gaussian_color);
						coeff = gaussian_color*kernel[ii+ks]*kernel[jj+ks];
						sum += color*coeff;
						norm_sum += coeff;
					}
				}
				data[i+w*j+c*wh] = sum/norm_sum;
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

LWImage<float> soft_contrast(const LWImage<float> u, const float lambda){
	// parameters
	int i, j, c, w = u.w, h = u.h, wh=w*h;
	float* data = new float[wh*3];
	float vij;

	// computing v = clamp((u-128)*lambda + 128, 0, 255) on the whole picture
	for(c =0; c<3; ++c){
		for(i=0; i<w; ++i){
			for(j=0; j<h; ++j){
				vij = lambda * ((*(u.pixel(i,j)+c*wh))-128) + 128;
				data[i+w*j+c*wh] = (vij > 255)?255:((vij<0)?0:vij);
			}
		}
	}
	LWImage<float> v(data, w, h);
	return v;
}

LWImage<float> blend(const LWImage<float> usm, const LWImage<float> sc){
	// parameters
	int i, j, c, w = usm.w, h = usm.h, wh=w*h;
	float* data = new float[wh*3];
	float uij, sij, lambda;

	// computing v = clamp((u-128)*lambda + 128, 0, 255) on the whole picture
	for(c =0; c<3; ++c){
		for(i=0; i<w; ++i){
			for(j=0; j<h; ++j){
				uij = (*(usm.pixel(i,j)+c*wh));
				sij = (*(sc.pixel(i,j)+c*wh));
				lambda = exp(-abs(uij-sij)/30);
				data[i+w*j+c*wh] = lambda*sij+(1-lambda)*uij;
			}
		}
	}
	LWImage<float> v(data, w, h);
	return v;
}


int main (int argc, char** argv)
{
	if(argc != 8){
		std::cerr << "Usage: " << argv[0]
				   << " input.png filtered.png output_path sd s sdi method"
				   << std::endl;
		return 1;
	}

    // Read image input
    LWImage<float> i_picture(0,0,0);
    if(! (loadImage(argv[1],i_picture)))
        return 1;

	LWImage<float> i_filtered(0,0,0);
    if(! (loadImage(argv[2],i_filtered)))
        return 1;

    // Read method
	float s, sd, sdi;		// sd = standard deviation of gaussian kernel
							// s = parameter for usm
							// sd = standard deviation of intensity gaussian kernel for bilateral filter
	float lambda;			// lambda = parameter for lowering contrast
    std::istringstream f4(argv[4]), f5(argv[5]), f6(argv[6]), f7(argv[7]);
    if(! ((f4>>sd).eof()) || !((f5>>s).eof()) || !((f6>>sdi).eof())|| !((f7>>lambda).eof())) {
        std::cerr << "Error reading s or sd or sdi" << std::endl;
        return 1;
    }

	// computationnal process
	
	// USM method

	if(sdi == 0){
		message("kernel computation");								
		float size = 2*floor(3*sd)+1;								// size of the kernel
		float* kernel = gaussianKernel(sd, sdi==0);					// kernel
	
		message("convolution process");										
		// linear filter
		message("linear filter");
		i_filtered = convol1D(i_picture, kernel, size, true);		// i_filtered is the less resolute version of the input picture
		i_filtered = convol1D(i_filtered, kernel, size, false);
	}
	
	LWImage<float> o_usm;

	if(lambda == 1){
		message("USM method");
		message("contrast enhancement");
		o_usm = usm(i_picture, i_filtered, s);
	}
	else{
		message("soft contrast blending method");
		LWImage<float> sc_pic = soft_contrast(i_picture, lambda);
		message("contrast enhancement");
		o_usm = usm(i_picture, i_filtered, s);
		message("blending pictures");
		o_usm = blend(o_usm, sc_pic);
	}
	
	message("saving process");
	saveImage(argv[3], o_usm);
	
    free(i_picture.data);

    return 0;
}
