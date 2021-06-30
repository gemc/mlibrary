
// c++
#include <iostream>
using namespace std;

// mlibrary
#include "gstring.h"
using namespace gstring;


void G4Setup::addG4Volume(G4Volume *g4v, string name) {

	// first check that the name does not already exist
	if(g4setup->find(name) != g4setup->end()) {
		(*g4setup)[name] = g4v;
	} else {
		G4cout << GWARNING << " Warning: volume " << name << " already exists." << G4endl;
	}
}


G4Volume* G4Setup::getG4Volume(string name) const
{
	if(g4setup->find(name) != g4setup->end()) {
		return (*g4setup)[name];
	} else {
		G4cout << GWARNING << " Warning: volume " << name << " does not exist in g4setup." << G4endl;
	}
	return nullptr;
}

G4VSolid* G4Setup::getSolid(string vname) const
{
	G4Volume* thisVolume = getG4Volume(vname);
	if(thisVolume) {
		return thisVolume->getSolid();
	}
	return nullptr;
}

G4LogicalVolume* G4Setup::getLogical(string vname) const
{
	G4Volume* thisVolume = getG4Volume(vname);
	if(thisVolume) {
		return thisVolume->getLogical();
	}
	return nullptr;
}

G4VPhysicalVolume* G4Setup::getPhysical(string vname) const
{
	G4Volume* thisVolume = getG4Volume(vname);
	if(thisVolume) {
		return thisVolume->getPhysical();
	}
	return nullptr;
}







