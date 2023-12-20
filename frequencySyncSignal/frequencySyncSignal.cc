// frequencySyncSignal
#include "frequencySyncSignal.h"

// gstring
#include "gstring.h"
using namespace gstring;

// c++
#include <random>
#include <iostream>
#include <algorithm> // for sort for gcc


// building first RF from the start time
oneRFOutput::oneRFOutput(int seed, double timeWindow, double startTime, double radioPeriod, int rfBunchGap)
{


	// generating random number between -timeWindow and timeWindow
	random_device randomDevice;
    mt19937 generator(randomDevice());

    if(seed != -1) {
        generator.seed(seed);
    }

	uniform_real_distribution<> randomDistribution(-timeWindow/radioPeriod, timeWindow/radioPeriod);

    // very first RF time
	double firstRF = startTime + radioPeriod*(int)randomDistribution(generator);

	// making sure the first RF is within the timewindow
	while(firstRF<0 || firstRF>timeWindow)
		firstRF = startTime + radioPeriod*(int)randomDistribution(generator);

	// now filling the full signal
	fillRFValues(firstRF, timeWindow, rfBunchGap*radioPeriod);
}

// building RF from existing RFs
oneRFOutput::oneRFOutput(double oneRFValue, double rfsDistance, double timeWindow, double intervalBetweenBunches)
{

	// going backward in time by the distance
	double firstRF = oneRFValue - rfsDistance;

	// if that doesn't work, going forward
	if(firstRF < 0 || firstRF > timeWindow) { firstRF = oneRFValue + rfsDistance; }

	// if that didn't work either, the distance between RFs is too big
	if(firstRF < 0 || firstRF > timeWindow) {
		cout << "  !! error: RF value is outside the timewindow." << endl;
	} else {
		fillRFValues(firstRF, timeWindow, intervalBetweenBunches);
	}
}

void oneRFOutput::fillRFValues(double firstRF, double timeWindow, double intervalBetweenBunches)
{
	double putRF = firstRF;

	// adding earlier RFs
	while(putRF>0) {
		rfValue.push_back(putRF);
		putRF -= intervalBetweenBunches;
	}

	// adding later RFs
	putRF = firstRF + intervalBetweenBunches;
	while(putRF<timeWindow) {
		rfValue.push_back(putRF);
		putRF += intervalBetweenBunches;
	}

	// sorting vector
	sort(rfValue.begin(), rfValue.end());

	// adding the id
	for(unsigned n = 0; n<rfValue.size(); n++) {
		rfID.push_back(n+1);
	}
}

//! overloading "<<" to print this class
ostream &operator<<(ostream &stream, oneRFOutput s)
{
	for(unsigned j=0; j< s.rfID.size(); j++) {
		stream << "  - RFID: "   << s.rfID[j] << "   RF Value: " << s.rfValue[j] << endl;

	}

	return stream;
}

//! overloading "<<" to print this class
ostream &operator<<(ostream &stream, FrequencySyncSignal s)
{
    stream << " Seed: "   << s.seed;
	stream << " Time Window: "   << s.timeWindow;
	stream << ", event Start Time: "   << s.startTime << endl;
	stream << " Radio Frequency: "   << s.radioFrequency << " MHz: Period is " << s.radioPeriod << "ns" << endl;
	stream << " Distance between RF bunches "   << s.rfBunchGap*s.radioPeriod << " ns" << endl;

	for(unsigned i=0; i<s.rfBunchDistance.size(); i++) {
		stream << " Additional RF signal n. " << i+2 << " is " <<  s.rfBunchDistance[i]*s.radioPeriod << " ns away." << endl;
	}

	for(unsigned i=0; i<s.output.size(); i++) {
		stream << " > RF Output n. " << i+1 << ":" << endl;
		stream << s.output[i] ;
	}

	return stream;
}


FrequencySyncSignal::FrequencySyncSignal(string setup)
{
	unsigned long minNumberOfArguments = 5;

	// setup is a string with at least minNumberOfArguments entries.
	// any entry after that will add an additional RFOutput
	vector<string> parsedSetup = getStringVectorFromString(setup);
	if(parsedSetup.size() < minNumberOfArguments) {
		cout << " !! Error: FrequencySyncSignal initializer incomplete: >" << setup << "< has not enough parameters, at least "
		<<  minNumberOfArguments << " needed. Exiting" << endl;
		exit(1);
	}

    seed           = stoi(parsedSetup[0]);
	timeWindow     = stod(parsedSetup[1]);
	startTime      = stod(parsedSetup[2]);
	radioFrequency = stod(parsedSetup[3]);
	radioPeriod    = 1.0/radioFrequency; // GHz > ns
	rfBunchGap     = stoi(parsedSetup[4]);


	// do nothing if timewindow is 0
	if(timeWindow == 0) {
		cout << " ! Warning: timewindow is set to 0. No RF output." << endl;
		return;
	}

	// first RF: use seed
	output.push_back(oneRFOutput(seed, timeWindow, startTime, radioPeriod, rfBunchGap));

	// other signals
	if(parsedSetup.size() > minNumberOfArguments) {
		// getting first RF values
		double oneValue = output[0].getValues().front();
		// total number of RF signals
		unsigned long remainingNRF = parsedSetup.size() - minNumberOfArguments;
		// adding oneRFOutput for each addtional RF signal
		for(unsigned i=0; i<remainingNRF; i++) {
			rfBunchDistance.push_back(stoi(parsedSetup[minNumberOfArguments+i]));
			output.push_back(oneRFOutput(oneValue, rfBunchDistance[i]*radioPeriod, timeWindow, radioPeriod*rfBunchGap));
		}
	}
}
