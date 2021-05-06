// gvolume
#include "gvolume.h"
#include "gsystem.h"

// c++
#include <fstream>

// mlibrary
#include "gstring.h"
using namespace gstring;

/*! \fn map<string, GOption> GSetup::defineOptions()

 \return defines GSetup specific options

 */
map<string, GOption> GSetup::defineOptions()
{
	map<string, GOption> optionsMap;

	optionsMap["addSystem"] = GOption("Adds a system", NOTAPPLICABLE, "system", true);
	optionsMap["addSystem"].addHelp("The mandatory arguments, separated by commas, are:\n");
	optionsMap["addSystem"].addHelp(string(GOPTIONITEM) + "system name \n");
	optionsMap["addSystem"].addHelp(string(GOPTIONITEM) + "system factory\n");
	optionsMap["addSystem"].addHelp("Optional arguments are:\n");
	optionsMap["addSystem"].addHelp(string(GOPTIONITEM) + "system variation (default: \"default\")\n");
	optionsMap["addSystem"].addHelp(string(GOPTIONITEM) + "system run number (default: 1)\n");
	optionsMap["addSystem"].addHelp(" Example: -addSystem=\"ctof, cad, default, 12\"\n");

	optionsMap["setupDir"] = GOption("Path(s) to check for setup, separated by space", NOTAPPLICABLE, "system", true);

	optionsMap["gvolumev"] = GOption("Volume Verbosity", 0, "verbosity");
	optionsMap["gvolumev"].addHelp("Possible values:\n");
	optionsMap["gvolumev"].addHelp(GVERBOSITY_SILENT_D);
	optionsMap["gvolumev"].addHelp(GVERBOSITY_SUMMARY_D);
	optionsMap["gvolumev"].addHelp(GVERBOSITY_DETAILS_D);
	optionsMap["gvolumev"].addHelp(GVERBOSITY_ALL_D);

	optionsMap["logVolume"] = GOption("Print everything related to this volume", NOTAPPLICABLE, "verbosity");

	return optionsMap;
}

