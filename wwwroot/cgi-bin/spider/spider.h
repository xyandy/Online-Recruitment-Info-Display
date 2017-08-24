#ifndef SPIDER_H
#define SPIDER_H

#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <regex>

using namespace std;



class Spider
{
public:
    explicit Spider(const string& url);//抓取文件存到_htmlContent
    string getHtmlContent();  
    vector<pair<string, string>> getInfo(); 
private:
    void parseHtml();
    static size_t writeData(char *ptr, size_t size, size_t nmemb, void *userdata);
    void getCompanyIp(const string& s, string& company, string& ip);
private:
    string _htmlContent;
    vector<pair<string, string>> _ret;
};









#endif
