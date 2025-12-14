#include <lingsysinfoupdater.h>

typedef struct LingSysInfoType{
    uint type;
    LING_SYS_INFO_UPDATE_CALLBACK cb;
    gpointer user_data;
    GList * objs;
}LingSysInfoType;

struct _LingSysInfoUpdater{
    GObject parent;
    uint timeout;
    GList * list;
};

G_DEFINE_FINAL_TYPE(LingSysInfoUpdater,ling_sys_info_updater,G_TYPE_OBJECT)

void ling_sys_info_updater_class_init(LingSysInfoUpdaterClass * klass){

}

static gboolean ling_sys_updater_timeout(gpointer user_data){
    LingSysInfoUpdater * self = user_data;
    for(GList * l=self->list;l!=NULL;l=l->next){
        LingSysInfoType * sit = (LingSysInfoType*)l->data;
        sit->cb(sit->objs,sit->user_data);
    }
    return G_SOURCE_CONTINUE;
}

void ling_sys_info_updater_init(LingSysInfoUpdater * self){
    self->timeout = g_timeout_add(LING_SYS_INFO_UPDATE_TIME,ling_sys_updater_timeout,self);
}

LingSysInfoUpdater * ling_sys_info_updater_new(){
    return LING_SYS_INFO_UPDATER(g_object_new(LING_TYPE_SYS_INFO_UPDATER,NULL));
}

//添加更新操作类型
void ling_sys_info_updater_add_type(LingSysInfoUpdater * self,uint type,LING_SYS_INFO_UPDATE_CALLBACK cb,gpointer user_data){
    LingSysInfoType * si = malloc(sizeof(LingSysInfoType));
    si->type = type;
    si->cb = cb;
    si->user_data = user_data;
    si->objs=NULL;
    self->list = g_list_append(self->list,si);
}

//添加想要自动更新的某一类型的物件
void ling_sys_info_updater_add_obj(LingSysInfoUpdater * self,uint type,gpointer obj){
    for(GList * l=self->list;l!=NULL;l=l->next){
        LingSysInfoType * sit = (LingSysInfoType*)l->data;
        if(sit->type==type) sit->objs = g_list_append(sit->objs,obj);
    }
}



