#include <cv.h>
#include <highgui.h>

#include <iostream>
using namespace std;

class Filters{
	
private:
	
	IplImage* src;
	IplImage* dst;
	
public:
	
	Filters(){}
	
	Filters(IplImage* img){
		src = img;
	}
	
	void setSource(IplImage *img){src = img;}
	
	IplImage* doBlur(){
		dst = cvCloneImage(src);
		cvSmooth(src, dst, CV_BLUR, 5, 5, 0, 0);
		return dst;
	}
	
	IplImage* doGauss(){
		dst = cvCloneImage(src);
		cvSmooth(src, dst, CV_GAUSSIAN, 5, 5, 0, 0);
		return dst;
	}
	
	IplImage* doSobel(){
		
		dst = cvCreateImage(cvGetSize(src), IPL_DEPTH_16S, 3);
		cvSobel(src, dst, 1, 0, 3);             
		IplImage* returnDst = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
		cvConvertScale(dst,returnDst, 1, 0);            
		return returnDst;
	}
	
	IplImage* doCanny(){
		
		/* convert to gray scale */
		IplImage* dst = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		cvCvtColor(src, dst, CV_RGB2GRAY);              
		cvCanny(dst, dst, 80, 160, 3);
		return dst;
	}
	
	IplImage* doHough(){
		
		/* preprocess with canny */
		IplImage* img = doCanny();
		
		/* create a memory storage */
		CvMemStorage *storage = cvCreateMemStorage(0);          
		
		CvSeq* line = cvHoughLines2(img, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 100, 100, 10);
		
		for (int i=0; i<line->total; i++) {
			CvPoint* lineEndPoints = (CvPoint*) cvGetSeqElem(line,i);
			cvLine(src, lineEndPoints[0], lineEndPoints[1], CV_RGB(255,0,0), 2);
		}               
		return src;
	}
	
	IplImage* doSkin(){
		
		/* convert to YCrCb */
		IplImage* srcYCrCb = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
		cvCvtColor(src, srcYCrCb, CV_BGR2YCrCb);
		
		/* split channels */
		IplImage* srcR = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		IplImage* srcG = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		IplImage* srcB = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);                
		
		IplImage* srcY = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		IplImage* srcCr = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		IplImage* srcCb = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);               
		
		cvSplit(src, srcB, srcG, srcR, NULL);
		cvSplit(srcYCrCb, srcY, srcCr, srcCb, NULL);            
		
		/* access image pixels */               
		for (int i=0; i<(srcCr->height); i++){
			for (int j=0; j<(srcCr->width); j++){ 
				
				double crPixelValue = cvGetReal2D(srcCr, i, j);
				double cbPixelValue = cvGetReal2D(srcCb, i, j);
				
				double rPixelValue = cvGetReal2D(srcR, i, j);
				double gPixelValue = cvGetReal2D(srcG, i, j);
				double bPixelValue = cvGetReal2D(srcB, i, j);
				
				
				if ((rPixelValue > 95 && gPixelValue > 40 && bPixelValue > 20) &&
					(fabs(rPixelValue - gPixelValue) > 15) &&
					(rPixelValue > gPixelValue && rPixelValue > bPixelValue)){
					
					if ((crPixelValue > 135 && crPixelValue < 180) &&
						(cbPixelValue > 85 && cbPixelValue < 135)) {                    
					}                                       
				}
				else{
					
					/* set non-skin pixels to black */
					CvScalar s = cvGet2D(src, i, j); 
					s.val[0]=0;s.val[1]=0;s.val[2]=0;
					cvSet2D(src,i,j,s);                                                                                                                     
				}                              
			}
		}
		return src;
	}
	
};

class FilterBySource: public Filters{
	
private:
	
	string filtername;
	
	IplImage* callFilterFromFiltername(){
		if(!filtername.compare("-blur"))
			return doBlur();
		else if(!filtername.compare("-gauss"))
			return doGauss();
		else if(!filtername.compare("-sobel"))
			return doSobel();
		else if(!filtername.compare("-canny"))
			return doCanny();
		else if(!filtername.compare("-hough"))
			return doHough();
		else if(!filtername.compare("-skin"))
			return doSkin();
		else
			return NULL;
	}
	
public:
	
