/*运行环境：Visual Studio Community 2019
项目属性：SDL检查-否
预处理器定义：WIN32;_DEBUG;_CONSOLE;%(PreprocessorDefinitions); _CRT_SECURE_NO_WARNINGS*/

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<time.h>
#include<ctype.h>

#define TRUE 1
#define FALSE 0
#define MAX_ARRAY_LEN 200

/*---------------- 数据结构定义 ----------------*/
// 日期
struct Date 
{
	int year;
	int month;
	int day;
};

// 图书管理员
struct Librarian 
{
	char JobNumber[11]; // 工号 10位数字
	char name[16];      // 名字
	char email[40];     // 邮箱
	char password[20];  // 密码
};

// 学生
struct Student 
{
	char StudentID[11];  		// 学号 10位数字
	char faculty[20];           // 院系
	char major[20];             // 专业
	char name[16];              // 名字
	char gender[8];             // 性别
	char phone[12];				// 11位电话
	char email[40];				// 邮箱
	char password[20];          // 密码
	int frozen;                 // 冻结状态 0 = 正常, 1 = 冻结
};

// 图书信息结点
struct Book 
{
	char BookID[7];		    // 图书编号6位数字
	char name[32];		    // 图书名字
	char author[40];        // 作者
	char publisher[40];     // 出版社
	char blurb[100];	    // 简介
	float price;		    // 价格
	Date date;			    // 出版日期
	int BorrowTotalCount;   // 借阅总次数 
	int position;           // 图书位置
	int BorrowState;        // 0=未借出;1=借出
	time_t BorrowTotalTime; // 借阅总时长
};

// 图书借阅记录
struct BorrowRecord 
{
	char BookID[7];     // 图书编号
	char StudentID[11]; // 学生学号
	time_t BorrowTime;  // 借书时间
	time_t ReturnTime;	// 还书时间
	int RenewCount;     // 续借次数
};

/*---------------- 全局变量定义 ----------------*/

// 存放图书的全局数组
Book g_Books[100];
// 图书数组下一个可用的下标
int g_BookIndex = 0;
// 存放学生的全局数组
Student g_Students[MAX_ARRAY_LEN];
// 学生数组下一个可用的下标
int g_StudentIndex = 0;
// 存放图书借阅记录的全局数组
BorrowRecord g_BorrowRecords[MAX_ARRAY_LEN];
// 图书借阅记录数组下一个可用的下标
int g_BorrowRecordIndex = 0;
// 存放管理员的全局数组
Librarian g_librarians[MAX_ARRAY_LEN];
// 管理员数组下一个可用的下标
int g_LibrarianIndex = 0;
// 当前登录学生在学生数组中的下标
int g_LoginStudentIndex = -1;
// 当前登录图书管理员在图书管理员数组中的下标
int g_LoginLibrarianIndex = -1;
// 准备借阅图书购物车数组
Book g_BorrowCart[3] = { 0 };
// 准备借阅购物车中图书的数量
int g_CartCount = 0;

/*---------------- 数据结构存储恢复相关 ----------------*/

// 将图书数据保存到文件
void StoreBookDataToFile()
{
	// 打开文件——注意，文件为二进制格式
	FILE* fp = fopen("book.data", "wb");
	
	// 写入下一个可用的下标
	fwrite(&g_BookIndex, sizeof(int), 1, fp);

	if (0 != g_BookIndex)
	{
		// 写入数据
		fwrite(g_Books, sizeof(struct Book) * g_BookIndex, 1, fp);
	}

	// 关闭文件
	fclose(fp);
}

// 从文件中恢复图书数据
void LoadBookDataFromFile()
{
	// 打开文件
	FILE* fp = fopen("book.data", "rb");
	
	// 如果文件不存在,初始化下一个可用的下标为0
	if (fp == 0)
	{
		g_BookIndex = 0;
		return;
	}
	
	// 读取下一个可用的下标
	fread(&g_BookIndex, sizeof(int), 1, fp);
	
	// 读取数据
	if (g_BookIndex != 0)
	{
		fread(g_Books, sizeof(struct Book) * g_BookIndex, 1, fp);
	}
	
	// 关闭文件
	fclose(fp);
}

// 保存学生数据到文件
void StoreStudentDataToFile()
{
	// 打开文件——注意，文件为二进制格式
	FILE* fp = fopen("student.data", "wb");

	// 写入下一个可用的下标
	fwrite(&g_StudentIndex, sizeof(int), 1, fp);

	if (g_StudentIndex != 0)
	{
		// 写入数据
		fwrite(g_Students, sizeof(struct Student) * g_StudentIndex, 1, fp);
	}

	// 关闭文件
	fclose(fp);
}

// 从文件恢复学生数据
void LoadStudentDataFromFile()
{
	// 打开文件
	FILE* fp = fopen("student.data", "rb");

	// 如果文件不存在,初始化下一个可用的下标为0
	if (fp == 0)
	{
		g_StudentIndex = 0;
		return;
	}

	// 读取下一个可用的下标
	fread(&g_StudentIndex, sizeof(int), 1, fp);

	// 读取数据
	if (g_StudentIndex != 0)
	{
		fread(g_Students, sizeof(struct Student) * g_StudentIndex, 1, fp);
	}

	// 关闭文件
	fclose(fp);
}

// 保存图书管理员数据到文件
void StoreLibrarianDataToFile()
{
	// 打开文件
	FILE* fp = fopen("librarian.data", "wb");

	// 写入下一个可用的下标
	fwrite(&g_LibrarianIndex, sizeof(int), 1, fp);

	if (g_LibrarianIndex != 0)
	{
		// 写入数据
		fwrite(g_librarians, sizeof(struct Librarian) * g_LibrarianIndex, 1, fp);
	}

	// 关闭文件
	fclose(fp);
}

// 从文件中恢复图书管理员数据
void LoadLibrarianDataFromFile()
{
	// 打开文件
	FILE* fp = fopen("librarian.data", "rb");

	// 如果文件不存在,初始化下一个可用的下标为0
	if (fp == 0)
	{
		g_LibrarianIndex = 0;
		return;
	}

	// 读取下一个可用的下标
	fread(&g_LibrarianIndex, sizeof(int), 1, fp);

	// 读取数据
	if (g_LibrarianIndex != 0)
	{
		fread(g_librarians, sizeof(struct Librarian) * g_LibrarianIndex, 1, fp);
	}

	// 关闭文件
	fclose(fp);
}

// 保存图书借阅数据到文件
void StortBorrowRecordDataToFile()
{
	// 打开文件
	FILE* fp = fopen("borrowRecord.data", "wb");

	// 写入下一个可用的下标
	fwrite(&g_BorrowRecordIndex, sizeof(int), 1, fp);

	if (g_BorrowRecordIndex != 0)
	{
		// 写入数据
		fwrite(g_BorrowRecords, sizeof(struct BorrowRecord) * g_BorrowRecordIndex, 1, fp);
	}

	// 关闭文件
	fclose(fp);
}

// 从文件中恢复图书借阅数据
void LoadBorrowRecordDataFromFile()
{
	// 打开文件
	FILE* fp = fopen("borrowRecord.data", "rb");

	// 如果文件不存在,初始化下一个可用的下标为0
	if (fp == 0)
	{
		g_BorrowRecordIndex = 0;
		return;
	}

	// 读取下一个可用的下标
	fread(&g_BorrowRecordIndex, sizeof(int), 1, fp);

	// 读取数据
	if (g_BorrowRecordIndex != 0)
	{
		fread(g_BorrowRecords, sizeof(struct BorrowRecord) * g_BorrowRecordIndex, 1, fp);
	}
}

// 从文件中回复所有相关数据
void LoadData()
{
	LoadBookDataFromFile();
	LoadBorrowRecordDataFromFile();
	LoadLibrarianDataFromFile();
	LoadStudentDataFromFile();
}


/*---------------- 检查输入合法性相关方法 ----------------*/

// 检查学号,工号是否有效
int IsValidId(const char* value)
{
	unsigned int i = 0;

	// 检查长度是否为10
	if (strlen(value) != 10)
	{
		return FALSE;
	}

	// 检查是否由数字组成
	for (i = 0; i < strlen(value); i++)
	{
		if (value[i] < '0' || value[i] > '9')
		{
			return FALSE;
		}
	}

	return TRUE;
}

// 检查性别有效性
int IsValidGender(const char* gender)
{
	if (strcmp(gender, "男") == 0 || strcmp(gender, "女") == 0)
	{
		return TRUE;
	}

	return FALSE;
}

// 检查密码有效性(密码由字母和数字组成不允许有特殊符号,最少6位数字)
int IsValidPassword(const char* password)
{
	int i = 0;

	// 检查长度是否大于等于6且小于等于16
	if (strlen(password) < 6 || strlen(password) > 16)
	{
		return FALSE;
	}

	// 检查是否只包含数字和字母
	for (i = 0; i < (int)strlen(password); i++)
	{
		if (!((password[i] >= 'a' && password[i] <= 'z') || (password[i] >= 'A' && password[i] <= 'Z') || (password[i] >= '0' && password[i] <= '9')))
		{
			return FALSE;
		}
	}

	return TRUE;
}

