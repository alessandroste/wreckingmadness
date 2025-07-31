#include "ServiceClient.h"
#include "json/document.h"
#include "json/prettywriter.h"

#ifndef SERVICE_BASE_URL
#define SERVICE_BASE_URL "http://localhost:7071/api"
#endif

#ifndef SERVICE_API_KEY
#define SERVICE_API_KEY "testKey"
#endif

using namespace cocos2d;
using namespace cocos2d::network;
using namespace rapidjson;
using namespace wreckingmadness;

ServiceClient::ServiceClient() {
    httpClient = HttpClient::getInstance();
    httpClient->setTimeoutForConnect(5);
    httpClient->setTimeoutForRead(5);
}

ServiceClient::~ServiceClient() {}

ServiceClient& ServiceClient::getInstance() {
    static ServiceClient instance;
    return instance;
}

void ServiceClient::getNewPlayerId(
    GetPlayerIdSuccessCallback const& getPlayerIdSuccessCallback,
    GetPlayerIdFailureCallback const& getPlayerIdFailureCallback) {
    this->getPlayerIdSuccessCallback = getPlayerIdSuccessCallback;
    this->getPlayerIdFailureCallback = getPlayerIdFailureCallback;
    auto url = getEndpointUrl(std::string(SERVICE_ENDPOINT_GETPLAYERID));
    auto request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::GET);
    request->setUrl(url);
    request->setResponseCallback(std::bind(&ServiceClient::handleResponse, this, std::placeholders::_1, std::placeholders::_2));
    setDefaultHeaders(*request);
    request->setTag(SERVICE_ENDPOINT_GETPLAYERID);
    CCLOG("[ServiceClient] Sending player id request");
    httpClient->send(request);
    request->release();
}

void ServiceClient::sendScore(
    std::string& playerId,
    unsigned int topScore,
    ScoreUpdateSuccessCallback const& scoreUpdateSuccessCallback,
    ScoreUpdateFailureCallback const& scoreUpdateFailureCallback) {
    this->scoreUpdateSuccessCallback = scoreUpdateSuccessCallback;
    this->scoreUpdateFailureCallback = scoreUpdateFailureCallback;
    Document data(Type::kObjectType);
    auto& allocator = data.GetAllocator();
    data.AddMember("id", rapidjson::Value().SetString(playerId.c_str(), allocator), allocator);
    data.AddMember("topScore", rapidjson::Value().SetUint(topScore), allocator);
    auto url = getEndpointUrl(std::string(SERVICE_ENDPOINT_UPDATESCORE));
    auto request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(url);
    request->setResponseCallback(std::bind(&ServiceClient::handleResponse, this, std::placeholders::_1, std::placeholders::_2));
    setDefaultHeaders(*request);
    StringBuffer stringBuffer;
    Writer<StringBuffer> writer(stringBuffer);
    data.Accept(writer);
    std::string dataString(stringBuffer.GetString());
    CCLOG("[ServiceClient] Sending score");
    request->setRequestData(dataString.c_str(), dataString.length());
    request->setTag(SERVICE_ENDPOINT_UPDATESCORE);
    httpClient->send(request);
    request->release();
}

void ServiceClient::handleResponse(cocos2d::network::HttpClient* client, cocos2d::network::HttpResponse* response) {
    if (std::string(SERVICE_ENDPOINT_GETPLAYERID) == std::string(response->getHttpRequest()->getTag())) {
        if (response->getResponseCode() != 200) {
            auto header = response->getResponseHeader();
            auto error = std::string(&header->front(), header->size());
            getPlayerIdFailureCallback();
        }
        else {
            auto responseData = response->getResponseData();
            auto result = std::string(&(responseData->front()), responseData->size());
            Document document;
            document.Parse(result.c_str());
            auto id = std::string(document.GetString(), document.GetStringLength());
            getPlayerIdSuccessCallback(id);
        }
    }
    else if (std::string(SERVICE_ENDPOINT_UPDATESCORE) == std::string(response->getHttpRequest()->getTag())) {
        if (response->getResponseCode() != 200) {
            scoreUpdateFailureCallback();
        }
        else {
            auto responseData = response->getResponseData();
            auto result = std::string(&(responseData->front()), responseData->size());
            Document document;
            document.Parse(result.c_str());
            auto& percentage = document["percentile"];
            scoreUpdateSuccessCallback(percentage.GetFloat());
        }
    }
}

std::string ServiceClient::getEndpointUrl(const std::string& endpointPath) {
    std::string path(SERVICE_BASE_URL);
    if (path[path.length() - 1] != '/')
        path.append("/");
    path.append(endpointPath);
    return path;
}

void ServiceClient::setDefaultHeaders(HttpRequest& request) {
    auto headers = request.getHeaders();
    setHeader(headers, SERVICE_AUTH_HEADER, SERVICE_API_KEY);
    setHeader(headers, SERVICE_CONTENT_TYPE_HEADER, "application/json");
    request.setHeaders(headers);
}

void ServiceClient::setHeader(std::vector<std::string>& headers, const std::string& key, const std::string& value) {
    auto headerPos = std::find_if(headers.begin(), headers.end(), [key](const std::string& i) {
        return i.find(key) == 0;
        });
    if (std::end(headers) != headerPos) {
        *headerPos = key + ':' + value;
    }
    else {
        headers.emplace_back(key + ':' + value);
    }
}