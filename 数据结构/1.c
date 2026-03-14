#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
	char buf1[100] = {0};
	char buf2[100] = {0};

	printf("请输入两个字符串:\n");
	scanf("%s", buf1);
	scanf("%s", buf2);

	int len1 = strlen(buf1);
	int len2 = strlen(buf2);
	int buf[len1][len2];
	bzero(buf, sizeof(buf));

	int temp, flag = 0;
	for (int i = 0; i < len1; i++)
	{
		for (int j = 0; j < len2; j++)
		{
			if (buf1[i] == buf2[j])
			{
				buf[i][j] = 1;
				if (flag == 0)
				{
					temp = j;
					flag++;
				}
			}
		}
	}

	for (int i = 0; i < len1; i++)
	{
		for (int j = 0; j < len2; j++)
		{
			printf("%d ", buf[i][j]);
		}
		printf("\n");
	}

	int sum = 0;
	int i = 0, j = 0;

	while (i < len1 && j < len2)
	{
		if (buf[i][j] == 1)
		{

			i++;
			j++;
			sum++;
		}
		else
		{
			j++;

			if (j >= len2)
			{
				i++;
				j = 0;
			}
		}
	}
	printf("%d\n", sum);
	int k = 0;
	while (k < sum)
	{
		printf("%c", buf2[temp + k]);
		k++;
	}
	printf("\n");

	return 0;
}
