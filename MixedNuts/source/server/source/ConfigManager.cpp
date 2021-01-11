#include <boost/algorithm/string.hpp>
#include "ConfigManager.h"
#include "system_adapter.h"
#include "Tokenizer.h"

//Config Parser
bool 
ConfigParser::LoadConfig(char* szFileName)
{
	filetype* fp = NULL;
	fp = file_open(szFileName,mode_read);
	
	if (!fp) return false;
	
	char buffer[1024];
	
	// parse each line in config-file
	while (file_readline(fp, buffer, sizeof(buffer)))
	{
		size_t size = strlen(buffer);
		std::string strData(buffer);
		Tokens tokens;
		Tokenize(strData,tokens,CONFIG_DELIM);
		
		//ignore blank lines
		if(tokens.size() <= 0 || 
			(tokens.size() == 1 && tokens[0]=="")) continue;

		//skip comment
		if( (tokens[0].substr(0,1)) == CONFIG_COMMENT)
			continue;
		boost::trim(tokens[0]);
		boost::trim(tokens[1]);
		m_ConfigVars[tokens[0]] = tokens[1];
	}

	if(fp){
		file_close(fp);
	}
	return true;
}

bool ConfigParser::Get(const std::string &name, std::string &value) const
{
	std::map<std::string,std::string>::const_iterator it = 
		m_ConfigVars.find(name);
	
	if (it == m_ConfigVars.end())
		return false;
	
	value = it->second;
	
	return true;
}

std::string ConfigParser::Get(const std::string &name) const
{
	std::string value = "";
	
	Get(name, value);
	
	return value;
}

bool ConfigParser::GetBool(const std::string &name, bool &value) const
{
	std::string svalue;
	if (!Get(name, svalue))
		return false;
	
	if (svalue == "true" || svalue == "yes" || svalue == "1")
		value = true;
	else
		value = false;
	
	return true;
}

bool ConfigParser::GetInt(const std::string &name, int &value) const
{
	std::string svalue;
	if (!Get(name, svalue))
		return false;
	
	value = atoi(svalue.c_str());
	
	return true;
}

//Config Manager

ConfigManager::ConfigManager(void)
{
}

ConfigManager::~ConfigManager(void)
{
}

bool 
ConfigManager::LoadConfig(char* szFileName,
						  ServerConfig* out)
{
	int value = 0;
	bool bValue = false;

	if(!m_Parser.LoadConfig(szFileName)) {
		LoadDefault(out);
		return false;
	}	

	if(m_Parser.GetInt(CONFIG_PORT,value))
		out->sPort = (short)value;
	else
		out->sPort = CONFIG_DEFAULT_PORT;

	if(m_Parser.GetInt(CONFIG_POOLSIZE,value))
		out->nPoolSize = value;
	else
		out->nPoolSize = CONFIG_DEFAULT_POOLSIZE;

	if(m_Parser.GetBool(CONFIG_USEIPV6,bValue))
		out->bIpv6 = bValue;
	else
		out->bIpv6 = CONFIG_DEFAULT_USEIPV6;

	return true;
}

void 
ConfigManager::LoadDefault(ServerConfig* out)
{
	out->sPort = CONFIG_DEFAULT_PORT;
	out->bIpv6 = CONFIG_DEFAULT_USEIPV6;
	out->nPoolSize = CONFIG_DEFAULT_POOLSIZE;
}