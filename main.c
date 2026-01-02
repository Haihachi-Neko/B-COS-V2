#define BARCODE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "barcode.h"

/* プロトタイプ宣言 */
void help(void);
int seek();
int generate();
int bc_register();

/* 構造体（ID保存用） */
typedef struct {
    int id;
    char cat[32]; /* カテゴリ（冗長なため短縮） */
    char name[64];
} Containers;

int main(int argc, char *argv[]) {

    if (argc >= 2) {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1] ,"/h") == 0 || strcmp(argv[1], "-h") == 0) {
            help();
            exit(0);
        } else if (strcmp(argv[1], "/p") == 0 || strcmp(argv[1], "-p") == 0) {
            printf("パブリックモードで起動しています\n");
        }
    }

    while (1) {
        printf("----- コンテナマネージャー B-COS V2 -----\n");
        printf("* ヘルプを表示するには1を入力           *\n");
        printf("* 注意：文字を入力しないでください。    *\n");
        printf("-----------------------------------------\n");
        printf("コマンドを入力 >>>");
        int input;
        
        if (scanf("%d",&input) != 1) {
            printf("文字ではなく数値を入力してください。\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            printf("\n");
            continue;
        }

        printf("\n");
        
        switch (input) {
            case 1:
                help();
                break;
            case 2:
                if (argc >= 2) {
                    if (strcmp(argv[1], "/p") == 0 || strcmp(argv[1], "-p") == 0) {
                        printf("この機能はパブリックモードでは使用できません\n");
                    } else {
                        printf("終了しています...\n");
                        exit(0);
                    }
                } else {
                    printf("終了しています...\n");
                    exit(0);
                }
                break;
            case 3:
                seek();
                break;
            case 4:
                generate();
                break;
            default:
                printf("予期されていない値です。やり直してください。");
                break;
        }
        printf("\n");
    }
}

void help(void) {
    printf("----- B-COS V2 ヘルプ画面 -----\n");
    printf("* 1: ヘルプ画面を表示         *\n");
    printf("* 2: このプログラムを終了     *\n");
    printf("* 3: コードを照会する         *\n");
    printf("* 4: IDからバーコードを生成   *\n");
    printf("-------------------------------\n");
}

int seek() {
    int id1;

    printf("----- B-COS V2 コード照会画面 -----\n");
    printf("* リーダでバーコードを読み取るか  *\n");
    printf("* 手動でIDを直接入力してください  *\n");
    printf("-----------------------------------\n");
    printf("ID >>>");
    if (scanf("%d",&id1) != 1) {
        printf("IDは数字のみで構成する必要があります。\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        printf("\n");
    }
    printf("\n");
    
    return 0;
}

int generate() {
    char str1[16];

    printf("----- B-COS V2 バーコード生成画面 -----\n");
    printf("* IDやバーコードからバーコードを      *\n");
    printf("* 生成します。                        *\n");
    printf("* リーダでバーコードを読み取るか      *\n");
    printf("* 手動でIDを直接入力してください      *\n");
    printf("* ！注意！ 同じディレクトリに         *\n");
    printf("* barcode.svgがあると上書きされます。 *\n");
    printf("---------------------------------------\n");
    printf("ID >>>");

    scanf("%s", str1);
    printf("\n");

    char *svg_data = generate_nw7_svg(str1, 'A', 'A', 2, 100);
    if (svg_data == NULL) {
        fprintf(stderr, "バーコードの生成に失敗しました\n");
        return -1;
    }

    FILE *fp1 = fopen("barcode.svg", "w");
    if (fp1 == NULL) {
        fprintf(stderr, "ファイルを開けませんでした\n");
        free(svg_data);
        return -1;
    }
    fprintf(fp1, "%s", svg_data);
    fclose(fp1);
    free(svg_data);
    printf("バーコードをbarcode.svgに保存しました。");

    return 0;
}

int bc_register() {
    printf("----- B-COS V2 ID登録画面 -----\n");
    printf("* 新規登録するコンテナの      *\n");
    printf("* カテゴリを入力してください  *\n");
    printf("-------------------------------\n");
    return 0;
}