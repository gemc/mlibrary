

#define DIGITNAMESUFFIX 

class GMediaROOTFactory : public GMedia
{
public:
	GMediaROOTFactory() : gRootTrees(nullptr) {}
	
private:
	// open and close the output media
	bool openConnection();
	bool closeConnection();

	// start and end each event
	bool startEvent();
	bool endEvent();

	// write the header
	bool publishHeader(GHeader gh);
	
	// write digitized observables
	bool publishDetectorDigiObservables(GDetectorObservables *detectorHits);
	
	// write true observables
	bool publishDetectorTrueObservables(GDetectorObservables *detectorHits);

private:
	TFile *rootfile;
	map<string, GRootTree*> *gRootTrees;
	
	// return header tree from map. If not there, initialize it.
	GRootTree *getHeaderGRootTree();

	// return observables tree from map. If not there, initialize it.
	GRootTree *getGRootTree(string treeName, vector<string> varNames, GObservables* observable);
	
};



#endif // GMEDIAROOTFACTORY_H
