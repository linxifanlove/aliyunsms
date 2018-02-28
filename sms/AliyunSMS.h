#pragma once
#include <iostream>
#include <string>
#include "HMAC_SHA1.h"
#include "base64.h"
#include <time.h>
#ifdef WIN32
#include <windows.h>
#include <wininet.h>
#include <process.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Wininet.lib")
#else
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#endif
using namespace std;
class CAliyunSMS
{
public:
	CAliyunSMS();
	~CAliyunSMS();
	void init(string ak, string aks, string signName, string templateCode);
	void sendSm(int shortCode, string phone);
	/**
	* @brief URLEncode 对字符串URL编码(阿里云版本)
	加号（+）替换成 %20、星号（*）替换成 %2A、%7E 替换回波浪号（~）
	加号（+）在实际使用的时候替换的是%2B 官方文档写的是%20 但是不可用 java测试的时候使用的也是%2B
	* @param str 原字符串
	* @return: 成功返回 编码字符串 失败返回 ""
	*/
	string specialUrlEncode(const char* str);
	//hmacsha1算法签名
	string sign(const char* key, const char* text);
	//http get 方式请求
private:
	//aliyun AK
	string m_accessKeyId;
	//aliyun AKS
	string m_accessSecret;
	//短信服务签名名称 需要传入的字符串编码为UTF8
	string m_signName;
	//短信服务模板代码
	string m_templateCode;

};
