/// \file goptions.cc
/// \author \n Maurizio Ungaro
/// \author mail: ungaro@jlab.org\n\n\n

// option framework
#include "goptions.h"

// c++
#include <fstream>


//! overloading "<<" to print this class
ostream &operator<<(ostream &stream, GOption gopt)
{
	switch(gopt.type) {
		case isString: stream << "\"" << gopt.valueS << "\""; break;
		case isDouble: stream << "< " << gopt.valueD << " >"; break;
	}
	stream << " (" << gopt.title << ")";
	return stream;
}


//! constructor - ignore is optional
GOptions::GOptions(int argc, char *argv[], map<string, GOption> om, bool ignore) : ignoreNotFound(ignore)
{
	optionsMap = om;
	cout << endl;

	// fill the categories set
	for (const auto &om : optionsMap) {
		if(categories.find(om.second.getCategory()) == categories.end())
			categories.insert(om.second.getCategory());
	}

	//! parse configuration file
	parseConfigurationFile(findConfigurationFile(argc, argv));

	//! now parse command line arguments
	checkAndParseCommandLine(argc, argv);

	// sorting userSetting
	sort(userSettings.begin(), userSettings.end());

	// now print the user settings
	printUserSettings();
	cout << endl;
}

/*! \fn string GOptions::findConfigurationFile(int argc, char *argv[])

Finds a configuration file (gcard). Returns "na' if not found.
 
 \param argc, argv the program arguments
 \return The filename if a gcard was found. "na" otherwise
 */
string GOptions::findConfigurationFile(int argc, char *argv[])
{
	// finds gcard file as one of the argument
	// extension is .gcard
	for(int i=1;i<argc;i++)
	{
		string arg = argv[i];
		size_t pos = arg.find(".gcard");
		if(pos != string::npos) return arg;
	}
	// finds gcard file as one of the options
	for(int i=1;i<argc;i++)
	{
		string arg = argv[i];
		size_t pos = arg.find("gcard=");
		if(pos != string::npos) return arg.substr(pos+6);
	}

	return "na";
}



/*! \fn int GOptions::parseConfigurationFile(string file)

Parse a configuration file into the GOptions map
 \param file the filename
 \return 1 if a file could be parsed. 0 if no file was found

 */
int GOptions::parseConfigurationFile(string file)
{
	if(file == "na") return 0;

	// this will fail if gcard not valid or not existing
	QDomDocument domDocument = checkAndParseGCard(file);

	QDomNodeList options = domDocument.firstChildElement().elementsByTagName("option");
	for(int i = 0; i < options.count(); i++)
	{
		QDomNode elm = options.at(i);
		if(elm.isElement())
		{
			QDomElement e = elm.toElement();
			string optionKey = e.attribute("name").toStdString();
			string value     = e.attribute("value").toStdString();

			// set option value if found
			setOptionValue(optionKey, value);
		}

	}
	cout << " Configuration file: " << file << " parsed into options map." << endl;
	return 1;
}


/*! \fn  GOptions::checkAndParseCommandLine(int argc, char *argv[])

 - Checks commands line options for special directives such as -h, -help, etc
 - Parse commands line options to the options map

 */
void GOptions::checkAndParseCommandLine(int argc, char *argv[])
{
	// -h, -help, --help print general help
	if(findCLOption("-h",         argc, argv) == "yes") printGeneralHelp();
	if(findCLOption("-help",      argc, argv) == "yes") printGeneralHelp();
	if(findCLOption("--help",     argc, argv) == "yes") printGeneralHelp();
	if(findCLOption("--h",        argc, argv) == "yes") printGeneralHelp();

	// prints html file
	if(findCLOption("-help-html", argc, argv) == "yes") printHTMLHelp();

	// prints all help
	if(findCLOption("-help-all", argc, argv) == "yes")
	printAvailableHelp("all");
	// prints category help
	else  {
		string catCandidate = findCLOption("-help-", argc, argv);
		if(catCandidate != "no") {
		 if(categories.find(catCandidate) != categories.end())
			printCategoryHelp(catCandidate);
		}
	}

	// scanning the arguments for valid option
	for(int i=1; i<argc; i++)
	{
		string argument = argv[i];

		// candidate option is -OPTIONKEY=Value
		if(argument.find("=") != string::npos)
		{
			string optionKey = argument.substr(1, argument.find("=")-1);
			string value     = argument.substr(argument.find("=")+1, argument.size());

			// set option value if found
			setOptionValue(optionKey, value);
		}
	}
	cout << " Command line parsed into options map." << endl;


}
/*! \fn int GOptions::setOptionValue(string optionKey, string value)

  - set optionsMap[optionKey] value to value
  - adds a REPETITION of the option can be repeated

 */
