#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include "../IO/IOFunctions.hpp"

namespace Graphics {
	namespace CameraUtils {
		/// @brief Initializes the camera and returns the chosen index
		unsigned initializeCamera();
	}

	class ImageManager {
	public:

		// Create a detection square at (SquareX, SquareY)
		static void createSquare(unsigned SquareX, unsigned SquareY, unsigned SquareKey, std::string SquareDisplayName);

		// Resize jump-area dependent images
		static void resizeJumpArea();

		// Draw the jump line and status UI
		void drawJumpLine(cv::Mat imgFlip);

		// Preprocess region above jump line
		void preprocessImageAboveLine(cv::Mat imgFlip);

		// Preprocess squares and update their frames
		void preprocessImage(cv::Mat imgFlip);

		// Draw all configured squares
		void drawSquares(cv::Mat imgFlip);
	};
}