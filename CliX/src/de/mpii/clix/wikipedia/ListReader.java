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

public class ListReader extends DefaultHandler
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

    net.didion.jwnl.dictionary.Dictionary dict;
    PointerUtils putils;

    // global data structure that maps a Wiki url to its labels
    private HashMap<String,List<label>> url2label;
    
    public ListReader()
    {
        dict=net.didion.jwnl.dictionary.Dictionary.getInstance();
        putils=PointerUtils.getInstance();
        
        elementStack=new LinkedList<element>();
        
        url2label=new HashMap<String,List<label>>();
        
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
    }

    public static void main(String[] args)
    {
        ListReader handler=null;
        
        try
        {
            JWNL.initialize(new FileInputStream("file_properties.xml"));
        }
        catch(Exception e)
        {
            System.out.println("JWNL init failed:\n"+e);
        }
        
        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+G$ermans.xml"; // works well
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
//        String inputfile="\\\\mpiat5316\\Wikipedia\\pages\\Li\\List+of+S$tanford+U$niversity+people.xml"; // not enough annotated data
        
        
        try
        {
            SAXParserFactory factory = SAXParserFactory.newInstance();
            SAXParser parser = factory.newSAXParser();
            handler = new ListReader();
            factory.setFeature("http://apache.org/xml/features/validation/schema",true);
            
            parser.parse(inputfile,handler);
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
        
        handler.show();
        
//        handler.consolidateLabels();
        
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
        String id;
        
        double confidence=1.0; //default, may be overridden by constructor
        
        public label(String c, String i)
        {
            concept=c;
            id=i;
        }
        
        public label (String c, String i, double conf)
        {
            this(c,i);
            confidence=conf;
        }
        
        public boolean equals(Object o)
        {
            if (o instanceof label)
            {
                label p=(label)o;
                return ((concept.equals(p.concept)&&(id.equals(p.id))));
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
            
            // hard-coded structure of Wiki lists: //entry/link[1]
            // if it does not match this structure, forget it
            // (this is a conservative pattern matcher; we may want to
            //  add some learning process here in the future)
            
            if (el.pos!=1) return;
            element parent=elementStack.peek();
            if (parent==null) return;
            if (parent.tag.compareTo("entry")!=0)
            {
                if (parent.tag.compareTo("it")==0)
                {
                    if (elementStack.size()>1)
                    {
                        element grandparent=elementStack.get(1);
                        if (grandparent.tag.compareTo("entry")!=0)
                            return;
                    }
                }
                else return;
            }
            
            // hard-coded workaround for years that are really often linked to in lists
            
            char test=el.target.charAt(3);
            if (Character.isDigit(test)) return;
            
            // read, for each concept mapped to this link's target,
            // all hypernyms from wordnet and add them to the
            // target url's label set
            
            HashSet<label> labels=new HashSet<label>();
            try
            {
                this.readPageConceptMapping.setString(1,el.target);
                
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
                        
//                        System.out.print("-->"+syn+"<-");
                        
                        Pointer pointers[]=syn.getPointers(PointerType.HYPERNYM);
                        for (int k=0;k<pointers.length;k++)
                        {
                            try
                            {
                                Synset s=(Synset)pointers[k].getTargetSynset();
                                synsets.add(s);
                                
                                // find the corresponding pair
                                
                                label np=new label(syn.getWords()[0].getLemma(),Long.toString(syn.getOffset()),conf);
                                labels.add(np);                    
                            }
                            catch(Exception e)
                            {
                                System.out.println("strange exception:\n"+e);
                            }
                        }
                    }
                    
//                    labels.add(new pair(rs.getString(1),rs.getString(2)));
                }
                rs.close();
            }
            catch(Exception e)
            {
                System.out.println("cannot load:\n"+e);
            }
            
            System.out.print(el.xpath+"\t"+el.target+"\t");
            Iterator<label> it=labels.iterator();
            for (int i=0;i<labels.size();i++)
            {
                label p=it.next();
                if (i>0) System.out.print(",");
                System.out.print(p);
            }
            System.out.println();
            
            url2label.put(el.target,new LinkedList<label>(labels));
            
            el.urls.add(el.target);
            
            Iterator<label> labelit=labels.iterator();
            while (labelit.hasNext())
            {
                label l=labelit.next();
                el.labels.put(l, l.confidence);
            }
        }
        
        // the element is finished here.
        // we have to
        // (1) consolidate it (i.e., consider the descendants' labels and urls
        //     and compute new labels from them
        // (2) move the labels and the urls to the parent, if there is one
        
        // consolidation
        
        consolidate(el);
        
        // move
        
        if (elementStack.isEmpty()==false) move(el,elementStack.peek());
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
            
//            for (int i=arg3.getLength();i>=0;i--)
//                System.out.println("1-"+arg3.getURI(i)+"\t2-"+arg3.getLocalName(i)+"\t3-"+arg3.getQName(i));
        }
    }
    
    final static int MINSUPPORT=3;
    final static double THRESHOLD=0.5;
    
