#include <iostream>
#include <string>
#include "sms/AliyunSMS.h"
using namespace std;
void main()
{
	srand(time(NULL));
	string ak = "yourAccessKeyId";
	string aks = "yourAccessKeySecret";
	string signName = "yourSignName";
	string templateCode = "SMS_000000000";

	CAliyunSMS sms;
	sms.init(ak, aks, signName, templateCode);
	int shortCode = rand() % 9000 + 1000;//发送给手机的验证码
	string phone = "137xxxxxxxxx";
	sms.sendSm(shortCode, phone);

	getchar();
}
