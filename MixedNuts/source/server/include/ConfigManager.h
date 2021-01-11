#ifndef _CONFIGMANAGER_H_
#define _CONFIGMANAGER_H_

#include <map>
#include <string>

//config delimeters
#define CONFIG_DELIM	"="
#define CONFIG_COMMENT  "#"

//Server Config Variables
#define CONFIG_PORT		"port"
#define CONFIG_USEIPV6  "use_ipv6"
#define CONFIG_POOLSIZE "pool_size"

//config default values
#define CONFIG_DEFAULT_PORT			(1983)
#define CONFIG_DEFAULT_USEIPV6		(false)
#define CONFIG_DEFAULT_POOLSIZE		(2)

//server configuration
typedef struct {
		short sPort;		//server port	
		bool bIpv6;			//use ipv6 ip addres
		std::size_t nPoolSize;	//this is equivalent to number of cores
}ServerConfig;	

class ConfigParser
{
public:
	ConfigParser(){}
	~ConfigParser(){}
	bool LoadConfig(char* szFName);
	bool Get(const std::string &name, std::string &value) const;	
	bool GetBool(const std::string &name, bool &value) const;
	bool GetInt(const std::string &name, int &value) const;
	std::string Get(const std::string &name) const;

private:
	std::map<std::string, std::string> m_ConfigVars;
};


class ConfigManager
{
public:	
	ConfigManager(void);
	~ConfigManager(void);
	bool LoadConfig(char* szFileName, 
		ServerConfig* out);
	void LoadDefault(ServerConfig* out);
private:
	ConfigParser m_Parser;
};

#endif