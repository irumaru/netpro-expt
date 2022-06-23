#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Fast Name, Last NAmeの各長さ
//byte数+1
#define NAME_LEN 64

//Email Addressの長さ
#define EMAIL_ADDR_LEN 64

//アドレステーブルの構造体
struct RECORD
{
	RECORD* next;//次のレコードのアドレス
	int id;//ID
	char lname[NAME_LEN];//Last Name
	char fname[NAME_LEN];//Fast Name
	char email[EMAIL_ADDR_LEN];//Email Addres
}EmailTable;

//高速化用
//末尾のID
int LastId = 0;
//レコード数
int RecordCount = 0;

//プロトタイプ宣言
int Add(char [NAME_LEN], char [NAME_LEN], char [EMAIL_ADDR_LEN]);
int Delete(int);
int Find(char[NAME_LEN + EMAIL_ADDR_LEN], bool, bool, bool);
int Sort(int, bool);
int RecordPrint(RECORD**, int);
int ReadData();
int WriteData();

//メイン関数
int main()
{	
	char command, fname[NAME_LEN], lname[NAME_LEN], email[EMAIL_ADDR_LEN], keyword[NAME_LEN + EMAIL_ADDR_LEN];
	int id;
	bool nextLoop = true;

	ReadData();

	do {
		//コマンド入力
		scanf_s("%c", &command, 1);
		fflush(stdin);

		switch (command)
		{
		case 'a':
			//入力
			printf("アドレス帳に追加します。\n");
			printf("名: ");
			scanf_s("%s", fname, NAME_LEN);
			printf("姓: ");
			scanf_s("%s", lname, NAME_LEN);
			printf("メールアドレス: ");
			scanf_s("%s", email, NAME_LEN);
			//追加
			id = Add(lname, fname, email);
			//mes
			printf("追加しました。ID=%d\n", id);
			break;
		case 'f':
			printf("検索するキーワードを入力してください。\n");
			scanf_s("%s", &keyword, NAME_LEN + EMAIL_ADDR_LEN);
			Find(keyword, true, true, true);
			break;
		case 'p':
			int orderBy, order;
			printf("ソート方法を入力してください。\nソート基準(lname=0, fname=1, email=2): ");
			scanf_s("%d", &orderBy);
			printf("ソート順(昇順=0, 降順=1): ");
			scanf_s("%d", &order);
			Sort(orderBy, order);
			break;
		case 'd':
			printf("削除する要素のIDを入力してください。\nID: ");
			scanf_s("%d", &id);
			Delete(id);
			break;
		case 'q':
			//次のコマンドを待ち受けずに終了
			nextLoop = false;
			break;
		}
	} while (nextLoop);

	WriteData();

	return 0;
}

//アドレステーブルにアドレスレコードを追加する
int Add(char lname[NAME_LEN], char fname[NAME_LEN], char email[EMAIL_ADDR_LEN])
{
	//新しいレコードのポインタを宣言
	RECORD* EmailRecordP;

	//新しいレコードを作成
	EmailRecordP = (RECORD*)malloc(sizeof(RECORD));

	//テーブル最後尾のIDの更新
	LastId++;

	//レコード数の加算
	RecordCount++;

	//新しいレコードのIDを初期化
	EmailRecordP->id = LastId;

	//新しいレコードの値を初期化
	strcpy_s(EmailRecordP->lname, NAME_LEN, lname);
	strcpy_s(EmailRecordP->fname, NAME_LEN, fname);
	strcpy_s(EmailRecordP->email, NAME_LEN, email);

	//最後尾のレコードなので、次のレコードを示すnextをNULLで初期化
	EmailRecordP->next = NULL;

	//直前のレコードと末尾のレコードのポインタを宣言
	RECORD* next, * prev;

	//先頭のレコード
	prev = &EmailTable;

	//アドレステーブルの末尾のポインタを探す
	for (next = EmailTable.next; next != NULL; next = next->next)
	{
		//nextはNULLでない
		prev = next;
	}

	//レコードをテーブルに追加
	prev->next = EmailRecordP;

	//IDを返す
	return LastId;
}

//レコードの削除
int Delete(int id)
{
	//初期アドレス
	RECORD* prev = &EmailTable;

	for (RECORD* next = EmailTable.next; next != NULL; next = next->next)
	{
		if (next->id == id)
		{
			//IDが一致&レコード(線形リスト)の繋ぎ変え
			prev->next = next->next;

			//削除するレコードのメモリを解放
			free(next);

			//レコード数の減算
			RecordCount--;

			//終了&削除
			return 0;
		}
		prev = next;
	}

	//終了&削除するIDが見つからない
	return 1;
}

