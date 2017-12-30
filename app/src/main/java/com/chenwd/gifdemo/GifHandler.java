package com.chenwd.gifdemo;

import android.graphics.Bitmap;

/**
 * Created by Administrator on 2017/12/30.
 */

public class GifHandler {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
    long gifPoint;

    public GifHandler(String path) {
        load(path);
    }

    public void load(String path){
        gifPoint= loadGif(path);
    }

    // long  c指针
    private native long loadGif(String path);
    private native int getWidth(long gifPoint);
    private native int getHeight(long gifPoint);
    private native int updteFrame(Bitmap bitmap,long gifPoint);

    public int getWidth(){
        return getWidth(gifPoint);
    }
    public int getHeight(){
        return getHeight(gifPoint);
    }

    public int updteFrame(Bitmap bitmap){
        return updteFrame(bitmap,gifPoint);
    }


}
