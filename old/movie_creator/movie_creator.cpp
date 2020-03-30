#include "movie_creator.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void createMovie()
{
    std::string image_path = "../images/";
    std::vector<std::string> stringarray;
    boost::filesystem::path b_path(image_path);
    boost::filesystem::directory_iterator end;
    int num_files = 0;
    for (boost::filesystem::directory_iterator i(b_path); i != end; ++i)
    {
        num_files++;
    }

    const std::string movie_full_path = "../movie.avi";
    const cv::Mat tmp_image = cv::imread(image_path + "0.png");
    cv::Size frame_size = cv::Size(tmp_image.cols, tmp_image.rows);
    const double frames_per_second = 25.0f;

    cv::VideoWriter output_video;
    output_video.open(movie_full_path, CV_FOURCC('D', 'I', 'V', 'X'), frames_per_second, frame_size, true);
    if (output_video.isOpened())
    {
        for (int i = 0; i < num_files - 1; i += 2)
        {
            const cv::Mat image = cv::imread(image_path + std::to_string(i) + ".png");
            cv::Mat flipped; // dst must be a different Mat
            cv::flip(image, flipped, 0);
            output_video << flipped;
            std::cout << "Frame: " << i << std::endl;
        }
    }
    else
    {
        std::cout << "Could not open video files!" << std::endl;
        // Error Failed to open video file
    }

    output_video.release();
}