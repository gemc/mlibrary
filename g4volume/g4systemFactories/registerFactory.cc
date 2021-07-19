
void G4Setup::registerFactoriesAndBuildG4Volumes(GSetup* gsetup, GOptions* gopt)
{
	
	
	// now building geant4 objects
	// every volume that is not built (due to dependencies) increments remainingVolumes
	int remainingVolumes;
	int allRemainingVolumes = 0;
	do {
		remainingVolumes = 0;
		// looping over systems
		for(auto &s : gsetup->getSetup()) {
			string factory = s.second->getFactory();
			// looping over volumes in that system
			for(auto &v : s.second->getSytems()) {
				if(g4setupactory.find(factory) != g4setupactory.end()) {
					if(g4setupactory[factory]->loadG4Setup(gopt, v.second, g4setup) == false) remainingVolumes = remainingVolumes + 1;
				} else {
					cerr << FATALERRORL << "  Fatal Error: g4setupactory factory <" << factory << "> not found." << endl;
					exit(0);
				}
			}
			if(verbosity == GVERBOSITY_ALL) {
				cout << setupLogHeader << " " << s.first << " remaining g4volumes to be built: " << remainingVolumes << endl;
			}
		}
		if(allRemainingVolumes != 0 && remainingVolumes != 0) {
			if(allRemainingVolumes >= remainingVolumes) {
				cerr << FATALERRORL << " Fatal Error: dependencies are not being resolved: their number should diminish." << endl;
				exit(0);
			}
		} else {
			allRemainingVolumes = remainingVolumes;
		}
	} while (remainingVolumes > 0);
}
