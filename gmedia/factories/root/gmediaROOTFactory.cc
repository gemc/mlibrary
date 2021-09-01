// txt gmedia
#include "gmediaROOTFactory.h"

GRootTree* GMediaROOTFactory::getHeaderGRootTree()
{
	// tree not found, initializing it
	if(gRootTrees->find("header") == gRootTrees->end()) {
		(*gRootTrees)["header"] = new GRootTree();
	}
	
	// return initialized tree
	return (*gRootTrees)["header"];
}

GRootTree* GMediaROOTFactory::getGRootTree(string treeName, vector<string> varNames, GObservables* observable)
{
	// tree not found, initializing it
	if(gRootTrees->find(treeName) == gRootTrees->end()) {
		(*gRootTrees)[treeName] = new GRootTree(treeName, varNames, observable);
	}
	
	// return initialized tree
	return (*gRootTrees)[treeName];	
}



// tells the DLL how to create a GMediaFactory
extern "C" GMedia* GMediaFactory(void) {
	return static_cast<GMedia*>(new GMediaROOTFactory);
}
