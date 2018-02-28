#include "AliyunSMS.h"

#ifndef WIN32
#include <time.h>
unsigned int timeGetTime()
{
	unsigned int uptime = 0;
	struct timespec on;
	if (clock_gettime(CLOCK_MONOTONIC, &on) == 0)
		uptime = on.tv_sec * 1000 + on.tv_nsec / 1000000;
	return uptime;
}
#endif
CAliyunSMS::CAliyunSMS()
{
}

CAliyunSMS::~CAliyunSMS()
{
}
#ifdef WIN32
static void cdecl openurl(void* param)
#else
static void* openurl(void* param)
#endif
{
	//http GET
	string url = (char*)param;
	string host = "dysmsapi.aliyuncs.com";
	string request;
	request += "GET " + url;
	request += " HTTP/1.0\r\n";
	request += "Host: " + host + "\r\n";
	request += "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.2.3) Gecko/20100401 Firefox/3.6.3\r\n";
	request += "Connection:close\r\n\r\n";
	struct sockaddr_in address;
	struct hostent *server;
	address.sin_family = AF_INET;
	address.sin_port = htons(80);
	server = gethostbyname(host.c_str());
	if (server == NULL)
	{
		cout << "gethostbyname error!" << endl;
#ifdef WIN32
		return;
#else
		pthread_detach(pthread_self());
		return NULL;
#endif
	}
	memcpy((char *)&address.sin_addr.s_addr, (char*)server->h_addr, server->h_length);

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == connect(sockfd, (struct sockaddr *)&address, sizeof(address))) {
		cout << "connection error!" << endl;
#ifdef WIN32
		return;
#else
		pthread_detach(pthread_self());
		return NULL;
#endif
	}
#ifdef WIN32
	send(sockfd, request.c_str(), request.size(), 0);
#else
	write(sockfd, request.c_str(), request.size());
#endif
	char *buf = new char[1024 * 1024];
	int offset = 0;
	int rc;
#ifdef WIN32
	while (rc = recv(sockfd, buf + offset, sizeof(buf), 0))
#else
	while (rc = read(sockfd, buf + offset, sizeof(buf)))
#endif
	{
		offset += rc;
	}
	buf[offset] = 0;
	cout << "openurl ret :" << endl << buf << std::endl;
	if (buf) delete[] buf;
#ifdef WIN32
	closesocket(sockfd);
	return;
#else
	close(sockfd);
	pthread_detach(pthread_self());
	return NULL;
#endif
}
void CAliyunSMS::init(string ak, string aks, string signName, string templateCode)
{
#ifdef WIN32
	WSADATA wsa = { 0 };
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return;
	}
#endif
	m_accessKeyId = ak;
	m_accessSecret = aks + "&";
	m_signName = specialUrlEncode(signName.c_str());
	m_templateCode = templateCode;
}
void CAliyunSMS::sendSm(int shortCode, string phone)
{
	char Timestamp[64] = { 0 };
	struct tm *tms;
	time_t now;
	time(&now);
	now -= 8 * 60 * 60;
	tms = localtime(&now);
	tms->tm_year += 1900;
	tms->tm_mon += 1;
	char * str1 = new char[256];
	sprintf(Timestamp, "%4d-%02d-%02dT%02d:%02d:%02dZ",
		tms->tm_year, tms->tm_mon, tms->tm_mday, tms->tm_hour, tms->tm_min, tms->tm_sec);

	unsigned int tm = timeGetTime();
	char str[256] = { 0 };
	sprintf(str, "%d-%d-%d", rand() % 100000, tm, rand() % 100000);
	string sn = str;

	char TemplateParam[256] = { 0 };
	sprintf(TemplateParam, "{\"code\":\"%d\",\"product\":\"dsd\"}", shortCode);

	string queryString;
	queryString += "AccessKeyId=" + m_accessKeyId;
	queryString += "&Action=SendSms";
	queryString += "&Format=XML";
	queryString += "&OutId=123";
	queryString += "&PhoneNumbers=" + phone;
	queryString += "&RegionId=cn-hangzhou";
	queryString += "&SignName=" + m_signName;
	queryString += "&SignatureMethod=HMAC-SHA1";
	queryString += "&SignatureNonce=" + sn;
	queryString += "&SignatureVersion=1.0";
	queryString += "&TemplateCode=" + m_templateCode;
	queryString += "&TemplateParam=" + specialUrlEncode(TemplateParam);
	queryString += "&Timestamp=" + specialUrlEncode(Timestamp);
	queryString += "&Version=2017-05-25";

	string stringToSign = "GET&" + specialUrlEncode("/") + "&" + specialUrlEncode(queryString.c_str());
	string signstr = sign(m_accessSecret.c_str(), stringToSign.c_str());
	string signature = specialUrlEncode(signstr.c_str());
	static string url;
	url = "http://dysmsapi.aliyuncs.com/?Signature=" + signature + "&" + queryString;

#ifdef WIN32
	_beginthread(openurl, 0, (void*)url.c_str());
#else
	pthread_t m_thread;
	pthread_create(&m_thread, NULL, openurl, (void*)url.c_str());
#endif
}
string CAliyunSMS::specialUrlEncode(const char* str)
{
	if (str == NULL) return "";
	char ch;
	int strSize = strlen(str);
	string result;
	for (int i = 0; i < strSize; i++) {
		ch = str[i];
		if (ch == '+') {
			result += "%2B";
		} else if (ch == '*') {
			result += "%2A";
		} else if (ch == '~') {
			result += '~';
		} else if (ch == ' ') {
			result += '+';
		} else if (ch == '.' || ch == '-' || ch == '_' || ch == '*') {
			result += ch;
		} else if (ch >= 'A'&&ch <= 'Z' || ch >= 'a'&&ch <= 'z' || ch >= '0'&&ch <= '9') {
			result += ch;
		} else {
			char temp[64] = "";
			sprintf(temp, "%%%02X", (unsigned char)ch);
			result += temp;
		}
	}
	return result;
}
string CAliyunSMS::sign(const char* key, const char* text)
{
	CHMAC_SHA1 hmac;
	char digest[256] = { 0 };
	hmac.HMAC_SHA1(text, key, digest);
	string signstr = digest;
	encode64(signstr);
	return signstr;
}
