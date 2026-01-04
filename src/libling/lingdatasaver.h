#pragma once

#include <gtk/gtk.h>

char * path_full_name(const char * name);

void ling_data_saver_save(const char * file_path,GKeyFile *keyfile,const gchar *group_name,const gchar *key,...);

char ** ling_data_saver_get(const char * file_path,const gchar *group_name,const gchar *key,int * arg_num);

void ling_data_saver_clear();

void ling_data_saver_ret_free(char ** ret,int num);
