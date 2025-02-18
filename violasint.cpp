#include <pdal/PointView.hpp>
#include <pdal/StageFactory.hpp>
#include <pdal/io/LasReader.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace pdal;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input.las> <output.png>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = argv[2];

    // Set up PDAL LAS reader
    StageFactory factory;
    LasReader reader;
    Options options;
    options.add("filename", inputFile);
    reader.setOptions(options);
    
    // Prepare pipeline
    PointTable table;
    reader.prepare(table);
    PointViewSet viewSet = reader.execute(table);
    PointViewPtr view = *viewSet.begin();

    // Determine point cloud bounds
    double minX = std::numeric_limits<double>::max(), maxX = std::numeric_limits<double>::lowest();
    double minY = std::numeric_limits<double>::max(), maxY = std::numeric_limits<double>::lowest();
    double minIntensity = std::numeric_limits<double>::max(), maxIntensity = std::numeric_limits<double>::lowest();

    for (PointId i = 0; i < view->size(); ++i) {
        double x = view->getFieldAs<double>(Dimension::Id::X, i);
        double y = view->getFieldAs<double>(Dimension::Id::Y, i);
        double intensity = view->getFieldAs<double>(Dimension::Id::Intensity, i);

        minX = std::min(minX, x);
        maxX = std::max(maxX, x);
        minY = std::min(minY, y);
        maxY = std::max(maxY, y);
        minIntensity = std::min(minIntensity, intensity);
        maxIntensity = std::max(maxIntensity, intensity);
    }

    // Define image resolution
    int width = 1080, height = 1080;
    cv::Mat image = cv::Mat::zeros(height, width, CV_8UC1);

    // Populate image with intensity values
    for (PointId i = 0; i < view->size(); ++i) {
        double x = view->getFieldAs<double>(Dimension::Id::X, i);
        double y = view->getFieldAs<double>(Dimension::Id::Y, i);
        double intensity = view->getFieldAs<double>(Dimension::Id::Intensity, i);
        
        // Normalize coordinates to image dimensions
        int imgX = static_cast<int>(((x - minX) / (maxX - minX)) * (width - 1));
        int imgY = static_cast<int>(((y - minY) / (maxY - minY)) * (height - 1));
        
        // Normalize intensity to 8-bit grayscaleb
        float gamma = 0.5; // Adjust between 0.1 (very bright) to 1.0 (original)
        int imgIntensity = static_cast<int>(255.0 * pow((intensity - minIntensity) / (maxIntensity - minIntensity), gamma));
        
        uchar& current = image.at<uchar>(height - 1 - imgY, imgX);
        if (imgIntensity > current) {
            current = imgIntensity; // Keep the brightest value
        }
    }

    // Save the image
    cv::Mat equalizedImage;
    cv::equalizeHist(image, equalizedImage);
    cv::imwrite(outputFile, equalizedImage); 
    std::cout << "Saved intensity image to " << outputFile << std::endl;
    
    return 0;
}
