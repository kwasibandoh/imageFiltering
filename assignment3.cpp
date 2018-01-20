#include <CImg.h>
using cimg_library::CImg;

float sub128(const float filterPixelVal){
	return (filterPixelVal - 128);
}
unsigned char add128(const unsigned char filterPixelVal){
	return (filterPixelVal + 128);
}
float clipOutputPixel(const float outputImgPixel){
	unsigned char safeOutputPixel = outputImgPixel;
	if(outputImgPixel > 255){
		safeOutputPixel = 255;
	}
	if(outputImgPixel < 0){
		safeOutputPixel = 0;
	}
	return safeOutputPixel;
}

unsigned char computeFilter(const CImg<unsigned char>& image, const CImg<unsigned char>& filter, int x, int y, int s){
	int filterDistanceFromCenterX = filter.width()/2;
	int filterDistanceFromCenterY = filter.height()/2;
	int topLeftOfFilterX = 0, topLeftOfFilterY = 0, workingFilterX = 0, workingFilterY = 0;
	int workingImgX = x - filterDistanceFromCenterX, workingImgY = y - filterDistanceFromCenterY;
	int topLeftOfImageX = x - filterDistanceFromCenterX, topLeftOfImageY = y - filterDistanceFromCenterY;
	float totalSummation = 0, divisor = 0, subOutput = 0;
	unsigned char finalOutput = 0; 

	for(int i = 0; i < filter.height(); i++){
		workingFilterY = topLeftOfFilterY + i;
		workingImgY = topLeftOfImageY + i;

		for(int j = 0; j < filter.width(); j++){
			workingFilterX = topLeftOfFilterX + j;
			workingImgX = topLeftOfImageX + j;

			if(workingImgY >= 0 && workingImgY < image.height() && 
			   workingImgX >= 0 && workingImgX < image.width()){					  

			    totalSummation += (sub128((float)image(workingImgX, workingImgY, image.depth() - 1, s))) * 
			   					 (sub128((float)filter(workingFilterX, workingFilterY, filter.depth() - 1, filter.spectrum()-1)));
			   	//std::cout<<"Current Summation: "<< totalSummation << "\n";


				divisor += sub128((float)filter(workingFilterX, workingFilterY, filter.depth() - 1, filter.spectrum() - 1));
				//std::cout<<"Current Divisor: "<< divisor<< "\n\n";
			}
		}
	}
	if(totalSummation == 0 || divisor == 0){
		finalOutput = 0;
	}
	else{
		subOutput = (totalSummation/divisor);
		//std::cout<<"subOutput: "<< subOutput<< "\n\n";
		subOutput = add128(subOutput);
		subOutput = clipOutputPixel(subOutput);
		finalOutput = static_cast<unsigned char>(subOutput);
	};

	return finalOutput;
}

CImg<unsigned char> FilterImage(const CImg<unsigned char>& image, const CImg<unsigned char>& filter){
	CImg<unsigned char> filteredImage(image.width(), image.height(), image.depth(), image.spectrum(), 0);
	int imgWidth = image.width(), imgHeight = image.height(), imgDepth = image.depth(), imgSpectrum = image.spectrum();
	int i, j, k;
	for(i = 0; i < imgSpectrum; i++){
		for(j = 0; j < imgHeight; j++){
			for(k = 0; k < imgWidth; k++){
				filteredImage(k, j, imgDepth - 1, i) = computeFilter(image, filter, k, j, i);
			}
		}
	}
	return filteredImage;
}