// 检查电话号码有效性
int IsValidPhone(const char* phone)
{
	int i = 0;

	// 检查电话长度是否为11
	if (strlen(phone) != 11)
	{
		return FALSE;
	}
		
	for (i = 0; i < (int)strlen(phone); i++)
	{
		if (phone[i] < '0' || phone[i] > '9')
		{
			return FALSE;
		}
	}

	return TRUE;
}

// 检查邮箱有效性
int IsValidEmail(const char* email)
{
	int i = 0;
	int j = 0;
	int k = 0;
	// 找到@符号
	while (email[i] != '\0' && email[i] != '@')
	{
		//检测.是否出现在@之前
		if (email[i] == '.')
		{
			printf("1\n");
			return FALSE;
		}
		i++;
	}
	j = i ;
	// 如果@后无内容或没有找到@
	if (email[i + 1] == '\0' || email[i] == '.')
	{
		printf("2\n");
		return FALSE;
	}
	// 找到@后的.符号
	while (email[i] != '\0' && email[i] != '.')
	{
		//检测@与.是否相邻
		if (email[j + 1] == '.')
		{
			printf("3\n");
			return FALSE;
		}
		//检测@之后.之前有无@
		if (i != j && email[i] == '@')
		{
			printf("4\n");
			return FALSE;
		}
		i++;
	}
	// 如果没有找到.或者.后无内容
	if (email[i + 1] == '\0'|| email[i] == '\0')
	{
		printf("5\n");
		return FALSE;
	}
	while (email[i + 1] != '\0')
	{
		//检测两个.是否相邻
		if (email[i] == '.')
		{
			if ((i - k) == 1)
			{
				printf("6\n");
				return FALSE;
			}
			k = i;
		}
		//检测.之后有无@
		if (email[i + 1] == '@')
		{
			printf("7\n");
			return FALSE;
		}
		i++;
	}
	return TRUE;
}

// 检查图书ID有效性
int IsValidBookID(const char* BookID)
{
	int i;

	// 检查长度
	if (strlen(BookID) != 6)
	{
		return FALSE;
	}

	// 检查是否由数字组成
	for (i = 0; i < (int)strlen(BookID); i++)
	{
		if (BookID[i] < '0' || BookID[i] > '9')
		{
			return FALSE;
		}
	}

	// 检查图书ID是否重复
	for (i = 0; i < g_BookIndex; i++)
	{
		if (0 == strcmp(g_Books[i].BookID, BookID))
		{
			return FALSE;
		}
	}

	return TRUE;
}

// 检查书架位置有效性
int IsValidBookPosition(int position) 
{
	int i;
	
	// 检查是否重复
	if (!(position > 100000 && position <= 999999))
	{
		return FALSE;
	}
	for (i = 0; i < g_BookIndex; i++)
	{
		if (g_Books[i].position == position)
		{
			return FALSE;
		}
	}

	return TRUE;
}

// 检查学生学号是否重复
int IsStudendIDRepeat(const char* StudentID)
{
	int i;

	// 检查是否重复
	for (i = 0; i < g_StudentIndex; i++)
	{
		if (strcmp(g_Students[i].StudentID, StudentID) == 0)
		{
			return FALSE;
		}
	}

	return TRUE;
}

// 检查图书管理员工号是否重复
int IsJobNumberRepeat(const char* JobNumber)
{
	int i;

	// 检查是否重复
	for (i = 0; i < g_LibrarianIndex; i++)
	{
		if (strcmp(g_librarians[i].JobNumber, JobNumber) == 0)
		{
			return FALSE;
		}
	}

	return TRUE;
}

// 检查年份的有效性
int IsValidYear(int year)
{
	if (year > 2020 || year < 1900)
	{
		return FALSE;
	}

	return TRUE;
}

// 检查月份有效性
int IsValidMonth(int month)
{
	if (month > 12 || month < 1)
	{
		return FALSE;
	}

	return TRUE;
}