//キーワードで検索して表示
//(検索キーワード, emailで検索, fnameで検索, lnameで検索)
int Find(char keyword [NAME_LEN + EMAIL_ADDR_LEN], bool fFname, bool fLname, bool fEmail)
{
	RECORD* next;
	bool find;
	int count = 0;

	//検索結果のアドレス
	RECORD** results = (RECORD**)malloc(sizeof(RECORD*) * RecordCount);
	if (results == NULL)
		exit(0);

	for (next = EmailTable.next; next != NULL; next = next->next)
	{
		find = false;
		//fnameで検索
		if (fFname)
			if (strcmp(next->fname, keyword) == 0)
				find = true;
		//lnameで検索
		if (fLname)
			if (strcmp(next->lname, keyword) == 0)
				find = true;
		//emailで検索
		if (fEmail)
			if (strcmp(next->email, keyword) == 0)
				find = true;

		if (find)
		{
			//検索結果のアドレスを記録
			results[count] = next;
			//カウントアップ
			count++;
		}
	}

	//出力
	if (count == 0)
	{
		printf("検索結果は見つかりませんでした。\n");
		return 0;
	}

	printf("検索結果 %d件\n", count);
	RecordPrint(results, count);
	free(results);

	return 0;
}

//ソートして表示
//orderBy=0: lname, 1: fname, 2: email
//order=true: 昇順(s->l), false: 降順
int Sort(int orderBy, bool order)
{
	RECORD* next;
	RECORD** p = (RECORD**)malloc(sizeof(RECORD*) * RecordCount);
	if (p == NULL)
		exit(0);

	//アドレス配列を作成
	int i = 0;
	for (next = EmailTable.next; next != NULL; next = next->next)
	{
		p[i] = next;
		i++;
	}

	//並べ替え
	for (i = 0; i < RecordCount; i++)
		for (int j = i + 1; j < RecordCount; j++)
		{
			//比較用ポインタ
			char* pi, * pj;
			//C4703対策
			pi = pj = NULL;

			switch (orderBy)
			{
			case 0:
				//lname
				pi = p[i]->lname;
				pj = p[j]->lname;
				break;
			case 1:
				//fname
				pi = p[i]->fname;
				pj = p[j]->fname;
				break;
			case 2:
				//email
				pi = p[i]->email;
				pj = p[j]->email;
				break;
			}
			//並べ替えの条件 昇順 && 降順
			if ((order && strcmp(pi, pj) < 0) || (!order && 0 < strcmp(pi, pj)))
			{
				//並べ替え
				RECORD* tmp = p[i];
				p[i] = p[j];
				p[j] = tmp;
			}
		}
	
	printf("アドレス帳 一覧\n");
	RecordPrint(p, RecordCount);
	free(p);

	return 0;
}

//レコードの出力
int RecordPrint(RECORD** puts, int count)
{
	printf("------------------------------------------------------\n");
	printf(" ID\t姓\t名\tメールアドレス\n");

	for (int i = 0; i < count; i++)
		printf(" %d\t%s\t%s\t%s\n", puts[i]->id, puts[i]->lname, puts[i]->fname, puts[i]->email);

	printf("------------------------------------------------------\n");

	return 0;
}

//ファイルからデータを読み込む
int ReadData()
{
	//ファイルポインタ
	FILE* fp;

	//ファイルを開く
	if (fopen_s(&fp, "adbook.txt", "r") != 0)
	{
		//ファイルが開けない
		printf("既存のアドレス帳のファイルが見つからないため、新規アドレス帳を新しいファイルに保存します。\n");
		return 1;
	}

	//データ数
	int len;

	//データ数の読み込み
	fscanf_s(fp, "%d", &len);

	//データの読み込み
	for (int i = 0; i < len; i++)
	{
		char lname[NAME_LEN], fname[NAME_LEN], email[EMAIL_ADDR_LEN];

		//ファイルの読み込み
		fscanf_s(fp, "%s %s %s", fname, NAME_LEN, lname, NAME_LEN, email, EMAIL_ADDR_LEN);

		//テーブルに追加
		Add(lname, fname, email);
	}

	//ファイルを閉じる
	fclose(fp);

	return 0;
}

//ファイル書き込み
int WriteData()
{
	//ファイルポインタ
	FILE* fp;

	//ファイルを開く
	if (fopen_s(&fp, "adbook.txt", "w") != 0)
	{
		//ファイルが開けないor作成できない
		printf("ファイルが開けない、又は作成できないため、データを保存できません。\n");
		return 1;
	}

	//データ数書き込み
	fprintf(fp, "%d\n", RecordCount);

	//データ書き込み
	for (RECORD* next = EmailTable.next; next != NULL; next = next->next)
		fprintf(fp, "%s %s %s\n", next->fname, next->lname, next->email);

	//ファイルを閉じる
	fclose(fp);

	return 0;
}