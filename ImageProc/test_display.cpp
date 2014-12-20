#include "singleton_factory.hpp"
#include <iostream>
#include <string>
#include <opencv2/highgui/highgui.hpp>

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
  
  auto processorFactory = SingletonFactory<std::function<std::unique_ptr<Processor>()>>::getInstance();
  auto names = processorFactory.getNames();
  auto processor = processorFactory.getObject( names[0] )();
  auto output = (*processor)( input.clone() );
  
  cv::namedWindow( "Input", CV_WINDOW_AUTOSIZE );
  cv::namedWindow( "Processed", CV_WINDOW_AUTOSIZE );
  cv::imshow( "Input", input);
  cv::imshow( "Processed", output);
  cv::waitKey();

  return 0;
}
