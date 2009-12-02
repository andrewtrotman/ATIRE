/*
 * Created on 21.09.2006
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */

package de.mpii.clix.wikipedia;

import java.sql.*;
import java.util.*;
import javax.xml.parsers.*;
import java.io.*;

import org.xml.sax.*;
import org.xml.sax.helpers.*;

import net.didion.jwnl.*;
import net.didion.jwnl.data.*;

import de.mpii.clix.support.*;

public class ClusteringListReader extends DefaultHandler
{
    private class element
    {
        String tag;
        int pos; //position of this element relative to its parent and its tag, to be used for the xpath
        StringBuffer content;
        String target; // target of a link attribute
        
        String xpath; // xpath of this element
        
        HashMap<String,Integer> childrenCount; // current counters for tag-specific positions among the children
        
        HashSet<String> urls; // urls, accumulated from all descendants and the element itself
        HashMap<label,Double> labels; // labels, accumulated from all descendants and the element itself;
        
        public element()
        {
            childrenCount=new HashMap<String,Integer>();
            
            urls=new HashSet<String>();
            labels=new HashMap<label,Double>();
        }
        
        public String toString()
        {
            return xpath;
        }
    }

    LinkedList<element> elementStack;

    SAXParser parser;
    DefaultHandler handler;
    
    private Database wikiDB;
    private PreparedStatement readPageConceptMapping;

    private PreparedStatement writeNewFact;

    net.didion.jwnl.dictionary.Dictionary dict;
    PointerUtils putils;

    // global data structure that maps a Wiki url to its labels
    private HashMap<String,List<label>> url2label;
    
    private HashSet<String> allPaths;
    
    private HashMap<String,String> path2url;
    
    private HashSet<String> clusteredPaths;
    
    private static String currentList;
    
    private HashMap<label,HashSet<label>> incompatibilities;
    
    public ClusteringListReader()
    {
        dict=net.didion.jwnl.dictionary.Dictionary.getInstance();
        putils=PointerUtils.getInstance();
        
        elementStack=new LinkedList<element>();
        
        url2label=new HashMap<String,List<label>>();
        
        allPaths=new HashSet<String>();
        
        path2url=new HashMap<String,String>();
        
        clusteredPaths=new HashSet<String>();
        
        try
        {
            SAXParserFactory factory = SAXParserFactory.newInstance();
            parser = factory.newSAXParser();
            handler = new DefaultHandler();
        }
        catch(Exception e)
        {
            System.out.println(e);
        }

        wikiDB=new Database();
        wikiDB.openDatabase("test", "test", "mpiat5316", "destiny.mpi");
        
        readPageConceptMapping=wikiDB.prepareStatement("select concept,conceptid,confidence from conceptmap where pagetitle=?");
        
        writeNewFact=wikiDB.prepareStatement("insert into newfacts3(url,concept,confidence,lineage) values(?,?,?,?)");

        initIncompatibilities();
    }

    public static void main(String[] args)
    {
        try
        {
            JWNL.initialize(new FileInputStream("file_properties.xml"));
        }
        catch(Exception e)
        {
            System.out.println("JWNL init failed:\n"+e);
        }
        
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+G$ermans.xml"; // works well
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+painters+by+name.xml"; // works well
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+D$utch+painters.xml"; // not enough annotated entries
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+passenger+airlines.xml"; // messed up WSD for "airline", but otherwise works well
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+record+labels.xml"; // messed up WSD for "label", but otherwise works well
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+assassinated+people.xml"; // messed up WSD for "ruler", but otherwise works well
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+dinosaurs.xml"; // seems to work well
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+songs+with+a+color+word(s)+in+their+names.xml"; // does not work (irregular list structure with and without links)
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+members+of+the+A$cad=C3=A9mie+fran=C3=A7aise.xml"; // not enough annotated entries
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+athletes+by+nickname.xml"; // does not work (heuristics for link detection fails)
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+automobile+manufacturers.xml"; // not enough annorated entries, messed up WSD for auto/car
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+R$ussians.xml"; // works well
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+disco+artists.xml"; // not enough annotated entries
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+astronomers.xml"; // works well
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+commercial+brands+of+beer.xml"; // WSD completey messed up
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+municipalities+in+G$ermany.xml"; // not enough annotated entries
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+naval+ships+of+G$ermany.xml"; // not enough annotated data, unclear structure
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+islands+by+name.xml"; // not enough annotated data
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+cathedrals.xml"; // not enough annotated data
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+novelists+by+nationality.xml"; // works well
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+aircraft+(A$-B$).xml"; // not enough annotated data, irregular structure
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+B$ritish+J$ews.xml"; // works well
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+I$talian+A$mericans.xml"; // works well
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+female+singers.xml"; // works well
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+bass+guitarists.xml"; // works well
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+historians.xml"; // works well
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+billionaires+(2005).xml"; // works well
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+people+by+name=3A+E$l.xml"; // works well
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+films=3A+J$-R$.xml"; // works well
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+mountains.xml"; // works well
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+military+aircraft+of+G$ermany.xml"; // not enough annotated data
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+rivers+in+the+U$nited+S$tates=3A+B$.xml"; // works well
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+S$chlager+musicians.xml"; // not enough annotated data
//      String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+S$tanford+U$niversity+people.xml"; // not enough annotated data
        
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+rock+and+roll+albums.xml"; // great example for the new clustering approach!
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+films+set+in+N$ew+Y$ork+C$ity.xml"; // works well
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+film+noir.xml"; // works well (but already well annotated)
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+F$ormula+O$ne+drivers.xml"; // works well (but already well annotated)
        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+P$oles.xml"; 
        analyzeList(inputfile);

        System.exit(1);
        
        String inputDir="\\\\mpiat5316/Wikipedia/pages/Li/";

        File activeFile = new File(inputDir);

        String[] filelist = activeFile.list();
        
        if(null == filelist)
        {
            System.out.println("the directory name doesn't exist ...");
            return;         
        }
         
        for (int i = 0; i < filelist.length; ++i) 
        {
            if ((filelist[i].startsWith("List")&&(filelist[i].endsWith(".xml"))))
            {
                File _file = new File(inputDir + filelist[i]);

                if (_file.isDirectory()==false)
                    analyzeList(inputDir+filelist[i]);
            }
            else
                System.out.println("ignore "+filelist[i]);
        }
    }

