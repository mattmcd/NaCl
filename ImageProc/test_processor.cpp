#include "singleton_factory.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
  cv::Mat input = cv::Mat( 2, 2, CV_8UC4, cv::Scalar(128,192,216,255));
  std::cout << "Input = " << std::endl << input << std::endl << std::endl;

  auto processorName = "Invert";
  if (argc > 1) {
    processorName = argv[1];
  }
  auto processorFactory = SingletonFactory<std::function<std::unique_ptr<Processor>()>>::getInstance();
  auto processor = processorFactory.getObject( processorName )();
  auto output = (*processor)( input );
  
  std::cout << "Output = " << std::endl << output << std::endl << std::endl;

  return 0;
}
