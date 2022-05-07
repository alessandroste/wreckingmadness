#ifndef __SERVICECLIENT_H__
#define __SERVICECLIENT_H__

#include "cocos2d.h"
#include "network\HttpClient.h"

#define SERVICE_BASE_URI "http://localhost:7071/api"
#define SERVICE_ENDPOINT_GETPLAYERID "GetPlayerId"
#define SERVICE_ENDPOINT_UPDATESCORE "UpdateTopScoreForPlayer"
#define SERVICE_AUTH_HEADER "x-functions-key:"

#ifndef SERVICE_KEY
#define SERVICE_KEY "testKey"
#endif

namespace wreckingmadness {
    class ServiceClient {
    private:
        ServiceClient();

        cocos2d::network::HttpClient* httpClient;
        std::function<void(std::string&)> getPlayerIdCallback;
        std::function<void(float)> scoreUpdateCallback;

        static std::string getEndpointUrl(std::string& endpointPath);
        static std::vector<std::string> getDefaultHeaders();
    public:
        static ServiceClient& getInstance();
        ~ServiceClient();

        void handleResponse(cocos2d::network::HttpClient* client, cocos2d::network::HttpResponse* response);
        void getNewPlayerId(std::function<void(std::string&)> const& playerIdCallback);
        void sendScore(std::string& playerId, unsigned int score, std::function<void(float)>const& scoreUpdateCallback);
    };
}

#endif