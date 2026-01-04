#include <lingdatasaver.h>
#include <wordexp.h>

char * path_full_name(const char * name){
    wordexp_t exp;
    wordexp(name,&exp,0);
    //g_print("NAME:%s\n",exp.we_wordv[0]);
    return exp.we_wordv[0];
}

void ling_data_saver_save(const char * file_path,GKeyFile *keyfile,const gchar *group_name,const gchar *key,...){
    va_list list;
    va_start(list,key);
    int arg_num = 0;
    GString * str = g_string_new("");
    for(;;){
        char * arg = va_arg(list,char*);
        if(arg==NULL)break;
        if(arg_num!=0)g_string_append(str,",");
        g_string_append(str,arg);
        arg_num++;
    }
    g_string_append(str,";");
    //g_print("data:%s\n",str->str);
    //GKeyFile *keyfile = g_key_file_new();
    g_key_file_set_string(keyfile, group_name, key,str->str);
    g_key_file_save_to_file(keyfile, path_full_name(file_path), NULL);
    va_end(list);
}

char ** ling_data_saver_get(const char * file_path,const gchar *group_name,const gchar *key,int * arg_num){
    GKeyFile *keyfile = g_key_file_new();
    if (!g_key_file_load_from_file(keyfile, path_full_name(file_path), G_KEY_FILE_NONE, NULL))return NULL;
    if (!g_key_file_has_key(keyfile,group_name, key, NULL))return NULL;

    gchar * str = g_key_file_get_string(keyfile,group_name, key, NULL);

    //g_print("str:%s\n",str);
    int start=0,num=0;
    char ** ret=malloc(sizeof(char*)*10);
    for(int i=0;i<=strlen(str);i++){
        if(str[i]==';'){
            ret[num]=malloc(i-start+1);
            for(int j=0;j<i-start;j++){
                ret[num][j]=str[start+j];
            }
            ret[num][i-start]='\0';
            num++;
            break;
        }
        else if(str[i]==','){
            ret[num]=malloc(i-start+1);
            for(int j=0;j<i-start;j++){
                ret[num][j]=str[start+j];
            }
            ret[num][i-start]='\0';
            start=i+1;
            num++;
        }
    }

    *arg_num=num;

    return ret;
}

void ling_data_saver_clear(){

}

//释放ling_data_saver_get的返回数据
void ling_data_saver_ret_free(char ** ret,int num){
    if(ret==NULL)return;
    for(int i=0;i<num;i++){
        if(ret[i]==NULL)continue;
        free(ret[i]);
    }
}
