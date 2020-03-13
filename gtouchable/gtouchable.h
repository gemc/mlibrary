#ifndef  GTOUCHABLE_H
#define  GTOUCHABLE_H 1

// c++
#include <vector>
#include <string>  // < not needed with clang but needed on Linux gcc 4.8.5
using namespace std;

enum GType { readout, flux, counter };


class GTouchable
{

public:
	GTouchable(string sensitivity, string gtouchableString);

public:
	string getGTouchableDescriptionString();
    double getEnergyMultiplier() {return eMultiplier;}
    
private:
	// set by constructor
	vector<int>  gTid;   ///< Uniquely identify a sensitive element

	// set by sensitive detector constructor.
	GType  gType;
	
	// set in sensitiveDetector::ProcessHit
	int  trackId;   ///< Used to determine if steps belong to the same hit for flux/counter detectors

	// set by processGTouchable in the digitization plugin. Defaulted to 1. Used to share energy / create new hits.
	double  eMultiplier;   ///< Energy Multiplier. By default it is 1, but energy could be shared (or created) among volumes

	// used to determine if a hit is within the same detector electronic time window
	// if the index is 0, this quantity is ignored
	// if it is > 0 then it is used to distinguish hits in separate time windows
	int gridTimeIndex;
	
	// users may want to define and save information from the processGTouchable function
	// to be used in the digitization later
	vector<double> userInformations;
};


#endif