    private static void analyzeList(String list)
    {
        currentList=list.substring(list.lastIndexOf('\\')+1);
        
        System.out.println("\nanalyze "+currentList);
        
        ClusteringListReader handler=null;
        
        try
        {
            SAXParserFactory factory = SAXParserFactory.newInstance();
            SAXParser parser = factory.newSAXParser();
            handler = new ClusteringListReader();
            factory.setFeature("http://apache.org/xml/features/validation/schema",true);
            
            parser.parse(list,handler);
        }
        catch(SAXParseException e)
        {
            System.out.println("SAX parse exception: "+e);
            System.exit(1);
        }
        catch(SAXException e)
        {
            System.out.println("SAX exception: "+e);
            System.exit(1);
        }
        catch(Exception e)
        {
            System.out.println("Exception: "+e);
            e.printStackTrace();
            System.exit(1);
        }
        
//        handler.show();
    }
    
    public void characters(char[] text, int start, int length) throws SAXException
    {
//      System.out.println("\ttext: \""+new String(text).substring(start,start+length)+"\" "+length+" bytes");
        
        element el=elementStack.getFirst();
//      if (el.content!=null) el.content.append(new String(text).substring(start,start+length));
//      else el.content=new StringBuffer(new String(text).substring(start,start+length));

        if (el.content!=null) el.content.append(text,start,length);
        else el.content=new StringBuffer(new String(text).substring(start,start+length));

//      cnt++;
//      if (cnt%1000==0)
//          System.out.println("["+cnt+"] content now "+el.content.length()+" bytes");
    }

    public void endDocument() throws SAXException
    {

    }
    
    private class label
    {
        String concept;
        int id;
        
        double confidence=1.0; //default, may be overridden by constructor
        
        public label(String c, int i)
        {
            concept=c;
            id=i;
        }
        
        public label (String c, int i, double conf)
        {
            this(c,i);
            confidence=conf;
        }
        
        public boolean equals(Object o)
        {
            if (o instanceof label)
            {
                label p=(label)o;
                return ((concept.equals(p.concept)&&(id==p.id)));
            }
            else return false;
        }
        
        public int hashCode()
        {
            return concept.hashCode();
        }
        
        public String toString()
        {
            return concept+"["+id+"]@"+confidence;
        }
    }

    private class synsetComparator implements Comparator<Synset>
    {
        public int compare(Synset s1, Synset s2)
        {
            return Integer.signum((int)(s1.getOffset()-s2.getOffset()));
        }

    }
    
    public void endElement(String namespaceURI, String localName, String qualifiedName) throws SAXException
    {
        //System.out.println("endElement("+namespaceURI+","+localName+","+qualifiedName+")");

        element el=elementStack.removeFirst();
                
        if (el.tag.compareTo("link")==0)
        {
            // read, for each concept mapped to this link's target,
            // all hypernyms from wordnet and add them to the
            // target url's label set
            
            HashSet<label> labels=getAndConsolidateLabels(el.target);
            
            print(el.xpath+"\t"+el.target+"\t");
            Iterator<label> it=labels.iterator();
            for (int i=0;i<labels.size();i++)
            {
                label p=it.next();
                if (i>0) print(",");
                print(p.toString());
            }
            println();
            
            url2label.put(el.target,new LinkedList<label>(labels));
            
            el.urls.add(el.target);
            
            allPaths.add(el.xpath);
            
            Iterator<label> labelit=labels.iterator();
            while (labelit.hasNext())
            {
                label l=labelit.next();
                el.labels.put(l, l.confidence);
            }
        }
        
        checkClusters(el);
        
    }

