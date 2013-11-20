package org.atire;

import org.atire.swig.atire_apis;

public class Atire {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		System.loadLibrary("atire_jni");
		
		if (args.length == 0) {
			atire_apis.run_atire("-q:+-findex+/mnt/sdcard/Android/data/au.com.tyo.wiki.offline/index.aspt+-fdoclist+/mnt/sdcard/Android/data/au.com.tyo.wiki.offline/doclist.aspt");
		}
		atire_apis.run_atire(args[0]);
	}
}