//    private void consolidateLabels()
//    {
//        // find all link[1]-instances and group them by prefix
//        
//        Set<String> allPaths=new HashSet<String>(url2label.keySet());
//        
//        Iterator<String> it=allPaths.iterator();
//        while (it.hasNext())
//        {
//            String path=it.next();
//  
////            String path="/article[1]/bdy[1]/sec[13]/p[1]/list[1]/entry[22]/link[1]";
//            
//            // check if we already removed this path
//            if (url2label.containsKey(path)==false) continue;
//            
//            // chop the path in its steps and try to find matches
//            
//            int index=0;
//            
//            while (index!=-1)
//            {
//                int next=path.substring(index).indexOf('[');
//                if (next==-1) break;
//                
//                index+=next;
//                int targetindex=index+path.substring(index).indexOf(']');
//                
//                // 0..index-1 is the prefix, targetindex+1..len is the suffix
//                
//                String prefix=path.substring(0,index+1);
//                String suffix=path.substring(targetindex);
//                
//                System.out.println(prefix+"-"+suffix);
//                
//                if (prefix.endsWith("entry[")==false)
//                {
//                    index++; continue;
//                }
//                
//                // do something with this prefix/suffix pair
//                
//                HashSet<String> matches=new HashSet<String>();
//                
//                Iterator<String> pit=url2label.keySet().iterator();
//                while (pit.hasNext())
//                {
//                    String candidate=pit.next();
//                    if (candidate.startsWith(prefix)&&(candidate.endsWith(suffix)))
//                        matches.add(candidate);
//                }
//                
//                if (matches.size()>=MINSUPPORT)
//                {
//                    System.out.println("found a potential cluster (size "+matches.size()+"), checking");
//                    Iterator<String> mit=matches.iterator();
//                    while (mit.hasNext())
//                    {
//                        String m=mit.next();
//                        HashSet<label> p=url2label.get(m);
//                        int i=0;
//                        Iterator<label> pairit=p.iterator();
//                        while (pairit.hasNext())
//                        {
//                            if (i>0) System.out.print(",");
//                            System.out.print(pairit.next());
//                            i++;
//                        }
//                        System.out.println();
//                    }
//                    
//                    // assume matches, so remove matched paths
//                    
//                    mit=matches.iterator();
//                    while (mit.hasNext())
//                    {
//                        url2label.remove(mit.next());
//                    }
//                }
//                
//                index++;
//                
//            }
//        }
//        
//    }

    // consider the collected set of labels and urls and compute new labels from them,
    // if thresholds are met
    
    private void consolidate(element e)
    {        
        // compute the number of urls among the descendants of this element
        // this is the *new* algorithm, the *old* algorithm simply used
        // the size of e.urls
        
        double urlCount=0;
        Iterator<String> uit=e.urls.iterator();
        
        for (int i=e.urls.size();i>0;i--)
            {
                String url=uit.next();
                List<label> list=url2label.get(url);
                if ((list!=null)&&(list.size()>0)) urlCount++;
            }

//        System.out.println("urlCount="+urlCount+", size="+e.urls.size());

        System.out.println("consolidate("+e+") ["+urlCount+" labeled urls, "+(e.urls.size()-urlCount)+" unlabeled urls, "+e.labels.size()+" labels]");

        if (urlCount<MINSUPPORT) return; // there are not enough labeled urls, so stop here
        
        Iterator<Map.Entry<label,Double>> labelit=e.labels.entrySet().iterator();
        
        while (labelit.hasNext())
        {
            Map.Entry<label,Double> entry=labelit.next();
            
            // check the support of this label
            
            double support=entry.getValue()/urlCount;
            
            label l=entry.getKey();
            if (support<THRESHOLD)
            {
                System.out.println("rejected "+l+": confidence="+support+" (from "+entry.getValue()+" examples)");
                continue;
            }
            
            System.out.println("accepted "+l+": confidence="+support+" (from "+entry.getValue()+" examples)");
                        
            // this label has a high support, so add it to all urls in the set

            Iterator<String> urlsit=e.urls.iterator();
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
                    if (la.equals(l))
                    {
                        found=true;
                        if (la.confidence<support) la.confidence=support;
                        break;
                    }
                }
                if (found==false) list.add(new label(l.concept,l.id,support));
            }
        }
    }
    
    private void move(element source, element target)
    {
        target.urls.addAll(source.urls);
        
        Iterator<Map.Entry<label,Double>> labelit=source.labels.entrySet().iterator();
        while (labelit.hasNext())
        {
            Map.Entry<label,Double> e=labelit.next();
            label l=e.getKey();
            double count=e.getValue();
            
            if (target.labels.containsKey(l))
            {
                target.labels.put(l, target.labels.get(l)+count);
            }
            else target.labels.put(l,count);
        }
    }
    
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
}