    public void startElement(String namespaceURI, String localName, String qualifiedName, Attributes arg3) throws SAXException
    {
        //System.out.println("startElement("+namespaceURI+","+localName+","+qualifiedName+")");
        
        // create a new element and put it on the stack
        
        element el=new element();
        el.tag=qualifiedName;
        
        //compute the xpath
        
        if (elementStack.size()>0)
        {
            element parent=elementStack.peek();
            int pos=0;
            Integer posI=parent.childrenCount.get(qualifiedName);
            if (posI!=null) pos=posI.intValue();
            pos++;
            parent.childrenCount.put(qualifiedName,pos);
            
            el.pos=pos;
            el.xpath=parent.xpath+"/"+qualifiedName+"["+pos+"]";
        }
        else
        {
            el.pos=1;
            el.xpath="/"+qualifiedName+"[1]";
        }
        elementStack.addFirst(el);
        
        if (qualifiedName.compareTo("link")==0)
        {
            int pos=arg3.getIndex("xlink:href");
            if (pos!=-1) el.target=arg3.getValue(pos);
            
            path2url.put(el.xpath,el.target);
            
//            for (int i=arg3.getLength();i>=0;i--)
//                System.out.println("1-"+arg3.getURI(i)+"\t2-"+arg3.getLocalName(i)+"\t3-"+arg3.getQName(i));
        }
    }
    
    final static int MINSUPPORT=5;
    final static double THRESHOLD=0.75;
    
    // consider all urls and their labels from paths in a cluster
    // and compute new labels from them, if thresholds are met
    
    private void consolidate(String cluster, Set<String> clusterPaths)
    {        
        // compute the number of urls among the descendants of this element
        // this is the *new* algorithm, the *old* algorithm simply used
        // the size of e.urls
        
        HashMap<label,Double> labels=new HashMap<label,Double>();
        HashSet<String> urls=new HashSet<String>();
        
        double urlCount=0;
        Iterator<String> uit=clusterPaths.iterator();
        
        for (int i=clusterPaths.size();i>0;i--)
            {
                String path=uit.next();
                String url=path2url.get(path);
                urls.add(url);
                List<label> list=url2label.get(url);
                if ((list!=null)&&(list.size()>0)) urlCount++;
                Iterator<label> lit=list.iterator();
                for (int k=list.size();k>0;k--)
                {
                    label l=lit.next();
                    if (labels.containsKey(l)) labels.put(l,labels.get(l)+1.0);
                    else labels.put(l,1.0);
                }
            }

//        System.out.println("urlCount="+urlCount+", size="+e.urls.size());

        println("consolidate("+cluster+") ["+urlCount+" labeled urls, "+(clusterPaths.size()-urlCount)+" unlabeled urls, "+labels.size()+" labels]");

        if (urlCount<MINSUPPORT) return; // there are not enough labeled urls, so stop here
        
        Iterator<Map.Entry<label,Double>> labelit=labels.entrySet().iterator();
        
        while (labelit.hasNext())
        {
            Map.Entry<label,Double> entry=labelit.next();
            
            // check the support of this label
            
            double support=entry.getValue()/urlCount;
            
            label l=entry.getKey();
            if (support<THRESHOLD)
            {
//                System.out.println("rejected "+l+": confidence="+support+" (from "+entry.getValue()+" examples)");
                continue;
            }
            
            System.out.println("accepted "+l+": confidence="+support+" (from "+entry.getValue()+" examples)");
                
            HashSet<label> incomp=new HashSet<label>();
            
            if (incompatibilities.containsKey(l.concept)) incomp=incompatibilities.get(l.concept);
            
            // this label has a high support, so add it to all urls in the set

            Iterator<String> urlsit=urls.iterator();
            while (urlsit.hasNext())
            {
                String url=urlsit.next();
                List<label> list=url2label.get(url);
                if (list==null)
                {
                    System.out.println("\t**** did not find list for "+url);
                    continue;
                }
                ListIterator<label> it=list.listIterator();
                boolean found=false;
                while (it.hasNext())
                {
                    label la=it.next();
                    if (incomp.contains(la))
                    {
                        System.out.println(url+": incompatible label "+la+", abort");
                        found=true;
                        break;
                    }
                    if (la.equals(l))
                    {
                        found=true;
                        if (la.confidence<support)
                        {
                            la.confidence=support;
                            System.out.println("\t"+url+" => "+l+", new confidence="+support);
                            writeNewFact(url,l);
                        }
                        //break;
                    }
                }
                if (found==false)
                {
                    label lab=new label(l.concept,l.id,support);
                    
                    if (consolidateNewLabel(list,lab))
                    {
                        System.out.println("\t"+url+" => "+lab);
                        list.add(lab);
                        writeNewFact(url,lab);
                    }
                    else
                        System.out.println("\tREJECT "+url+" => "+lab);
                }
            }
        }
    }
    
    /*
    private void show()
    {
        System.out.println("\nshow results:\n");
        
        Iterator<Map.Entry<String, List<label>>> it=url2label.entrySet().iterator();
        
        while (it.hasNext())
        {
            Map.Entry<String,List<label>> e=it.next();
            String url=e.getKey();
            List<label> l=e.getValue();
            
            System.out.print(url+"\t");
            
            ListIterator<label> lit=l.listIterator();
            
            for (int i=0;i<l.size();i++)
            {
                if (i>0) System.out.print(",");
                System.out.print(lit.next());
            }
            System.out.println();
        }
    }
    */
    
    final static int CLUSTER_SIZE_THRESHOLD=5;
    final static double CLUSTER_CONFIDENCE_THRESHOLD=0.75;
    
