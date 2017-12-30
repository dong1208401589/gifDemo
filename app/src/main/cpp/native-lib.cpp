#include <jni.h>
#include <android/log.h>
#include <malloc.h>
#include <string.h>
#include <android/bitmap.h>
#include "gif/gif_lib.h"
#include "android/bitmap.h"
#define TAG "gifPlayer"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
#define  argb(a,r,g,b) ( ((a) & 0xff) << 24 ) | ( ((b) & 0xff) << 16 ) | ( ((g) & 0xff) << 8 ) | ((r) & 0xff)

typedef struct GifBean{
    //帧数
    int currentFrame;
    //帧的播放时长
    int* delays;
    //总帧数
    int totalFrame;
}GifBean;

/**
 * 加载gif
 */

void drawFrame(GifFileType *gifFileType, GifBean *gifBean, AndroidBitmapInfo *info, void *pixels);

extern "C"
JNIEXPORT jlong JNICALL
Java_com_chenwd_gifdemo_GifHandler_loadGif(JNIEnv *env, jobject instance, jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);
    int error;
    GifFileType * gifFileType=DGifOpenFileName(path,&error);

    //初始化结构体变量
    DGifSlurp(gifFileType);

    GifBean* gifBean= (GifBean *) malloc(sizeof(GifBean));
    memset(gifBean,0, sizeof(GifBean));
    gifBean->delays= (int *) malloc(sizeof(int) * gifFileType->ImageCount);
    //播放时长赋值
    ExtensionBlock *extensionBlock;
    for (int i = 0; i < gifFileType->ImageCount; ++i) {
        SavedImage frame=gifFileType->SavedImages[i];
        for (int j = 0; j < frame.ExtensionBlockCount; ++j) {
            if (GRAPHICS_EXT_FUNC_CODE==frame.ExtensionBlocks[j].Function){
                extensionBlock=&frame.ExtensionBlocks[j];
                break;
            }
        }
        if (extensionBlock){
            //计算一帧的时长
            //extensionBlock->Bytes  这个数组的第二三个元素 存放的时长
            int delays=10*(extensionBlock->Bytes[2]<<8| extensionBlock->Bytes[1]);
            gifBean->delays[i]=delays;
        }
    }

    gifBean->totalFrame=gifFileType->ImageCount;
    gifFileType->UserData=gifBean;
    env->ReleaseStringUTFChars(path_, path);
    return (jlong) gifFileType;
}


//绘制图像
void drawFrame(GifFileType *gifFileType, GifBean *gifBean, AndroidBitmapInfo info, void *pixels) {
    //拿到当前帧
    SavedImage savedImage=gifFileType->SavedImages[gifBean->currentFrame];
    //拿当前拓展块的偏移量
    GifImageDesc frameInfo=savedImage.ImageDesc;
    //某个像素的位置
    int pointPixels;
    GifByteType gifByteType ;
    ColorMapObject *colorMapObject=frameInfo.ColorMap;
    GifColorType gifColorType;
    int *px= (int *) pixels;
    px= (int *) ((char *)px + info.stride * frameInfo.Top);
    int *line;
    for (int y = frameInfo.Top; y < frameInfo.Top+frameInfo.Height; ++y) {
        line=px;
        for (int x = frameInfo.Left; x < frameInfo.Left+frameInfo.Width; ++x) {
            pointPixels=(y-frameInfo.Top)*frameInfo.Width+(x-frameInfo.Left);
            //从savedImage.RasterBits[pointPixels] 这里拿出来的是经过gif 压缩过的数据 要做一次解压 (在gif文件中用一char表示的像素)
            gifByteType = savedImage.RasterBits[pointPixels];
            //拿到解压的数据 拿到3个字节的rgb结构体
            gifColorType =colorMapObject->Colors[gifByteType];
            line[x]=argb(255,gifColorType.Red,gifColorType.Green,gifColorType.Blue);
        }
        px=(int *) ((char *)px+info.stride);
    }
}

/**
 * 得到gif宽度
 */
extern "C"
JNIEXPORT jint JNICALL
Java_com_chenwd_gifdemo_GifHandler_getWidth(JNIEnv *env, jobject instance, jlong gifPoint) {
    GifFileType* gifFileType= (GifFileType *) gifPoint;
    return gifFileType->SWidth;

}

/**
 * 得到gif高度
 */
extern "C"
JNIEXPORT jint JNICALL
Java_com_chenwd_gifdemo_GifHandler_getHeight(JNIEnv *env, jobject instance, jlong gifPoint) {

    GifFileType* gifFileType= (GifFileType *) gifPoint;
    return gifFileType->SHeight;

}

/**
 * 绘制
 */
extern "C"
JNIEXPORT jint JNICALL
Java_com_chenwd_gifdemo_GifHandler_updteFrame(JNIEnv *env, jobject instance, jobject bitmap,
                                              jlong gifPoint) {
    GifFileType *gifFileType = (GifFileType *) gifPoint;
    GifBean *gifBean= (GifBean *) gifFileType->UserData;

    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(env,bitmap,&info);
    void *pixels;
    //锁定画布
    AndroidBitmap_lockPixels(env,bitmap,&pixels);

    drawFrame(gifFileType,gifBean,info,pixels);

    //控制当前播放量
    gifBean->currentFrame+=1;
    //播放到最后一帧
    LOGI("当前帧 %d",gifBean->currentFrame);
    if (gifBean->currentFrame>gifBean->totalFrame-1){
        gifBean->currentFrame=0;
        LOGI("重新播放 %d",gifBean->currentFrame);
    }

    AndroidBitmap_unlockPixels(env,bitmap);

    return gifBean->delays[gifBean->currentFrame];
}