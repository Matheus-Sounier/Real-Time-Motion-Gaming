#include <iostream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include "IOFunctions.hpp"
#include "Var/IOValues.hpp"
#include "../Graphics/GraphicsFunctions.hpp"
#include "../Graphics/Var/GraphicsValues.hpp"
#include "../Detection/DetectionFunctions.hpp"
#include "../Detection/Var/DetectionValues.hpp"
#include "../MapKeys/MapKeys.hpp"

using namespace std;
using namespace cv;

void IO::FileManager::readSavedSquares() {
    ifstream SavedSquares;
    int QntSquares;

    SavedSquares.open("data/SavedSquaresPositions.txt");

    if (SavedSquares.is_open()) {
        SavedSquares >> QntSquares;
        for (int i = 0; i < QntSquares; i++) {
            unsigned SquareX, SquareY, SquareKey;
            string SquareDisplayName;
            SavedSquares >> SquareX >> SquareY >> SquareKey >> SquareDisplayName;
            Graphics::ImageManager::createSquare(SquareX, SquareY, SquareKey, SquareDisplayName);
        }
        cout << "The saved squares were loaded successfully." << endl;
        SavedSquares.close();
    }
    else { cout << "'SavedSquaresPositions.txt' file does not exist, unable to load saved squares." << endl; }
}

void IO::FileManager::saveSquares() {
    try {
        ofstream SavedSquares("data/SavedSquaresPositions.txt");

        if (SavedSquares.is_open()) {

            SavedSquares << GraphicsValues::CVSquares::Squares.size() << endl;

            for (int i = 0; i < GraphicsValues::CVSquares::Squares.size(); i++) {

                SavedSquares << GraphicsValues::CVSquares::Squares[i].TL.x << endl;
                SavedSquares << GraphicsValues::CVSquares::Squares[i].TL.y << endl;
                SavedSquares << GraphicsValues::CVSquares::Squares[i].KEY << endl;
                SavedSquares << GraphicsValues::CVSquares::Squares[i].DISPLAYKEY << endl;
            }

            SavedSquares.close();
        }
    }
    catch (const exception& e) {
        cerr << "The program could not save properly to SavedSquaresPositions.txt, the program returned the following error: " << e.what() << endl;
    }
}

