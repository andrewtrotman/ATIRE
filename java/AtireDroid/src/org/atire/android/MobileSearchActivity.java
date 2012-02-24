package org.atire.android;

import java.io.File;
import java.util.ArrayList;

import org.atire.swig.atire_apis;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

public class MobileSearchActivity extends Activity implements OnClickListener {
	private TextView textViewFiles;
	private TextView textViewResult;
	private TextView textViewStatus;
	private ArrayList<String> files;
	
	boolean mExternalStorageAvailable = false;
	boolean mExternalStorageWriteable = false;
	
	private static final String OPTION_SEPARATOR = "+";
	
	private static final String DATA_PATH = "/data/data/org.atire.android";
	private static final String INDEX_FILE = DATA_PATH + File.separator + "index.aspt";
	private static final String DOCLIST_FILE = DATA_PATH + File.separator + "doclist.aspt";
	
	private static final String INDEX_OPTION = "-findex" + OPTION_SEPARATOR + INDEX_FILE;
	private static final String DOCLIST_OPTION = "-fdoclist" + OPTION_SEPARATOR + DOCLIST_FILE;
	
	private String options;
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        System.loadLibrary("atire_android_jni");
        
        Button button = (Button)findViewById(R.id.index_button);
        button.setOnClickListener(this);
        
        textViewFiles = (TextView)findViewById(R.id.textViewFiles);
        textViewResult = (TextView)findViewById(R.id.textViewResult);
        textViewStatus = (TextView)findViewById(R.id.textViewStatus);
        
		options = "-rtrec" + OPTION_SEPARATOR + INDEX_OPTION + OPTION_SEPARATOR + DOCLIST_OPTION + OPTION_SEPARATOR + "/sdcard/data/moby.txt";
		
		this.checkExternalStorage();
		if (mExternalStorageWriteable)
			textViewStatus.setText("External Storage is writable.");
        
    }

	@Override
	public void onClick(View v) {
		if (mExternalStorageWriteable) {
			atire_apis.atire_index(options);
			textViewResult.setText("Index finished");
		}
	}
	
	private void checkExternalStorage() {
		String state = Environment.getExternalStorageState();

		if (Environment.MEDIA_MOUNTED.equals(state)) {
		    // We can read and write the media
		    mExternalStorageAvailable = mExternalStorageWriteable = true;
		    
		} else if (Environment.MEDIA_MOUNTED_READ_ONLY.equals(state)) {
		    // We can only read the media
		    mExternalStorageAvailable = true;
		    mExternalStorageWriteable = false;
		} else {
		    // Something else is wrong. It may be one of many other states, but all we need
		    //  to know is we can neither read nor write
		    mExternalStorageAvailable = mExternalStorageWriteable = false;
		}
	}
}