// 检查日期有效性
int IsValidDay(int year, int month, int day)
{
	// 闰年中每个月的天数
	int leapYear[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	// 平年中每个月的天数
	int ordinaryYear[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	// 判断是平年还是闰年
	if ((year % 4 == 0 && year % 100 != 0) || (year % 100 == 0 && year % 400 == 0))
	{
		// 闰年
		if (day < 1 || day > leapYear[month - 1])
		{
			return FALSE;
		}
	}
	else
	{
		// 平年
		if (day < 1 || day > ordinaryYear[month - 1])
		{
			return FALSE;
		}
	}
	return TRUE;
}

/*---------------- 通用功能 ----------------*/
// 根据图书ID获取指定图书在数组中的下标,如果图书ID指定的图书不存在,返回-1
int GetBookIndexByID(const char* BookID)
{
	int i;

	for (i = 0; i < g_BookIndex; i++)
	{
		if (strcmp(g_Books[i].BookID, BookID) == 0)
		{
			return i;
		}
	}
	return -1;
}

// 输出指定下标的图书内容
void OutputBook(int index)
{
	int time = (int)g_Books[index].BorrowTotalTime;
	printf("图书ID:%s 书名:%s 作者:%s 出版社:%s 介绍:%s \n出版日期:%d年%d月%d日 价格:%.2f 书架位置:%d 总借出次数:%d 总借阅时长:%d",
		g_Books[index].BookID,
		g_Books[index].name,
		g_Books[index].author,
		g_Books[index].publisher,
		g_Books[index].blurb,
		g_Books[index].date.year,
		g_Books[index].date.month,
		g_Books[index].date.day,
		g_Books[index].price,
		g_Books[index].position,
		g_Books[index].BorrowTotalCount,
		time);

	if (g_Books[index].BorrowState == 1)
	{
		printf(" 借出状态:已借出\r\n\n");
	}
	else
	{
		printf(" 借出状态:未借出\r\n\n");
	}
}

// 输出指定下标的学生内容
void OutputStudent(int index)
{
	printf("学号:%s 姓名:%s 学院:%s 专业:%s 性别:%s 电话:%s 邮箱:%s 密码:%s",
		g_Students[index].StudentID,
		g_Students[index].name,
		g_Students[index].faculty,
		g_Students[index].major,
		g_Students[index].gender,
		g_Students[index].phone,
		g_Students[index].email,
		g_Students[index].password);

	if (g_Students[index].frozen == 0)
	{
		printf(" 账号状态:正常\r\n");
	}
	else
	{
		printf(" 账号状态:冻结\r\n");
	}
}

// 查找指定出版社的所有图书
void SelectBookByPublisher() 
{
	char buffer[256] = { 0 };
	int flag = FALSE;
	int i;

	if (g_BookIndex == 0)
	{
		printf("图书馆暂无图书,无法提供该操作!\r\n");
		return;
	}

	printf("请输入指定的出版社名称:");
	//将文件内部指针重新指向开头，清空缓存区
	rewind(stdin);
	scanf_s("%s", buffer, 256);

	for (i = 0; i < g_BookIndex; i++)
	{
		if (strcmp(g_Books[i].publisher, buffer) == 0)
		{
			OutputBook(i);
			flag = TRUE;
		}
	}

	if (flag == TRUE)
	{
		printf("查询完毕!\r\n");
	}
	else
	{
		printf("没有符合要求的查询结果!\r\n");
	}
}

// 查找指定名字的所有图书
void SelectBookByName()
{
	char buffer[256] = { 0 };
	int flag = FALSE;
	int i;

	if (g_BookIndex == 0)
	{
		printf("图书馆暂无图书,无法提供该操作!\r\n");
		return;
	}

	printf("请输入指定的图书名称:");
	//将文件内部指针重新指向开头，清空缓存区
	rewind(stdin);
	scanf_s("%s", buffer, 256);

	for (i = 0; i < g_BookIndex; i++)
	{
		if (strcmp(g_Books[i].name, buffer) == 0)
		{
			OutputBook(i);
			flag = TRUE;
		}
	}

	if (flag == TRUE)
	{
		printf("查询完毕!\r\n");
	}
	else
	{
		printf("没有符合要求的查询结果!\r\n");
	}
}

// 查询指定作者的图书
void SelectBookByAuthor() 
{
	char buffer[256] = { 0 };
	int flag = FALSE;
	int i;

	if (g_BookIndex == 0)
	{
		printf("图书馆暂无图书,无法提供该操作!\r\n");
		return;
	}

	printf("请输入指定的作者姓名:");
	//将文件内部指针重新指向开头，清空缓存区
	rewind(stdin);
	scanf_s("%s", buffer, 256);

	for (i = 0; i < g_BookIndex; i++)
	{
		if (strcmp(g_Books[i].author, buffer) == 0)
		{
			OutputBook(i);
			flag = TRUE;
		}
	}

	if (flag == TRUE)
	{
		printf("查询完毕!\r\n");
	}
	else
	{
		printf("没有符合要求的查询结果!\r\n");
	}
}

// 查询全部图书
void SelectBook()
{
	int i;

	if (g_BookIndex == 0)
	{
		printf("图书馆暂无图书,无法提供该操作!\r\n");
		return;
	}

	for (i = 0; i < g_BookIndex; i++)
	{
		OutputBook(i);
	}

	printf("查询完毕!\r\n");
}

// 获取指定学号学生在数组中的下标,如果找不到指定学号的学生返回-1
int GetStudentIndexByID(const char* StudentID)
{
	int i;

	for (i = 0; i < g_StudentIndex; i++)
	{
		if (strcmp(g_Students[i].StudentID, StudentID) == 0)
		{
			return i;
		}
	}
	return -1;
}

// 输出指定的借书记录
void OutputBorrowRecord(int index)
{
	//链表
	struct tm* tmTime = 0;
	tmTime = localtime(&(g_BorrowRecords[index].BorrowTime));

	printf("图书ID:%s 学号:%s 借书时间:%d-%d-%d %d:%d",
		g_BorrowRecords[index].BookID,
		g_BorrowRecords[index].StudentID,
		tmTime->tm_year + 1900,
		tmTime->tm_mon + 1,
		tmTime->tm_mday,
		tmTime->tm_hour,
		tmTime->tm_min);

	if (g_BorrowRecords[index].ReturnTime != 0)
	{
		tmTime = localtime(&(g_BorrowRecords[index].ReturnTime));
		printf(" 还书时间:%d-%d-%d %d:%d\r\n",
			tmTime->tm_year + 1900,
			tmTime->tm_mon + 1,
			tmTime->tm_mday,
			tmTime->tm_hour,
			tmTime->tm_min);
	}
	else
	{
		printf(" 还书时间:未归还\r\n");
	}
}

/*---------------- 学生功能 ----------------*/
// 学生注册
void StudentRegister()
{
	char buffer[256] = { 0 };

	// 检查数组长度
	if (g_StudentIndex == MAX_ARRAY_LEN)
	{
		printf("存储空间已满,无法继续注册!\r\n");
		return;
	}

	while (TRUE)
	{
		printf("请输入您的学号:");
		//将文件内部指针重新指向开头，清空缓存区
		rewind(stdin);
		scanf_s("%s", buffer, 256);

		if (IsValidId(buffer)==FALSE)
		{
			printf("学号格式不符合要求,请重新输入\r\n");
			continue;
		}

		if (IsStudendIDRepeat(buffer) == FALSE)
		{
			printf("学号重复,请重新输入\r\n");
			continue;
		}

		strcpy_s(g_Students[g_StudentIndex].StudentID, 11, buffer);
		break;
	}
	
	printf("请输入您的院系:");
	//将文件内部指针重新指向开头，清空缓存区
	rewind(stdin);
	scanf_s("%s", g_Students[g_StudentIndex].faculty, 19);

	printf("请输入您的专业:");
	//将文件内部指针重新指向开头，清空缓存区
	rewind(stdin);
	scanf_s("%s", g_Students[g_StudentIndex].major, 19);

	printf("请输入您的姓名:");
	//将文件内部指针重新指向开头，清空缓存区
	rewind(stdin);
	scanf_s("%s", g_Students[g_StudentIndex].name, 15);

	while (TRUE)
	{
		printf("请输入您的性别(男/女):");
		//将文件内部指针重新指向开头，清空缓存区
		rewind(stdin);
		scanf_s("%s", buffer, 256);

		if (IsValidGender(buffer) == FALSE)
		{
			printf("性别格式错误,请输入“男”或“女”\r\n");
			continue;
		}

		strcpy_s(g_Students[g_StudentIndex].gender, 8, buffer);
		break;
	}

	while (TRUE)
	{
		printf("请输入您的电话:");
		//将文件内部指针重新指向开头，清空缓存区
		rewind(stdin);
		scanf_s("%s", buffer, 256);

		if (IsValidPhone(buffer) == FALSE)
		{
			printf("电话格式错误,请重新输入\r\n");
			continue;
		}

		strcpy_s(g_Students[g_StudentIndex].phone, 12, buffer);
		break;
	}

	while (TRUE)
	{
		printf("请输入您的密码(数字或英文字母组成,6位到16位):");
		rewind(stdin);
		scanf_s("%s", buffer, 256);

		if (IsValidPassword(buffer) == FALSE)
		{
			printf("密码格式错误,请重新输入\r\n");
			continue;
		}

		strcpy_s(g_Students[g_StudentIndex].password, 20, buffer);
		break;
	}

	while (TRUE)
	{
		printf("请输入您的邮箱:");
		//将文件内部指针重新指向开头，清空缓存区
		rewind(stdin);
		scanf_s("%s", buffer, 256);

		if (IsValidEmail(buffer) == FALSE)
		{
			printf("邮箱格式错误,请重新输入\r\n");
			continue;
		}

		strcpy_s(g_Students[g_StudentIndex].email, 40, buffer);
		break;
	}

	// 账号状态设置为正常
	g_Students[g_StudentIndex].frozen = 0;
	// 下一个可用位置的下标递增
	g_StudentIndex++;

	// 保存数据到文件
	StoreStudentDataToFile();
	printf("账号注册成功!\r\n");
}

// 学生登陆,登录成功返回TRUE,失败返回FALSE
int StudentLogin()
{
	int i;
	char StudentID[256] = { 0 };  
	char password[256] = { 0 };   
	printf("请输入您的学号:");
	rewind(stdin);
	scanf_s("%s", StudentID, 256);
	printf("请输入您的密码:");
	rewind(stdin);
	scanf_s("%s", password, 256);

	for (i = 0; i < g_StudentIndex; i++)
	{
		if (strcmp(g_Students[i].StudentID, StudentID) == 0 && strcmp(g_Students[i].password, password) == 0)
		{
			// 设置当前登录学生在数组中的下标
			g_LoginStudentIndex = i;
			return TRUE;
		}
	}

	return FALSE;
}

// 根据价格降序排列所有图书
void SortBookByPrice()
{
	int i;
	int j;
	Book book;

	if (g_BookIndex == 0)
	{
		printf("图书馆暂无图书,无法提供该操作!\r\n");
		return;
	}

	// 排序
	for (i = 0; i < g_BookIndex - 1; i++)
	{
		for (j = i + 1; j < g_BookIndex; j++)
		{
			if (g_Books[i].price < g_Books[j].price)
			{
				book = g_Books[i];
				g_Books[i] = g_Books[j];
				g_Books[j] = book;
			}
		}
	}

	// 输出
	for (i = 0; i < g_BookIndex; i++)
	{
		OutputBook(i);
	}
}

// 根据总借阅次数降序排列所有图书
void SortBookByBorrowTotalCount()
{
	int i;
	int j;
	Book book;

	if (g_BookIndex == 0)
	{
		printf("图书馆暂无图书,无法提供该操作!\r\n");
		return;
	}

	// 排序
	for (i = 0; i < g_BookIndex - 1; i++)
	{
		for (j = i + 1; j < g_BookIndex; j++)
		{
			if (g_Books[i].BorrowTotalCount < g_Books[j].BorrowTotalCount)
			{
				book = g_Books[i];
				g_Books[i] = g_Books[j];
				g_Books[j] = book;
			}
		}
	}

	// 输出
	for (i = 0; i < g_BookIndex; i++)
	{
		OutputBook(i);
	}
}

// 修改学生邮箱
void ModifyStudentEmail()
{
	char buffer[256] = { 0 };

	while (TRUE)
	{
		printf("请输入新的邮箱:");
		rewind(stdin);
		scanf_s("%s", buffer, 256);

		if (IsValidEmail(buffer) == FALSE)
		{
			printf("邮箱格式不正确,请重新输入\r\n");
			continue;
		}

		strcpy_s(g_Students[g_LoginStudentIndex].email, 20, buffer);
		break;
	}

	// 保存到文件
	StoreStudentDataToFile();
	printf("学生邮箱修改成功!\r\n");
}

// 修改学生电话
void ModifyStudentPhone()
{
	char buffer[256] = { 0 };

	while (TRUE)
	{
		printf("请输入新的电话号码:");
		rewind(stdin);
		scanf_s("%s", buffer, 256);

		if (IsValidPhone(buffer) == FALSE)
		{
			printf("电话号码格式不正确,请重新输入\r\n");
			continue;
		}

		strcpy_s(g_Students[g_LoginStudentIndex].phone, 12, buffer);
		break;
	}

	// 保存到文件
	StoreStudentDataToFile();
	printf("学生电话号码修改成功!\r\n");
}

// 修改学生密码
void ModifyStudentPassword()
{
	char buffer[256] = { 0 };

	while (TRUE)
	{
		printf("请输入新的密码:");
		rewind(stdin);
		scanf_s("%s", buffer, 256);

		if (IsValidPassword(buffer) == FALSE)
		{
			printf("密码格式不正确,请重新输入\r\n");
			continue;
		}

		strcpy_s(g_Students[g_LoginStudentIndex].password, 20, buffer);
		break;
	}

	// 保存到文件
	StoreStudentDataToFile();
	printf("学生密码修改成功!\r\n");
}

//显示学生信息
void ShowStudentInfo()
{
	printf("姓名:%s\t学号:%s\t性别:%s\t院系:%s\t专业:%s\t\n电话:%s\t邮箱:%s\t\n",
		g_Students[g_LoginStudentIndex].name,
		g_Students[g_LoginStudentIndex].StudentID,
		g_Students[g_LoginStudentIndex].gender,
		g_Students[g_LoginStudentIndex].faculty,
		g_Students[g_LoginStudentIndex].major,
		g_Students[g_LoginStudentIndex].phone,
		g_Students[g_LoginStudentIndex].email);
	if (g_Students[g_LoginStudentIndex].frozen == 0)
	{
		printf(" 账号状态:正常\r\n");
	}
	else
	{
		printf(" 账号状态:冻结\r\n");
	}
}

// 查询个人借阅记录
void SelectLoginStudentBorrowRecord()
{
	int i;

	for (i = 0; i < g_BorrowRecordIndex; i++)
	{
		if (strcmp(g_BorrowRecords[i].StudentID, g_Students[g_LoginStudentIndex].StudentID) == 0)
		{
			OutputBorrowRecord(i);
		}
	}

	printf("个人借阅信息查询完毕!\r\n");
}

// 选择准备借阅的图书
void AddBookToCart()
{
	char BookID[256] = { 0 };
	int BookIndex = -1;
	int i = 0;
	int count = 0;

	// 检查学生账号冻结状态
	if (g_Students[g_LoginStudentIndex].frozen == 1)
	{
		printf("您的账号已冻结.请解除冻结状态后再挑选图书!\r\n");
		return;
	}

	// 统计未还图书数量
	for (i = 0; i < g_BorrowRecordIndex; i++)
	{
		if (0 == strcmp(g_BorrowRecords[i].StudentID, g_Students[g_LoginStudentIndex].StudentID) && g_BorrowRecords[i].ReturnTime == 0)
		{
			count++;
		}
	}

	// 未还图书数量加上已挑选的图书的数量总数不能超过3本
	if (count + g_CartCount >= 3)
	{
		printf("未还图书数量:%d 已挑选图书数量%d 达到3本上限,不能继续借书了!\r\n", count, g_CartCount);
		return;
	}

	// 获取图书ID
	printf("请输入准备借阅图书的ID:");
	rewind(stdin);
	scanf_s("%s", BookID, 256);

	// 获取指定图书的下标
	BookIndex = GetBookIndexByID(BookID);

	if (-1 == BookIndex)
	{
		printf("没有图书ID = %s 的图书!\r\n", BookID);
		return;
	}

	// 检查图书是否已经借出
	if (g_Books[BookIndex].BorrowState == 1)
	{
		printf("图书ID = %s 的图书已借出!\r\n", BookID);
		return;
	}

	// 检查图书是否已在借书购物车中
	for (i = 0; i < g_CartCount; i++)
	{
		if (0 == strcmp(g_BorrowCart[i].BookID, g_Books[BookIndex].BookID))
		{
			printf("图书ID = %s 的图书您已经在您的待借清单中了!\r\n", BookID);
			return;
		}
	}

	// 添加图书到待借购物车
	g_BorrowCart[g_CartCount] = g_Books[BookIndex];
	// 待借图书数量递增
	g_CartCount++;
	printf("待借图书添加成功!\r\n");
}

// 删除待借购物车中的图书
void DeleteBookFromCart()
{
	int i = 0;
	int j = 0;
	char BookID[256] = { 0 };

	if (g_CartCount == 0)
	{
		printf("您当前没有待借图书!\r\n");
		return;
	}

	printf("请输入要在待借购物车中删除的图书的图书ID:");
	rewind(stdin);
	scanf_s("%s", BookID, 256);

	for (i = 0; i < g_CartCount; i++)
	{
		if (0 == strcmp(g_BorrowCart[i].BookID, BookID))
		{
			// 被删除节点后边的节点前移
			for (j = i; j < g_CartCount - 1; j++)
			{
				g_BorrowCart[j] = g_BorrowCart[j + 1];
			}

			g_CartCount--;
			printf("图书移除完毕!\r\n");
			return;
		}
	}

	printf("您的待借购物车中没有这本图书!\r\n");
}

// 借书
void BorrowBook()
{
	int i = 0;
	int BookIndex = 0;
	int count = 0;

	// 检查待借图书清单
	if (g_CartCount == 0)
	{
		printf("您的待借图书购物车为空,请先挑选要借阅的图书!\r\n");
		return;
	}

	// 添加借书记录
	for (i = 0; i < g_CartCount; i++)
	{
		// 学生ID
		strcpy(g_BorrowRecords[g_BorrowRecordIndex].StudentID, g_Students[g_LoginStudentIndex].StudentID);
		// 图书ID
		strcpy(g_BorrowRecords[g_BorrowRecordIndex].BookID, g_BorrowCart[i].BookID);
		// 借书时间
		time(&(g_BorrowRecords[g_BorrowRecordIndex].BorrowTime));
		
		// 获取图书下标
		BookIndex = GetBookIndexByID(g_BorrowCart[i].BookID);

		if (-1 != BookIndex)
		{
			// 修改图书借出状态
			g_Books[BookIndex].BorrowState = 1;

			// 修改图书借出次数
			g_Books[BookIndex].BorrowTotalCount++;
		}
		
		// 借书记录下标递增
		g_BorrowRecordIndex++;
	}

	// 保存记录到文件
	StortBorrowRecordDataToFile();
	StoreBookDataToFile();

	// 清空待借购物车
	g_CartCount = 0;
	printf("借阅成功!\r\n");
}

// 还书
void ReturnBook()
{
	char BookID[256] = { 0 };
	int BorrowRecordIndex = -1;
	int BookIndex = -1;
	int i;
	
	printf("请输入要还的图书的ID:");
	rewind(stdin);
	scanf_s("%s", BookID, 256);

	// 查找借书记录
	for (i = 0; i < g_BorrowRecordIndex; i++)
	{
		if (0 == strcmp(g_BorrowRecords[i].BookID, BookID) && g_BorrowRecords[i].ReturnTime == 0)
		{
			BorrowRecordIndex = i;
			break;
		}
	}

	if (-1 == BorrowRecordIndex)
	{
		printf("你的未还图书中不包括这本书!\r\n");
		return;
	}

	// 设置还书时间
	time(&(g_BorrowRecords[BorrowRecordIndex].ReturnTime));

	// 获取图书节点
	BookIndex = GetBookIndexByID(g_BorrowRecords[BorrowRecordIndex].BookID);

	if (-1 != BookIndex)
	{
		// 设置图书是否借出的状态
		g_Books[BookIndex].BorrowState = 0;
	}

	// 保存数据到文件
	StortBorrowRecordDataToFile();
	StoreBookDataToFile();

	printf("图书ID为%s的图书已归还!\r\n", BookID);
}

// 续借
void RenewBook()
{
	time_t CurrentTime = 0;
	time_t diff = 0;
	char BookID[256] = { 0 };
	int BorrowRecordIndex = -1;
	int BookIndex = -1;
	int i;
	int day;

	printf("请输入续借的图书的ID:");
	rewind(stdin);
	scanf_s("%s", BookID, 256);

	// 查找借书记录
	for (i = 0; i < g_BorrowRecordIndex; i++)
	{
		if (strcmp(g_BorrowRecords[i].BookID, BookID) == 0 && g_BorrowRecords[i].ReturnTime == 0)
		{
			BorrowRecordIndex = i;
			break;
		}
	}

	if (BorrowRecordIndex == -1)
	{
		printf("你的未还借阅记录中没有这本书!\r\n");
		return;
	}

	// 判断续借次数
	if (g_BorrowRecords[BorrowRecordIndex].RenewCount >= 2)
	{
		printf("续借次数已经达到上限,无法再次续借!\r\n");
		return;
	}

	// 获取当前时间
	time(&CurrentTime);

	// 获取时间差,单位是秒
	diff = CurrentTime - g_BorrowRecords[BorrowRecordIndex].BorrowTime;
	// 秒换算成天
	day = (int)(diff / 60 / 60 / 24);
	
	// 判断时间
	if (day > (g_BorrowRecords[BorrowRecordIndex].RenewCount + 1) * 30 + 30)
	{
		printf("图书借阅时间已经超出最高限制,无法续借!\r\n");
		return;
	}

	// 续借
	g_BorrowRecords[BorrowRecordIndex].RenewCount++;

	// 保存到文件
	StortBorrowRecordDataToFile();

	printf("图书ID:%s 续借成功!\r\n", g_BorrowRecords[BorrowRecordIndex].BookID);
}

/*---------------- 图书管理员功能 ----------------*/
// 图书管理员注册
void LibrarianRegister()
{
	char buffer[256] = { 0 };
	
	// 检查数组长度
	if (g_LibrarianIndex == MAX_ARRAY_LEN)
	{
		printf("存储空间已满,无法继续注册!\r\n");
		return;
	}

	while (TRUE)
	{
		printf("请输入您的工号:");
		rewind(stdin);
		scanf_s("%s", buffer, 256);

		if (IsValidId(buffer) == FALSE)
		{
			printf("工号格式不正确,请重新输入\r\n");
			continue;
		}

		if (IsJobNumberRepeat(buffer) == FALSE)
		{
			printf("工号重复,请重新输入\r\n");
			continue;
		}

		strcpy_s(g_librarians[g_LibrarianIndex].JobNumber, 11, buffer);
		break;
	}


	printf("请输入您的姓名:");
	rewind(stdin);
	scanf_s("%s", g_librarians[g_LibrarianIndex].name, 15);

	while (TRUE)
	{
		printf("请输入您的密码(6到16位):");
		rewind(stdin);
		scanf_s("%s", buffer, 256);

		if (IsValidPassword(buffer) == FALSE)
		{
			printf("密码格式不正确,请重新输入\r\n");
			continue;
		}

		strcpy_s(g_librarians[g_LibrarianIndex].password, 20, buffer);
		break;
	}

	while (TRUE)
	{
		printf("请输入您的邮箱:");
		rewind(stdin);
		scanf_s("%s", buffer, 256);

		if (IsValidEmail(buffer) == FALSE)
		{
			printf("邮箱格式不正确,请重新输入\r\n");
			continue;
		}

		strcpy_s(g_librarians[g_LibrarianIndex].email, 40, buffer);
		break;
	}
	
	// 下一个可用位置的下标递增
	g_LibrarianIndex++;

	// 保存数据到文件
	StoreLibrarianDataToFile();
	printf("账号注册成功!\r\n");
}

// 图书管理员登陆,登录成功返回TRUE,失败返回FALSE
int LibrarianLogin()
{
	int i;
	char JobNumber[256] = { 0 };    
	char password[256] = { 0 }; 
	printf("请输入您的工号:");
	rewind(stdin);
	scanf_s("%s", JobNumber, 256);
	printf("请输入您的密码:");
	rewind(stdin);
	scanf_s("%s", password, 256);

	for (i = 0; i < g_LibrarianIndex; i++)
	{
		if (strcmp(g_librarians[i].JobNumber, JobNumber) == 0 && strcmp(g_librarians[i].password, password) == 0)
		{
			g_LoginLibrarianIndex = i;
			return TRUE;
		}
	}

	return FALSE;
}

// 添加图书
void AddBook() 
{

	char buffer[256] = { 0 };
	
	// 检查数组长度
	if (g_BookIndex == MAX_ARRAY_LEN)
	{
		printf("存储空间已满,无法继续注册!\r\n");
		return;
	}

	while (TRUE)
	{
		printf("请输入图书ID:");
		rewind(stdin);
		scanf_s("%s", buffer, 256);

		if (IsValidBookID(buffer) == FALSE)
		{
			printf("图书ID格式不正确或重复,请重新输入\r\n");
			continue;
		}

		strcpy_s(g_Books[g_BookIndex].BookID, 7, buffer);
		break;
	}


	printf("请输入书名:");
	rewind(stdin);
	scanf_s("%s", g_Books[g_BookIndex].name, 32);

	while (TRUE)
	{
		printf("请输入书架位置(六位全数字组成):");
		rewind(stdin);
		scanf_s("%d", &(g_Books[g_BookIndex].position));

		if (IsValidBookPosition(g_Books[g_BookIndex].position) == FALSE)
		{
			printf("书架位置重复或错误,请重新输入\r\n");
			continue;
		}
		break;
	}


	printf("请输入作者名:");
	rewind(stdin);
	scanf_s("%s", g_Books[g_BookIndex].author, 40);

	printf("请输入出版社:");
	rewind(stdin);
	scanf_s("%s", g_Books[g_BookIndex].publisher, 40);

	printf("请输入图书简介:");
	rewind(stdin);
	scanf_s("%s", g_Books[g_BookIndex].blurb, 100);

	printf("请输入图书价格:");
	rewind(stdin);
	scanf_s("%f", &(g_Books[g_BookIndex].price));

	while (TRUE)
	{
		printf("请输入出版年份:");
		rewind(stdin);
		scanf_s("%d", &(g_Books[g_BookIndex].date.year));

		if (IsValidYear(g_Books[g_BookIndex].date.year) == FALSE)
		{
			printf("年份格式错误,请重新输入\r\n");
			continue;
		}
		break;
	}

	while (TRUE)
	{
		printf("请输入出版月份:");
		rewind(stdin);
		scanf_s("%d", &(g_Books[g_BookIndex].date.month));

		if (IsValidMonth(g_Books[g_BookIndex].date.month) == FALSE)
		{
			printf("月份格式错误,请重新输入\r\n");
			continue;
		}
		break;
	}

	while (TRUE)
	{
		printf("请输入出版日期:");
		rewind(stdin);
		scanf_s("%d", &(g_Books[g_BookIndex].date.day));

		if (IsValidDay(g_Books[g_BookIndex].date.year, g_Books[g_BookIndex].date.month, g_Books[g_BookIndex].date.day) == FALSE)
		{
			printf("日期格式错误,请重新输入\r\n");
			continue;
		}
		break;
	}

	g_Books[g_BookIndex].BorrowTotalCount = 0;
	g_Books[g_BookIndex].BorrowState = 0;
	g_Books[g_BookIndex].BorrowTotalTime = 0;
	g_BookIndex++;

	// 保存到文件
	StoreBookDataToFile();
	printf("图书添加成功!\r\n");
}

// 修改图书简介
void ModifyBookBlurb(int index)
{
	printf("请输入新的图书介绍(50字以内）:\r\n");
	scanf_s("%s", g_Books[index].blurb, 100);

	// 保存修改
	StoreBookDataToFile();
	printf("图书介绍修改成功\r\n");
}

// 修改图书位置
void ModifyBookPosition(int index) {
	int NewPosition;

	while (true)
	{
		printf("输入要修改到的位置:");
		rewind(stdin);
		scanf_s("%d", &NewPosition);

		if (!IsValidBookPosition(NewPosition))
		{
			printf("该位置已经有图书或位置错误,请重新输入\r\n");
			continue;
		}

		g_Books[index].position = NewPosition;
		break;
	}

	// 保存修改到文件
	StoreBookDataToFile();
	printf("图书位置修改成功!");
}

// 修改图书
void ModifyBook()
{
	int ModifyIndex = -1;
	char buffer[256] = { 0 };
	int operate = 0;

	if (g_BookIndex == 0)
	{
		printf("图书馆暂无图书,无法提供该操作!\r\n");
		return;
	}

	printf("请输入要修改图书的ID:");
	rewind(stdin);
	scanf_s("%s", buffer, 256);

	// 获取图书下标
	ModifyIndex = GetBookIndexByID(buffer);

	if (ModifyIndex == -1)
	{
		printf("ID为%s的图书不存在\r\n", buffer);
		return;
	}

	// 检查该图书是否已借出
	if (g_Books[ModifyIndex].BorrowState == 1)
	{
		printf("该图书已借出,无法执行修改操作\r\n");
		return;
	}

	printf("请选择要修改的内容:\r\n");
	printf("1 修改图书简介\r\n");
	printf("2 修改图书的存放位置\r\n");

	// 获取用户选择
	rewind(stdin);
	(void)scanf("%d", &operate);

	if (operate == 1)
	{
		// 修改图书简介
		ModifyBookBlurb(ModifyIndex);
	}
	else if (operate == 2)
	{
		// 修改图书的存放位置
		ModifyBookPosition(ModifyIndex);
	}
}

// 删除图书
void DeleteBook()
{
	char buffer[256] = { 0 };
	int i;
	int BookIndex = -1;

	if (g_BookIndex == 0)
	{
		printf("图书馆暂无图书,无法提供该操作!\r\n");
		return;
	}


	printf("请输入要删除图书的ID:");
	rewind(stdin);
	scanf_s("%s", buffer, 256);

	// 获取图书下标
	BookIndex = GetBookIndexByID(buffer);

	if (BookIndex == -1)
	{
		printf("ID为%s的图书不存在\r\n", buffer);
		return;
	}
	
	// 检查该图书是否已借出
	if (g_Books[BookIndex].BorrowState == 1)
	{
		printf("该图书已借出,无法执行删除操作\r\n");
		return;
	}

	// 删除图书
	for (i = BookIndex; i < g_BookIndex - 1; i++)
	{
		g_Books[i] = g_Books[i + 1];
	}

	// 递减全局下标
	g_BookIndex--;

	// 更新文件
	StoreBookDataToFile();
	printf("编号为%s的图书删除成功\r\n", buffer);
}

// 修改图书管理员密码
void ModifyLibrarianPassword()
{
	char buffer[256] = { 0 };

	while (TRUE)
	{
		printf("请输入新的密码:");
		rewind(stdin);
		scanf_s("%s", buffer, 256);

		if (IsValidPassword(buffer) == FALSE)
		{
			printf("密码格式不正确,请重新输入\r\n");
			continue;
		}

		strcpy_s(g_librarians[g_LoginLibrarianIndex].password, 20, buffer);
		break;
	}

	// 保存到文件
	StoreLibrarianDataToFile();
	printf("图书管理员密码修改成功!\r\n");
}

// 修改图书管理员邮箱
void ModifyLibrarianEmail()
{
	char buffer[256] = { 0 };

	while (TRUE)
	{
		printf("请输入新的邮箱:");
		rewind(stdin);
		scanf_s("%s", buffer, 256);

		if (IsValidEmail(buffer) == FALSE)
		{
			printf("邮箱格式不正确,请重新输入\r\n");
			continue;
		}

		strcpy_s(g_librarians[g_LoginLibrarianIndex].email, 40, buffer);
		break;
	}

	// 保存到文件
	StoreLibrarianDataToFile();
	printf("图书管理员邮箱修改成功!\r\n");
}

void ShowLibrarianInfo()
{
	printf("姓名:%s\t工号:%s\t邮箱:%s\n", g_librarians[g_LoginLibrarianIndex].name,g_librarians[g_LoginLibrarianIndex].JobNumber, g_librarians[g_LoginLibrarianIndex].email);
}

// 查询超期未还学生清单
void SelectOverdueStudent()
{
	time_t CurrentTime = 0;
	time_t diff = 0;
	int day = 0;
	int StudentIndex = -1;
	int i;

	// 获取当前时间
	time(&CurrentTime);

	for (i = 0; i < g_BorrowRecordIndex; i++)
	{
		// 检查未归还图书
		if (g_BorrowRecords[i].ReturnTime == 0)
		{
			// 计算时间差(单位为秒)
			diff = CurrentTime - g_BorrowRecords[i].BorrowTime;
			// 秒换算成天
			day = (int)(diff / 60 / 60 / 24);
			// 逾期检查
			if (day > (g_BorrowRecords[i].RenewCount * 30) + 30)
			{
				// 获取学生下标
				StudentIndex = GetStudentIndexByID(g_BorrowRecords[i].StudentID);

				if (StudentIndex != -1)
				{
					// 输出学生信息
					OutputStudent(StudentIndex);
				}
			}
		}
	}

	printf("逾期未还学生清单查询完毕!\r\n");
}

// 查询超期未还得图书清单
void SelectOverdueBook()
{
	time_t CurrentTime = 0;
	time_t diff = 0;
	int day = 0;
	int BookID = -1;
	int i;

	// 获取当前时间
	time(&CurrentTime);

	for (i = 0; i < g_BorrowRecordIndex; i++)
	{
		// 检查未归还图书
		if (g_BorrowRecords[i].ReturnTime == 0)
		{
			// 计算时间差(单位为秒)
			diff = CurrentTime - g_BorrowRecords[i].BorrowTime;
			// 秒换算成天
			day = (int)(diff / 60 / 60 / 24);
			// 逾期检查
			if (day > (g_BorrowRecords[i].RenewCount * 30) + 30)
			{
				// 获取图书下标
				BookID = GetBookIndexByID(g_BorrowRecords[i].BookID);

				if (BookID != -1)
				{
					// 输出图书信息
					OutputBook(BookID);
				}
			}
		}
	}

	printf("逾期未还图书清单查询完毕!\r\n");
}

// 根据书名查询借阅信息
void SelectBorrowRecordByBookName()
{
	char buffer[256] = { 0 };
	int i; 
	int BookIndex = -1;

	printf("请输入书名:");
	rewind(stdin);
	scanf_s("%s", buffer, 256);

	// 获取图书下标
	for (i = 0; i < g_BookIndex; i++)
	{
		if (strcmp(g_Books[i].name, buffer) == 0)
		{
			BookIndex = i;
			break;
		}
	}

	if (BookIndex == -1)
	{
		printf("图书馆中没有书名为%s的图书!\r\n", buffer);
		return;
	}

	// 查找记录
	for (i = 0; i < g_BorrowRecordIndex; i++)
	{
		if (strcmp(g_BorrowRecords[i].BookID, g_Books[BookIndex].BookID) == 0)
		{
			// 输出借阅记录
			OutputBorrowRecord(i);
		}
	}
	
	printf("书名:%s的借阅记录查询完毕!\r\n", buffer);
}

// 根据学生名查询借阅信息
void SelectBorrowRecordByStudentName()
{
	char buffer[256] = { 0 };
	int i;
	int StudentIndex = -1;

	printf("请输入学生姓名:");
	rewind(stdin);
	scanf_s("%s", buffer, 256);

	// 获取学生下表
	for (i = 0; i < g_StudentIndex; i++)
	{
		if (strcmp(g_Students[i].name, buffer) == 0)
		{
			StudentIndex = i;
			break;
		}
	}

	if (StudentIndex == -1)
	{
		printf("没有名字为%s的学生!\r\n", buffer);
		return;
	}

	// 查找记录
	for (i = 0; i < g_BorrowRecordIndex; i++)
	{
		if (strcmp(g_BorrowRecords[i].StudentID, g_Students[StudentIndex].StudentID) == 0)
		{
			// 输出借阅记录
			OutputBorrowRecord(i);
		}
	}

	printf("名字:%s的学生借阅记录查询完毕!\r\n", buffer);
}

// 查询所有图书借阅信息(借出数量,图书馆图书数量,在馆数量)
void SelectBookBorrowInfo()
{
	int total = 0;
	int RemainCount = 0;
	int i;

	for (i = 0; i < g_BookIndex; i++)
	{
		total++;

		if (g_Books[i].BorrowState - 1)
		{
			RemainCount++;
		}
	}

	printf("图书馆共有藏书%d本;当前未借出在馆%d本;借出未还%d本\r\n", total, RemainCount, total - RemainCount);
}

// 按借阅次数排序所有在馆图书
void SortBookByBorrowCount()
{
	int i = 0;
	int j = 0;
	Book TempBook[MAX_ARRAY_LEN] = { 0 };
	int CurrentBookCount = 0;
	Book temp;

	// 获取所有在馆图书
	for (i = 0; i < g_BookIndex; i++)
	{
		if (g_Books[i].BorrowState == 0)
		{
			TempBook[CurrentBookCount++] = g_Books[i];
		}
	}

	// 按借阅次数降序排序
	for (i = 0; i < CurrentBookCount - 1; i++)
	{
		for (j = i + 1; j < CurrentBookCount; j++)
		{
			if (TempBook[i].BorrowTotalCount < TempBook[j].BorrowTotalCount)
			{
				temp = TempBook[i];
				TempBook[i] = TempBook[j];
				TempBook[j] = temp;
			}
		}
	}

	for (i = 0; i < CurrentBookCount; i++)
	{
		int time = (int)TempBook[i].BorrowTotalTime;
		printf("图书ID:%s 书名:%s 作者:%s 出版社:%s 介绍:%s \n出版日期:%d年%d月%d日 价格:%.2f 书架位置:%d 总借出次数:%d 总借阅时长:%d",
			TempBook[i].BookID,
			TempBook[i].name,
			TempBook[i].author,
			TempBook[i].publisher,
			TempBook[i].blurb,
			TempBook[i].date.year,
			TempBook[i].date.month,
			TempBook[i].date.day,
			TempBook[i].price,
			TempBook[i].position,
			TempBook[i].BorrowTotalCount,
			time);

		if (TempBook[i].BorrowState == 1)
		{
			printf(" 借出状态:已借出\r\n\n");
		}
		else
		{
			printf(" 借出状态:未借出\r\n\n");
		}
	}
}

// 按借阅时间长降序排序所有图书
void SortBookByBorrowTime()
{
	int i;
	int j;
	int BookIndex = -1;
	Book temp;

	// 初始化所有图书的借阅时长
	for (i = 0; i < g_BookIndex; i++)
	{
		g_Books[i].BorrowTotalTime = 0;
	}

	// 查找借阅记录,计算所有已归还的记录的借阅时长
	for (i = 0; i < g_BorrowRecordIndex; i++)
	{
		if (g_BorrowRecords[i].ReturnTime != 0)
		{
			// 获取图书下标
			BookIndex = GetBookIndexByID(g_BorrowRecords[i].BookID);

			if (BookIndex != -1)
			{
				// 递增借阅时间
				g_Books[BookIndex].BorrowTotalTime += (g_BorrowRecords[i].ReturnTime - g_BorrowRecords[i].BorrowTime);
			}
		}
	}

	// 排序
	for (i = 0; i < g_BookIndex - 1; i++)
	{
		for (j = i + 1; j < g_BookIndex; j++)
		{
			if (g_Books[i].BorrowTotalTime < g_Books[j].BorrowTotalTime)
			{
				temp = g_Books[i];
				g_Books[i] = g_Books[j];
				g_Books[j] = temp;
			}
		}
	}

	// 显示
	for (i = 0; i < g_BookIndex; i++)
	{
		OutputBook(i);
	}
}

// 冻结超期未还的学生账号
void FreezeOverdueStudent()
{
	time_t CurrentTime = 0;
	time_t diff = 0;
	int StudentIndex = 1;
	int day = 0;
	int i;
	// 获取当前时间
	time(&CurrentTime);

	for (i = 0; i < g_BorrowRecordIndex; i++)
	{
		// 检查图书是否尚未归还
		if (g_BorrowRecords[i].ReturnTime == 0)
		{
			// 获取时间差
			diff = CurrentTime - g_BorrowRecords[i].BorrowTime;
			// 计算天数
			day = (int)(diff / 60 / 60 / 24);
			// 检查是否超期
			if (day > (g_BorrowRecords[i].RenewCount * 30) + 30)
			{
				// 获取学生下标
				StudentIndex = GetStudentIndexByID(g_BorrowRecords[i].StudentID);

				if (StudentIndex == -1)
				{
					// 冻结账号
					g_Students[StudentIndex].frozen = 1;
					printf("学号:%s 姓名:%s 的账号预期被冻结\r\n", g_Students[StudentIndex].StudentID, g_Students[StudentIndex].name);
				}
			}
		}
	}

	// 更新文件
	StoreStudentDataToFile();
	printf("冻结操作执行完毕!\r\n");
}

// 解冻所有符合要求的学生账号
void DefreezeStudent()
{
	time_t CurrentTime = 0;
	time_t diff = 0;
	int day = 0;
	int i;
	int j;
	int flag = 0;

	// 获取当前时间
	time(&CurrentTime);

	for (i = 0; i < g_StudentIndex; i++)
	{
		flag = 0;

		// 检查账号是否处于冻结状态
		if (g_Students[i].frozen == 1)
		{
			// 遍历所有图书借阅记录
			for (j = 0; j < g_BorrowRecordIndex; j++)
			{
				if (strcmp(g_BorrowRecords[j].StudentID, g_Students[i].StudentID) == 0 && g_BorrowRecords[j].ReturnTime == 0)
				{
					// 获取时间差,单位为秒
					diff = CurrentTime - g_BorrowRecords[j].BorrowTime;
					// 秒换算成天
					day = (int)(diff / 60 / 60 / 24);
					// 检查是否超时
					if (day > (g_BorrowRecords[j].RenewCount * 30) + 30)
					{
						// 超时
						flag = 1;
						break;
					}
				}
			}

			if (flag == 0)
			{
				// 如果不存在逾期情况,解除该账号冻结状态
				g_Students[i].frozen = 0;
			}
		}
	}

	// 更新文件
	StoreBookDataToFile();
	printf("解冻操作执行完毕!\r\n");
}

// 按专业统计该专业最喜欢的图书
void CountBookByMajor()
{
	int StudentIndex = -1;
	int BookIndex = -1;
	Book book = { 0 };
	int i;
	char major[256] = { 0 };
	printf("请输入要统计的专业\r\n");
	rewind(stdin);
	scanf_s("%s", major, 256);


	for (i = 0; i < g_BorrowRecordIndex; i++)
	{
		// 获取学生下标
		StudentIndex = GetStudentIndexByID(g_BorrowRecords[i].StudentID);

		// 判断学生专业
		if (StudentIndex != -1 && strcmp(g_Students[StudentIndex].major, major) == 0)
		{
			// 获取图书下标
			BookIndex = GetBookIndexByID(g_BorrowRecords[i].BookID);

			if (BookIndex != -1)
			{
				if (book.date.year == 0)
				{
					book = g_Books[BookIndex];
				}
				else
				{
					if (g_Books[BookIndex].BorrowTotalCount > book.BorrowTotalCount)
					{
						book = g_Books[BookIndex];
					}
				}
			}
		}
	}

	if (book.date.year == 0)
	{
		printf("该专业没借阅过任何图书\r\n");
	}
	else
	{
		printf("该专业最喜欢的图书 图书ID:%s 书名:%s\r\n", book.BookID, book.name);
	}
}

/*---------------- 界面菜单 ----------------*/

// 学生图书查询菜单
void StudentBookQueryMenu()
{
	int operate = 0;

	while (true)
	{
		system("cls");

		printf("+---------------------------------------------------------------------------------------------------+\n");
		printf("|                                                                                                   |\n");
		printf("|                                          学生图书查询界面                                         |\n");
		printf("|                                                                                                   |\n");
		printf("|                1--按书名查询                                 2--按作者查询                        |\n");
		printf("|                                                                                                   |\n");
		printf("|                3--根据借阅次数降序排序所有图书               4--根据价格降序排序所有图书          |\n");
		printf("|                                                                                                   |\n");
		printf("|                5--返回上一级菜单                                                                  |\n");
		printf("|                                                                                                   |\n");
		printf("+---------------------------------------------------------------------------------------------------+\n");
		printf("请选择您的操作:");

		// 获取用户选择
		rewind(stdin);
		scanf_s("%d", &operate);

		switch (operate)
		{
			case 1:  // 按书名查询
				{
					SelectBookByName();
					break;
				}
			case 2:  // 按作者查询
				{
					SelectBookByAuthor();
					break;
				}
			case 3:  // 根据借阅次数降序排序所有图书
				{
					SortBookByBorrowTotalCount();
					break;
				}
			case 4:  // 根据价格降序排序所有图书
				{
					SortBookByPrice();
					break;
				}
			case 5:  // 返回上一级菜单
				return;
			default:
				{
					printf("您的选择不正确,请重新选择!\r\n");
					break;
				}
		}
		system("PAUSE");
	}
}

// 图书查询菜单
void BookQueryMenu()
{
	int operate = 0;

	while (true)
	{
		system("cls");

		printf("+---------------------------------------------------------------------------------------------------+\n");
		printf("|                                                                                                   |\n");
		printf("|                                          管理员图书查询界面                                       |\n");
		printf("|                                                                                                   |\n");
		printf("|                               1--按书名查询               2--按出版社查询                         |\n");
		printf("|                                                                                                   |\n");
		printf("|                               3--按作者查询               4--查询全部图书                         |\n");
		printf("|                                                                                                   |\n");
		printf("|                               5--返回上一级菜单                                                   |\n");
		printf("|                                                                                                   |\n");
		printf("+---------------------------------------------------------------------------------------------------+\n");
		printf("请选择您的操作:");

		// 获取用户选择
		rewind(stdin);
		scanf_s("%d", &operate);

		switch (operate)
		{
			case 1:	 // 按书名查询
				{
					SelectBookByName();
					break;
				}
			case 2:  // 按出版社查询
				{
					SelectBookByPublisher();
					break;
				}
			case 3:  // 按作者查询
				{
					SelectBookByAuthor();
					break;
				}
			case 4:  // 查询全部图书
				{
					SelectBook();
					break;
				}
			case 5:  // 返回上一级菜单
				return;
			default:
				{
					printf("您的选择不正确,请重新选择!\r\n");
					break;
				}
		}
		system("PAUSE");
	}
}

// 图书管理界面菜单
void BookManageMenu()
{
	int operate = 0;

	while (true)
	{
		system("cls");

		printf("+---------------------------------------------------------------------------------------------------+\n");
		printf("|                                                                                                   |\n");
		printf("|                                           图书管理界面                                            |\n");
		printf("|                                                                                                   |\n");
		printf("|                               1--图书查询                 2--添加图书                             |\n");
		printf("|                                                                                                   |\n");
		printf("|                               3--修改图书                 4--删除图书                             |\n");
		printf("|                                                                                                   |\n");
		printf("|                               5--返回上一级菜单                                                   |\n");
		printf("|                                                                                                   |\n");
		printf("+---------------------------------------------------------------------------------------------------+\n");
		printf("请选择您的操作:");

		// 获取用户选择
		rewind(stdin);
		scanf_s("%d", &operate);

		switch (operate)
		{
			case 1:  // 图书查询
				{
					BookQueryMenu();
					break;
				}
			case 2:  // 添加图书
				{
					AddBook();
					break;
				}
			case 3:  // 修改图书
				{
					ModifyBook();
					break;
				}
			case 4:  // 删除图书
				{
					DeleteBook();
					break;
				}
			case 5:  // 返回上一级菜单
				return;
			default:
				{
					printf("您的选择不正确,请重新选择!\r\n");
					break;
				}
		}
		system("PAUSE");
	}
}

// 显示修改管理员个人信息菜单
void ModifyLibrarianMenu()
{
	int operate = 0;

	while (true)
	{
		system("cls");

		printf("+---------------------------------------------------------------------------------------------------+\n");
		printf("|                                                                                                   |\n");
		printf("|                                        管理员个人信息管理界面                                     |\n");
		printf("|                                                                                                   |\n");
		printf("|                               1--修改密码                 2--修改邮箱                             |\n");
		printf("|                                                                                                   |\n");
		printf("|                               3--显示个人信息             4--返回上一级菜单                       |\n");
		printf("|                                                                                                   |\n");
		printf("+---------------------------------------------------------------------------------------------------+\n");
		printf("请选择您的操作:");


		// 获取用户选择
		rewind(stdin);
		scanf_s("%d", &operate);

		switch (operate)
		{
			case 1:  // 修改密码
				{
					ModifyLibrarianPassword();
					break;
				}
			case 2:  // 修改邮箱
				{
					ModifyLibrarianEmail();
					break;
				}
			case 3:  //显示管理员个人信息
				{
					ShowLibrarianInfo();
					break;
				}
			case 4:  // 返回上一级菜单
				return;

			default:
				{
					printf("您的选择不正确,请重新选择!\r\n");
					break;
				}
		}
		system("PAUSE");
	}
}

// 修改学生个人信息菜单
void ModifyStudentMenu()
{
	int operate = 0;

	while (true)
	{
		system("cls");

		printf("+---------------------------------------------------------------------------------------------------+\n");
		printf("|                                                                                                   |\n");
		printf("|                                       学生个人信息管理界面                                        |\n");
		printf("|                                                                                                   |\n");
		printf("|                               1--修改密码                 2--修改邮箱                             |\n");
		printf("|                                                                                                   |\n");
		printf("|                               3--修改电话                 4--显示个人信息                         |\n");
		printf("|                                                                                                   |\n");
		printf("|                               5--返回上一级菜单                                                   |\n");
		printf("|                                                                                                   |\n");
		printf("+---------------------------------------------------------------------------------------------------+\n");
		printf("请选择您的操作:");

		// 获取用户选择
		rewind(stdin);
		scanf_s("%d", &operate);

		switch (operate)
		{
			case 1:  // 修改密码
				{
					ModifyStudentPassword();
					break;
				}
			case 2:  // 修改邮箱
				{
					ModifyStudentEmail();
					break;
				}
			case 3:  // 修改电话
				{
					ModifyStudentPhone();
					break;
				}
			case 4:
				{
					ShowStudentInfo();
					break;
				}
			case 5:  // 返回上一级菜单
				return;
			default:
				{
					printf("您的选择不正确,请重新选择!\r\n");
					break;
				}
		}
		system("PAUSE");
	}
}

// 借阅管理菜单
void BorrowManageMenu()
{
	int operate = 0;

	while (true)
	{
		system("cls");

		printf("+---------------------------------------------------------------------------------------------------+\n");
		printf("|                                                                                                   |\n");
		printf("|                                              借阅管理界面                                         |\n");
		printf("|                                                                                                   |\n");
		printf("|                      1--查询图书借阅情况                 2--根据学生姓名查询借阅图书记录          |\n");
		printf("|                                                                                                   |\n");
		printf("|                      3--根据书名查询借阅图书记录         4--查询超期未还的图书                    |\n");
		printf("|                                                                                                   |\n");
		printf("|                      5--查询超期未还的学生               6--冻结超期学生账号                      |\n");
		printf("|                                                                                                   |\n");
		printf("|                      7--解冻学生账号                     8--返回上一级菜单                        |\n");
		printf("|                                                                                                   |\n");
		printf("+---------------------------------------------------------------------------------------------------+\n");
		printf("请选择您的操作:");

		// 获取用户选择
		rewind(stdin);
		scanf_s("%d", &operate);

		switch (operate)
		{
			case 1:  // 查询图书借阅情况
				{
					SelectBookBorrowInfo();
					break;
				}
			case 2:  // 根据学生姓名查询借阅图书信息
				{
					SelectBorrowRecordByStudentName();
					break;
				}
			case 3:  // 根据书名查询借阅图书信息
				{
					SelectBorrowRecordByBookName();
					break;
				}
			case 4:  // 查询超期未还的图书清单
				{
					SelectOverdueBook();
					break;
				}
			case 5:  // 查询超期为还得学生清单
				{
					SelectOverdueStudent();
					break;
				}
			case 6:  // 冻结逾期未还账号
				{
					FreezeOverdueStudent();
					break;
				}
			case 7:  // 解冻归还图书账号
				{
					DefreezeStudent();
					break;
				}
			case 8:  // 返回上一级菜单
				return;
			default:
				{
					printf("您的选择不正确,请重新选择!\r\n");
					break;
				}
		}
		system("PAUSE");
	}
}

void StatisticsMenu()
{
	int operate = 0;

	while (true)
	{
		system("cls");

		printf("+---------------------------------------------------------------------------------------------------+\n");
		printf("|                                                                                                   |\n");
		printf("|                                           借阅信息统计界面                                        |\n");
		printf("|                                                                                                   |\n");
		printf("|              1--按借阅时间长短排序所有图书                 2--按借阅次数排序所有在馆图书          |\n");
		printf("|                                                                                                   |\n");
		printf("|              3--按专业统计学生最爱借阅的图书               4--返回上一级菜单                      |\n");
		printf("|                                                                                                   |\n");
		printf("+---------------------------------------------------------------------------------------------------+\n");
		printf("请选择您的操作:");

		// 获取用户选择
		rewind(stdin);
		scanf_s("%d", &operate);

		switch (operate)
		{
			case 1:  // 按借阅时间长短排序所有图书
				{
					SortBookByBorrowTime();
					break;
				}
			case 2:  // 按借阅次数排序所有在馆图书
				{
					SortBookByBorrowCount();
					break;
				}
			case 3:  // 按专业统计学生最爱借阅的图书
				{
					CountBookByMajor();
					break;
				}
			case 4:  // 返回上一级菜单
				return;
			default:
				{
					printf("您的选择不正确,请重新选择!\r\n");
					break;
				}
		}
		system("PAUSE");
	}
}

// 图书管理员菜单
void LibrarianMenu()
{
	int operate = 0;

	while (true)
	{
		system("cls");

		printf("管理员您好:\n");
		printf("+---------------------------------------------------------------------------------------------------+\n");
		printf("|                                                                                                   |\n");
		printf("|                                            管理员管理界面                                         |\n");
		printf("|                                                                                                   |\n");
		printf("|                               1--图书管理                 2--借阅管理                             |\n");
		printf("|                                                                                                   |\n");
		printf("|                               3--统计借阅信息             4--个人信息管理                         |\n");
		printf("|                                                                                                   |\n");
		printf("|                               5--返回上一级菜单                                                   |\n");
		printf("|                                                                                                   |\n");
		printf("+---------------------------------------------------------------------------------------------------+\n");
		printf("请选择您的操作:");

		rewind(stdin);
		scanf_s("%d", &operate);

		switch (operate)
		{
		case 1:  // 图书管理
			{
				BookManageMenu();
				break;
			}
		case 2:  // 借阅管理
			{
				BorrowManageMenu();
				break;
			}
		case 3:  // 统计借阅信息
			{
				StatisticsMenu();
				break;
			}
		case 4:  // 个人信息管理
			{
				ModifyLibrarianMenu();
				break;
			}
		case 5:  // 返回上一级菜单
			return;
		default:
			{
				printf("您的选择不正确,请重新选择!\r\n");
				break;
			}
		}
		system("PAUSE");
	}
}

// 学生菜单
void StudentMenu()
{
	int operate = 0;

	while (true)
	{
		system("cls");

		printf("同学您好, 欢迎您的到来\n");
		printf("+---------------------------------------------------------------------------------------------------+\n");
		printf("|                                                                                                   |\n");
		printf("|                                            学生管理界面                                           |\n");
		printf("|                                                                                                   |\n");
		printf("|                               1--图书查询                 2--选择准备借阅图书                     |\n");
		printf("|                                                                                                   |\n");
		printf("|                               3--删除未确认借阅图书       4--确定借阅	                            |\n");
		printf("|                                                                                                   |\n");
		printf("|                               5--续借图书                 6--归还图书                             |\n");
		printf("|                                                                                                   |\n");
		printf("|                               7--查询借阅记录             8--个人信息管理                         |\n");
		printf("|                                                                                                   |\n");
		printf("|                               9--返回上一级菜单                                                   |\n");
		printf("|                                                                                                   |\n");
		printf("+---------------------------------------------------------------------------------------------------+\n");
		printf("请选择您的操作:");

		rewind(stdin);
		scanf_s("%d", &operate);

		switch (operate)
		{
			case 1:  // 图书管理
				{
					StudentBookQueryMenu();
					break;
				}
			case 2:  // 挑选准备借阅的图书
				{
					AddBookToCart();
					break;
				}
			case 3:  // 删除准备借阅的图书
				{
					DeleteBookFromCart();
					break;
				}
			case 4:  // 借阅
				{
					BorrowBook();
					break;
				}
			case 5:  // 续借
				{
					RenewBook();
					break;
				}
			case 6:  // 还书
				{
					ReturnBook();
					break; 
				}
			case 7:  // 查看个人借阅记录
				{
					SelectLoginStudentBorrowRecord();
					break;
				}
			case 8:  // 个人信息管理
				{
					ModifyStudentMenu();
					break;
				}
			case 9:  // 退出登录
				return;
			default:
				{
					printf("您的选择不正确,请重新选择!\r\n");
					break;
				}
		}
		system("PAUSE");
	}
}

// 主菜单
void MainMenu()
{
	int operate = 0;
	int result = 0;

	while (true)
	{
		system("cls");

		printf("+---------------------------------------------------------------------------------------------------+\n");
		printf("|                                                                                                   |\n");
		printf("|                                  欢迎来到北京邮电大学图书馆借阅管理系统                           |\n");
		printf("|                                                                                                   |\n");
		printf("|                                   1--学生注册        2--图书管理员注册                            |\n");
		printf("|                                                                                                   |\n");
		printf("|                                   3--学生登录        4--图书管理员登录                            |\n");
		printf("|                                                                                                   |\n");
		printf("|                                   5--退出系统	                                                    |\n");
		printf("|                                                                                                   |\n");
		printf("+---------------------------------------------------------------------------------------------------+\n");

		// 获取用户选择
		rewind(stdin);
		printf("请选择您的操作:");
		result = scanf("%d", &operate);

		switch (operate)
		{
			case 1:  // 学生注册
				{
					StudentRegister();
					break;
				}
			case 2:  // 图书管理员注册
				{
					LibrarianRegister();
					break;
				}
			case 3:  // 学生登陆
				if (StudentLogin() == 1)
				{
					printf("登陆成功\r\n");
					system("PAUSE");
					StudentMenu();
				}
				else
				{
					printf("账号或密码错误\r\n");
				}
				break;
			case 4:  // 图书管理员登录
				if (LibrarianLogin()==1)
				{
					printf("登陆成功\r\n");
					system("PAUSE");
					LibrarianMenu();
				}
				else
				{
					printf("账号或密码错误\r\n");
				}
				break;
			case 5:  // 退出程序
				printf("感谢您的使用,再会\r\n");
				system("PAUSE");
				return;
			default:
				printf("错误!请重新选择\r\n");
				break;
		}
		system("PAUSE");
	}
}

// 运行系统
void run()
{
	// 从文件中恢复相关数据
	LoadData();
	g_CartCount = 0;
	MainMenu();
}

int main()
{
	run();
	return 0;
}
