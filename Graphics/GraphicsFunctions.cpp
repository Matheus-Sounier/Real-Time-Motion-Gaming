#include <opencv2/opencv.hpp>
#include <vector>
#include <random>
#include <algorithm>
#include "GraphicsFunctions.hpp"
#include "Var/GraphicsValues.hpp"
#include "../IO/IOFunctions.hpp"
#include "../Detection/Var/DetectionValues.hpp"

using namespace std;
using namespace cv;

static Ptr<CLAHE> g_clahe; // CLAHE shared instance

unsigned Graphics::CameraUtils::initializeCamera() {
    for (unsigned i = 0; i < 5; i++) {

        VideoCapture cap(i);

        if (cap.isOpened()) {

            cout << "Camera index chosen: " << i << endl;
            cap.release();

            return i;
        }
    }
    cerr << "The program could not find an available camera on the system" << endl;

    return 1;
}

void Graphics::ImageManager::createSquare(unsigned SquareX, unsigned SquareY, unsigned SquareKey, string SquareDisplayName) {
    if (SquareX > 0 && SquareY > 0 && SquareX + 80 <= GraphicsValues::CVMatFrames::imgFlip.size().width && SquareY + 100 <= GraphicsValues::CVMatFrames::imgFlip.size().height) {
        try {
            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<int> dis(1, 1000);

            GraphicsValues::CVSquares::Rectangles new_square{};
            new_square.TL = Point(SquareX, SquareY);
            new_square.BR = Point(SquareX + 80, SquareY + 100);
            new_square.COLOR = Scalar(0, 0, 255); // BGR FORMAT
            new_square.KEY = SquareKey;
            new_square.DISPLAYKEY = SquareDisplayName;
            new_square.SELECTED = false;
            new_square.MOTION_DETECTED = false;
            new_square.ID = dis(gen);

            GraphicsValues::CVSquares::Squares.push_back(new_square);

            GraphicsValues::CVSquares::Images new_frame{};
            new_frame.ID = new_square.ID;
            GraphicsValues::CVSquares::Frames.push_back(new_frame);

            IO::FileManager::saveSquares();
        }
        catch (const exception& e) {
            cerr << "It was not possible to create a square, the program returned the following error: " << e.what() << endl;
        }
    }
}

void Graphics::ImageManager::resizeJumpArea() {
    try {
        Size new_jumpareasize(GraphicsValues::CVMatFrames::imgFlip.size().width, GraphicsValues::CVJumpLine::Lines[0].Position.y);

        resize(GraphicsValues::CVMatFrames::JumpIMGCrop, GraphicsValues::CVMatFrames::JumpIMGCrop, new_jumpareasize);
        resize(GraphicsValues::CVMatFrames::JumpIMGGray, GraphicsValues::CVMatFrames::JumpIMGGray, new_jumpareasize);
        resize(GraphicsValues::CVMatFrames::JumpIMGBackground, GraphicsValues::CVMatFrames::JumpIMGBackground, new_jumpareasize);
        resize(GraphicsValues::CVMatFrames::JumpIMGThres, GraphicsValues::CVMatFrames::JumpIMGThres, new_jumpareasize);
        resize(GraphicsValues::CVMatFrames::JumpIMGSub, GraphicsValues::CVMatFrames::JumpIMGSub, new_jumpareasize);
        resize(GraphicsValues::CVMatFrames::JumpIMGDil, GraphicsValues::CVMatFrames::JumpIMGDil, new_jumpareasize);
    }
    catch (const exception& e) {
        cerr << "It was not possible to resize the jump detection image, the program returned the following error: " << e.what() << endl;
    }
}

