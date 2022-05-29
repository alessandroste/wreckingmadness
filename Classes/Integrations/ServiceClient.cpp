#include "ServiceClient.h"
#include "json/document.h"
#include "json/prettywriter.h"

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
    CCLOG("[ServiceClient] Sending score with body: %s", dataString.c_str());
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
    std::string path(SERVICE_BASE_URI);
    if (path[path.length() - 1] != '/')
        path.append("/");
    path.append(endpointPath);
    return path;
}

void ServiceClient::setDefaultHeaders(HttpRequest& request) {
    auto headers = request.getHeaders();
    setHeader(headers, SERVICE_AUTH_HEADER, SERVICE_KEY);
    setHeader(headers, SERVICE_CONTENT_TYPE_HEADER, "application/json");
    request.setHeaders(headers);
}

void ServiceClient::setHeader(std::vector<std::string>& headers, const char* key, const char* value) {
    std::string keyToFind(key);
    auto checkFunction = [keyToFind](const std::string& i) {
        auto pos = i.find(keyToFind);
        return pos == 0;
    };

    auto headerPos = std::find_if(headers.begin(), headers.end(), checkFunction);
    if (std::end(headers) != headerPos) {
        *headerPos = keyToFind.append(value);
    }
    else {
        headers.emplace_back(keyToFind.append(value));
    }
}