	FilterBySource(string filtername){this->filtername = filtername;}
	
	void filterLiveCapture(){
		
		/* allocate resources */
		cvNamedWindow("Original", CV_WINDOW_AUTOSIZE);
		cvNamedWindow("Filtered", CV_WINDOW_AUTOSIZE);
		CvCapture* capture = cvCaptureFromCAM(0);
		
		do {
			
			IplImage* img = cvQueryFrame(capture);
			cvShowImage("Original", img);
			
			IplImage* imgClone = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
			imgClone = cvCloneImage(img);			
			setSource(imgClone);			
			IplImage* dst = callFilterFromFiltername();
			
			cvShowImage("Filtered", dst);			
			cvWaitKey(10);
			
		} while (1);
		
		/* deallocate resources */
		cvDestroyWindow("Original");
		cvDestroyWindow("Filtered");
		cvReleaseCapture(&capture);
	}
	
	int filterImage(string filename){
		
		IplImage* img = cvLoadImage(filename.c_str(), CV_LOAD_IMAGE_UNCHANGED);
		if (img!=NULL){
			
			cvNamedWindow("Original", CV_WINDOW_AUTOSIZE);
			cvNamedWindow("Filtered", CV_WINDOW_AUTOSIZE);
			cvShowImage("Original", img);
			
			setSource(img);
			IplImage* dst = callFilterFromFiltername();
			cvShowImage("Filtered", dst);
			
			cin.get();
			cvDestroyWindow("Original");
			cvDestroyWindow("Filtered");		
			return 0;
		}
		else
			return 1;
	}
	
	int filterVideo(string filename){
	
		CvCapture* capture = cvCaptureFromFile(filename.c_str());
		if (capture!=NULL){

			/* allocate resources */
			cvNamedWindow("Original", CV_WINDOW_AUTOSIZE);
			cvNamedWindow("Filtered", CV_WINDOW_AUTOSIZE);
			
			do {
				
				IplImage* img = cvQueryFrame(capture);
				cvShowImage("Original", img);
				
				IplImage* imgClone = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
				imgClone = cvCloneImage(img);		
				setSource(imgClone);
				IplImage* dst = callFilterFromFiltername();
				
				cvShowImage("Filtered", dst);			
				cvWaitKey(1);
				
			} while (1);			
			
			/* deallocate resources */
			cvDestroyWindow("Original");
			cvDestroyWindow("Filtered");
			cvReleaseCapture(&capture);
			
			return 0;
		}	
		else
			return 1;
	}
	
	bool static ifFilterAvailable(string filtername){
		if(filtername.compare("-blur") &&
		   filtername.compare("-gauss") &&
		   filtername.compare("-sobel") &&
		   filtername.compare("-canny") &&
		   filtername.compare("-hough") &&
		   filtername.compare("-skin"))
			return false;
		else 
			return true;
	}
};

int main(int argc, char* argv[]){
	
	if (argc == 2 || argc == 4){

		string filtername = argv[1];
		bool ifFilter = FilterBySource::ifFilterAvailable(filtername);
		
		if(ifFilter){

			FilterBySource *filterBySource = new FilterBySource(filtername);					
			
			if (argc == 2) 	
				filterBySource -> filterLiveCapture();									
			else{
				
				char flag = *(argv[2] + 1);
				flag = tolower(flag);			
				if (flag == 'i' || flag == 'v'){
					
					string filename = argv[3];
					if (flag == 'i'){
						if(filterBySource -> filterImage(filename))
							cout << "cannot load file";
					}
					else{
						if(filterBySource -> filterVideo(filename))				
							cout << "cannot load file";
					}
				}
			}
			
			delete(filterBySource);
		}	
		else{
			cout << "filter not available" << endl;
			return 0;
		}				
	}
	else{
		cout << "usage: filter <filterName> [<filenameFlag> <filename>]" << endl;
		cout << "filterName: [-blur] | [-gauss] | [-sobel] | [-canny] | [-hough] | [-skin]" << endl;
		cout << "filenameFlag: [-i] | [-v]" << endl;			
	}
	return 0;
}