void Graphics::ImageManager::drawJumpLine(Mat imgFlip) {
    try {
        if (GraphicsValues::CVJumpLine::Lines.size() == 0) {

            GraphicsValues::CVJumpLine::Line new_jumpline;
            new_jumpline.Position = Point(0, 120);
            new_jumpline.COLOR = Scalar(255, 255, 255); // BGR FORMAT
            new_jumpline.SELECTED = false;

            GraphicsValues::CVJumpLine::Lines.push_back(new_jumpline);
        }

        if (GraphicsValues::CVJumpLine::Lines[0].SELECTED == true) {

            GraphicsValues::CVJumpLine::Lines[0].COLOR = Scalar(255, 0, 0); // BGR FORMAT
        }
        if (GraphicsValues::CVJumpLine::Lines[0].SELECTED == false) {

            GraphicsValues::CVJumpLine::Lines[0].COLOR = Scalar(255, 255, 255); // BGR FORMAT
        }

        line(imgFlip, GraphicsValues::CVJumpLine::Lines[0].Position, Point(imgFlip.size().width, GraphicsValues::CVJumpLine::Lines[0].Position.y), GraphicsValues::CVJumpLine::Lines[0].COLOR, 1.5);

        GraphicsValues::CVJumpLine::LineExists = true;

        try {
            const int barWidth = 300;
            const int barHeight = 18;
            int x = 20;
            int y = imgFlip.size().height - 40;

            if (DetectionValues::chargingJump) {
                int durationSoFar = DetectionValues::currentChargingMs;
                // background
                rectangle(imgFlip, Point(x, y), Point(x + barWidth, y + barHeight), Scalar(50, 50, 50), cv::FILLED);
                // compute fraction using same clamps
                const int MIN_POWER = 50;
                const int MAX_POWER = 800;
                int clamped = durationSoFar;
                if (clamped < MIN_POWER) clamped = MIN_POWER;
                else if (clamped > MAX_POWER) clamped = MAX_POWER;
                float frac = (float)(clamped - MIN_POWER) / (float)(MAX_POWER - MIN_POWER);
                rectangle(imgFlip, Point(x + 2, y + 2), Point(x + 2 + static_cast<int>((barWidth - 4) * frac), y + barHeight - 2), Scalar(0, 255, 255), cv::FILLED);
                rectangle(imgFlip, Point(x, y), Point(x + barWidth, y + barHeight), Scalar(200, 200, 200), 1);
                char buf[64];
                snprintf(buf, sizeof(buf), "Charging: %d ms", durationSoFar);
                putText(imgFlip, buf, Point(x, y - 8), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(220, 220, 220), 1);
            }
            else if (DetectionValues::jumpStored) {
                // show stored power
                int stored = DetectionValues::storedJumpPower;
                // background
                rectangle(imgFlip, Point(x, y), Point(x + barWidth, y + barHeight), Scalar(40, 40, 40), cv::FILLED);
                const int MIN_POWER = 50;
                const int MAX_POWER = 800;
                int clamped = stored;
                if (clamped < MIN_POWER) clamped = MIN_POWER;
                else if (clamped > MAX_POWER) clamped = MAX_POWER;
                float frac = (float)(clamped - MIN_POWER) / (float)(MAX_POWER - MIN_POWER);
                rectangle(imgFlip, Point(x + 2, y + 2), Point(x + 2 + static_cast<int>((barWidth - 4) * frac), y + barHeight - 2), Scalar(180, 180, 0), cv::FILLED);
                rectangle(imgFlip, Point(x, y), Point(x + barWidth, y + barHeight), Scalar(200, 200, 200), 1);
                char buf[64];
                snprintf(buf, sizeof(buf), "Stored: %d ms", stored);
                putText(imgFlip, buf, Point(x, y - 8), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(220, 220, 220), 1);
                // indicate waiting for direction
                putText(imgFlip, "Select direction", Point(x + barWidth + 10, y + barHeight - 2), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(200, 200, 200), 1);
            }
            else if (DetectionValues::executingJump) {
                putText(imgFlip, "Executing jump...", Point(x, y - 8), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(180, 200, 255), 1);
            }
        }
        catch (...) {
            // ignore
        }
    }
    catch (const exception& e) {
        cerr << "It was not possible to draw the jump line, the program returned the following error: " << e.what() << endl;
    }
}