# 在线招聘信息展示

## 开发环境：

Linux（Ubuntu 16.04）

## 主要内容：

编写Web服务器，并且利用此服务器爬虫实时抓取本校最新的招聘信息，并且回显到用户浏览器上

## 项目详情：

主要包括3个模块：
1. 服务器构建模块，利用C语言进行socket编程构建Web服务器，完成对http请求报文的解析，并且针对服务器的并发性进行优化。
2. 信息抓取模块，利用libcurl库爬虫抓取科大最新招聘信息，并利用regex匹配关键信息；
3. 在线显示模块，将关键信息以html格式返回给用户浏览器

## 效果如下：

![1](https://github.com/xyandy/TinySpiderServer/blob/master/1.png)

---

![2](https://github.com/xyandy/TinySpiderServer/blob/master/2.png)

---

![3](https://github.com/xyandy/TinySpiderServer/blob/master/3.png)
