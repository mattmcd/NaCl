#include "singleton_factory.hpp"
#include <iostream>
#include <string>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

int main(int argc, char* argv[])
{

  auto fname = "mmcdonne.jpg";
  if (argc > 1) {
    fname = argv[1];
  }
  cv::Mat input;
  input = cv::imread( fname, CV_LOAD_IMAGE_COLOR );
  if (!input.data) {
    std::cout << "Could not read image " << fname << std::endl;
    return -1;
  }
  // Processors expect 8UC4
  cv::Mat im;
  cv::cvtColor( input, im, CV_BGR2RGBA );
  
  // Use first processor
  auto processorFactory = SingletonFactory<std::function<std::unique_ptr<Processor>()>>::getInstance();
  auto names = processorFactory.getNames();
  auto processor = processorFactory.getObject( names[0] )();
  if ( names[0] == "Smiley!" ) {
    cv::Mat smiley;
    smiley = cv::imread( "smiley_200x200.png", CV_LOAD_IMAGE_UNCHANGED );
    processor->init( smiley );
  }
  auto output = (*processor)( im );
  
  cv::cvtColor( output, output, CV_RGBA2BGR );
  
  cv::namedWindow( "Input and Processed", CV_WINDOW_AUTOSIZE );
  // Create large image showing original and processed side by side
  cv::Mat combined(input.rows, input.cols + output.cols, input.type());
  cv::Mat roiLeft = combined(cv::Rect(0,0,input.cols,input.rows));
  cv::Mat roiRight = combined(cv::Rect(input.cols,0,output.cols,output.rows));
  input.copyTo( roiLeft );
  output.copyTo( roiRight );
  cv::imshow( "Input and Processed", combined);
  cv::moveWindow( "Input and Processed", 100, 100 );
  cv::waitKey();

  return 0;
}