void GOptions::setOptionValue(string optionKey, string value)
{
	// optionKey matches the GOption map
	// if ignoreNotFound is true then we don't care
	int matches = ignoreNotFound ? 1 : 0;


	for (auto &om : optionsMap) {
		// looking for a valid option. If a two instances of the same option exist
		if(optionKey == om.first) {
			matches++;
			// first time option is found, or if it cannot be repeated:
			// modify its value
			if(userDefinedOptions(om.first).size() == 0) {
				userSettings.push_back(om.first);
				om.second.setValue(value);
			} else if(!om.second.canItBeRepeated()) {
				// its not the first time we see this option.
				// Overwrite value with this one
				om.second.setValue(value);
			} else if(om.second.canItBeRepeated()) {
				// new option is created with key same as the first one but
				// with the string HELPREPETITION# appended
				string newKey = om.first + HELPREPETITION + to_string(userDefinedOptions(om.first).size());
				userSettings.push_back(newKey);
				optionsMap[newKey] = GOption(om.second);
				optionsMap[newKey].setValue(value);
			}
			// option is found, we can break from the GOption map loop
			break;
		}
	}
	// option was not found. if ignoreNotFound is 1 matches is 1.
	if(matches < 2) {
		cout << "  !! Attention: the option " << optionKey
		<< " is not known to this system. Please check your spelling." << endl;
	}
	// option was not found. ignoreNotFound is 0
	if(matches == 0) exit(0);
}







/*! \fn  GOptions::checkAndParseGCard(string file)

 - Checks if the gcard is valid
 - Exits if the gcard has wrong format or doesn't exist
 - Parse the gcard onto a QDomDocument

 \param file the filename
 \return the QDomDocument

 */
QDomDocument GOptions::checkAndParseGCard(string file)
{
	QDomDocument domDocument;

	QFile gcard(file.c_str());

	if( !gcard.exists() )
	{
		cout << " >>  gcard: " << file << " not found. Exiting." << endl;
		exit(0);
	}

	// opening gcard and filling domDocument
	if(!domDocument.setContent(&gcard))
	{
		cout << " >>  xml format for file <" << file << "> is wrong - check XML syntax. Exiting." << endl;
		exit(0);
	}
	gcard.close();

	return domDocument;
}

/*! \fn GOptions::findCLOption(string o, int argc, char *argv[])

 - finds an option from the command line arguments

 */
string GOptions::findCLOption(string o, int argc, char *argv[])
{
	string found = "no";
	for(int i=1; i<argc; i++) {
		string argument = argv[i];
		// return "yes" if it's an exact match
		if(argument == o) found = "yes";
		// otherwise return the string after the match
		else if(argument.find(o) != string::npos) found = argument.substr(o.size(), argument.size());

	}
	return found;
}


/*! \fn  GOptions::printUserSettings(string file)

 - Loops over the user settings and print on screen.

 */
void GOptions::printUserSettings()
{
	if(userSettings.size())
	{
		cout << " > Selected User Options: " << endl;
		for (auto &s : userSettings) {
			cout <<  "   - " ;
			cout.width(20);
			cout.fill('.');

			string optionKey = s;

			// parse out REPETITION
			auto repPos = s.find(HELPREPETITION) ;
			if(repPos != string::npos)
				optionKey = s.substr(0, repPos);

			cout << left << optionKey << ": " << optionsMap[s] << endl;
		}
	}
}