    private void checkClusters(element e)
    {
//        System.out.println("checkClusters("+e+")");
        
        // create candidates for clusters from e's xpath
        // these are xpaths where e's child has a wildcard index
        
        Iterator<Map.Entry<String,Integer>> tagit=e.childrenCount.entrySet().iterator();
        for (int i=e.childrenCount.keySet().size();i>0;i--)
        {
            Map.Entry<String,Integer> entry=tagit.next();
            String tag=entry.getKey();
            int count=entry.getValue();
            
            String matcher=e.xpath+"/"+tag+"[";

            List<String> matches=new LinkedList<String>();
            
            for (int pos=count;pos>0;pos--)
            {
                String path=matcher+pos+"]";
                
                // find matching elements
                
                Iterator<String> paths=allPaths.iterator();
                for (int k=allPaths.size();k>0;k--)
                {
                    String elpath=paths.next();
                    
                    if (clusteredPaths.contains(elpath)) continue;
                    
                    if (elpath.startsWith(path))
                    {
                        String match=elpath;
                        String suffix=match.substring(matcher.length());
                        suffix=suffix.substring(suffix.indexOf(']')+1);
                        
                        if (suffix.length()==0) continue;
                        matches.add(suffix);
                    }
                }
            }
            
            Collections.sort(matches);
            
            if (matches.size()>0) println("possible clusters in the "+e.xpath+"/"+tag+"[*] range:");
            String lastPath="";
            int cnt=0;
            
            ListIterator<String> mit=matches.listIterator();
            for (int k=matches.size();k>0;k--)
            {
                String match=mit.next();
                
                if (lastPath.equals(match)==false)
                {
                    if (lastPath.length()>0)
                    {
                        if (cnt>=CLUSTER_SIZE_THRESHOLD)
                        {
                            println("\t"+lastPath+" ("+cnt+" matches)");
                            checkCluster(matcher,lastPath);
                        }
                    }
                    
                    lastPath=match; cnt=1;
                }
                else cnt++;
            }
            if (cnt>0)
                if (cnt>=CLUSTER_SIZE_THRESHOLD)
                {
                    println("\t"+lastPath+" ("+cnt+" matches)");                
                    checkCluster(matcher,lastPath);
                }
        }
    }
    
    private void checkCluster(String prefix, String suffix)
    {
        println("\tcheckCluster: "+prefix+"*]"+suffix);
        
        // check all paths, find matching paths,
        // and collect labels
        
        HashMap<label,Integer> counter=new HashMap<label,Integer>();
        
        int numPaths=0;
        int numLabeledPaths=0;
        
        HashSet<String> clusterPaths=new HashSet<String>();
        
        Iterator<String> paths=allPaths.iterator();
        for (int i=allPaths.size();i>0;i--)
        {
            String path=paths.next();
            if (path.startsWith(prefix)==false) continue;
            if (path.endsWith(suffix)==false) continue;
            
            int pos=path.substring(prefix.length()).indexOf(suffix);
            if (pos>3) continue;
            
            // this is a structurally valid path, so get its url and its labels
            
            numPaths++;
            
            String url=path2url.get(path);
            if (url==null) continue;
            
            println("\t\t\t"+url);
            
            List<label> labels=url2label.get(url);
            if (labels==null) continue;
            
            clusterPaths.add(path);
            
            if (labels.size()>0) numLabeledPaths++;
            
            ListIterator<label> lit=labels.listIterator();
            for (int k=labels.size();k>0;k--)
            {
                label l=lit.next();
//                System.out.println("label: "+l);

                /*
                Synset syn=null;
                
                try
                {
                    syn=dict.getSynsetAt(POS.NOUN, Long.parseLong(l.id));
                    
                    PointerTargetTree tree=PointerUtils.getInstance().getHypernymTree(syn);
                    
                    PointerTargetNodeList list[]=tree.reverse();
                    
                    PointerTargetNodeList rootList=list[0];

                    PointerTargetNode root=(PointerTargetNode) rootList.get(0);
                    
                    Synset rootsyn=root.getSynset();
                    
                    String rootlabel=rootsyn.getWord(0).getLemma();
                    
                    System.out.println(rootlabel);
                }
                catch(Exception e)
                {
                    System.out.println("cannot read synset for "+l+":\n"+e);
                }
*/
                if (counter.containsKey(l)) counter.put(l, counter.get(l)+1);
                else counter.put(l, 1);
            }
        }
        
        println("\t\tnumPaths="+numPaths+", numLabeledPaths="+numLabeledPaths);
        
        if (2*numLabeledPaths<numPaths)
        {
            println("\t\tthis is not a cluster (only "+numLabeledPaths+" of "+numPaths+" urls have labels)");
            return;
        }
        
        int maxHits=0;
        
        Iterator<Map.Entry<label,Integer>> it=counter.entrySet().iterator();
        for (int i=counter.size();i>0;i--)
        {
            Map.Entry<label,Integer> e=it.next();
            int num=e.getValue();
            
//            System.out.println("\t"+l+":"+num+" hits");
            
            if (maxHits<num) maxHits=num;
        }
        
//        double confidence=((double)maxHits)/numLabeledPaths;
        double confidence=((double)maxHits)/numPaths;
        
        if (confidence>=CLUSTER_CONFIDENCE_THRESHOLD)
        {
            println("\t\tcluster identified (cluster confidence="+confidence+")");
            
            clusteredPaths.addAll(clusterPaths);
            
            consolidate(prefix+"*]"+suffix,clusterPaths);
        }
        else
            println("\t\tthis is not a cluster (cluster confidence="+confidence+")");
    }
    
