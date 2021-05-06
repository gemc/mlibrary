// gVolume
#include "gsystem.h"

// mlibrary
#include "gstring.h"
using namespace gstring;





GSetup::GSetup(GOptions* gopt, int runNo)
{
	// this is already validated for XML syntax and file existance
	QDomDocument domDocument = gopt->configurationQDomDocument;
	int verbosity = gopt->getInt("gvolumev");
	
	// load the systems
	QDomNode domNode = domDocument.documentElement().firstChild();
	while(!domNode.isNull()) {
		QDomElement domElement = domNode.toElement();                     // converts the node to an element.
		if(!domElement.isNull()) {                                        // the node really is an element.
			if(qVariantToStdString(domElement.tagName()) == "system")  {  // selecting "system" nodes
				
				string     thisNname = assignAttribute(domElement, "name",      NOTAPPLICABLE);
				string   thisFactory = assignAttribute(domElement, "factory",   NOTAPPLICABLE);
				string thisVariation = assignAttribute(domElement, "variation", "default");
				int    thisRunNumber = assignAttribute(domElement, "runno",     runNo);
				
				if(thisNname != NOTAPPLICABLE && thisFactory != NOTAPPLICABLE) {
					setup[thisNname] = new GSystem(thisNname,
												   thisFactory,
												   thisVariation,
												   thisRunNumber,
												   runNo,
												   verbosity);
				}
			}
		}
		domNode = domNode.nextSibling();
	}
	
	// load the modifiers
	domNode = domDocument.documentElement().firstChild();
	while(!domNode.isNull()) {
		QDomElement domElement = domNode.toElement();                       // converts the node to an element.
		if(!domElement.isNull()) {                                          // the node really is an element.
			if(qVariantToStdString(domElement.tagName()) == "modifier")  {  // selecting "modifier" nodes
				
				string    thisNname = assignAttribute(domElement, "volume",   NOTAPPLICABLE);
				string thisPosition = assignAttribute(domElement, "position", "default");
				string thisRotation = assignAttribute(domElement, "rotation", "default");
				string     presence = assignAttribute(domElement, "present",  "yes");
				if(thisNname != NOTAPPLICABLE && (thisPosition != "default" || thisRotation != "default" || presence != "yes") ){
					setupModifiers[thisNname] = new GModifiers(thisNname,
															   thisPosition,
															   thisRotation,
															   presence,
															   verbosity);
				}
			}
		}
		domNode = domNode.nextSibling();
	}
	
	// now loading gmanager
	gSystemManager = GManager(verbosity-1);
	// register the factories needed and load the systems
	registerFactoriesAndLoadSystems(gopt);
}

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



