#include "spider.h"

int main()
{
    string url = "http://jiuye.www.sust.edu.cn/zph.jsp?urltype=tree.TreeTempUrl&wbtreeid=1003";
    Spider spider(url); 
    //string html = spider.getHtmlContent();
    //cout << html << endl;
        
    vector<pair<string, string>> v = spider.getInfo();
    //创建html格式
    cout << "<html lang=\"en\">" << endl;
    cout << "<head>"<< endl;
    cout << "<meta charset=\"UTF-8\">"<< endl;
    cout << "</head>"<< endl;
    
    cout << "<body>" << endl;
    cout << "<h1>科大最新招聘信息如下：</h1>"<< endl; 
    for (auto x : v)
        cout << "<p>" << x.first << x.second << "</p>" <<endl;
    
    
    cout << "</body>" << endl;
    cout << "</html>" << endl;
    return 0;
}


//<html lang="en">
//<head>
//    <meta charset="UTF-8">
//
//</head>
//
//<body>
//</body>
//</html>
