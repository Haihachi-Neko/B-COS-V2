# `barcode.h` 取扱説明書

`barcode.h` は、文字列からCode 39形式のバーコードを生成し、SVG（Scalable Vector Graphics）画像として出力するためのC言語用シングルヘッダーライブラリです。

このライブラリは、動的にSVGデータを生成するため、Webページへの埋め込みや、ベクター画像としての保存が容易になります。

## 1. 使用方法

このライブラリを使用するには、C/C++ソースファイルに`barcode.h`をインクルードします。
**いずれか一つの**ソースファイルで、`barcode.h`をインクルードする前に`BARCODE_IMPLEMENTATION`を定義してください。これにより、関数の実装がそのファイルに展開されます。

### サンプルコード (`main.c`)

以下は、"HELLO-123" という文字列でバーコードを生成し、`barcode.svg`という名前のファイルに出力する簡単な例です。

```c
#define BARCODE_IMPLEMENTATION
#include "barcode.h"
#include <stdio.h>

int main() {
    // 生成したい文字列
    const char* text_to_encode = "HELLO-123";
    
    // バーコードのサイズ設定
    int scale = 2;      // 細いバーの幅 (ピクセル単位)
    int height = 100;   // バーコードの高さ (ピクセル単位)

    // SVGデータを生成
    char* svg_data = generate_code39_svg(text_to_encode, scale, height);

    if (svg_data == NULL) {
        fprintf(stderr, "バーコードの生成に失敗しました。\n");
        return 1;
    }

    // SVGデータをファイルに書き出す
    FILE* fp = fopen("barcode.svg", "w");
    if (fp == NULL) {
        fprintf(stderr, "ファイルを開けませんでした。\n");
        free(svg_data); // メモリ解放を忘れない
        return 1;
    }
    fprintf(fp, "%s", svg_data);
    fclose(fp);

    // 忘れずにメモリを解放する
    free(svg_data);

    printf("バーコードが barcode.svg として正常に生成されました。\n");

    return 0;
}
```

## 2. APIリファレンス

### `char* generate_code39_svg(const char* text, int scale, int height)`

Code 39形式のバーコードをSVG文字列として生成します。

#### 引数

-   `const char* text`
    -   バーコードにエンコードしたい文字列。
    -   使用可能な文字については、「3. Code 39 文字セット」を参照してください。
-   `int scale`
    -   バーコードの最小単位となる「細いバー」の幅をピクセル単位で指定します。
    -   「太いバー」の幅は、この値の3倍になります。
    -   `1`以上の整数を指定してください。
-   `int height`
    -   バーコード全体の高さをピクセル単位で指定します。

#### 戻り値

-   **成功した場合:**
    -   SVG形式の文字列データへのポインタ (`char*`) を返します。
    -   このメモリ領域は`malloc`によって動的に確保されているため、**使用後は必ず `free()` で解放する必要があります。**
-   **失敗した場合:**
    -   `NULL`を返します。
    -   失敗する主な原因は以下の通りです。
        -   `text`にCode 39でサポートされていない文字が含まれている。
        -   メモリの確保に失敗した。

## 3. Code 39 文字セット

Code 39でエンコード可能な文字は以下の通りです。

-   大文字アルファベット: `A` - `Z`
-   数字: `0` - `9`
-   記号: `-` (ハイフン), `.` (ピリオド), ` ` (スペース), `$` (ドル), `/` (スラッシュ), `+` (プラス), `%` (パーセント)

**注意:**
-   小文字アルファベットはサポートされていません。内部で大文字に変換されます。
-   アスタリスク (`*`) は、開始/終了キャラクタとして内部的に自動で使用されるため、エンコードする文字列に含める必要はありません。

## 4. コンパイル方法

`main.c`と`barcode.h`が同じディレクトリにある場合、以下のコマンドでコンパイルできます。

```sh
# GCC or Clang
gcc main.c -o barcode_generator
```

コンパイルが成功すると、`barcode_generator`という実行ファイルが生成されます。これを実行すると、`barcode.svg`ファイルが作成されます。

```sh
./barcode_generator
```
