#include "ServiceClient.h"
#include "json/document.h"

using namespace cocos2d;
using namespace cocos2d::network;
using namespace rapidjson;
using namespace wreckingmadness;

ServiceClient::ServiceClient() {
    httpClient = HttpClient::getInstance();
}

ServiceClient::~ServiceClient() {}

ServiceClient& ServiceClient::getInstance() {
    static ServiceClient instance;
    return instance;
}

void ServiceClient::getNewPlayerId(std::function<void(std::string&)> const& playerIdCallback) {
    getPlayerIdCallback = playerIdCallback;
    auto url = getEndpointUrl(std::string(SERVICE_ENDPOINT_GETPLAYERID));
    auto request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::GET);
    request->setUrl(url);
    request->setResponseCallback(std::bind(&ServiceClient::handleResponse, this, std::placeholders::_1, std::placeholders::_2));
    request->setHeaders(getDefaultHeaders());
    request->setTag(SERVICE_ENDPOINT_GETPLAYERID);
    httpClient->send(request);
}

void ServiceClient::sendScore(std::string& playerId, unsigned int topScore, std::function<void(float)> const& scoreUpdateCallback) {
    this->scoreUpdateCallback = scoreUpdateCallback;
    auto data = Document();
    data.SetObject();
    // data.AddMember("id", playerId, data.GetAllocator());
    // data.AddMember("topScore", topScore, data.GetAllocator());
    auto url = getEndpointUrl(std::string(SERVICE_ENDPOINT_UPDATESCORE));
    auto request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(url);
    request->setResponseCallback(std::bind(&ServiceClient::handleResponse, this, std::placeholders::_1, std::placeholders::_2));
    request->setHeaders(getDefaultHeaders());
    request->setRequestData(data.GetString(), data.GetStringLength());
    request->setTag(SERVICE_ENDPOINT_UPDATESCORE);
    httpClient->send(request);
}

void ServiceClient::handleResponse(cocos2d::network::HttpClient* client, cocos2d::network::HttpResponse* response) {
    if (std::string(SERVICE_ENDPOINT_GETPLAYERID) == std::string(response->getHttpRequest()->getTag())) {
        auto result = response->getResponseDataString();
        getPlayerIdCallback(std::string(result));
    }
    else if (std::string(SERVICE_ENDPOINT_UPDATESCORE) == std::string(response->getHttpRequest()->getTag())) {
        auto responseData = response->getResponseDataString();
        auto responseDataDocument = Document();
        responseDataDocument.Parse(responseData);
        auto percentage = responseDataDocument["percentile"].GetFloat();
        scoreUpdateCallback(percentage);
    }
}

std::string ServiceClient::getEndpointUrl(std::string& endpointPath) {
    std::string path(SERVICE_BASE_URI);
    if (path[path.length() - 1] != '/')
        path.append("/");
    path.append(endpointPath);
    return path;
}

std::vector<std::string> ServiceClient::getDefaultHeaders() {
    auto headers = std::vector<std::string>();
    headers.emplace_back(std::string(SERVICE_AUTH_HEADER).append(SERVICE_KEY));
    return headers;
}