    private void writeNewFact(String url, label l)
    {
//        System.out.println("writeNewFact("+url+",wordnet_"+l.concept+"_"+l.id+","+l.confidence+","+currentList+")");
        
        if (true) return;
        
        try
        {
            writeNewFact.setString(1,url);
            writeNewFact.setString(2,"wordnet_"+l.concept+"_"+l.id);
            writeNewFact.setDouble(3,l.confidence);
            writeNewFact.setString(4, currentList);
            
            writeNewFact.execute();
        }
        catch(Exception e)
        {
            System.out.println("cannot write fact "+url+"->"+l+":\n"+e);
        }
    }
    
    final boolean PRINT=false;
    
    private void println(String text)
    {
        if (PRINT) System.out.println(text);
    }

    private void println()
    {
        if (PRINT) System.out.println();
    }

    private void print(String text)
    {
        if (PRINT) System.out.print(text);
    }

    private HashSet<label> baseTypes;
    
    final label LIVING_THING=new label("living_thing",3009);
    final label WHOLE=new label("whole",2645);
    final label THING=new label("thing",4253302);
    final label ACT=new label("act",26194);
    final label SUBSTANCE=new label("substance",17572);
    final label LOCATION=new label("location",22625);
    final label CAUSAL_AGENT=new label("causal_agent",5598);
    final label ABSTRACTION=new label("abstraction",20486);
    final label STATE=new label("state",24568);
    final label GROUP=new label("group",26769);
    final label EVENT=new label("event",25950);
    final label POSSESSION=new label("possession",27371);
    
    private void initIncompatibilities()
    {
        // list of the base types
        
        baseTypes=new HashSet<label>();
        baseTypes.add(LIVING_THING);
        baseTypes.add(WHOLE);
        baseTypes.add(THING);
        baseTypes.add(SUBSTANCE);
        baseTypes.add(LOCATION);
        baseTypes.add(CAUSAL_AGENT);
        baseTypes.add(ACT);
        baseTypes.add(ABSTRACTION);
        baseTypes.add(STATE);
        baseTypes.add(GROUP);
        baseTypes.add(EVENT);
        baseTypes.add(POSSESSION);
        
        incompatibilities=new HashMap<label,HashSet<label>>();
        
        // init incompatible base types
        
        // usage of this table: the list for an entry shows all other types that are incompatible
        // to this entry (and therefore should not be assigned to the same instance)
        
        // living_thing (subclass of object, which is in turn a subclass of entity)
        HashSet<label> s=new HashSet<label>();
        s.add(WHOLE);
        s.add(SUBSTANCE);
        s.add(STATE);
        s.add(THING);
        s.add(LOCATION);
        s.add(EVENT);
        s.add(ABSTRACTION); // unclear
        s.add(ACT);
        s.add(POSSESSION);
        incompatibilities.put(LIVING_THING, s);
        
        // thing & whole (seem to be similar classes)
        s=new HashSet<label>();
        s.add(LIVING_THING);
        s.add(SUBSTANCE);
        s.add(STATE);
        s.add(LOCATION);
        s.add(EVENT);
        s.add(ABSTRACTION); 
        s.add(ACT);
        s.add(POSSESSION);
        s.add(CAUSAL_AGENT);
        s.add(GROUP);
        incompatibilities.put(THING, s);
        incompatibilities.put(WHOLE, s);

        // substance 
        s=new HashSet<label>();
        s.add(LIVING_THING);
        s.add(WHOLE);
        s.add(THING);
        s.add(STATE);
        s.add(LOCATION);
        s.add(EVENT);
        s.add(ABSTRACTION);
        s.add(ACT);
        s.add(POSSESSION);
        s.add(GROUP);
        s.add(CAUSAL_AGENT);
        incompatibilities.put(SUBSTANCE, s);

        // location
        s=new HashSet<label>();
        s.add(LIVING_THING);
        s.add(WHOLE);
        s.add(THING);
        s.add(STATE);
        s.add(SUBSTANCE);
        s.add(EVENT);
        s.add(ABSTRACTION);
        s.add(ACT);
        s.add(POSSESSION);
        s.add(GROUP);
        s.add(CAUSAL_AGENT);
        incompatibilities.put(LOCATION, s);
        
        // causal_agent
        s=new HashSet<label>();
        s.add(WHOLE);
        s.add(THING);
        s.add(STATE);
        s.add(LOCATION);
        s.add(EVENT);
        s.add(ABSTRACTION);
        s.add(ACT);
        s.add(POSSESSION);
//        s.add(GROUP);
        incompatibilities.put(CAUSAL_AGENT, s);

        // group
        s=new HashSet<label>();
        s.add(WHOLE);
        s.add(THING);
        s.add(STATE);
        s.add(LOCATION);
        s.add(SUBSTANCE);
        s.add(EVENT);
        s.add(ABSTRACTION);
        s.add(ACT);
        s.add(POSSESSION);
        incompatibilities.put(GROUP, s);

        // abstraction (incompatible with all other base types)
        s=new HashSet<label>();
        s.add(WHOLE);
        s.add(THING);
        s.add(LIVING_THING);
        s.add(CAUSAL_AGENT);
        s.add(LOCATION);
        s.add(SUBSTANCE);
        s.add(STATE);
        s.add(ACT);
        s.add(POSSESSION);
        s.add(GROUP);
        incompatibilities.put(ABSTRACTION, s);

        // act (incompatible with all other base types)
        s=new HashSet<label>();
        s.add(WHOLE);
        s.add(THING);
        s.add(LIVING_THING);
        s.add(CAUSAL_AGENT);
        s.add(LOCATION);
        s.add(SUBSTANCE);
        s.add(STATE);
        s.add(EVENT);
        s.add(ABSTRACTION);
        s.add(POSSESSION);
        s.add(GROUP);
        incompatibilities.put(ACT, s);

        // state (incompatible with all other base types)
        s=new HashSet<label>();
        s.add(WHOLE);
        s.add(THING);
        s.add(LIVING_THING);
        s.add(LOCATION);
        s.add(CAUSAL_AGENT);
        s.add(SUBSTANCE);
        s.add(EVENT);
        s.add(ABSTRACTION);
        s.add(ACT);
        s.add(POSSESSION);
        s.add(GROUP);
        incompatibilities.put(STATE, s);

        // state (incompatible with all other base types)
        s=new HashSet<label>();
        s.add(WHOLE);
        s.add(THING);
        s.add(LIVING_THING);
        s.add(LOCATION);
        s.add(CAUSAL_AGENT);
        s.add(SUBSTANCE);
        s.add(STATE);
        s.add(ACT);
        s.add(POSSESSION);
        s.add(GROUP);
        incompatibilities.put(EVENT, s);

        // state (incompatible with all other base types)
        s=new HashSet<label>();
        s.add(WHOLE);
        s.add(THING);
        s.add(LIVING_THING);
        s.add(LOCATION);
        s.add(CAUSAL_AGENT);
        s.add(SUBSTANCE);
        s.add(EVENT);
        s.add(ABSTRACTION);
        s.add(ACT);
        s.add(STATE);
        s.add(GROUP);
        incompatibilities.put(POSSESSION, s);
    }
    
