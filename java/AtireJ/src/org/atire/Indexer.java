package org.atire;

import org.atire.swig.atire_apis;

public class Indexer {
    static {
	    System.loadLibrary("atire_jni");
	}
    
    public void index(String what) {
    	atire_apis.index(what);
    }
    
    public static void usage() {
    	System.out.println("usage: program option1;option2;...;file1;file2;file3;...");
    }

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		if (args.length == 0) {
			usage();
			System.exit(-1);
		}
		new Indexer().index(args[0]);
	}

}
