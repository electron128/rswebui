#include <QIcon>
#include <QMessageBox>
#include <map>
#include <sstream>

#include <retroshare/rsplugin.h>
#include <util/rsversion.h>

#include "WebUIPlugin.h"
#include "WebUImain.h"
#include <gui/RsWebUIConfig.h>

static void *inited = new WebUIPlugin() ;

extern "C" {
	// This is *the* functions required by RS plugin system to give RS access to the plugin.
	// Be careful to:
	// - always respect the C linkage convention
	// - always return an object of type RsPlugin*
	//
	void *RETROSHARE_PLUGIN_provide()
	{
		static WebUIPlugin *p = new WebUIPlugin() ;

		return (void*)p ;
	}

	// This symbol contains the svn revision number grabbed from the executable. 
	// It will be tested by RS to load the plugin automatically, since it is safe to load plugins
	// with same revision numbers, assuming that the revision numbers are up-to-date.
	//
	uint32_t RETROSHARE_PLUGIN_revision = SVN_REVISION_NUMBER ;

	// This symbol contains the svn revision number grabbed from the executable. 
	// It will be tested by RS to load the plugin automatically, since it is safe to load plugins
	// with same revision numbers, assuming that the revision numbers are up-to-date.
	//
	uint32_t RETROSHARE_PLUGIN_api = RS_PLUGIN_API_VERSION ;
}

QIcon *WebUIPlugin::qt_icon() const
{
	static QIcon *icon = new QIcon(":images/emblem-web.png") ;

	return icon ;
}

void WebUIPlugin::getPluginVersion(int& major,int& minor,int& svn_rev) const
{
	major = 5 ;
	minor = 4 ;
	svn_rev = SVN_REVISION_NUMBER ;
}

ConfigPage *WebUIPlugin::qt_config_page() const
{
//	static RsWebUIConfig *cfg_widget = NULL ;
	
//	if(cfg_widget == NULL)
RsWebUIConfig	*cfg_widget = new RsWebUIConfig ;

	return cfg_widget ;
}

QDialog *WebUIPlugin::qt_about_page() const
{
	static QMessageBox *about_dialog = NULL ;

	if(about_dialog == NULL)
	{
		about_dialog = new QMessageBox() ;

		QString text ;
		text += QObject::tr("<h3>RetroShare WebUI plugin</h3>* Contributors: Cyril Soler<br/>") ;
		text += QObject::tr("<p>The WebUI plugin provides a web interface to Retroshare.") ;
		text += QObject::tr("It is easily configurable from the Config->WebUI page, and is based on Wt (Witty)</p>.") ;

		about_dialog->setText(text) ;
		about_dialog->setStandardButtons(QMessageBox::Ok) ;
	}

	return about_dialog ;
}

WebUIPlugin::WebUIPlugin()
{
	mPlugInHandler = NULL;
}

void WebUIPlugin::setParams(std::string params)
{
	std::cerr<<"WebUIPlugin::setParams(\""<<params<<"\")"<<std::endl;
	const uint8_t NAME  = 0;
	const uint8_t KEY   = 1;
	const uint8_t VALUE = 2;
	uint8_t readingState = NAME;
	std::string name;
	std::string key;
	std::string value;
	std::map<std::string, std::string> paramMap;
	
	// split a string like "namespace:key=value;namespace:key2=value"
	// key=value pairs in namespace "rswebui" are added to a map
	for(int i=0; i<params.length(); i++)
	{
		char c = params[i];
		switch(readingState)
		{
			case NAME:
				if(c == ':'){
					readingState = KEY;
				}else{
					name += c;
				}
				break;
			case KEY:
				if(c == '='){
					readingState = VALUE;
				}else{
					key += c;
				}
				break;
			case VALUE:
				if(c == ';'){
					if(name == "rswebui"){
						paramMap[key] = value;
					}
					name  = "";
					key   = "";
					value = "";
					readingState = NAME;
				}else{
					value += c;
				}
				break;
		}
	}
	
	std::map<std::string, std::string>::iterator it = paramMap.find(std::string("port"));
	if(it != paramMap.end()){
		uint16_t port;
		std::istringstream ss(it->second);
		ss >> port;
		std::cerr << "Setting port of RsWebUI to " << (int)port << std::endl;
		RSWebUI::setPort(port);
	}
}

void WebUIPlugin::setInterfaces(RsPlugInInterfaces &interfaces)
{
	std::cerr << "Setting plugin interfaces for WebUI plugin..." << std::endl;
	plugin_interfaces = interfaces ;

	std::cerr << "Starting the WebUI" << std::endl;
	RSWebUI::start(interfaces) ;
}

void WebUIPlugin::stop()
{
	std::cerr << "Stopping the WebUI" << std::endl;
	RSWebUI::stop();
}

void WebUIPlugin::setPlugInHandler(RsPluginHandler *pgHandler)
{
    mPlugInHandler = pgHandler;
}

std::string WebUIPlugin::getShortPluginDescription() const
{
	return std::string("This plugin provides a WebUI to retroshare.") ;
}

std::string WebUIPlugin::getPluginName() const
{
	return std::string("WebUI Plugin");
}