    private HashSet<label> getLabels(String url)
    {
        HashSet<label> labels=new HashSet<label>();
        
        try
        {
            this.readPageConceptMapping.setString(1,url);
            
            ResultSet rs=readPageConceptMapping.executeQuery();
            while (rs.next())
            {
                String concept=rs.getString(1);
                String id=rs.getString(2);
                double conf=rs.getDouble(3);
                
                Synset syn=null;
                
                try
                {
                    syn=dict.getSynsetAt(POS.NOUN, Long.parseLong(id));
                }
                catch(Exception e)
                {
                    System.out.println("cannot read synset for "+concept+"["+id+"]:\n"+e);
                }
                
                // move up the wordnet tree and collect other synsets

                TreeSet<Synset> synsets=new TreeSet<Synset>(new synsetComparator());
                synsets.add(syn);
                
                while (synsets.size()!=0)
                {
                    syn=synsets.first();
                    synsets.remove(syn);
                    
//                    System.out.print("-->"+syn+"<-");
                    
                    Pointer pointers[]=syn.getPointers(PointerType.HYPERNYM);
                    for (int k=0;k<pointers.length;k++)
                    {
                        try
                        {
                            Synset s=(Synset)pointers[k].getTargetSynset();
                            synsets.add(s);
                            
                            // find the corresponding pair
                            
                            label np=new label(syn.getWords()[0].getLemma(),(int)syn.getOffset(),conf);
                            labels.add(np);          
                        }
                        catch(Exception e)
                        {
                            System.out.println("strange exception:\n"+e);
                        }
                    }
                }
                
//                labels.add(new pair(rs.getString(1),rs.getString(2)));
            }
            rs.close();
        }
        catch(Exception e)
        {
            System.out.println("cannot load:\n"+e);
        }

        return labels;
    }
    
