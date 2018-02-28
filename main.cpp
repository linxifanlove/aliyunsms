#include <iostream>
#include <string>
#include "sms/AliyunSMS.h"
using namespace std;
void main()
{
	srand(time(NULL));

		m_accessKeyId = ak;
		m_accessSecret = aks + "&";
		m_signName = specialUrlEncode(signName.c_str());
		m_templateCode = templateCode;
	string ak = "yourAccessKeyId";
	string aks = "yourAccessKeySecret";
	string signName = "yourSignName";
	string templateCode = "SMS_000000000";

	CAliyunSMS sms;
	sms.init(ak, aks, signName, templateCode);
	int shortCode = rand() % 9000 + 1000;//发送给手机的验证码
	string phone = "13751762631";
	sms.sendSm(shortCode, phone);

	getchar();
}
