/*
    設問4
    ネットワーク経路にあるスイッチに障害が発生した場合、そのスイッチの配下にあるサーバの応答がすべてタイムアウトすると想定される。
    そこで、あるサブネット内のサーバが全て故障（ping応答がすべてN回以上連続でタイムアウト）している場合は、
    そのサブネット（のスイッチ）の故障とみなそう。
    設問2または3のプログラムを拡張して、各サブネット毎にネットワークの故障期間を出力できるようにせよ。
*/
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
struct Subnet{
    char address[16];
    char fail_time[16];
    char repair_time[16];
    int subnet_count;
}

int main(int argc,char *argv[], int N){
    //N取り込み
    int N;
    printf("N? :");
    scanf("%d",&N);

    //監視ログファイル読み込み
    FILE *fp;
    fp = fopen(argv[1],"r");

    //ループ1。各行読む。
    char str[64];
    char *ptr;
    int i, j, k, l, m, s;
    int timeout;
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

    struct failure fail[CONST];

    struct Subnet sub[CONST];

    //ループ2。配列3中の'-'を見つける
    for(i=0,j=0,l=0 ; i<line; i++){

        //分岐。'-'だった
        if(strncmp(log[i].reply,"-",1)==0){
            //分岐。故障<サーバアドレス>記録済み？
            if(j==0){
                //故障日時記録
                //故障<サーバアドレス>記録。配列4に
                strcpy(fail[j].address,log[i].address);
                //故障日時記録。配列5に
                strcpy(fail[j].fail_time,log[i].time);

                ptr = strtok(log[i].address, "/");
                strcpy(sub[l].address,ptr);
                strcpy(sub[l].fail_time,log[i].time);
                sub[l].subnet_count = 1;
                l++;

                //タイムアウト回数加算
                fail[j].count = 1;
                j++;
            }

            for(k=0; k<j; k++){
                //記録済み
                if(strcmp(log[i].address,fail[k].address)==0){
                    //タイムアウト回数加算
                    fail[j].count++;

                    /*ptr = strtok(log[i].address, "/");
                    for(m=0; m<l; m++){
                        if(strcmp(ptr,sub[m].address)==0){
                            sub[m].count += 1;
                            break;
                        }
                    }*/

                //未記録=>記録
                }else{
                    //故障日時記録
                    //故障<サーバアドレス>記録。配列4に
                    strcpy(fail[j].address,log[i].address);
                    //故障日時記録。配列5に
                    strcpy(fail[j].fail_time,log[i].time);
                    //タイムアウト回数加算
                    fail[j].count = 1;
                    j++;
                    
                    ptr = strtok(log[i].address, "/");
                    for(m=0; m<l; m++){
                        if(strcmp(ptr,sub[m].address)==0){
                            sub[m].subnet_count += 1;
                            break;
                        }else{
                            strcpy(sub[l].address,ptr);
                            strcpy(sub[l].fail_time,log[i].time);
                            sub[l].subnet_count = 1;
                            l++;
                        }
                    }
                }
            }
        //'-'でなかった
        }else{
            //分岐。該当<サーバアドレス>は配列4に記録済み？
            for(k=0; k<j; k++){
                if(strcmp(log[i].address,fail[k].address)==0){
                    if(fail[k].count>=N){
                        //復活が確認できた。
                        //復活日時を配列6に書き込み
                        strcpy(fail[k].repair_time,log[i].time);
                        fail[k].count = 0;
                        
                        //サブネットの故障
                        if(k <= l){
                            for(s=0,timeout=0; s<sub[k].count; s++){
                                //サブネット内の故障しているサーバの数を計上
                                if(ptrcmp()){
                                timeout += log[s]
                                }else{
                                    sub[k].subnet_count = 0;
                                }
                            }
                            //故障サーバ合計がN＊サーバ数以上ならサブネットが故障している
                        }
                    }    
                //未記録=>何もしない
                }else{
                    ;
                }
            }
        }
    }            
    //配列4,5,6をファイルに出力
    fclose(fp);
    
    fp = fopen("failure_time.txt","w");
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