    private HashSet<label> getAndConsolidateLabels(String url)
    {
        // while loading the labels, we count
        // for each base type, the set of labels that belong to it
        
        HashMap<label,HashSet<label>> map=null;
        
        HashSet<label> readLabels=new HashSet<label>();
        
        try
        {
            this.readPageConceptMapping.setString(1,url);
            
            ResultSet rs=readPageConceptMapping.executeQuery();
            while (rs.next())
            {
                String concept=rs.getString(1);
                String id=rs.getString(2);
                double conf=rs.getDouble(3);

                readLabels.add(new label(concept,Integer.parseInt(id),conf));
            }
            rs.close();
        }
        catch(Exception e)
        {
            System.out.println("cannot load concepts for "+url+":\n"+e);
        }

        HashSet<label> labels=null;

        // build the complete set of labels (including ancestors in the WordNet tree)
        // until there are no incompatibilities left
        
        boolean incompatibilitiesFound=true;
        
        while (incompatibilitiesFound)
        {
            labels=new HashSet<label>();
            map=new HashMap<label,HashSet<label>>();
            
            Iterator<label> it=readLabels.iterator();
            for (int i=readLabels.size();i>0;i--)
            {
                label la=it.next();
                
                Synset syn=null;
                
                try
                {
                    syn=dict.getSynsetAt(POS.NOUN, la.id);
                }
                catch(Exception e)
                {
                    System.out.println("cannot read synset for "+la.concept+"["+la.id+"]:\n"+e);
                }
                
                TreeSet<Synset> synsets=new TreeSet<Synset>(new synsetComparator());
                synsets.add(syn);
                
                while (synsets.size()!=0)
                {
                    syn=synsets.first();
                    synsets.remove(syn);
                    
//                    System.out.println("-->"+syn+"<-");
                    
                    try
                    {
                        // find the corresponding pair
                        
                        String term=syn.getWords()[0].getLemma();
                        
//                        System.out.println("current term: "+term);
                        
                        label np=new label(term,(int)syn.getOffset(),la.confidence);
                        labels.add(np);
                        
                        if (baseTypes.contains(np))
                        {
                            HashSet<label> set=map.get(np);
                            if (set==null)
                            {
                                set=new HashSet<label>();
                                map.put(np,set);
                            }
                            set.add(la);
//                            System.out.println("add "+la+" to "+np);
                        }
                    }
                    catch(Exception e)
                    {
                        System.out.println("strange exception:\n"+e);
                    }

                    // move up the wordnet tree and collect other synsets

                    Pointer pointers[]=syn.getPointers(PointerType.HYPERNYM);
                    for (int k=0;k<pointers.length;k++)
                    {
                        try
                        {
                            Synset s=(Synset)pointers[k].getTargetSynset();
                            synsets.add(s);
                        }
                        catch(Exception e)
                        {
                            System.out.println("strange exception:\n"+e);
                        }
                    }
                    
                }
            }
            // now check for incompatibilities
            
            HashMap<label,HashSet<label>> numIncomp=new HashMap<label,HashSet<label>>();
            Iterator<Map.Entry<label, HashSet<label>>> typeit=map.entrySet().iterator();
            while (typeit.hasNext())
            {
                Map.Entry<label,HashSet<label>> e=typeit.next();
                
                Iterator<label> confit=incompatibilities.get(e.getKey()).iterator();
                while (confit.hasNext())
                {
                    label c=confit.next();
                    if (map.containsKey(c))
                    {
//                        System.out.println("found incomp: "+e.getKey()+"<->"+c+" ["+map.get(c).size()+"]");
                        HashSet<label> conflabels=numIncomp.get(e.getKey());
                        if (conflabels==null)
                        {
                            conflabels=new HashSet<label>();
                            numIncomp.put(e.getKey(),conflabels);
                        }
                        conflabels.addAll(map.get(c));
                        // in case a label is mapped to two different base types,
                        // remove it again as we don't count these cases
                        conflabels.removeAll(map.get(e.getKey()));
                    }
                }
            }
            
            if (numIncomp.size()>0)
            {
                // there was at least one incompatibility, so try to find
                // the type with most incompatibilities
                // (that's a heuristic to find the best set of nonconflicting labels)
                
                int maxConfs=0;
                int minConfs=Integer.MAX_VALUE;
                
                label maxConcept=null;
                
                Iterator<Map.Entry<label,HashSet<label>>> mit=numIncomp.entrySet().iterator();
                while (mit.hasNext())
                {
                    Map.Entry<label,HashSet<label>> e=mit.next();
                    int s=e.getValue().size();
//                    System.out.println("check: "+e.getKey()+"->"+s);
                    if (s>maxConfs)
                    {
                        maxConfs=s;
                        maxConcept=e.getKey();
                    }
                    
                    if (s<minConfs) minConfs=s;
                }
                
                if (maxConcept!=null)
                {
                    if (maxConfs==minConfs)
                    {
//                        System.out.println("there is no single max type, abort.");
                        incompatibilitiesFound=false;
                        break;
                    }
                    
                    // remove all labels that belong to the maximally conflicting type
                    // and restart the procedure
                    
//                    System.out.println("max conflicting type is "+maxConcept+" ["+maxConfs+" conflicts, min="+minConfs+"], remove it.");
                    readLabels.removeAll(map.get(maxConcept));
                    //map.remove(maxConcept);
                }
                else
                {
                    System.out.println("Huh? Something broken, abort.");
                    break;
                }
            }
            else
                incompatibilitiesFound=false;
        }
        
        return labels;
    }
    
    // check if adding a new label to an url's set of labels causes a new conflict,
    // and reject it if this is the case
    
    // the situation is similar to the getAndConsolidate() case
    // however, we do not change the oldLabels set, but find only
    // conflicts with the new label
    
    // note that this is probably not the most efficient implementation
    
