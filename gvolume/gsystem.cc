

map<string, string> GSetup::getSensitiveVolumes() {
	map<string, string> sensD;
	
	for(auto &s : setup) {
		for(auto &gv : s.second->getSytems()) {
			string sensitivity = gv.second->getSensitivity();
			if(sensitivity != NOTAPPLICABLE) {
				sensD[sensitivity] = s.second->getSystemPath()  + "/" + sensitivity;
			}
		}
	}
	
	return sensD;
}