/*! \fn  GOptions::printGeneralHelp()

 - Print the general help and exit

 */
void GOptions::printGeneralHelp()
{
	cout << endl;
	cout <<  "    Usage:" <<  endl << endl ;
	cout <<  "   > -h, -help, --help: print this message and exit. "    << endl;
	cout <<  "   > -help-all:  print all available options and exit. "  << endl;
	cout <<  "   > -help-html:  print all available options in HTML format (options.html) and exit. "  << endl << endl;
	cout <<  "   > Available categories "  << endl;

	for(const auto &c : categories)
	{
		cout << "     -help-" ;
		cout.width(15);
		cout.fill('.');
		cout << left << c << ":  " << c << " related options" << endl;
	}
	cout << endl;
	exit(0);
}


/*! \fn  GOptions::printAvailableHelp(string search)

 - Print available options which description matches search
 - "all" will print all available options.

 */
void GOptions::printAvailableHelp(string search)
{
	cout << " > Available Options: " << endl;
	for (const auto &om : optionsMap) {
		if(search == "all" || om.second.getTitle().find(search) != string::npos)
			printOptionDetailedHelp(om.first);
	}
	exit(0);
}



/*! \fn  GOptions::printCategoryHelp(string cat)

 - print options that match a category

 */
void GOptions::printCategoryHelp(string cat)
{
	cout << " > " << cat << " Options: " << endl;
	for (const auto &om : optionsMap) {
		if(om.second.getCategory() == cat) printOptionDetailedHelp(om.first);
	}

	exit(0);
}

/*! \fn  GOptions::printOptionDetailedHelp(string cat)

 - print options details

 */
void GOptions::printOptionDetailedHelp(string which)
{

	cout <<  "   - " ;
	cout.width(20);
	cout.fill('.');
	cout << left << which << ": " << optionsMap[which].getTitle() << ". Default set to: " <<  optionsMap[which].getValue() << endl;

	// do not print the help if it's equal to the title
	if(optionsMap[which].getHelp() != HELPFILLSPACES + optionsMap[which].getTitle()) cout << optionsMap[which].getHelp() << endl;

}

/*! \fn  GOptions::getDoubleValue(string optionKey)

 - return the option value if the option is in the map

 */
double GOptions::getDoubleValue(string optionKey) {
	auto search = optionsMap.find(optionKey);

	if(search != optionsMap.end() ) return search->second.getDoubleValue();
	return -99;
}



/*! \fn  GOptions::getIntValue(string optionKey)

 - return the option value if the option is in the map

 */
int GOptions::getIntValue(string optionKey) {
	auto search = optionsMap.find(optionKey);

	if(search != optionsMap.end() ) return search->second.getIntValue();
	return -99;
}
/*! \fn  GOptions::getBoolValue(string optionKey)

 - return the option value if the option is in the map

 */
bool GOptions::getBoolValue(string optionKey) {
	auto search = optionsMap.find(optionKey);

	if(search != optionsMap.end() ) return search->second.getBoolValue();
	return false;
}

/*! \fn  GOptions::getValue(string optionKey)

 - return the option value if the option is in the map

 */
string GOptions::getValue(string optionKey) {
	auto search = optionsMap.find(optionKey);

	if(search != optionsMap.end() ) return search->second.getValue();
		return "na";
}

/*! \fn  GOptions::getValues(string optionKey)

 - return the option values if the option is in the map

 */
vector<string> GOptions::getValues(string optionKey) {
	auto search = optionsMap.find(optionKey);

	if(search != optionsMap.end() ) return search->second.getValues();
	return {"na"};
}



/*! \fn  GOptions::printHTMLHelp()

 - Print help in html format and exit

 */
