/// \file example.cc

// example on how to use the translationTable  library
// this will load tables from Jefferson Lab CLAS12 CCDB database

// translationTable framework
#include "translationTable.h"


#include <CCDB/Calibration.h>
#include <CCDB/Model/Assignment.h>
#include <CCDB/CalibrationGenerator.h>
using namespace ccdb;



//! example of main declaring GOptions
int main(int argc, char* argv[])
{
	bool verbosity = false;

	if(argc > 1) {
		string arg1 = argv[1];
		if(arg1 == "v")
			verbosity = true;
	}

	TranslationTable TT("ec");

	// loads translation table from CLAS12 Database:
	// Translation table for EC (ECAL+PCAL).
	// Crate sector assignments: ECAL/FADC=1,7,13,19,25,31 ECAL/TDC=2,8,14,20,26,32
	// PCAL/FADC=3,9,15,21,27,33 PCAL/TDC=4,10,16,22,28,34.
	// ORDER: 0=FADC 2=TDC.

	string connection =  "mysql://clas12reader@clasdb.jlab.org/clas12";
	string database   = "/daq/tt/ec:1";

	vector<vector<double> > data;
	auto_ptr<Calibration> calib(CalibrationGenerator::CreateCalibration(connection));

	data.clear(); calib->GetCalib(data, database);

	cout << " Data loaded from CCDB with " << data.size() << " columns." << endl;

	// filling translation table
	for(unsigned row = 0; row < data.size(); row++)
	{
		int crate   = data[row][0];
		int slot    = data[row][1];
		int channel = data[row][2];

		int sector = data[row][3];
		int layer  = data[row][4];
		int pmt    = data[row][5];

		if(verbosity) {
			cout << " crate: " << crate << "  slot: " << slot << "  channel: " << channel ;
			cout << " sector: " << sector << "  layer: " << layer << "  pmt: " << pmt << endl;
		}


		TT.addHardwareItem({sector, layer, pmt}, Hardware(crate, slot, channel));
	}
	cout << " Data loaded in translation table. " << endl;


	// now inquiring TT for sector     | layer      | component  =  4          | 4          | 1-36

	for(int i=0; i<36; i++) {
		cout << " hardware: " << TT.getHardware({4, 4, i}) << endl;
	}

	return 1;

}