    private boolean consolidateNewLabel(List<label> oldLabels, label newLabel)
    {
        // iterate through the old labels and count,
        // for each base type, the set of labels that belong to it
        
        HashMap<label,HashSet<label>> map=null;

        // the set of base types found for the new label
        HashSet<label> newLabelBaseTypes=new HashSet<label>();
        
        // this is the returned label set
        HashSet<label> labels=null;

        // build the complete set of labels (including ancestors in the WordNet tree)
        // until there are no incompatibilities left
        
        labels=new HashSet<label>();
        map=new HashMap<label,HashSet<label>>();
        
        Iterator<label> it=oldLabels.iterator();
        for (int i=oldLabels.size();i>0;i--)
        {
            label la=it.next();
            
            Synset syn=null;
            
            try
            {
                syn=dict.getSynsetAt(POS.NOUN, la.id);
            }
            catch(Exception e)
            {
                System.out.println("cannot read synset for "+la.concept+"["+la.id+"]:\n"+e);
            }
            
            TreeSet<Synset> synsets=new TreeSet<Synset>(new synsetComparator());
            synsets.add(syn);
            
            while (synsets.size()!=0)
            {
                syn=synsets.first();
                synsets.remove(syn);
                
//                    System.out.println("-->"+syn+"<-");
                
                try
                {
                    // find the corresponding pair
                    
                    String term=syn.getWords()[0].getLemma();
                    
//                        System.out.println("current term: "+term);
                    
                    label np=new label(term,(int)syn.getOffset(),la.confidence);
                    labels.add(np);
                    
                    if (baseTypes.contains(np))
                    {
                        HashSet<label> set=map.get(np);
                        if (set==null)
                        {
                            set=new HashSet<label>();
                            map.put(np,set);
                        }
                        set.add(la);
                        System.out.println("[new] add "+la+" to "+np);
                    }
                }
                catch(Exception e)
                {
                    System.out.println("strange exception:\n"+e);
                }

                // move up the wordnet tree and collect other synsets

                Pointer pointers[]=syn.getPointers(PointerType.HYPERNYM);
                for (int k=0;k<pointers.length;k++)
                {
                    try
                    {
                        Synset s=(Synset)pointers[k].getTargetSynset();
                        synsets.add(s);
                    }
                    catch(Exception e)
                    {
                        System.out.println("strange exception:\n"+e);
                    }
                }
                
            }
        }
        
        // after this step, labels() contains the expanded label set for the labels in oldLabels
        // (i.e., all concepts up to the WordNet roots)
        
        // expand the new label up to the WordNet roots
        // (we don't store the labels, we just store base types)
        
        Synset syn=null;
        
        try
        {
            syn=dict.getSynsetAt(POS.NOUN, newLabel.id);
        }
        catch(Exception e)
        {
            System.out.println("cannot read synset for "+newLabel.concept+"["+newLabel.id+"]:\n"+e);
        }
        
        TreeSet<Synset> synsets=new TreeSet<Synset>(new synsetComparator());
        synsets.add(syn);
        
        while (synsets.size()!=0)
        {
            syn=synsets.first();
            synsets.remove(syn);
            
//                System.out.println("-->"+syn+"<-");
            
            try
            {
                // find the corresponding pair
                
                String term=syn.getWords()[0].getLemma();
                
//                    System.out.println("current term: "+term);
                
                label np=new label(term,(int)syn.getOffset(),newLabel.confidence);
                labels.add(np);
                
                if (baseTypes.contains(np))
                {
                    newLabelBaseTypes.add(np);
                    System.out.println("add new "+newLabel+" to "+np);
                }
            }
            catch(Exception e)
            {
                System.out.println("strange exception:\n"+e);
            }

            // move up the wordnet tree and collect other synsets

            Pointer pointers[]=syn.getPointers(PointerType.HYPERNYM);
            for (int k=0;k<pointers.length;k++)
            {
                try
                {
                    Synset s=(Synset)pointers[k].getTargetSynset();
                    synsets.add(s);
                }
                catch(Exception e)
                {
                    System.out.println("strange exception:\n"+e);
                }
            }             
        }
        
        // now check for incompatibilities
        // it is enough if we find *any* incompatibility to reject this label
        // (this may be too restrictive)

        boolean incompatibilitiesFound=false;
        
        // iterate over all base types of the old labels
        Iterator<Map.Entry<label, HashSet<label>>> typeit=map.entrySet().iterator();
        while (typeit.hasNext())
        {                
            Map.Entry<label,HashSet<label>> e=typeit.next();
            
            // iterate over all incompatible base types for this base type
            Iterator<label> confit=incompatibilities.get(e.getKey()).iterator();
            while (confit.hasNext())
            {
                label c=confit.next();
                if (newLabelBaseTypes.contains(c))
                {
                    System.out.println("\tfound incomp: "+e.getKey()+"<->"+c);
                    // we reject *only* if the old labels do not already contain
                    // the conflicting base type
                    // (this can happen if the cleanup did not find a single
                    // maximal base type to delete)
                    
                    if (map.containsKey(c)==false)
                    {
                        incompatibilitiesFound=true;
                        break;
                    }
                }
            }
            if (incompatibilitiesFound) break;
        }
        
        if (incompatibilitiesFound)
        {
//            System.out.println("\treject new label.");
            return false;
        }
        else
        {
            List<label> result=new LinkedList<label>(oldLabels);
            result.add(newLabel);
            return true;
        }
    }

}
