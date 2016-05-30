#include "BlobDetector.hpp"

BlobDetector::BlobDetector(int kernelShape, Size kernelSize, Point kernelAnchorPoint, int minArea, int maxArea, int minT, int maxT){
	morphologicalKernel = getStructuringElement(kernelShape, kernelSize, kernelAnchorPoint);
	minAreaBlob = minArea;
	maxAreaBlob = maxArea;

#if vOpenCV
	backgroungSubtractor = createBackgroundSubtractorMOG2();
#else
	backgroungSubtractor = BackgroundSubtractorMOG2();
#endif

	MIN_THRESHOLD = minT;
	DEST_THRESHOLD = maxT;
}


/*** Reseta Background ***/
/**************************************************/
void BlobDetector::resetBackground(){
#if vOpenCV
	backgroungSubtractor = createBackgroundSubtractorMOG2();
#else
	backgroungSubtractor = BackgroundSubtractorMOG2();
#endif
} /************************************************/


/*** Operacoes 'dilate' e 'erode' consecutivas preparando imagem para melhor tratamento ***/
/**********************************************************/
void BlobDetector::morphologyOperations(Mat orig, Mat dest){
	//blur(orig, dest,Size(3,3));
	blur(orig, dest, Size(5,5));

	dilate(orig, dest, morphologicalKernel);
	erode(orig, dest, morphologicalKernel);

	erode(orig, dest, morphologicalKernel);
	dilate(orig, dest, morphologicalKernel);
  
	dilate(orig,dest,morphologicalKernel);
} /*********************************************************/


/*** Procura Blobs na imagem ***/
/*************************************************************/
Mat BlobDetector::findBlobs(Mat orig){
	Mat foreground;
	vector < vector<Point> > contours;

#if vOpenCV
	backgroungSubtractor->apply(orig.clone(), foreground, learnRate); //3o PARAMETRO: Learning rate do BackgroundSubtractor
#else
	backgroungSubtractor(orig.clone(), foreground, learnRate);
#endif

	threshold(foreground, foreground, MIN_THRESHOLD, DEST_THRESHOLD, CV_THRESH_BINARY);

	findContours(foreground.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	hulls.clear();
	for(int i = 0; i < contours.size(); i++){
		vector<Point> hull;
		convexHull( Mat(contours[i]), hull, false);
		double area = contourArea(hull);
		if(area > minAreaBlob && area < maxAreaBlob)
			hulls.push_back(hull);
	}

	return foreground;
} /***********************************************************/

/*** Procura Ellipses na imagem ***/
/*************************************************************/
Mat BlobDetector::findEllipses(Mat orig){
	Mat foreground;
	vector < vector<Point> > contours;

//	cvtColor(orig.clone(), orig, CV_BGR2GRAY);
	GaussianBlur( orig, orig, Size( 9, 9 ), 2, 2 );

#if vOpenCV
	backgroungSubtractor->apply(orig.clone(), foreground, learnRate); //3o PARAMETRO: Learning rate do BackgroundSubtractor
#else
	backgroungSubtractor(orig.clone(), foreground, learnRate);
#endif

	threshold(foreground, foreground, MIN_THRESHOLD, DEST_THRESHOLD, CV_THRESH_BINARY);

	findContours(foreground.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	minEllipse.clear();
	for( int i = 0; i < contours.size(); i++ ){
		if( contours[i].size() > 20){
			RotatedRect ellHull = fitEllipse( Mat(contours[i]) );
			Size2f areaEll = ellHull.size;
			double area = areaEll.width * areaEll.height;
			if(area > minAreaBlob && area < maxAreaBlob){
				if(areaEll.width/areaEll.height <= ECC_ELL && areaEll.height/areaEll.width <= ECC_ELL) minEllipse.push_back( ellHull );
			}
		}
	}

	return foreground;
} /***********************************************************/

/*** Aplica Hough Transform na imagem ***/
/*************************************************************/
Mat BlobDetector::findHough(Mat orig){	
	houghBlobs.clear();
	cvtColor(orig.clone(), orig, CV_BGR2GRAY);
	GaussianBlur( orig, orig, Size(9, 9), 2, 2 );
	HoughCircles( orig, houghBlobs, CV_HOUGH_GRADIENT, 1, orig.rows/16, 125, 30, 0, 0 );

	return orig;
} /***********************************************************/



/*** Devolve lista de blobs ***/ 
/***********************************************/
vector< vector<Point> > BlobDetector::getBlobs(){
	return hulls;
} /*********************************************/

/*** Devolve lista de elipses ***/ 
/***********************************************/
vector< RotatedRect > BlobDetector::getEllipses(){
	return minEllipse;
} /*********************************************/

/*** Devolve lista de hough circles ***/ 
/***********************************************/
vector< Vec3f > BlobDetector::getHough(){
	return houghBlobs;
} /*********************************************/


/*** Desenha blobs identificados na imagem ***/
/****************************************************/
void BlobDetector::drawBlobs(Mat frame, Scalar color){
	for(int i = 0; i < hulls.size(); i++){
		drawContours(frame, hulls, i, color, 1, 8, vector<Vec4i>(), 0, Point());
	}
} /**************************************************/

/*** Desenha elipses identificadas na imagem ***/
/****************************************************/
void BlobDetector::drawEllipses(Mat frame, Scalar color){
	for(int i = 0; i < minEllipse.size(); i++){
		ellipse(frame, minEllipse[i], color, 2, 8 );
	}
} /**************************************************/

/*** Desenha Hough Circles identificados na imagem ***/
/****************************************************/
void BlobDetector::drawHough(Mat frame, Scalar color){
	for(int i = 0; i < houghBlobs.size(); i++){
		Point center( cvRound(houghBlobs[i][0]), cvRound(houghBlobs[i][1]) );
		int radius = cvRound(houghBlobs[i][2]);
		circle( frame, center, radius, color, 2, 8, 0);
	}
} /**************************************************/

		


void BlobDetector::boundBlobs(Mat frame, Scalar color){
	vector< vector<Point> >::const_iterator it = hulls.begin();
	vector< vector<Point> >::const_iterator end = hulls.end();
  
	while(it != end){
		Rect bounds = boundingRect(*it);
		rectangle(frame, bounds, color, 2);
		++it;
	}
}










