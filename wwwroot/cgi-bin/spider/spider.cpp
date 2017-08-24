#include "spider.h"


class UniquePtr
{
public:
    UniquePtr(CURL* ptr)
    : _p(ptr) 
    {}

    ~UniquePtr()
    {
        curl_easy_cleanup(_p);
    }
private:
    UniquePtr(const UniquePtr& );
    UniquePtr& operator=(const UniquePtr& );

private:
    CURL* _p;

};


Spider::Spider(const string& url)
{
    if (url.empty())
    {
        cout << "url empty" << endl;
        return;
    }

    CURL *_handle = curl_easy_init();


    if (_handle == NULL)
    {
        cout << "_handle NULL" << endl;
        return;
    }
    UniquePtr up(_handle); //RAII 自动释放资源

    
    curl_easy_setopt(_handle, CURLOPT_URL, url.c_str()); //抓取的网页
    curl_easy_setopt(_handle, CURLOPT_WRITEFUNCTION, writeData); //回调函数
    curl_easy_setopt(_handle, CURLOPT_WRITEDATA, &_htmlContent);//html内容写到_htmlContent 
    
    CURLcode ret = curl_easy_perform(_handle);
    if (ret != CURLE_OK)
    {
        cout << "cur_easy_perform failed" << endl;
    }
    //cout << "succeed in obtaining the html and html size is " << _htmlContent.size() << endl;
    //cout << _htmlContent << endl;
    parseHtml(); //解析文本到Vector中
}

size_t Spider::writeData(char *ptr, size_t size, size_t n, void *userdata)
{
    if (ptr == NULL)
    {
        cout << "write_data : data is NULL" << endl;
        return 0;
    }
    string tmp(ptr, size*n);
    *(string*)userdata += tmp;
    return size*n;
}

string Spider::getHtmlContent()
{
    return _htmlContent;
}

void Spider::parseHtml()
{
    string s("<a href=\"/new.*>");
    regex reg(s);
    regex_iterator<string::iterator> rit(_htmlContent.begin(), _htmlContent.end(), reg);
    regex_iterator<string::iterator> rend;
    vector<string> v(20);
    int size = 0;
    while (rit != rend)
    {
        //cout << rit->str() << endl;
        v[size++] = rit->str(); 
        rit++;
    }
    for (int i = 0; i < size; i++)
    {
        string company;
        string ip = "http://jiuye.www.sust.edu.cn";
        string str = v[i];
        getCompanyIp(str, company, ip);
        //cout << company << ip << endl;
        pair<string, string> p(company, ip);
        _ret.push_back(p); 
    }

}

void Spider::getCompanyIp(const string& s, string& company, string& ip)
{
    int i = 9;
    int sz = s.size();
    while (i < sz)
    {
        while (i < sz && s[i] != '\"')
            ++i;
        ip += s.substr(9, i-9);
        i++;
        while (i < sz && s[i] != '\"')
            ++i;
        int beg = ++i;
        while (i < sz && s[i] != '\"')
            ++i;
        company += s.substr(beg, i-beg);
        break;
    }
}

vector<pair<string, string>> Spider::getInfo()
{
    return _ret;
}


