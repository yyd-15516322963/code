#include "myhead.h"
#include "cJSON.h"
/*
	1.CJSON库采用数据结构中的双向循环链表存放要解析json数据

*/
int main()
{
	int i;
	// 准备一个json数据
	char *jsondata = "{\"姓名\":\"马云\",\"成绩\":68,\"爱好\":[\"吹牛\",\"摇滚\",\"赚钱\"]}";

	// cjson库提供接口函数来解析这个json数据
	// 第一步：把字符串格式的json数据转换成链表存放起来
	cJSON *jsonhead = cJSON_Parse(jsondata);

	// 第二步：严格按照你看到的json数据格式，剥洋葱一层层把数据解析
	cJSON *json1 = cJSON_GetObjectItem(jsonhead, "姓名"); // 我要获取对象中，"姓名"这个键对应的值
	printf("姓名这个键对应的值是: %s\n", json1->valuestring);

	cJSON *json2 = cJSON_GetObjectItem(jsonhead, "成绩"); // 我要获取对象中，"成绩"这个键对应的值
	printf("成绩这个键对应的值是: %d\n", json2->valueint);

	cJSON *json3 = cJSON_GetObjectItem(jsonhead, "爱好"); // 我要获取对象中，"爱好"这个键对应的值

	int n = cJSON_GetArraySize(json3);
	printf("%d\n", n);
	while (i < n)
	{
		cJSON *item = cJSON_GetArrayItem(json3, i);
		printf("%s\n", item->valuestring);
		i++;
	}

	cJSON_Delete(jsonhead);
}
