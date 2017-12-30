package com.chenwd.gifdemo;

import android.graphics.Bitmap;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.support.v4.os.EnvironmentCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.bumptech.glide.Glide;

import java.io.File;
import java.lang.reflect.Field;

public class MainActivity extends AppCompatActivity {

    Bitmap bitmap;
    ImageView imageView;
    GifHandler gifHandler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        imageView=findViewById(R.id.iv);

    }

    public void ndkLoadGif(View view){
        File f=new File(Environment.getExternalStorageDirectory(),"demo.gif");
        gifHandler=new GifHandler(f.getAbsolutePath());
        int width=gifHandler.getWidth();
        int height=gifHandler.getHeight();
        bitmap=Bitmap.createBitmap(width,height,Bitmap.Config.ARGB_8888);

        LinearLayout.LayoutParams layoutParams= (LinearLayout.LayoutParams) imageView.getLayoutParams();
        layoutParams.width=width;
        layoutParams.height=height;

        int delays=gifHandler.updteFrame(bitmap);
        handler.sendEmptyMessageDelayed(1,delays);
    }

    Handler handler=new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            int delays=gifHandler.updteFrame(bitmap);
            handler.sendEmptyMessageDelayed(1,delays);
            imageView.setImageBitmap(bitmap);
        }
    };

    public void glideLoadGif(View view) {
        File f=new File(Environment.getExternalStorageDirectory(),"demo.gif");
        Glide.with(this).load(f).into(imageView);
    }
}
