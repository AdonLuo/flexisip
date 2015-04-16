/*
		Flexisip, a flexible SIP proxy server with media capabilities.
	Copyright (C) 2012  Belledonne Communications SARL.
	Author: Guillaume Beraudo

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Affero General Public License as
	published by the Free Software Foundation, either version 3 of the
	License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Affero General Public License for more details.

	You should have received a copy of the GNU Affero General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef registrardb_redis_hh
#define registrardb_redis_hh

#include "registrardb.hh"
#include "recordserializer.hh"
#include <sofia-sip/sip.h>
#include <sofia-sip/nta.h>
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include "agent.hh"

struct RedisParameters {
	RedisParameters() : port(0), timeout(0) {}
	std::string domain;
	std::string auth;
	int port;
	int timeout;
	int slave_check_timeout;
};

/**
 * @brief The RedisHost struct, which is used to store redis slave description.
 */
struct RedisHost {
	RedisHost(int id, const std::string& address, unsigned short port, const std::string& state)
		: id(id), address(address), port(port), state(state){}

	RedisHost() {
		// invalid host
		id = -1;
	}

	/**
	 * @brief parseSlave this class method will parse a line from Redis where a slave information is expected.
	 *
	 * If the parsing goes well, the returned host will have the id passed as argument, otherwise -1.
	 * @param slaveLine the Redis answer line where a slave is defined. Format is "host,port,state"
	 * @param id an ID to give to this slave, usually its number.
	 * @return A RedisHost with a valid ID or -1 if the parsing failed.
	 */
	static RedisHost parseSlave(const std::string& slaveLine, int id);
	int id;
	std::string address;
	unsigned short port;
	std::string state;
};

class RegistrarDbRedisAsync : public RegistrarDb {
public:
	struct RegistrarUserData;
	RegistrarDbRedisAsync(const string &preferredRoute, su_root_t* root, RecordSerializer* serializer, RedisParameters params);

protected:
	bool connect();
	bool disconnect();

	virtual void doBind(const BindParameters &params, const std::shared_ptr<RegistrarDbListener> &listener);
	virtual void doClear(const sip_t *sip, const std::shared_ptr<RegistrarDbListener> &listener);
	virtual void doFetch(const url_t *url, const std::shared_ptr<RegistrarDbListener> &listener);

private:
	RegistrarDbRedisAsync(Agent *agent, RedisParameters params);
	~RegistrarDbRedisAsync();
	static void sConnectCallback(const redisAsyncContext *c, int status);
	static void sDisconnectCallback(const redisAsyncContext *c, int status);
	bool isConnected();
	friend class RegistrarDb;
	Agent* mAgent;
	redisAsyncContext *mContext;
	RecordSerializer *mSerializer;
	std::string sDomain;
	std::string sAuthPassword;
	int sPort;
	int sTimeout;
	su_root_t *mRoot;
	vector<RedisHost> vSlaves;
	bool bShouldReconnect;
	su_timer_t* mReplicationTimer;
	int slave_check_timeout;

	/* callbacks */
	void handleAuthReply(const redisReply* reply);
	void handleBind(redisReply *reply, RegistrarUserData *data);
	void handleBindReplyAorSet(redisReply *reply, RegistrarUserData *data);
	void handleClear(redisReply *reply, RegistrarUserData *data);
	void handleFetch(redisReply *reply, RegistrarUserData *data);
	void handleReplicationInfoReply(const char* str);
	void onConnect( const redisAsyncContext* c, int status);
	void onDisconnect(const redisAsyncContext*c, int status);

	/* replication */
	void getReplicationInfo();
	void updateSlavesList(const map<string,string> redisReply );
	void tryReconnect();

	/* static handlers */
	static void sHandleAorGetReply(struct redisAsyncContext*, void *r, void *privdata);
	static void shandleAuthReply ( redisAsyncContext* ac, void *r, void *privdata );
	static void sHandleBind(redisAsyncContext* ac, redisReply *reply, RegistrarUserData *data);
	static void sHandleClear(redisAsyncContext* ac, redisReply *reply, RegistrarUserData *data);
	static void sHandleFetch(redisAsyncContext* ac, redisReply *reply, RegistrarUserData *data);
	static void sHandleInfoTimer(void *unused, su_timer_t *t, void *data);
	static void sHandleReplicationInfoReply( redisAsyncContext* ac, void* r, void* privdata);
	static void sHandleSet(redisAsyncContext* ac, void *r, void *privdata);

};


#endif
