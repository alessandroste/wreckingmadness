#ifndef __SERVICECLIENT_H__
#define __SERVICECLIENT_H__

#include "cocos2d.h"
#include "network\HttpClient.h"

#define SERVICE_BASE_URI "http://localhost:7071/api"
#define SERVICE_ENDPOINT_GETPLAYERID "GetPlayerId"
#define SERVICE_ENDPOINT_UPDATESCORE "UpdateScore"
#define SERVICE_AUTH_HEADER "x-functions-key:"
#define SERVICE_CONTENT_TYPE_HEADER "Content-Type:"

#ifndef SERVICE_KEY
#define SERVICE_KEY "testKey"
#endif

typedef std::function<void(std::string&)> GetPlayerIdSuccessCallback;
typedef std::function<void(float)> ScoreUpdateSuccessCallback;
typedef std::function<void()> GetPlayerIdFailureCallback;
typedef std::function<void()> ScoreUpdateFailureCallback;

namespace wreckingmadness {
    class ServiceClient {
    private:
        static ServiceClient* serviceClient;
        ServiceClient();

        cocos2d::network::HttpClient* httpClient;
        GetPlayerIdSuccessCallback getPlayerIdSuccessCallback;
        GetPlayerIdFailureCallback getPlayerIdFailureCallback;
        ScoreUpdateSuccessCallback scoreUpdateSuccessCallback;
        ScoreUpdateFailureCallback scoreUpdateFailureCallback;

        static std::string getEndpointUrl(std::string& endpointPath);
        static void setDefaultHeaders(cocos2d::network::HttpRequest& request);
        static void setHeader(std::vector<std::string>& headers, const char* key, const char* value);
    public:
        static ServiceClient& getInstance();
        ~ServiceClient();

        void handleResponse(cocos2d::network::HttpClient* client, cocos2d::network::HttpResponse* response);
        void getNewPlayerId(
            GetPlayerIdSuccessCallback const& playerIdSuccessCallback,
            GetPlayerIdFailureCallback const& playerIdFailureCallback);
        void sendScore(
            std::string& playerId,
            unsigned int score,
            ScoreUpdateSuccessCallback const& scoreUpdateSuccessCallback,
            ScoreUpdateFailureCallback const& scoreUpdateFailureCallback);
    };
}

#endif