/* 
 * client.c --- ソケット通信のためのクライアントプログラム
 * 
 * （1）ソケットを作成
 * （2）そのソケットをサーバー側のソケットに接続する
 * （3）Hello worldという文字列を送る
 * （4）ソケットを閉じる
 * 
 * コマンドライン形式は、
 * ./client hostname portnumber
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>
#include <unistd.h>

int main(int argc, char **argv) {
    /* 送信する文字列 */
    static char msg[] = "hello world\n";

    int sock;  /* ソケットのファイルディスクリプタ */
    struct hostent *hp; /* サーバーのホストIPアドレス */
    struct sockaddr_in server; /* サーバーのソケットIPアドレス */
    char buf[1024];

    /*
     * （1）ソケットの作成
     * 
     * ソケットシステムコールを使ってソケットを作成
     * 引数で通信プロトコルを指定する
     * 
     * AF_INET ... インターネット
     * SOCK_STREAM ... ストリーム通信
     */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        /* ソケット失敗 */
        perror("opening stream socket");
        exit(1);
    }

    /*
     * （2）ソケットをサーバー側のソケットに接続する
     */

    /*
     * hostnameからipアドレスを調べる
     */
    hp = gethostbyname(argv[1]);
    if (hp == 0) {
        /* gethostbynameの失敗 */
        fprintf(stderr, "%s: unknown host\n", argv[1]);
        exit(2);
    }

    /*
     * サーバー側のソケットのアドレス表す構造体に記入
     */
    server.sin_family = AF_INET; /* プロトコルの種類を記入 */
    /* 
     * サーバーのホストアドレスを記入
     * server.sin_addrにhp->h_addrを記入
     * h_lengthはhp->h_addrのバイトの長さ
     */
    bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
    server.sin_port = htons(atoi(argv[2])); /* ポート番号を記入 */
    /* 
     * htonsは、整数のバイトオーダーをbin-endianに変換する
     * ネットワーク上で整数はbin-endianで表現されることになっている
     * 両方のマシンが同じバイトオーダを使っていれば問題ないが
     * intelマシンとSPARCマシンの間で通信するときなどは注意が必要である
     */

    /*
     * connectシステムコールを使ってソケットを
     * サーバー側のソケットに接続する
     */
    if (connect(sock , (struct sockaddr *)&server, sizeof(server)) < 0) {
        /* connect失敗 */
        perror("connecting stream socket");
        exit(1);
    }

    /*
     * （3）Hello worldという文字列を送る
     * 
     * 送信文字列のsizeof(msg)は末尾の\0（ヌル文字列）を
     * 含むことに注意
     */
    if (write(sock, msg, sizeof(msg) - 1) < 0) {
        /* write失敗 */
        perror("writing on stream socket");
    }

    /*
     * （4）ソケットを閉じる
     */
    exit(0);
}


