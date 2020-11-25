/*
    設問3
    サーバが返すpingの応答時間が長くなる場合、サーバが過負荷状態になっていると考えられる。
    そこで、直近m回の平均応答時間がtミリ秒を超えた場合は、サーバが過負荷状態になっているとみなそう。
    設問2のプログラムを拡張して、各サーバの過負荷状態となっている期間を出力できるようにせよ。
    mとtはプログラムのパラメータとして与えられるようにすること。
*/
// 応答のない("-"が返る)ときは0ミリ秒とする

#include<stdio.h>
#include<string.h>
#define CONST 10

struct log_split{
    char time[16];
    char address[16];
    char reply[4];
};
struct failure{
    char fail_time[16];
    char repair_time[16];
    char address[16];
    int count;
};
struct overlord{
    int reply_sum;
    char address[16];
    char start[16];
    char end[16];
};
void Shift(int a[], int t){
    for(int i=0; i<t-1; i++){
        a[i] = a[i+1];
    }
}
void Shift_str(char a[][16], int t){
    for(int i=0; i<t-1; i++){
        strcpy(a[i],a[i+1]);
    }
}

int main(int argc,char *argv[]){

    //N,t,m取り込み
    int N, m, t;
    printf("[N m t]? :");
    scanf("%d %d %d",&N,&m,&t);

    //監視ログファイル読み込み
    FILE *fp;
    fp = fopen(argv[1],"r");

    //ループ1。各行読む。
    char str[64];
    char *ptr;
    int i, j, k;
    int line = 0;
    while(fgets(str, 64, fp) != NULL) {
        line++;
    }
    fseek(fp, 0, SEEK_SET);

    struct log_split log[line];
    for(i=0; i<line; i++){

        //<確認日時><サーバアドレス><応答結果>を配列に保存
        fgets(str, 256, fp);
        ptr = strtok(str, ",");

        //配列1に<確認日時>
        strcpy(log[i].time,ptr);
        ptr = strtok(NULL, ",");

        //配列2に<サーバアドレス>
        strcpy(log[i].address,ptr);
        ptr = strtok(NULL, ",");

        //配列3に<応答結果>
        strcpy(log[i].reply,ptr);
    }

    //過負荷を先に記述する
    fclose(fp);
    fp = fopen("output3.txt","w");

    struct failure fail[CONST];

    struct overlord over[CONST];
    //直近m回の<確認日時>を保持する配列
    char overlord_time_log[CONST][m][16];
    //直近m回の<応答結果>を保持する配列
    int overlord_reply_log[CONST][m];
    memset(overlord_reply_log,0,sizeof(overlord_reply_log));

    //ループ2。配列3中の'-'を見つける
    for(i=0,j=0 ; i<line; i++){

        //分岐。'-'だった
        if(strncmp(log[i].reply,"-",1)==0){
            //分岐。故障<サーバアドレス>記録済み？
            if(j==0){
                //故障日時記録
                //故障<サーバアドレス>記録。配列4に
                strcpy(fail[j].address,log[i].address);
                //故障日時記録。配列5に
                strcpy(fail[j].fail_time,log[i].time);

                strcpy(over[j].address,log[i].address);
                strcpy(over[j].start,log[i].time);
                strcpy(over[j].end,log[i].time);
                strcpy(overlord_time_log[j][m-1],log[i].time);
                overlord_reply_log[j][m-1] = 0;
                over[j].reply_sum = 0;

                //タイムアウト回数加算
                fail[j].count = 1;

                j++;
                break;
            }

            for(k=0; k<j; k++){
                //記録済み
                if(strcmp(log[i].address,fail[k].address)==0){
                    //タイムアウト回数加算
                    fail[j].count++;;

                    itoa(overlord_time_log[k][0],over[k].start,10);
                    over[k].reply_sum = over[k].reply_sum - overlord_reply_log[k][0] + atoi(log[i].reply);
                    strcpy(over[k].end,log[i].time);
                    Shift(overlord_reply_log[k], t);
                    Shift_str(overlord_time_log[k], t);
                    strcpy(overlord_time_log[k][m-1],log[i].time);
                    overlord_reply_log[k][m-1] = 0;

                    break;

                //未記録=>記録
                }else{
                    //故障日時記録
                    //故障<サーバアドレス>記録。配列4に
                    strcpy(fail[j].address,log[i].address);
                    //故障日時記録。配列5に
                    strcpy(fail[j].fail_time,log[i].time);
                    //タイムアウト回数加算
                    fail[j].count = 1;

                    strcpy(over[j].address,log[i].address);
                    strcpy(over[j].start,log[i].time);
                    strcpy(over[j].end,log[i].time);
                    strcpy(overlord_time_log[j][m-1],log[i].time);
                    overlord_reply_log[j][m-1] = 0;
                    j++;
                    break;
                }
            }
        //'-'でなかった
        }else{
            //分岐。該当<サーバアドレス>は配列4に記録済み？
            for(k=0; k<j; k++){
                if(strcmp(log[i].address,fail[k].address)==0){
                    if(fail[k].count>=(N-1)){
                        //復活が確認できた。
                        //復活日時を配列6に書き込み
                        strcpy(fail[k].repair_time,log[i].time);
                    }

                    //overlord各種パラメータ更新。古いもの消してから新しいものを入れる
                    itoa(overlord_time_log[k][0],over[k].start,10);
                    over[k].reply_sum = over[k].reply_sum - overlord_reply_log[k][0] + atoi(log[i].reply);
                    strcpy(over[k].end,log[i].time);
                    Shift(overlord_reply_log[k], t);
                    Shift_str(overlord_time_log[k], t);
                    strcpy(overlord_time_log[k][m-1],log[i].time);
                    overlord_reply_log[k][m-1] = atoi(log[i].reply);
                    break;

                //未記録=>何もしない
                }else{
                    ;
                }
            }
        }
        if( (over[k].reply_sum/m) > t ){
            strcpy(str,over[k].address);
            strcat(str,": ");
            strcat(str,over[k].start);
            strcat(str," - ");
            strcat(str,over[k].end);
            strcat(str,"\n");

            fprintf(fp,str);
        }
    }            
    //配列4,5,6をファイルに出力
    fprintf(fp,"\n");
    for(i=0; i<j; i++){
        strcpy(str,fail[i].address);
        strcat(str,": ");
        strcat(str,fail[i].fail_time);
        strcat(str," - ");
        strcat(str,fail[i].repair_time);
        strcat(str,"\n");

        fprintf(fp,str);
    }

    fclose(fp);

    return 0;
}
