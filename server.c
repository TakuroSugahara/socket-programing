/* 
 * ソケット通信のサーバー側のプログラム
 * 
 * （1）socket
 * （1）bind
 * （1）listen
 * （1）accept
 * （1）close
 * 
 * 
 * （1）ソケットを作成
 * （2）作ったソケットをポートに割り当てる
 * （3）そのソケットを「クライアントからの接続要求受付中」という状態にする
 * （4）接続を要求してきたクライアントに対して、データの送受信用のソケットを新たに作成
 * （5）クライアントが送ってきたデータを読み取り、それを表示
 * （6）（4）で作ったソケットを閉じる
 * （7）（4）に戻る
 *
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

int main() {
  int sock; /* クライアントから接続受付用ソケット */
  int msgsock; /* クライアントとのデータ送受信用ソケット */
  struct sockaddr_in server; /* sockのアドレス */

  char buf[1024]; /* 読み取ったデータを処理したデータの一次保存 */
  int rval; /* クライアントから送信されたデータを読み取ったデータ */

  /* 
   * （1）ソケットを作成
   */

  /*
   * socketシステムコールを作成
   * 引数で通信プロトコルを指定
   * 
   * AF_INET ...インターネット
   * SOCK_STREAM ...ストリーム通信
   */
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    /* socket失敗 */
    perror("opening stream socket");
    exit(1);
  }

  /* 
   * （2）作ったソケットをポートに割り当てる
   */

  /* 
   * ソケットのアドレスを表すデータ構造に、必要事項を記入
   */
  server.sin_family = AF_INET; /* プロトコルの種類を記入 */

  /* 
   * サーバー側のソケットでは
   * ホストのアドレスは指定しなくてよい
   */
  server.sin_addr.s_addr = INADDR_ANY;

  /* 
   * ポートを5001で登録しておく
   */
  server.sin_port = htons(5001);

  /* 
   * bindシステムコールを使って、ソケット上で記入した
   * アドレスを割り当てる
   */
  if (bind(sock, (struct sockaddr *)&server,
          sizeof server) < 0) {
    /* bind失敗 */
    perror("binding stream socket");
    exit(1);
  }

  /* 
   * （3）（2）で作ったソケットを「クライアントからの接続要求受付中」
   * という状態にする
   */
  listen(sock, 5);
  printf("lintening...\n");
  /* 
   * 解説
   * 
   * このlistenシステムコールによって、ソケットに対して
   * 「クライアントの待ち行列（queue）」が作れられる。
   * sockに対して（connectシステムコール）を出したクライアントは
   * まずこの待ち行列にいれられ、サーバー側が受け入れるのを待つことになる
   * 
   * 第2の引数の5は、待ち行列のサイズを表す。
   * 同時にこのサイズ以上の数のクライアントが接続要求を出した場合
   * 待ち行列に入りきらなかったクライアントのシステムコールは
   * エラーになる
   */
  for(;;) { /* 無限ループ */
    /* 
     * （4）接続を要求してきたクライアントに対して
     * データの送受信用のソケットを新たに作成する
     */

    /* 
     * acceptシステムコールは、接続要求を出しているクライアントと実際に接続し
     * そのクライアントとデータの送受信するためのソケットを新たに作成する
     * 接続要求のあるクライアントがない場合は、接続要求がくるまで待つ
     *
     * 第2第3引数をきちんと設定すれば、クライアント側の
     * ソケットのアドレスを得ることもできる。
     */
    msgsock = accept(sock, (struct sockaddr *)0, (socklen_t *)0);
    if (msgsock == -1) {
      /* accept失敗 */
      perror("accept error");
    }
    else {
      /* 
       * （5）クライアントが送ってきたデータを読み、それを標準出力にする
       */
      do {
        bzero(buf, sizeof(buf));
        if ((rval = read(msgsock, buf, 1024)) < 0) {
          /* read失敗 */
          perror("reading stream message");
        }

        if (rval == 0) {
          write(1, "End\n", 4);
        }
        else {
          write(1, buf, rval);
        }
      } while(rval != 0);
    }

    /* 
     * （6）（4）で作成したソケットを閉じる
     */
    close(msgsock);
    printf("close msgsock\n");
  }
  /* 
   * 注:
   * このプログラムは無限ループなので、sockをcloseしていない
   * 全てのソケットは、プログラムが終了した時に自動的にcloseされる
   */
}
