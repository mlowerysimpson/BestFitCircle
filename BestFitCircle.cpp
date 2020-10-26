#include "pch.h"
//applicate takes a text file of comma-delimited x and y data and tries to iteratively find the best center and radius of a circle that fits those points
using namespace System;
using namespace System::IO;


void ShowUsage() {
    Console::WriteLine("BestFitCircle");
    Console::WriteLine("Usage: BestFitCircle <path to comma-delimited x,y text data file.");
}

void GetBestCircleFit(array <double>^ xmag, array <double>^ ymag, int nNumVals, double %magXOffset, double %magYOffset) {//tries to find the approximate center and radius of a circle where nNumVals points of coordinates xmag, ymag go through the circumference of the circle
	//find center value of nNumVals of xmag, ymag values
	//first find min and max values to help get estimate of center	
	const int TRY_WIDTH = 200;//try this many points to left and right of center estimate to arrive at solution
	double dXMin = 100000, dXMax = -1000000, dYMin = 1000000, dYMax = -100000;
	for (int i = 0; i < nNumVals; i++) {
		if (xmag[i] < dXMin) {
			dXMin = xmag[i];
		}
		if (xmag[i] > dXMax) {
			dXMax = xmag[i];
		}
		if (ymag[i] < dYMin) {
			dYMin = ymag[i];
		}
		if (ymag[i] > dYMax) {
			dYMax = ymag[i];
		}
	}
	double dMinRadius = (dXMax - dXMin) / 4;
	double dMaxRadius = dXMax - dXMin;
	double dXCenter = (dXMin + dXMax) / 2;//estimate of x-center of circle
	double dYCenter = (dYMin + dYMax) / 2;//estimate of y-center of circle
	double dRadius = 0.0;
	double dBestScore = 0.0;
	for (double i = dXCenter - TRY_WIDTH; i <= dXCenter + TRY_WIDTH; i++) {
		for (double j = dYCenter - TRY_WIDTH; j <= dYCenter + TRY_WIDTH; j++) {
			double dRadiusDif = dMaxRadius - dMinRadius;
			double dLowRad = dMinRadius;
			double dHighRad = dMaxRadius;
			double dMidRad = (dLowRad + dHighRad) / 2;
			while (dRadiusDif > .2) {
				double dLowScore = 0.0;
				double dMidScore = 0.0;
				double dHighScore = 0.0;
				for (int k = 0; k < nNumVals; k++) {
					dLowScore += Math::Abs((xmag[k] - i) * (xmag[k] - i) + (ymag[k] - j) * (ymag[k] - j) - dLowRad * dLowRad);
					dMidScore += Math::Abs((xmag[k] - i) * (xmag[k] - i) + (ymag[k] - j) * (ymag[k] - j) - dMidRad * dMidRad);
					dHighScore += Math::Abs((xmag[k] - i) * (xmag[k] - i) + (ymag[k] - j) * (ymag[k] - j) - dHighRad * dHighRad);
				}
				if (dLowScore <= dHighScore) {
					dHighRad = dMidRad;
					if (dBestScore == 0.0 || dLowScore < dBestScore) {
						dBestScore = dLowScore;
						magXOffset = i;
						magYOffset = j;
					}
				}
				else {
					dLowRad = dMidRad;
					if (dBestScore == 0.0 || dHighScore < dBestScore) {
						dBestScore = dHighScore;
						magXOffset = i;
						magYOffset = j;
					}
				}
				dMidRad = (dLowRad + dHighRad)/2;
				dRadiusDif = dHighRad - dLowRad;
			}
		}
	}
}

int main(array<System::String ^> ^args)
{
    if (args->Length != 1) {
        ShowUsage();
        return -1;
    }
    String^ sFilename = args[0];
    if (!File::Exists(sFilename)) {
        String^ sErr = String::Format("Error, file: {0} does not exist.", sFilename);
        Console::WriteLine(sErr);
        return -2;
    }
    array <String ^> ^fileLines = File::ReadAllLines(sFilename);
    int nNumLines = fileLines->Length;
    array <double>^ xData = gcnew array<double>(nNumLines);
    array <double>^ yData = gcnew array<double>(nNumLines);
    array <String^>^ delims = gcnew array<String^>(1);
    delims[0] = ",";
    for (int i = 0; i < nNumLines; i++) {
        String^ sLine = fileLines[i];
        array <String ^> ^lineVals = sLine->Split(delims, StringSplitOptions::RemoveEmptyEntries);
        if (lineVals != nullptr && lineVals->Length >= 2) {
            if (!Double::TryParse(lineVals[0], xData[i])) {
                String^ sErr = String::Format("Error parsing data on line {0:D}.", i + 1);
                Console::WriteLine(sErr);
                return -3;
            }
            if (!Double::TryParse(lineVals[1], yData[i])) {
                String^ sErr = String::Format("Error parsing data on line {0:D}.", i + 1);
                Console::WriteLine(sErr);
                return -4;
            }
        }
        else {
            return -5;
        }
    }
	double magXOffset = 0.0, magYOffset = 0.0;
	GetBestCircleFit(xData, yData, nNumLines, magXOffset, magYOffset);
	String^ sResult = String::Format("magXOffset = {0:F1}, magYOffset = {1:F1}", magXOffset, magYOffset);
	Console::WriteLine(sResult);
    return 0;
}