void GOptions::printHTMLHelp()
{
	ofstream hf;
	hf.open("options.html");
	hf << "<html>" << endl;
	hf << "	<STYLE TYPE=\"text/css\">" << endl;
	hf << "<!--" << endl;
	hf << ".pretty-table" << endl;
	hf << "{" << endl;
	hf << "	padding: 0;" << endl;
	hf << "	margin: 0;" << endl;
	hf << "	border-collapse: collapse;" << endl;
	hf << "	border: 1px solid #333;" << endl;
	hf << "	font-family: \"Trebuchet MS\", Verdana, Arial, Helvetica, sans-serif;" << endl;
	hf << "	font-size: 0.8em;" << endl;
	hf << "	color: #000;" << endl;
	hf << "	background: #fcf0e4;" << endl;
	hf << "}" << endl;
	hf << ".pretty-table caption" << endl;
	hf << "{" << endl;
	hf << "	caption-side: bottom;" << endl;
	hf << "	font-size: 0.9em;" << endl;
	hf << "	font-style: italic;" << endl;
	hf << "	text-align: right;" << endl;
	hf << "	padding: 0.5em 0;" << endl;
	hf << "}" << endl;
	hf << ".pretty-table th, .pretty-table td" << endl;
	hf << "{" << endl;
	hf << "	border: 1px dotted #666;" << endl;
	hf << "	padding: 0.5em;" << endl;
	hf << "	text-align: left;" << endl;
	hf << "	color: #632a39;" << endl;
	hf << "}" << endl;
	hf << ".pretty-table th[scope=col]" << endl;
	hf << "{" << endl;
	hf << "	color: #000;" << endl;
	hf << "	background-color: #afadcc;" << endl;
	hf << "	text-transform: uppercase;" << endl;
	hf << "	font-size: 0.9em;" << endl;
	hf << "	border-bottom: 2px solid #333;" << endl;
	hf << "	border-right: 2px solid #333;" << endl;
	hf << "}" << endl;
	hf << ".pretty-table th+th[scope=col]" << endl;
	hf << "{" << endl;
	hf << "	color: #009;" << endl;
	hf << "	background-color: #7d98b3;" << endl;
	hf << "	border-right: 1px dotted #666;" << endl;
	hf << "}" << endl;
	hf << ".pretty-table th[scope=row]" << endl;
	hf << "{" << endl;
	hf << "	background-color: #b8cfe5;" << endl;
	hf << "	border-right: 2px solid #333;" << endl;
	hf << "}" << endl;
	hf << "pre{font-family:Helvetica;font-size:12pt" << endl;
	hf << "	overflow: auto; " << endl;
	hf << "}" << endl;

	hf << "--->" << endl;
	hf << "</STYLE>" << endl;
	hf << "</head>" << endl;
	hf << "<body>" << endl;
	hf << "<br><br>" << endl;
	hf << "<center>" << endl;
	hf << "<h1> Available options</h1>" << endl;
	hf << "</center>" << endl;
	hf << "<br><br><br>" << endl;
	hf << "<table cellsize=20>" << endl;
	hf << "<tr><td>" << endl;
	hf << "<table class=\"pretty-table\">" << endl;
	hf << "<caption>options. This table is produced with the option: -help-html </caption>" << endl;
	hf << "<tr><th scope=\"col\" >Category</th>" << endl;
	hf << "    <th scope=\"col\" >Option</th>" << endl;
	hf << "    <th scope=\"col\" >Help</th></tr>" << endl;
	for(const auto &c : categories)
		for(const auto &om : optionsMap)
			if(om.second.getCategory() == c)
			{
				hf << "<tr><th scope=\"row\">";
				hf << c ;

				hf << "</th> <td>";
				hf << om.first;
				hf << "</td><td><pre>" << endl;
				hf << om.second.getHelp();
				hf << "</pre></td></tr>" << endl;
		}

	hf << "</table>" << endl;
	hf << "</td>" << endl;
	hf << "<td>" << endl;
	hf << "</table>" << endl;
	hf << " </body></html>";

	hf.close();

	cout << " Options written to options.html." << endl;
	exit(0);
}




//! overload += to add two maps
map<string, GOption> &operator += (map<string, GOption> &original, map<string, GOption> toadd)
{
	for(const auto &mo : toadd) {
		original[mo.first] = mo.second;
	}
	return original;
}

