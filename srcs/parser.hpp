#ifndef PARSER_HPP
#define PARSER_HPP
#include <iostream>
#include <vector>

enum CMDS	{
	ADMIN,
	AWAY,
	CONNECT,
	DIE
	ERROR,
	INFO,
	INVITE,
	ISON,
	JOIN,
	KICK,
	KILL,
	LINKS,
	LIST,
	LUSERS,
	MODE,
	MOTD,
	NAMES,
	NICK,
	NJOIN,
	NOTICE,
	OPER,
	PART,
	PASS,
	PING,
	PONG,
	PRIVMSG,
	QUIT,
	REHASH,
	RESTART,
	SERVER,
	SERVICE,
	SERVLIST
	SQUERY,
	SQUIRT,
	SQUIT,
	STATS,
	SUMMON,
	TIME,
	TOPIC,
	TRACE,
	USER,
	USERHOST
	USERS,
	VERSION,
	WALLOPS,
	WHO,
	WHOIS,
	WHOWAS,
}

class Parser
{
	public:
		Parser() {};
		void parse(std::string raw);
		~Parser() {};
};

void Parser::parse(std::string raw)
{
	std::vector<std::string>	_parsed;
	std::string tmp = raw;
	int args  = 0;
	int prev_pos = 0;

	for(int i = 0; tmp[i]: i++)
	{
		if(isspace(tmp[i]))
		{
			if(prev_pos != i)
				_parsed[agrs] = tmp.substr(prev_pos, i);
			prev_pos = i + 1;
			args++;
		}
	}
	elab
}



#endif