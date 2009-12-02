package de.mpii.clix.imdb;

import java.io.*;
import java.sql.*;

import de.mpii.clix.support.*;

public class DB2XML
{
    // note the trailing '\\'
//	private static String DIR="c:\\projects\\imdb\\xml_reallynew\\";
	private static String DIR="e:\\IMDB2009\\";
	private static String dbUser="imdb2009";
	private static String dbPassword="imdb2009";
	private static String dbHost="mpiat5316.ag5.mpi-sb.mpg.de";
	private static String dbServiceName="destiny.mpi";
	
	private static String wikiBaseUrl="http://mpiao5500/wiki/wikipedia/";
	
	private Database db;
	
	private PreparedStatement selectMovieWithIDLimit;
	private PreparedStatement selectMovieByID;
	private PreparedStatement selectDirectorByMovieID;
	private PreparedStatement selectProducerByMovieID;
	private PreparedStatement selectComposerByMovieID;
	private PreparedStatement selectCostumeDesingerByMovieID;

	private PreparedStatement selectCastByMovieIDOrderByCredits;
	private PreparedStatement selectCastCountByMovieID;
	
	private PreparedStatement selectPlotByMovieID;
	private PreparedStatement selectKeywordsByMovieID;
	private PreparedStatement selectTaglinesByMovieID;
	private PreparedStatement selectGoofByMovieID;
	private PreparedStatement selectMovieTriviaByMovieID;
	private PreparedStatement selectGenresByMovieID;
	private PreparedStatement selectAlternateVersionsByMovieID;
	private PreparedStatement selectAlternativeTitlesByMovieID;
	private PreparedStatement selectCountryByMovieID;
	private PreparedStatement selectColourInfoByMovieID;
	private PreparedStatement selectSoundMixByMovieID;
	private PreparedStatement selectLanguageByMovieID;
	private PreparedStatement selectLocationByMovieID;

	private PreparedStatement selectPlotCountByMovieID;
	private PreparedStatement selectKeywordsCountByMovieID;
	private PreparedStatement selectTaglinesCountByMovieID;
	private PreparedStatement selectGoofCountByMovieID;
	private PreparedStatement selectMovieTriviaCountByMovieID;
	private PreparedStatement selectGenresCountByMovieID;
	private PreparedStatement selectAlternateVersionsCountByMovieID;
	private PreparedStatement selectAlternativeTitlesCountByMovieID;
	private PreparedStatement selectCountryCountByMovieID;
	
	private PreparedStatement selectMovieLinksByMovieID;
	private PreparedStatement selectMovieLinksCountByMovieID;

	private PreparedStatement selectPersonByID;
	private PreparedStatement selectFullBiographyByPersonID;
	private PreparedStatement selectActorTriviaByPersonID;
	private PreparedStatement selectActorTriviaCountByPersonID;
	private PreparedStatement selectBiographicalBooksByPersonID;
	private PreparedStatement selectBiographicalBooksCountByPersonID;
	private PreparedStatement selectBiographicalFilmsByPersonID;
	private PreparedStatement selectBiographicalFilmsCountByPersonID;
	private PreparedStatement selectAgentsByPersonID;
	private PreparedStatement selectAgentsCountByPersonID;
	private PreparedStatement selectArticlesByPersonID;
	private PreparedStatement selectArticlesCountByPersonID;
	private PreparedStatement selectAlternativeNamesByPersonID;
	private PreparedStatement selectAlternativeNamesCountByPersonID;
	private PreparedStatement selectCoverPhotosByPersonID;
	private PreparedStatement selectCoverPhotosCountByPersonID;
	private PreparedStatement selectGuestAppearancesByPersonID;
	private PreparedStatement selectGuestAppearancesCountByPersonID;
	private PreparedStatement selectInterviewsByPersonID;
	private PreparedStatement selectInterviewsCountByPersonID;
	private PreparedStatement selectOtherWorksByPersonID;
	private PreparedStatement selectOtherWorksCountByPersonID;
	private PreparedStatement selectPictorialsByPersonID;
	private PreparedStatement selectPictorialsCountByPersonID;
	private PreparedStatement selectPortraitsByPersonID;
	private PreparedStatement selectPortraitsCountByPersonID;
	private PreparedStatement selectSalariesByPersonID;
	private PreparedStatement selectSalariesByPersonIDMovieID;
	private PreparedStatement selectSalariesCountByPersonID;
	private PreparedStatement selectSpousesByPersonID;
	private PreparedStatement selectSpousesCountByPersonID;
	private PreparedStatement selectTrademarksByPersonID;
	private PreparedStatement selectTrademarksCountByPersonID;
	private PreparedStatement selectWhereNowByPersonID;
	private PreparedStatement selectWhereNowCountByPersonID;
	private PreparedStatement selectActorQuotesByPersonID;
	private PreparedStatement selectActorQuotesCountByPersonID;
	
	private PreparedStatement selectActsInMovieByPersonID;
	private PreparedStatement selectActsInMovieCountByPersonID;
	private PreparedStatement selectDirectsMovieByPersonID;
	private PreparedStatement selectDirectsMovieCountByPersonID;
	private PreparedStatement selectProducesMovieByPersonID;
	private PreparedStatement selectProducesMovieCountByPersonID;
	private PreparedStatement selectComposesMovieByPersonID;
	private PreparedStatement selectComposesMovieCountByPersonID;
	private PreparedStatement selectCostumeDesignsMovieByPersonID;
	private PreparedStatement selectCostumeDesignsMovieCountByPersonID;
	private PreparedStatement selectEditsMovieByPersonID;
	private PreparedStatement selectEditsMovieCountByPersonID;
	private PreparedStatement selectCinematographsMovieByPersonID;
	private PreparedStatement selectCinematographsMovieCountByPersonID;
	
	private PreparedStatement selectMiscByMovieID;
	private PreparedStatement selectMiscCountByMovieID;
	private PreparedStatement selectMiscByPersonID;
	private PreparedStatement selectMiscCountByPersonID;
	
	private PreparedStatement selectAlternativePersonID;
	private PreparedStatement selectPersonID;

	private PreparedStatement selectMovieID;
	private PreparedStatement selectMovieIDNULL;
	private PreparedStatement selectAlternativeMovieID;
	private PreparedStatement selectAlternativeMovieIDNULL;

	private String movieTypes[]={"","","TV","TV Series","TV Mini Series","Video","Video Game"};

	private String XMLTags_MovieType[]={"","movie","tv","tvseries","tvminiseries","video","videogame"};
	
	public static void main(String[] args)
	{
		System.out.println("DB2XML started.");
		
		DB2XML exporter=new DB2XML();
		
		exporter.initDatabase();
		
//		exporter.initFileSystem();
		
//		exporter.exportMovies();

		exporter.exportPersons();

		System.out.println("DB2XML finished.");
	}
	
	private void initDatabase()
	{
	    db=new Database();
	    db.openDatabase(dbUser,dbPassword,dbHost,dbServiceName);
	    
	    createTables();
	}
	
	private void createTables()
	{
	 	try
	  	{
			selectMovieWithIDLimit=db.prepareStatement("SELECT MovieID,Title,Year,Addition,Type,Episode FROM Movies WHERE MovieID<?");
			selectMovieByID=db.prepareStatement("SELECT Title,Year,Addition,Type,Episode FROM Movies WHERE MovieID=?");

			selectDirectorByMovieID=db.prepareStatement("SELECT Name,DirectorID From DirectsMovie D, Persons P WHERE D.MovieID=? AND D.DirectorID=P.PersonID");	
			selectProducerByMovieID=db.prepareStatement("SELECT Name,ProducerID,Extra From producesMovie D, Persons P WHERE D.MovieID=? AND D.ProducerID=P.PersonID");	

			selectCastByMovieIDOrderByCredits=db.prepareStatement("SELECT Name,PersonID,role,position FROM Persons P, ActsInMovie A WHERE P.PersonID=A.ActorID AND A.MovieID=? ORDER BY A.Position ASC");
			selectCastCountByMovieID=db.prepareStatement("SELECT count(*) FROM ActsInMovie A WHERE A.MovieID=?");

			selectPlotByMovieID=db.prepareStatement("SELECT author,plot FROM Plots WHERE MovieID=?");
			selectKeywordsByMovieID=db.prepareStatement("SELECT keyword FROM Keywords WHERE MovieID=?");
			selectTaglinesByMovieID=db.prepareStatement("SELECT tagline FROM taglines WHERE MovieID=?");
			selectGoofByMovieID=db.prepareStatement("SELECT type,goof FROM goofs WHERE MovieID=?");
			selectMovieTriviaByMovieID=db.prepareStatement("SELECT trivia FROM movietrivia WHERE MovieID=?");
			selectGenresByMovieID=db.prepareStatement("SELECT genre FROM Genres WHERE MovieID=?");
			selectAlternateVersionsByMovieID=db.prepareStatement("SELECT version FROM AlternateVersions WHERE MovieID=?");
			selectAlternativeTitlesByMovieID=db.prepareStatement("SELECT alternativetitle FROM AlternativeTitles WHERE MovieID=?");
			selectCountryByMovieID=db.prepareStatement("SELECT country FROM countries WHERE MovieID=? order by position");
			selectColourInfoByMovieID=db.prepareStatement("SELECT colourinfo FROM colourinfo WHERE MovieID=?");
			selectSoundMixByMovieID=db.prepareStatement("SELECT soundmix FROM soundmix WHERE MovieID=?");
			selectLanguageByMovieID=db.prepareStatement("SELECT language FROM languages WHERE MovieID=? order by position");
			selectLocationByMovieID=db.prepareStatement("SELECT location FROM locations WHERE MovieID=?");

			selectKeywordsCountByMovieID=db.prepareStatement("SELECT COUNT(*) FROM Keywords WHERE MovieID=?");
			selectTaglinesCountByMovieID=db.prepareStatement("SELECT COUNT(*) FROM Taglines WHERE MovieID=?");
			selectGoofCountByMovieID=db.prepareStatement("SELECT count(*) FROM goofs WHERE MovieID=?");
			selectMovieTriviaCountByMovieID=db.prepareStatement("SELECT count(*) FROM movietrivia WHERE MovieID=?");
			selectGenresCountByMovieID=db.prepareStatement("SELECT COUNT(*) FROM Genres WHERE MovieID=?");
			selectAlternateVersionsCountByMovieID=db.prepareStatement("SELECT COUNT(*) FROM AlternateVersions WHERE MovieID=?");
			selectAlternativeTitlesCountByMovieID=db.prepareStatement("SELECT COUNT(*) FROM AlternativeTitles WHERE MovieID=?");
			selectCountryCountByMovieID=db.prepareStatement("SELECT count(*) FROM countries WHERE MovieID=?");

			selectMovieLinksByMovieID=db.prepareStatement("SELECT targetID,L.type,title,year,addition,M.type,episode FROM MovieLinks L, Movies M WHERE SourceID=? AND TargetID=MovieID");

			selectMovieLinksCountByMovieID=db.prepareStatement("SELECT COUNT(*) FROM MovieLinks WHERE SourceID=?");

			selectPersonByID=db.prepareStatement("SELECT name,sex,RealName,NickName,DateBirth,PlaceBirth,DateDeath,PlaceDeath,CauseDeath,Height FROM Persons WHERE PersonID=?");

			selectFullBiographyByPersonID=db.prepareStatement("SELECT author,biography FROM FullBiographies WHERE PersonID=?");
			selectActorTriviaByPersonID=db.prepareStatement("SELECT trivia FROM actortrivia WHERE personID=?");
			selectActorTriviaCountByPersonID=db.prepareStatement("SELECT count(*) FROM actortrivia WHERE personID=?");
			selectActorQuotesByPersonID=db.prepareStatement("SELECT quote FROM actorquotes WHERE personID=?");
			selectActorQuotesCountByPersonID=db.prepareStatement("SELECT count(*) FROM actorquotes WHERE personID=?");
			selectAgentsByPersonID=db.prepareStatement("SELECT agent FROM agents WHERE personID=?");
			selectAgentsCountByPersonID=db.prepareStatement("SELECT count(*) FROM agents WHERE personID=?");
			selectAlternativeNamesByPersonID=db.prepareStatement("SELECT alternativename FROM alternativenames WHERE personID=?");
			selectAlternativeNamesCountByPersonID=db.prepareStatement("SELECT count(*) FROM alternativenames WHERE personID=?");
			selectArticlesByPersonID=db.prepareStatement("SELECT article FROM articles WHERE personID=?");
			selectArticlesCountByPersonID=db.prepareStatement("SELECT count(*) FROM articles WHERE personID=?");
			selectBiographicalBooksByPersonID=db.prepareStatement("SELECT book FROM biographicalbooks WHERE personID=?");
			selectBiographicalBooksCountByPersonID=db.prepareStatement("SELECT count(*) FROM biographicalbooks WHERE personID=?");
			selectBiographicalFilmsByPersonID=db.prepareStatement("SELECT film FROM biographicalfilms WHERE personID=?");
			selectBiographicalFilmsCountByPersonID=db.prepareStatement("SELECT count(*) FROM biographicalfilms WHERE personID=?");
			selectCoverPhotosByPersonID=db.prepareStatement("SELECT coverphoto FROM coverphotos WHERE personID=?");
			selectCoverPhotosCountByPersonID=db.prepareStatement("SELECT count(*) FROM coverphotos WHERE personID=?");
			selectGuestAppearancesByPersonID=db.prepareStatement("SELECT guestappearance FROM guestappearances WHERE personID=?");
			selectGuestAppearancesCountByPersonID=db.prepareStatement("SELECT count(*) FROM guestappearances WHERE personID=?");
			selectInterviewsByPersonID=db.prepareStatement("SELECT interview FROM interviews WHERE personID=?");
			selectInterviewsCountByPersonID=db.prepareStatement("SELECT count(*) FROM interviews WHERE personID=?");
			selectOtherWorksByPersonID=db.prepareStatement("SELECT work FROM otherworks WHERE personID=?");
			selectOtherWorksCountByPersonID=db.prepareStatement("SELECT count(*) FROM otherworks WHERE personID=?");
			selectPictorialsByPersonID=db.prepareStatement("SELECT pictorial FROM pictorials WHERE personID=?");
			selectPictorialsCountByPersonID=db.prepareStatement("SELECT count(*) FROM pictorials WHERE personID=?");
			selectPortraitsByPersonID=db.prepareStatement("SELECT portrait FROM portraits WHERE personID=?");
			selectPortraitsCountByPersonID=db.prepareStatement("SELECT count(*) FROM portraits WHERE personID=?");
			selectSalariesByPersonID=db.prepareStatement("SELECT movieid,salary FROM salaries WHERE personID=?");
			selectSalariesByPersonIDMovieID=db.prepareStatement("SELECT salary FROM salaries WHERE personID=? AND movieID=?");
			selectSalariesCountByPersonID=db.prepareStatement("SELECT count(*) FROM salaries WHERE personID=?");
			selectSpousesByPersonID=db.prepareStatement("SELECT spouse FROM spouses WHERE personID=?");
			selectSpousesCountByPersonID=db.prepareStatement("SELECT count(*) FROM spouses WHERE personID=?");
			selectTrademarksByPersonID=db.prepareStatement("SELECT trademark FROM trademarks WHERE personID=?");
			selectTrademarksCountByPersonID=db.prepareStatement("SELECT count(*) FROM trademarks WHERE personID=?");
			selectWhereNowByPersonID=db.prepareStatement("SELECT wherenow FROM wherenow WHERE personID=?");
			selectWhereNowCountByPersonID=db.prepareStatement("SELECT count(*) FROM wherenow WHERE personID=?");
			
			selectActsInMovieCountByPersonID=db.prepareStatement("SELECT COUNT(*) FROM actsinmovie WHERE ActorID=?");
			selectActsInMovieByPersonID=db.prepareStatement("SELECT movieid,role FROM ActsInMovie WHERE ActorID=?");
			selectDirectsMovieCountByPersonID=db.prepareStatement("SELECT COUNT(*) FROM directsmovie WHERE directorID=?");
			selectDirectsMovieByPersonID=db.prepareStatement("SELECT movieid FROM directsMovie WHERE directorID=?");
			selectProducesMovieCountByPersonID=db.prepareStatement("SELECT COUNT(*) FROM producesmovie WHERE producerID=?");
			selectProducesMovieByPersonID=db.prepareStatement("SELECT movieid,extra FROM producesMovie WHERE producerID=?");
			selectComposesMovieCountByPersonID=db.prepareStatement("SELECT COUNT(*) FROM composesformovie WHERE composerID=?");
			selectComposesMovieByPersonID=db.prepareStatement("SELECT movieid FROM composesforMovie WHERE composerID=?");
			selectCostumeDesignsMovieCountByPersonID=db.prepareStatement("SELECT COUNT(*) FROM costumedesignsmovie WHERE designerID=?");
			selectCostumeDesignsMovieByPersonID=db.prepareStatement("SELECT movieid FROM costumedesignsMovie WHERE designerID=?");
			selectEditsMovieCountByPersonID=db.prepareStatement("SELECT COUNT(*) FROM editsmovie WHERE editorID=?");
			selectEditsMovieByPersonID=db.prepareStatement("SELECT movieid FROM editsMovie WHERE editorID=?");
			selectCinematographsMovieCountByPersonID=db.prepareStatement("SELECT COUNT(*) FROM cinematographsmovie WHERE cinematographerID=?");
			selectCinematographsMovieByPersonID=db.prepareStatement("SELECT movieid FROM cinematographsMovie WHERE cinematographerID=?");
			
			selectPersonID=db.prepareStatement("SELECT PersonID FROM Persons WHERE name=?");
			selectAlternativePersonID=db.prepareStatement("SELECT PersonID FROM AlternativeNames WHERE alternativename=?");

			selectMovieID=db.prepareStatement("SELECT MovieID FROM Movies WHERE title=? and year=? and addition=? and type=?");
			selectMovieIDNULL=db.prepareStatement("SELECT MovieID FROM Movies WHERE title=? and year=? and addition IS NULL and type=?");		
			selectAlternativeMovieID=db.prepareStatement("SELECT MovieID FROM AlternativeTitles WHERE alternativetitle=? and year=? and addition=? and type=?");
			selectAlternativeMovieIDNULL=db.prepareStatement("SELECT MovieID FROM AlternativeTitles WHERE alternativetitle=? and year=? and addition IS NULL and type=?");		

			selectMiscByMovieID=db.prepareStatement("SELECT P.PersonID,job,M.extra,P.name FROM Miscellaneous M, Persons P WHERE MovieID=? AND M.PersonID=P.PersonID");
			selectMiscCountByMovieID=db.prepareStatement("SELECT COUNT(*) FROM Miscellaneous WHERE MovieID=?");
			selectMiscByPersonID=db.prepareStatement("SELECT MovieID,job,extra FROM Miscellaneous WHERE PersonID=?");
			selectMiscCountByPersonID=db.prepareStatement("SELECT COUNT(*) FROM Miscellaneous WHERE PersonID=?");

			getWikiLink=db.prepareStatement("SELECT url FROM WikiUrls WHERE url=?");
	  	}
	  	catch(Exception e)
	  	{
	  	    e.printStackTrace();
		  	System.exit(1);
	  	}
	} 
	
	private void initFileSystem()
	{
		int moviecnt=0;
		int personcnt=0;
		
		try
		{
			ResultSet rs=db.execQuery("SELECT MAX(MovieID) FROM Movies");
			if (rs.next())
			{
				moviecnt=rs.getInt(1);
			}
			rs.close();
			
			File movies=new File(DIR+"movies");
			movies.mkdir();
			
			int maxid=moviecnt/1000;
			
			for (int i=0; i<=maxid; i++)
			{
				movies=new File(DIR+"movies\\"+i);
				movies.mkdir();			
			}
			
		}
		catch(Exception e)
		{
			System.out.println("create movie dir: "+e);
			e.printStackTrace();
		}

		try
		{
			ResultSet rs=db.execQuery("SELECT MAX(PersonID) FROM Persons");
			if (rs.next())
			{
				personcnt=rs.getInt(1);
			}
			rs.close();
			
			File people=new File(DIR+"people");
			people.mkdir();

			int maxid=personcnt/1000;
			
			for (int i=0; i<=maxid; i++)
			{
				people=new File(DIR+"people\\"+i);
				people.mkdir();			
			}
		}
		catch(Exception e)
		{
			System.out.println("create people dir: "+e);
			e.printStackTrace();
		}
	}
	
	private void exportMovies()
	{
	    int moviecnt=0;
		try
		{
			ResultSet rs=db.execQuery("SELECT MAX(MovieID) FROM Movies");
			if (rs.next())
			{
				moviecnt=rs.getInt(1);
			}
			rs.close();
		}
		catch(Exception e)
		{
		    System.out.println("cannot read number of movies from database:\n"+e);
		    System.exit(1);
		}
		
		for (int i=1;i<=moviecnt;i++)
		    exportMovie(i);
		    
		    
//		exportMovie(65236);
	}
	
	static private String hostname="localhost";

	static
	{
		try
		{
			java.net.InetAddress localMachine = java.net.InetAddress.getLocalHost();	
			hostname=localMachine.getHostName();
			if (hostname.indexOf('.')!=-1) hostname=hostname.substring(0,hostname.indexOf('.'));
		}
		catch(java.net.UnknownHostException uhe)
		{
			//handle exception
		}
	}
	
	private void exportMovie(int id)
	{
		String title=null;
		int year=-1;
		String addition=null;
		int type=-1;
		String episode=null;
		
		File xmlfile=null;
		FileWriter xml=null;
	
		if (id%500==0) System.out.println("exportMovie("+id+")");
			
		try
		{
			selectMovieByID.setInt(1,id);
			
			ResultSet rs=selectMovieByID.executeQuery();
			if (rs.next())
			{
				title=rs.getString(1);
				year=rs.getInt(2);
				addition=rs.getString(3);
				type=rs.getInt(4);
				episode=rs.getString(5);
			}
			rs.close();
			
			xmlfile=new File(DIR+"movies\\"+id2Filename(id));
			xml=new FileWriter(xmlfile);
			
			xml.write("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
			xml.write("<!-- generated by CLiX/IMDB2XML [MPI-Inf, MMCI@UdS] $LastChangedRevision: 93 $ on "+Utils.getCurrentDate()+"["+hostname+"] -->\n");
			//xml.write("DOCTYPE movie SYSTEM \"..\\imdb.dtd\">");
			
			xml.write("<movie id=\""+id+"\">\n");
			
			xml.write("\t<title>"+XML.XMLify2(title)+"</title>\n");

			if (episode!=null) xml.write("\t<episode>"+XML.XMLify2(episode)+"</episode>\n");
			
			selectAlternativeTitlesCountByMovieID.setInt(1,id);
			rs=selectAlternativeTitlesCountByMovieID.executeQuery();
			int alttCount=0;
			if (rs.next()) alttCount=rs.getInt(1);
			rs.close();
			
			if (alttCount>0)
			{
				selectAlternativeTitlesByMovieID.setInt(1,id);
				rs=selectAlternativeTitlesByMovieID.executeQuery();
				while (rs.next())
				{
					String tagline=rs.getString(1);
					xml.write("\t<alternative_title>"+XML.XMLify2(tagline)+"</alternative_title>\n");
				}
				rs.close();					
			}

			// experimental Wikipedia support
			
//			xml.write(conditionalGenerateWikiLink(title));
			
			xml.write("\t<production_year>"+year+"</production_year>\n");
			if (addition!=null) xml.write("\t<uniqueid>"+addition+"</uniqueid>\n");
			if (type>1) xml.write("\t<type>"+movieTypes[type]+"</type>\n");

			selectCountryCountByMovieID.setInt(1,id);
			rs=selectCountryCountByMovieID.executeQuery();
			int countryCount=0;
			if (rs.next()) countryCount=rs.getInt(1);
			rs.close();
			
			if (countryCount>0)
			{
				if (countryCount>1) xml.write("\t<production_countries>\n");
				
				selectCountryByMovieID.setInt(1,id);
				rs=selectCountryByMovieID.executeQuery();
				while (rs.next())
				{
					String country=rs.getString(1);
					xml.write("\t\t<production_country>"+XML.XMLify2(country)+"</production_country>\n");
				}
				rs.close();					
	
				if (countryCount>1) xml.write("\t</production_countries>\n");
			}

			selectLanguageByMovieID.setInt(1,id);
			rs=selectLanguageByMovieID.executeQuery();
			while (rs.next())
			{
				String language=rs.getString(1);
				xml.write("\t\t<production_language>"+XML.XMLify2(language)+"</production_language>\n");
			}
			rs.close();					
			
			selectLocationByMovieID.setInt(1,id);
			rs=selectLocationByMovieID.executeQuery();
			while (rs.next())
			{
				String location=rs.getString(1);
				xml.write("\t\t<production_location>"+XML.XMLify2(location)+"</production_location>\n");
			}
			rs.close();				
			
			selectDirectorByMovieID.setInt(1,id);
			rs=selectDirectorByMovieID.executeQuery();
			if (rs.next())
			{
				String director=rs.getString(1);
				int directorID=rs.getInt(2);
			
				xml.write("\t<director xmlns:xlink=\""+XML.nsXLink+"\" xlink:type=\"simple\" xlink:href=\""+makeM2PLink(directorID)+"\">\n");	
				xml.write("\t\t"+XML.XMLify2(director)+"\n\t</director>\n");
			}
			rs.close();

			selectProducerByMovieID.setInt(1,id);
			rs=selectProducerByMovieID.executeQuery();
			if (rs.next())
			{
				String producer=rs.getString(1);
				int producerID=rs.getInt(2);
				String job=rs.getString(3);
			
				xml.write("\t<producer xmlns:xlink=\""+XML.nsXLink+"\" xlink:type=\"simple\" xlink:href=\""+makeM2PLink(producerID)+"\">\n");	
				xml.write("\t\t<name>"+XML.XMLify2(producer)+"</name>\n");
				if (job!=null) xml.write("\t\t<job>"+XML.XMLify2(job)+"</job>\n");
				xml.write("\t</producer>\n");
			}
			rs.close();
			
			selectCastCountByMovieID.setInt(1,id);
			rs=selectCastCountByMovieID.executeQuery();
			int castCount=0;
			if (rs.next()) castCount=rs.getInt(1);
			rs.close();
			
			if (castCount>0)
			{
				xml.write("\t<cast order=\"credits\">\n");
			
				selectCastByMovieIDOrderByCredits.setInt(1,id);
				rs=selectCastByMovieIDOrderByCredits.executeQuery();
				while (rs.next())
				{
					String actor=rs.getString(1);
					int actorID=rs.getInt(2);
					String role=rs.getString(3);
					int position=rs.getInt(4);
				
					xml.write("\t\t<casting");
					if (position>0) xml.write(" position=\""+position+"\"");
					xml.write(">\n");
					xml.write("\t\t\t<actor xmlns:xlink=\""+XML.nsXLink+"\" xlink:type=\"simple\" xlink:href=\""+makeM2PLink(actorID)+"\">");	
					xml.write(XML.XMLify2(actor)+"</actor>\n");
//					System.out.println("about to write role '"+role+"' ("+XML.XMLify2(role)+")");
					
					if (role!=null) xml.write("\t\t\t<role>"+XML.XMLify2(role)+"</role>\n");
					xml.write("\t\t</casting>\n");
				}
				rs.close();					
	
				xml.write("\t</cast>\n");
			}

			selectMiscCountByMovieID.setInt(1,id);
			rs=selectMiscCountByMovieID.executeQuery();
			int miscCount=0;
			if (rs.next()) miscCount=rs.getInt(1);
			rs.close();
			
			if (miscCount>0)
			{
				if (miscCount>1) xml.write("\t<misc>\n");
				
				selectMiscByMovieID.setInt(1,id);
				rs=selectMiscByMovieID.executeQuery();
				while (rs.next())
				{
					int personID=rs.getInt(1);
					String job=rs.getString(2);
					String extra=rs.getString(3);
					String name=rs.getString(4);
					xml.write("\t\t<miscEntry>\n");
					xml.write("\t\t\t<person xmlns:xlink=\""+XML.nsXLink+"\" xlink:type=\"simple\" xlink:href=\""+makeM2PLink(personID)+"\">"+XML.XMLify2(name)+"</person>\n");	
					xml.write("\t\t\t<job>"+XML.XMLify2(job)+"</job>\n");
					if (extra!=null) xml.write("\t\t\t<extra>"+XML.XMLify2(extra)+"</extra>\n");
					xml.write("\t\t</miscEntry>\n");
				}
				rs.close();					
	
				if (miscCount>1) xml.write("\t</misc>\n");
			}
			
			selectPlotByMovieID.setInt(1,id);
			rs=selectPlotByMovieID.executeQuery();
			while (rs.next())
			{
				String author=rs.getString(1);
				Reader plotR=rs.getCharacterStream(2);
				char plotA[]=new char[1000];
				String plot=null;
				boolean repeat=true;
				
				while (repeat)
				{
					int read=plotR.read(plotA);
					if ((read==-1)||(read<1000)) repeat=false;
					String blubb="";
					if (read!=-1) blubb=new String(plotA,0,read);
					if (plot==null) plot=blubb;
					else plot+=blubb;
				}
				
				xml.write("\t<plot");
				if (author!=null) xml.write(" author=\""+XML.XMLify2(author)+"\"");
				xml.write(">\n");
				xml.write("\t\t"+XML.XMLify2(plot)+"\n");
				xml.write("\t</plot>\n");
			}
			rs.close();

			selectGenresCountByMovieID.setInt(1,id);
			rs=selectGenresCountByMovieID.executeQuery();
			int genreCount=0;
			if (rs.next()) genreCount=rs.getInt(1);
			rs.close();
			
			if (genreCount>0)
			{
				xml.write("\t<genres>\n");
				
				selectGenresByMovieID.setInt(1,id);
				rs=selectGenresByMovieID.executeQuery();
				while (rs.next())
				{
					String genre=rs.getString(1);
					xml.write("\t\t<genre>"+XML.XMLify2(genre)+"</genre>\n");
				}
				rs.close();					
	
				xml.write("\t</genres>\n");
			}
			
			selectTaglinesCountByMovieID.setInt(1,id);
			rs=selectTaglinesCountByMovieID.executeQuery();
			int taglineCount=0;
			if (rs.next()) taglineCount=rs.getInt(1);
			rs.close();
			
			if (taglineCount>0)
			{
				xml.write("\t<taglines>\n");
				
				selectTaglinesByMovieID.setInt(1,id);
				rs=selectTaglinesByMovieID.executeQuery();
				while (rs.next())
				{
					String tagline=rs.getString(1);
					xml.write("\t\t<tagline>"+XML.XMLify2(tagline)+"</tagline>\n");
				}
				rs.close();					
	
				xml.write("\t</taglines>\n");
			}

			selectKeywordsCountByMovieID.setInt(1,id);
			rs=selectKeywordsCountByMovieID.executeQuery();
			int keywordCount=0;
			if (rs.next()) keywordCount=rs.getInt(1);
			rs.close();
			
			if (keywordCount>0)
			{
				xml.write("\t<keywords>\n");
				
				selectKeywordsByMovieID.setInt(1,id);
				rs=selectKeywordsByMovieID.executeQuery();
				while (rs.next())
				{
					String keyword=rs.getString(1);
					xml.write("\t\t<keyword>"+XML.XMLify2(keyword)+"</keyword>\n");
				}
				rs.close();					
	
				xml.write("\t</keywords>\n");
			}

			selectColourInfoByMovieID.setInt(1,id);
			rs=selectColourInfoByMovieID.executeQuery();
			while (rs.next())
			{
				String colourinfo=rs.getString(1);
				xml.write("\t\t<colourinfo>"+XML.XMLify2(colourinfo)+"</colourinfo>\n");
			}
			rs.close();					
			
			selectSoundMixByMovieID.setInt(1,id);
			rs=selectSoundMixByMovieID.executeQuery();
			while (rs.next())
			{
				String soundmix=rs.getString(1);
				xml.write("\t\t<soundmix>"+XML.XMLify2(soundmix)+"</soundmix>\n");
			}
			rs.close();					
			
			selectAlternateVersionsCountByMovieID.setInt(1,id);
			rs=selectAlternateVersionsCountByMovieID.executeQuery();
			int altCount=0;
			if (rs.next()) altCount=rs.getInt(1);
			rs.close();
			
			if (altCount>0)
			{
				if (altCount>1) xml.write("\t<alternate_versions>\n");
				selectAlternateVersionsByMovieID.setInt(1,id);
				rs=selectAlternateVersionsByMovieID.executeQuery();
				while (rs.next())
				{
					Reader altR=rs.getCharacterStream(1);
					char altA[]=new char[1000];
					String alt=null;
					boolean repeat=true;
					
					while (repeat)
					{
						int read=altR.read(altA);
						if ((read==-1)||(read<1000)) repeat=false;
						String blubb=new String(altA,0,read);
						if (alt==null) alt=blubb;
						else alt+=blubb;
					}
					
					xml.write("\t\t<alternate_version>\n");
					xml.write("\t\t\t"+insertLinks(alt)+"\n");
					xml.write("\t\t</alternate_version>\n");
				}
				rs.close();
				if (altCount>1) xml.write("\t</alternate_versions>\n");
			}

			selectMovieLinksCountByMovieID.setInt(1,id);
			rs=selectMovieLinksCountByMovieID.executeQuery();
			int linksCount=0;
			if (rs.next()) linksCount=rs.getInt(1);
			rs.close();
			
			if (linksCount>0)
			{
				xml.write("\t<links>\n");
				
				selectMovieLinksByMovieID.setInt(1,id);
				rs=selectMovieLinksByMovieID.executeQuery();
				while (rs.next())
				{
					int targetid=rs.getInt(1);
					String linkType=rs.getString(2);
					String targetTitle=rs.getString(3);
					int targetYear=rs.getInt(4);
					String targetAddition=rs.getString(5);
					int targetType=rs.getInt(6);
					String targetEpisode=rs.getString(7);
					
					xml.write("\t\t<link xmlns:xlink=\""+XML.nsXLink+"\" xlink:type=\"simple\" xlink:href=\""+makeM2MLink(targetid)+"\"");	
					xml.write(" linktype=\""+XML.XMLify2(linkType)+"\">\n");
					xml.write("\t\t\t<movie>\n");
					xml.write("\t\t\t\t<title>"+XML.XMLify2(targetTitle)+"</title>\n");
					if (targetEpisode!=null) xml.write("\t\t\t\t<episode>"+XML.XMLify2(targetEpisode)+"</episode>\n");
					xml.write("\t\t\t\t<production_year>"+targetYear+"</production_year>\n");
					if (targetAddition!=null) xml.write("\t\t\t\t<uniqueid>"+targetAddition+"</uniqueid>\n");
					if (targetType>1) xml.write("\t\t\t\t<type>"+movieTypes[targetType]+"</type>\n");
					xml.write("\t\t\t</movie>\n");
					xml.write("\t\t</link>\n");
				}
				rs.close();					
	
				xml.write("\t</links>\n");
			}
			
			selectGoofCountByMovieID.setInt(1,id);
			rs=selectGoofCountByMovieID.executeQuery();
			int goofCount=0;
			if (rs.next()) goofCount=rs.getInt(1);
			rs.close();
			
			if (goofCount>0)
			{
				xml.write("\t<goofs>\n");
				selectGoofByMovieID.setInt(1,id);
				rs=selectGoofByMovieID.executeQuery();
				while (rs.next())
				{
					String goofType=rs.getString(1);
					Reader goofR=rs.getCharacterStream(2);
					char goofA[]=new char[1000];
					String goof=null;
					boolean repeat=true;
					
					while (repeat)
					{
						int read=goofR.read(goofA);
						if ((read==-1)||(read<1000)) repeat=false;
						String blubb=new String(goofA,0,read);
						if (goof==null) goof=blubb;
						else goof+=blubb;
					}
					
					xml.write("\t\t<goof");
					if (goofType!=null)
					{
						String gt="illegal";
						if (goofType.compareTo("BOOM")==0) gt="boom mike visible";
						if (goofType.compareTo("CONT")==0) gt="continuity";
						if (goofType.compareTo("CREW")==0) gt="crew visible";
						if (goofType.compareTo("DATE")==0) gt="anachronism";
						if (goofType.compareTo("FACT")==0) gt="factual error";
						if (goofType.compareTo("FAIR")==0) gt="incorrect goof";
						if (goofType.compareTo("FAKE")==0) gt="revealing mistake";
						if (goofType.compareTo("GEOG")==0) gt="geograpy";
						if (goofType.compareTo("MISC")==0) gt="miscellaneous";
						if (goofType.compareTo("PLOT")==0) gt="plot hole";
						if (goofType.compareTo("SYNC")==0) gt="unsynchronized";

						xml.write(" type=\""+gt+"\"");
					}
					xml.write(">\n");
					xml.write("\t\t\t"+XML.XMLify2(goof)+"\n");
					xml.write("\t\t</goof>\n");
				}
				rs.close();
				xml.write("\t</goofs>\n");
			}
			
			selectMovieTriviaCountByMovieID.setInt(1,id);
			rs=selectMovieTriviaCountByMovieID.executeQuery();
			int triviaCount=0;
			if (rs.next()) triviaCount=rs.getInt(1);
			rs.close();
			
			if (triviaCount>0)
			{
				xml.write("\t<trivia>\n");
				selectMovieTriviaByMovieID.setInt(1,id);
				rs=selectMovieTriviaByMovieID.executeQuery();
				while (rs.next())
				{
					Reader triviaR=rs.getCharacterStream(1);
					char triviaA[]=new char[1000];
					String trivia=null;
					boolean repeat=true;
					
					while (repeat)
					{
						int read=triviaR.read(triviaA);
						if ((read==-1)||(read<1000)) repeat=false;
						String blubb=new String(triviaA,0,read);
						if (trivia==null) trivia=blubb;
						else trivia+=blubb;
					}
					
					xml.write("\t\t<item>\n");
					xml.write("\t\t\t"+insertLinks(trivia)+"\n");
					xml.write("\t\t</item>\n");
				}
				rs.close();
				xml.write("\t</trivia>\n");
			}
			
			xml.write("</movie>\n");
						
			xml.close();
		}
		catch(Exception e)
		{
			System.out.println("export movie "+id+": "+e);
			e.printStackTrace();
		}
	}
	
	private void exportPersons()
	{
	    int moviecnt=0;
		try
		{
			ResultSet rs=db.execQuery("SELECT MAX(PersonID) FROM Persons");
			if (rs.next())
			{
				moviecnt=rs.getInt(1);
			}
			rs.close();
		}
		catch(Exception e)
		{
		    System.out.println("cannot read number of persons from database:\n"+e);
		    System.exit(1);
		}
		
		for (int i=1186000;i<=moviecnt;i++)
		    exportPerson(i);
		
	}
	
	private void exportPerson(int id)
	{
		String name=null;
		int sex=-1;
		String realName=null;
		String nickName=null;
		String dateBirth=null;
		String placeBirth=null;
		String dateDeath=null;
		String placeDeath=null;
		String causeDeath=null;
		String height=null;
		
		File xmlfile=null;
		FileWriter xml=null;
	
		if (id%500==0) System.out.println("exportPerson("+id+")");
			
		try
		{
			selectPersonByID.setInt(1,id);
			
			ResultSet rs=selectPersonByID.executeQuery();
			if (rs.next())
			{
				name=rs.getString(1);
				sex=rs.getInt(2);
				realName=rs.getString(3);
				nickName=rs.getString(4);
				dateBirth=rs.getString(5);
				placeBirth=rs.getString(6);
				dateDeath=rs.getString(7);
				placeDeath=rs.getString(8);
				causeDeath=rs.getString(9);
				height=rs.getString(10);
			}
			rs.close();
			
			xmlfile=new File(DIR+"people\\"+id2Filename(id));
			xml=new FileWriter(xmlfile);
			
			xml.write("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
			xml.write("<!-- generated by CLiX/IMDB2XML $Revision: $ on "+Utils.getCurrentDate()+" -->\n");
			//xml.write("DOCTYPE movie SYSTEM \"..\\imdb.dtd\">");
			
			xml.write("<person id=\""+id+"\">\n");
			
			xml.write("\t<name>"+XML.XMLify2(name)+"</name>\n");
			if (realName!=null) xml.write("\t<realname>"+XML.XMLify2(realName)+"</realname>\n");
			if (nickName!=null) xml.write("\t<nickname>"+XML.XMLify2(nickName)+"</nickname>\n");

			selectAlternativeNamesByPersonID.setInt(1,id);
			rs=selectAlternativeNamesByPersonID.executeQuery();
			while (rs.next())
			{
				String aka=rs.getString(1);
					
				xml.write("\t\t<aka>\n");
				xml.write("\t\t\t"+XML.XMLify2(aka)+"\n");
				xml.write("\t\t</aka>\n");
			}
			rs.close();

			// experimental Wikipedia support
			
//			xml.write(conditionalGenerateWikiLink(name));
			
			if (sex==1)
			{
				xml.write("\t<sex>male</sex>\n");						
			}
			if (sex==2)
			{
				xml.write("\t<sex>female</sex>\n");						
			}

			if (dateBirth!=null)
			{
				xml.write("\t<born");
				if (placeBirth!=null) xml.write(" place=\""+XML.XMLify2(placeBirth)+"\"");
				xml.write(">"+dateBirth+"</born>\n");
			}

			if (dateDeath!=null)
			{
				xml.write("\t<died");
				if (placeDeath!=null) xml.write(" place=\""+XML.XMLify2(placeDeath)+"\"");
				if (causeDeath!=null) xml.write(" cause=\""+XML.XMLify2(causeDeath)+"\"");
				xml.write(">"+dateDeath+"</died>\n");
			}
			
			if (height!=null) xml.write("\t<height>"+XML.XMLify2(height)+"</height>\n");
			
			selectSpousesCountByPersonID.setInt(1,id);
			rs=selectSpousesCountByPersonID.executeQuery();
			int spCount=0;
			if (rs.next()) spCount=rs.getInt(1);
			rs.close();

			if (spCount>0)
			{
				if (spCount>1) xml.write("\t<spouses>\n");
				selectSpousesByPersonID.setInt(1,id);
				rs=selectSpousesByPersonID.executeQuery();
				while (rs.next())
				{
					String sp=rs.getString(1);
					
					xml.write("\t\t<spouse>\n");
					xml.write("\t\t\t"+insertLinks(sp)+"\n");
					xml.write("\t\t</spouse>\n");
				}
				rs.close();
				if (spCount>1) xml.write("\t</spouses>\n");
			}
			
			selectFullBiographyByPersonID.setInt(1,id);
			rs=selectFullBiographyByPersonID.executeQuery();
			while (rs.next())
			{
				String author=rs.getString(1);
				Reader bioR=rs.getCharacterStream(2);
				char bioA[]=new char[1000];
				String bio=null;
				boolean repeat=true;
				
				while (repeat)
				{
					int read=bioR.read(bioA);
					if ((read==-1)||(read<1000)) repeat=false;
					String blubb=new String(bioA,0,Math.max(0,read));
					if (bio==null) bio=blubb;
					else bio+=blubb;
				}
				
				xml.write("\t<biography");
				if (author!=null) xml.write(" author=\""+XML.XMLify2(author)+"\"");
				xml.write(">\n");
				xml.write("\t\t"+insertLinks(bio)+"\n");
				xml.write("\t</biography>\n");
			}
			rs.close();

			int actsCount=0;
			int directsCount=0;
			int editsCount=0;
			int producesCount=0;
			int cinematographsCount=0;
			int costumedesignsCount=0;
			int composesCount=0;
			int miscCount=0;
			
			selectActsInMovieCountByPersonID.setInt(1,id);
			rs=selectActsInMovieCountByPersonID.executeQuery();
			if (rs.next()) actsCount+=rs.getInt(1);
			rs.close();
			
			selectDirectsMovieCountByPersonID.setInt(1,id);
			rs=selectDirectsMovieCountByPersonID.executeQuery();
			if (rs.next()) directsCount+=rs.getInt(1);
			rs.close();
			
			selectProducesMovieCountByPersonID.setInt(1,id);
			rs=selectProducesMovieCountByPersonID.executeQuery();
			if (rs.next()) producesCount+=rs.getInt(1);
			rs.close();
			
			selectEditsMovieCountByPersonID.setInt(1,id);
			rs=selectEditsMovieCountByPersonID.executeQuery();
			if (rs.next()) editsCount+=rs.getInt(1);
			rs.close();
			
			selectComposesMovieCountByPersonID.setInt(1,id);
			rs=selectComposesMovieCountByPersonID.executeQuery();
			if (rs.next()) composesCount+=rs.getInt(1);
			rs.close();
			
			selectCinematographsMovieCountByPersonID.setInt(1,id);
			rs=selectCinematographsMovieCountByPersonID.executeQuery();
			if (rs.next()) cinematographsCount+=rs.getInt(1);
			rs.close();
			
			selectCostumeDesignsMovieCountByPersonID.setInt(1,id);
			rs=selectCostumeDesignsMovieCountByPersonID.executeQuery();
			if (rs.next()) costumedesignsCount+=rs.getInt(1);
			rs.close();
			
			selectMiscCountByPersonID.setInt(1,id);
			rs=selectMiscCountByPersonID.executeQuery();
			if (rs.next()) miscCount+=rs.getInt(1);
			rs.close();
			
			if (actsCount+producesCount+directsCount+editsCount+composesCount+costumedesignsCount+cinematographsCount+miscCount>0)
			{
				xml.write("\t<career>\n");

				if (actsCount>0)
				{
					selectActsInMovieByPersonID.setInt(1,id);
					rs=selectActsInMovieByPersonID.executeQuery();
					while (rs.next())
					{
						int movieID=rs.getInt(1);
						String role=rs.getString(2);
					
						xml.write("\t\t<acted_in xmlns:xlink=\""+XML.nsXLink+"\" xlink:type=\"simple\" xlink:href=\""+makeP2MLink(movieID)+"\"");
						if (role!=null) xml.write(" role=\""+XML.XMLify2(role)+"\"");
						xml.write("/>\n");
					}
					rs.close();					
				}
				
				if (directsCount>0)
				{
					selectDirectsMovieByPersonID.setInt(1,id);
					rs=selectDirectsMovieByPersonID.executeQuery();
					while (rs.next())
					{
						int movieID=rs.getInt(1);
					
						xml.write("\t\t<directed xmlns:xlink=\""+XML.nsXLink+"\" xlink:type=\"simple\" xlink:href=\""+makeP2MLink(movieID)+"\"/>\n");
					}
					rs.close();					
				}	
				
				if (producesCount>0)
				{
					selectProducesMovieByPersonID.setInt(1,id);
					rs=selectProducesMovieByPersonID.executeQuery();
					while (rs.next())
					{
						int movieID=rs.getInt(1);
						String role=rs.getString(2);
					
						xml.write("\t\t<produced xmlns:xlink=\""+XML.nsXLink+"\" xlink:type=\"simple\" xlink:href=\""+makeP2MLink(movieID)+"\"");
						if (role!=null) xml.write(" role=\""+XML.XMLify2(role)+"\"");
						xml.write("/>\n");
					}
					rs.close();					
				}	
				
				if (editsCount>0)
				{
					selectEditsMovieByPersonID.setInt(1,id);
					rs=selectEditsMovieByPersonID.executeQuery();
					while (rs.next())
					{
						int movieID=rs.getInt(1);
					
						xml.write("\t\t<acted_in xmlns:xlink=\""+XML.nsXLink+"\" xlink:type=\"simple\" xlink:href=\""+makeP2MLink(movieID)+"\"/>\n");
					}
					rs.close();					
				}	
				
				if (composesCount>0)
				{
					selectComposesMovieByPersonID.setInt(1,id);
					rs=selectComposesMovieByPersonID.executeQuery();
					while (rs.next())
					{
						int movieID=rs.getInt(1);
					
						xml.write("\t\t<composed_for xmlns:xlink=\""+XML.nsXLink+"\" xlink:type=\"simple\" xlink:href=\""+makeP2MLink(movieID)+"\"/>\n");
					}
					rs.close();					
				}	
				
				if (cinematographsCount>0)
				{
					selectCinematographsMovieByPersonID.setInt(1,id);
					rs=selectCinematographsMovieByPersonID.executeQuery();
					while (rs.next())
					{
						int movieID=rs.getInt(1);
					
						xml.write("\t\t<cinematographed xmlns:xlink=\""+XML.nsXLink+"\" xlink:type=\"simple\" xlink:href=\""+makeP2MLink(movieID)+"\"/>\n");
					}
					rs.close();					
				}	

				if (costumedesignsCount>0)
				{
					selectCostumeDesignsMovieByPersonID.setInt(1,id);
					rs=selectCostumeDesignsMovieByPersonID.executeQuery();
					while (rs.next())
					{
						int movieID=rs.getInt(1);
					
						xml.write("\t\t<designed_costumes_for xmlns:xlink=\""+XML.nsXLink+"\" xlink:type=\"simple\" xlink:href=\""+makeP2MLink(movieID)+"\"/>\n");
					}
					rs.close();					
				}	

				if (miscCount>0)
				{
					selectMiscByPersonID.setInt(1,id);
					rs=selectMiscByPersonID.executeQuery();
					while (rs.next())
					{
						int movieID=rs.getInt(1);
						String job=rs.getString(2);
						String extra=rs.getString(3);
					
						xml.write("\t\t<misc xmlns:xlink=\""+XML.nsXLink+"\" xlink:type=\"simple\" xlink:href=\""+makeP2MLink(movieID)+"\" job=\""+XML.XMLify2(job)+"\"");
						if (extra!=null) xml.write(" extra=\""+XML.XMLify2(extra)+"\"");
						xml.write("/>\n");
					}
					rs.close();					
				}	

				xml.write("\t</career>\n");
			}
			
			selectActorTriviaCountByPersonID.setInt(1,id);
			rs=selectActorTriviaCountByPersonID.executeQuery();
			int triviaCount=0;
			if (rs.next()) triviaCount=rs.getInt(1);
			rs.close();
			
			if (triviaCount>0)
			{
				xml.write("\t<trivia>\n");
				selectActorTriviaByPersonID.setInt(1,id);
				rs=selectActorTriviaByPersonID.executeQuery();
				while (rs.next())
				{
					Reader triviaR=rs.getCharacterStream(1);
					char triviaA[]=new char[1000];
					String trivia=null;
					boolean repeat=true;
					
					while (repeat)
					{
						int read=triviaR.read(triviaA);
						if ((read==-1)||(read<1000)) repeat=false;
						String blubb=new String(triviaA,0,read);
						if (trivia==null) trivia=blubb;
						else trivia+=blubb;
					}
					
					xml.write("\t\t<item>\n");
					xml.write("\t\t\t"+insertLinks(trivia)+"\n");
					xml.write("\t\t</item>\n");
				}
				rs.close();
				xml.write("\t</trivia>\n");
			}

			selectActorQuotesCountByPersonID.setInt(1,id);
			rs=selectActorQuotesCountByPersonID.executeQuery();
			int quotesCount=0;
			if (rs.next()) quotesCount=rs.getInt(1);
			rs.close();
			
			if (quotesCount>0)
			{
				xml.write("\t<quotes>\n");
				selectActorQuotesByPersonID.setInt(1,id);
				rs=selectActorQuotesByPersonID.executeQuery();
				while (rs.next())
				{
					Reader quoteR=rs.getCharacterStream(1);
					char quoteA[]=new char[1000];
					String quote=null;
					boolean repeat=true;
					
					while (repeat)
					{
						int read=quoteR.read(quoteA);
						if ((read==-1)||(read<1000)) repeat=false;
						String blubb=new String(quoteA,0,read);
						if (quote==null) quote=blubb;
						else quote+=blubb;
					}
					
					xml.write("\t\t<quote>\n");
					xml.write("\t\t\t"+insertLinks(quote)+"\n");
					xml.write("\t\t</quote>\n");
				}
				rs.close();
				xml.write("\t</quotes>\n");
			}

			selectBiographicalBooksCountByPersonID.setInt(1,id);
			rs=selectBiographicalBooksCountByPersonID.executeQuery();
			int bbCount=0;
			if (rs.next()) bbCount=rs.getInt(1);
			rs.close();

			if (bbCount>0)
			{
				xml.write("\t<biographical_books>\n");
				selectBiographicalBooksByPersonID.setInt(1,id);
				rs=selectBiographicalBooksByPersonID.executeQuery();
				while (rs.next())
				{
					String book=rs.getString(1);
					
					xml.write("\t\t<book>\n");
					xml.write("\t\t\t"+XML.XMLify2(book)+"\n");
					xml.write("\t\t</book>\n");
				}
				rs.close();
				xml.write("\t</biographical_books>\n");
			}

			selectBiographicalFilmsCountByPersonID.setInt(1,id);
			rs=selectBiographicalFilmsCountByPersonID.executeQuery();
			int bfCount=0;
			if (rs.next()) bfCount=rs.getInt(1);
			rs.close();

			if (bfCount>0)
			{
				xml.write("\t<biographical_films>\n");
				selectBiographicalFilmsByPersonID.setInt(1,id);
				rs=selectBiographicalFilmsByPersonID.executeQuery();
				while (rs.next())
				{
					String film=rs.getString(1);
					
					xml.write("\t\t<film>\n");
					xml.write("\t\t\t"+insertLinks(film)+"\n");
					xml.write("\t\t</film>\n");
				}
				rs.close();
				xml.write("\t</biographical_films>\n");
			}

			selectArticlesCountByPersonID.setInt(1,id);
			rs=selectArticlesCountByPersonID.executeQuery();
			int articlesCount=0;
			if (rs.next()) articlesCount=rs.getInt(1);
			rs.close();
			
			if (articlesCount>0)
			{
				xml.write("\t<articles>\n");
				selectArticlesByPersonID.setInt(1,id);
				rs=selectArticlesByPersonID.executeQuery();
				while (rs.next())
				{
					Reader articleR=rs.getCharacterStream(1);
					char articleA[]=new char[1000];
					String article=null;
					boolean repeat=true;
					
					while (repeat)
					{
						int read=articleR.read(articleA);
						if ((read==-1)||(read<1000)) repeat=false;
						String blubb=new String(articleA,0,read);
						if (article==null) article=blubb;
						else article+=blubb;
					}
					
					xml.write("\t\t<article>\n");
					xml.write("\t\t\t"+insertLinks(article)+"\n");
					xml.write("\t\t</article>\n");
				}
				rs.close();
				xml.write("\t</articles>\n");
			}

			selectAgentsCountByPersonID.setInt(1,id);
			rs=selectAgentsCountByPersonID.executeQuery();
			int agCount=0;
			if (rs.next()) agCount=rs.getInt(1);
			rs.close();

			if (agCount>0)
			{
				xml.write("\t<agents>\n");
				selectAgentsByPersonID.setInt(1,id);
				rs=selectAgentsByPersonID.executeQuery();
				while (rs.next())
				{
					String agent=rs.getString(1);
					
					xml.write("\t\t<agent>\n");
					xml.write("\t\t\t"+XML.XMLify2(agent)+"\n");
					xml.write("\t\t</agent>\n");
				}
				rs.close();
				xml.write("\t</agents>\n");
			}

			selectCoverPhotosCountByPersonID.setInt(1,id);
			rs=selectCoverPhotosCountByPersonID.executeQuery();
			int cvCount=0;
			if (rs.next()) cvCount=rs.getInt(1);
			rs.close();

			if (cvCount>0)
			{
				xml.write("\t<coverphotos>\n");
				selectCoverPhotosByPersonID.setInt(1,id);
				rs=selectCoverPhotosByPersonID.executeQuery();
				while (rs.next())
				{
					String cv=rs.getString(1);
					
					xml.write("\t\t<coverphoto>\n");
					xml.write("\t\t\t"+XML.XMLify2(cv)+"\n");
					xml.write("\t\t</coverphoto>\n");
				}
				rs.close();
				xml.write("\t</coverphotos>\n");
			}

			selectGuestAppearancesCountByPersonID.setInt(1,id);
			rs=selectGuestAppearancesCountByPersonID.executeQuery();
			int gaCount=0;
			if (rs.next()) gaCount=rs.getInt(1);
			rs.close();

			if (gaCount>0)
			{
				xml.write("\t<guest_appearances>\n");
				selectGuestAppearancesByPersonID.setInt(1,id);
				rs=selectGuestAppearancesByPersonID.executeQuery();
				while (rs.next())
				{
					String ga=rs.getString(1);
					
					xml.write("\t\t<appearance>\n");
					xml.write("\t\t\t"+insertLinks(ga)+"\n");
					xml.write("\t\t</appearance>\n");
				}
				rs.close();
				xml.write("\t</guest_appearances>\n");
			}

			selectInterviewsCountByPersonID.setInt(1,id);
			rs=selectInterviewsCountByPersonID.executeQuery();
			int itCount=0;
			if (rs.next()) itCount=rs.getInt(1);
			rs.close();

			if (itCount>0)
			{
				xml.write("\t<interviews>\n");
				selectInterviewsByPersonID.setInt(1,id);
				rs=selectInterviewsByPersonID.executeQuery();
				while (rs.next())
				{
					String it=rs.getString(1);
					
					xml.write("\t\t<interview>\n");
					xml.write("\t\t\t"+XML.XMLify2(it)+"\n");
					xml.write("\t\t</interview>\n");
				}
				rs.close();
				xml.write("\t</interviews>\n");
			}

			selectPictorialsCountByPersonID.setInt(1,id);
			rs=selectPictorialsCountByPersonID.executeQuery();
			int ptCount=0;
			if (rs.next()) ptCount=rs.getInt(1);
			rs.close();

			if (ptCount>0)
			{
				xml.write("\t<pictorials>\n");
				selectPictorialsByPersonID.setInt(1,id);
				rs=selectPictorialsByPersonID.executeQuery();
				while (rs.next())
				{
					String pt=rs.getString(1);
					
					xml.write("\t\t<pictorial>\n");
					xml.write("\t\t\t"+XML.XMLify2(pt)+"\n");
					xml.write("\t\t</pictorial>\n");
				}
				rs.close();
				xml.write("\t</pictorials>\n");
			}

			selectPortraitsCountByPersonID.setInt(1,id);
			rs=selectPortraitsCountByPersonID.executeQuery();
			int poCount=0;
			if (rs.next()) poCount=rs.getInt(1);
			rs.close();

			if (poCount>0)
			{
				xml.write("\t<portraits>\n");
				selectPortraitsByPersonID.setInt(1,id);
				rs=selectPortraitsByPersonID.executeQuery();
				while (rs.next())
				{
					String po=rs.getString(1);
					
					xml.write("\t\t<portrait>\n");
					xml.write("\t\t\t"+insertLinks(po)+"\n");
					xml.write("\t\t</portrait>\n");
				}
				rs.close();
				xml.write("\t</portraits>\n");
			}

			selectOtherWorksCountByPersonID.setInt(1,id);
			rs=selectOtherWorksCountByPersonID.executeQuery();
			int owCount=0;
			if (rs.next()) owCount=rs.getInt(1);
			rs.close();
			
			if (owCount>0)
			{
				if (owCount>1) xml.write("\t<other_works>\n");
				selectOtherWorksByPersonID.setInt(1,id);
				rs=selectOtherWorksByPersonID.executeQuery();
				while (rs.next())
				{
					Reader owR=rs.getCharacterStream(1);
					char owA[]=new char[1000];
					String ow=null;
					boolean repeat=true;
					
					while (repeat)
					{
						int read=owR.read(owA);
						if ((read==-1)||(read<1000)) repeat=false;
						String blubb=new String(owA,0,read);
						if (ow==null) ow=blubb;
						else ow+=blubb;
					}
					
					xml.write("\t\t<other_work>\n");
					xml.write("\t\t\t"+insertLinks(ow)+"\n");
					xml.write("\t\t</other_work>\n");
				}
				rs.close();
				if (owCount>1) xml.write("\t</other_works>\n");
			}

			selectTrademarksCountByPersonID.setInt(1,id);
			rs=selectTrademarksCountByPersonID.executeQuery();
			int tmCount=0;
			if (rs.next()) tmCount=rs.getInt(1);
			rs.close();
			
			if (tmCount>0)
			{
				if (tmCount>1) xml.write("\t<trademarks>\n");
				selectOtherWorksByPersonID.setInt(1,id);
				rs=selectOtherWorksByPersonID.executeQuery();
				while (rs.next())
				{
					Reader tmR=rs.getCharacterStream(1);
					char tmA[]=new char[1000];
					String tm=null;
					boolean repeat=true;
					
					while (repeat)
					{
						int read=tmR.read(tmA);
						if ((read==-1)||(read<1000)) repeat=false;
						String blubb=new String(tmA,0,read);
						if (tm==null) tm=blubb;
						else tm+=blubb;
					}
					
					xml.write("\t\t<trademark>\n");
					xml.write("\t\t\t"+insertLinks(tm)+"\n");
					xml.write("\t\t</trademark>\n");
				}
				rs.close();
				if (tmCount>1) xml.write("\t</trademarks>\n");
			}

			selectWhereNowCountByPersonID.setInt(1,id);
			rs=selectWhereNowCountByPersonID.executeQuery();
			int wnCount=0;
			if (rs.next()) wnCount=rs.getInt(1);
			rs.close();
			
			if (wnCount>0)
			{
				selectWhereNowByPersonID.setInt(1,id);
				rs=selectWhereNowByPersonID.executeQuery();
				while (rs.next())
				{
					Reader wnR=rs.getCharacterStream(1);
					char wnA[]=new char[1000];
					String wn=null;
					boolean repeat=true;
					
					while (repeat)
					{
						int read=wnR.read(wnA);
						if ((read==-1)||(read<1000)) repeat=false;
						String blubb=new String(wnA,0,read);
						if (wn==null) wn=blubb;
						else wn+=blubb;
					}
					
					xml.write("\t\t<where_now>\n");
					xml.write("\t\t\t"+insertLinks(wn)+"\n");
					xml.write("\t\t</where_now>\n");
				}
				rs.close();
			}

			xml.write("</person>\n");
						
			xml.close();
		}
		catch(Exception e)
		{
			System.out.println("export movie "+id+": "+e);
			e.printStackTrace();
		}
	}
	
	private String insertPersonLinks(String input)
	{
		String out=null;

		// search for links to actors and convert them to XML links
		for (int i=0; i<input.length();i++)
		{
			//System.out.println("i="+i);
			//System.out.println("consider substring "+input.substring(i));
			
			int apos=input.substring(i).indexOf('\'');
			//System.out.println("apos="+apos);
			if (apos==-1)
			{
				if (out==null) out=XML.XMLify2(input.substring(i));
				else out+=XML.XMLify2(input.substring(i));
				break;
			}
			int epos=input.substring(i+apos+1).indexOf('\'');
			//System.out.println("epos="+epos);
			if (epos==-1)
			{
				if (out==null) out=XML.XMLify2(input.substring(i));
				else out+=XML.XMLify2(input.substring(i));
				break;
			}
			//System.out.println("substring="+input.substring(i+apos+epos+2));
			if (!input.substring(i+apos+epos+2).startsWith(" (qv)"))
			{
				if (out==null) out=XML.XMLify2(input.substring(i,i+apos));
				else out+=XML.XMLify2(input.substring(i,i+apos));
				i+=apos+1;
				continue;
			}
			if (out==null) out=XML.XMLify2(input.substring(i,i+apos));
			else out+=XML.XMLify2(input.substring(i,i+apos));
			//System.out.println("added "+input.substring(i,i+apos));
			String link=input.substring(i+apos+1,i+apos+epos+1);
			System.out.println("link="+link);
			int personID=findPerson(link);
			if (personID==-1)
			{
				if (out==null) out=XML.XMLify2(link);
				else out+=XML.XMLify2(link);
			}
			else
			{
				if (out==null) out="<person xmlns:xlink=\""+XML.nsXLink+"\" xlink:type=\"simple\" xlink:href=\"../people/"+personID+".xml\">"+XML.XMLify2(link)+"</person>";
				else out+="<person xmlns:xlink=\""+XML.nsXLink+"\" xlink:type=\"simple\" xlink:href=\"../people/"+personID+".xml\">"+XML.XMLify2(link)+"</person>";
			}
			i+=apos+epos+2+4;
		}
		return out;
	}

	private String insertMovieLinks(String input)
	{
		String out=null;
		
		// search for links to actors and convert them to XML links
		for (int i=0; i<input.length();i++)
		{
			System.out.println("i="+i);
			System.out.println("consider substring "+input.substring(i));
			
			int apos=input.substring(i).indexOf('_');
			//System.out.println("apos="+apos);
			if (apos==-1)
			{
				if (out==null) out=XML.XMLify2(input.substring(i));
				else out+=XML.XMLify2(input.substring(i));
				break;
			}
			int epos=input.substring(i+apos+1).indexOf('_');
			//System.out.println("epos="+epos);
			if (epos==-1)
			{
				if (out==null) out=XML.XMLify2(input.substring(i));
				else out+=XML.XMLify2(input.substring(i));
				break;
			}
			//System.out.println("substring="+input.substring(i+apos+epos+2));
			if (!input.substring(i+apos+epos+2).startsWith(" (qv)"))
			{
				if (out==null) out=XML.XMLify2(input.substring(i,i+apos));
				else out+=XML.XMLify2(input.substring(i,i+apos));
				i+=apos+1;
				continue;
			}
			if (out==null) out=XML.XMLify2(input.substring(i,i+apos));
			else out+=XML.XMLify2(input.substring(i,i+apos));
			//System.out.println("added "+input.substring(i,i+apos));
			String link=input.substring(i+apos+1,i+apos+epos+1);
			System.out.println("movie link="+link);
			int movieID=findMovie(link);
			if (movieID==-1)
			{
				if (out==null) out=XML.XMLify2(link);
				else out+=XML.XMLify2(link);
			}
			else
			{
				if (out==null) out="<movie xmlns:xlink=\""+XML.nsXLink+"\" xlink:type=\"simple\" xlink:href=\""+movieID+".xml\">"+XML.XMLify2(link)+"</movie>";
				else out+="<movie xmlns:xlink=\""+XML.nsXLink+"\" xlink:type=\"simple\" xlink:href=\""+movieID+".xml\">"+XML.XMLify2(link)+"</movie>";
			}
			i+=apos+epos+2+4;
		}
		return out;
	}

	private String insertLinks(String input)
	{
		String out=null;

		// search for links to actors and convert them to XML links
		for (int i=0; i<input.length();i++)
		{
			//System.out.println("i="+i);
			//System.out.println("consider substring "+input.substring(i));
			
			int apos=input.substring(i).indexOf('\'');
			int Apos=input.substring(i).indexOf('_');

			//System.out.println("apos="+apos+"; Apos="+Apos);

			if ((apos==-1)&&(Apos==-1))
			{
				if (out==null) out=XML.XMLify2(input.substring(i));
				else out+=XML.XMLify2(input.substring(i));
				break;
			}
			else if ((apos!=-1)&&((apos<Apos)||(Apos==-1))) // first occurence is a person link
			{
				int epos=input.substring(i+apos+1).indexOf('\'');
				//System.out.println("epos="+epos);
				if (epos==-1)
				{
					if (Apos==-1)
					{
						if (out==null) out=XML.XMLify2(input.substring(i));
						else out+=XML.XMLify2(input.substring(i));
						break;
					}
					else
					{
						if (out==null) out=XML.XMLify2(input.substring(i,i+Apos));
						else out+=XML.XMLify2(input.substring(i,i+Apos));
						i+=Apos-1;
						continue;
					}
				}
				//System.out.println("substring="+input.substring(i+apos+epos+2));
				if (!input.substring(i+apos+epos+2).startsWith(" (qv)"))
				{
					if (out==null) out=XML.XMLify2(input.substring(i,i+apos));
					else out+=XML.XMLify2(input.substring(i,i+apos));
					i+=apos;
					continue;
				}
				
				if (out==null) out=XML.XMLify2(input.substring(i,i+apos));
				else out+=XML.XMLify2(input.substring(i,i+apos));
				//System.out.println("added "+input.substring(i,i+apos));
				String link=input.substring(i+apos+1,i+apos+epos+1);
				//System.out.print("person link="+link);
				int personID=findPerson(link);
				if (personID==-1)
				{
					//System.out.println(" (not found)");
					if (out==null) out=XML.XMLify2(link);
					else out+=XML.XMLify2(link);
				}
				else
				{
					//System.out.println(" ("+personID+")");
					if (out==null) out="<person xmlns:xlink=\""+XML.nsXLink+"\" xlink:type=\"simple\" xlink:href=\""+makeM2PLink(personID)+"\">"+XML.XMLify2(link)+"</person>";
					else out+="<person xmlns:xlink=\""+XML.nsXLink+"\" xlink:type=\"simple\" xlink:href=\""+makeM2PLink(personID)+"\">"+XML.XMLify2(link)+"</person>";
				}
				i+=apos+epos+2+4;
			}
			else if ((Apos!=-1)&&((Apos<apos)||(apos==-1))) // first occurence is a movie link
			{
				int Epos=input.substring(i+Apos+1).indexOf('_');
				//System.out.println("epos="+epos);
				if (Epos==-1)
				{
					if (apos==-1)
					{
						if (out==null) out=XML.XMLify2(input.substring(i));
						else out+=XML.XMLify2(input.substring(i));
						break;
					}
					else
					{
						if (out==null) out=XML.XMLify2(input.substring(i,i+apos));
						else out+=XML.XMLify2(input.substring(i,i+apos));
						i+=apos-1;
						continue;
					}
				}
				//System.out.println("substring="+input.substring(i+apos+epos+2));
				if (!input.substring(i+Apos+Epos+2).startsWith(" (qv)"))
				{
					if (out==null) out=XML.XMLify2(input.substring(i,i+Apos));
					else out+=XML.XMLify2(input.substring(i,i+Apos));
					i+=(Apos);
					continue;
				}
				if (out==null) out=XML.XMLify2(input.substring(i,i+Apos));
				else out+=XML.XMLify2(input.substring(i,i+Apos));
				//System.out.println("added "+input.substring(i,i+apos));
				String link=input.substring(i+Apos+1,i+Apos+Epos+1);
				//System.out.print("movie link="+link);
				int movieID=findMovie(link);
				if (movieID==-1)
				{
					//System.out.println(" (not found)");
					if (out==null) out=XML.XMLify2(link);
					else out+=XML.XMLify2(link);
				}
				else
				{
					//System.out.println(" ("+movieID+")");
					if (out==null) out="<movie xmlns:xlink=\""+XML.nsXLink+"\" xlink:type=\"simple\" xlink:href=\""+makeP2MLink(movieID)+"\">"+XML.XMLify2(link)+"</movie>";
					else out+="<movie xmlns:xlink=\""+XML.nsXLink+"\" xlink:type=\"simple\" xlink:href=\""+makeP2MLink(movieID)+"\">"+XML.XMLify2(link)+"</movie>";
				}
				i+=Apos+Epos+2+4;
				continue;
			}

		}
		return out;
	}

	// utility methods (will be moved to separate class)
	
	private class Title
	{
		static final int TTL_NORMAL=1;
		static final int TTL_TV=2;
		static final int TTL_TV_SERIES=3;
		static final int TTL_TV_MINI_SERIES=4;
		static final int TTL_VIDEO=5;
		static final int TTL_VIDEOGAME=6;
		
		String title;
		int year;
		int type;		
		String addition=null;
		
		public Title(String title,int year, int type, String addition)
		{
			this.title=title;
			this.year=year;
			this.type=type;
			this.addition=addition;
		}
		
		public void print()
		{
			System.out.println("title="+title+"; year="+year+"; type="+type+"; addition="+addition);
		}
	}
	
	private Title demangleComplexTitle(String complexTitle)
	{
		//System.out.println("demangleComplexTitle("+complexTitle+")");
		
		String title=null;
		int year=-1;
		int type=Title.TTL_NORMAL;
		String addition=null;
		
		if (complexTitle.endsWith("(mini)"))
		{
			type=Title.TTL_TV_MINI_SERIES;
			complexTitle=complexTitle.substring(0,complexTitle.length()-7);
		}

		if (complexTitle.endsWith("(TV)"))
		{
			type=Title.TTL_TV;
			complexTitle=complexTitle.substring(0,complexTitle.length()-5);
		}
		
		if (complexTitle.endsWith("(V)"))
		{
			type=Title.TTL_VIDEO;
			complexTitle=complexTitle.substring(0,complexTitle.length()-4);
		}

		if (complexTitle.endsWith("(VG)"))
		{
			type=Title.TTL_TV_MINI_SERIES;
			complexTitle=complexTitle.substring(0,complexTitle.length()-5);
		}

		// strip the year from the title
		
		int len=complexTitle.length();
		int parpos=complexTitle.lastIndexOf('(');
		if (parpos!=-1)
		{
			try
			{
				String yearString=complexTitle.substring(parpos+1,parpos+5);
				year=Integer.parseInt(yearString);
			}
			catch(Exception e)
			{ }

			if (parpos+6<len)
				addition=complexTitle.substring(parpos+6,len-1);			

			complexTitle=complexTitle.substring(0,parpos-1);
		}
				
		if (complexTitle.charAt(0)=='\"')
		{	
			// this is a TV movie
			if (type!=Title.TTL_TV_MINI_SERIES) type=Title.TTL_TV_SERIES;
			complexTitle=complexTitle.substring(1,complexTitle.length()-1);		
		}
/*
		System.out.println("stripped title is #"+complexTitle+"#");
		System.out.println("year="+year);
		System.out.println("addition="+addition);
		System.out.println("type="+type);
*/		
		return new Title(complexTitle,year,type,addition);
	}

	private int findPerson(String name)
	{
		int myID=-1;
		
		try
		{
			selectPersonID.setString(1,name);
			ResultSet rs=selectPersonID.executeQuery();
			if (rs.next())
			{
				myID=rs.getInt(1);
			}
			rs.close();
			
			if (myID==-1) // not found, so try alternative name
			{
				selectAlternativePersonID.setString(1,name);
				rs=selectAlternativePersonID.executeQuery();
				if (rs.next())
				{
					myID=rs.getInt(1);
				}
				rs.close();
			}
		}
		catch(Exception e)
		{
			System.out.println("findPersion("+name+"): "+e);
			e.printStackTrace();
		}
		
		return myID;
	}

	private int findMovie(String fullName)
	{
		int movieID=-1;
		
		try
		{
			Title t=demangleComplexTitle(fullName);
			if (t!=null)
			{
				if (t.addition!=null)
				{
					selectMovieID.setString(1,t.title);
					selectMovieID.setInt(2,t.year);
					selectMovieID.setString(3,t.addition);
					selectMovieID.setInt(4,t.type);
					ResultSet rs=selectMovieID.executeQuery();
					if (rs.next())
					{
						movieID=rs.getInt(1);
					}
					rs.close();
				}
				else
				{
					selectMovieIDNULL.setString(1,t.title);
					selectMovieIDNULL.setInt(2,t.year);
					selectMovieIDNULL.setInt(3,t.type);
					ResultSet rs=selectMovieIDNULL.executeQuery();
					if (rs.next())
					{
						movieID=rs.getInt(1);
					}
					rs.close();
				}

				if (movieID==-1) // did not found it, so look at alternative titles
				{
					if (t.addition!=null)
					{
						selectAlternativeMovieID.setString(1,t.title);
						selectAlternativeMovieID.setInt(2,t.year);
						selectAlternativeMovieID.setString(3,t.addition);
						selectAlternativeMovieID.setInt(4,t.type);
						ResultSet rs=selectAlternativeMovieID.executeQuery();
						if (rs.next())
						{
							movieID=rs.getInt(1);
						}
						rs.close();
					}
					else
					{
						selectAlternativeMovieIDNULL.setString(1,t.title);
						selectAlternativeMovieIDNULL.setInt(2,t.year);
						selectAlternativeMovieIDNULL.setInt(3,t.type);
						ResultSet rs=selectAlternativeMovieIDNULL.executeQuery();
						if (rs.next())
						{
							movieID=rs.getInt(1);
						}
						rs.close();
					}		
				}
			}
		}
		catch(Exception e)
		{
			System.out.println("findMovie("+fullName+"): "+e);
			e.printStackTrace();
		}
		
		return movieID;
	}
	
	private String id2Filename(int id)
	{
		String filename="";
		
		int id1=id/1000;
		int id2=id%1000;
		
		filename=id1+"\\"+id2+".xml";
		
		return filename;
	}
	
	private String id2Link(int id)
	{
		String filename="";
		
		int id1=id/1000;
		int id2=id%1000;
		
		filename=id1+"/"+id2+".xml";
		
		return filename;
	}
	
	private String makeP2PLink(int id)
	{
		return "../"+id2Link(id);
	}

	private String makeM2MLink(int id)
	{
		return "../"+id2Link(id);
	}

	private String makeP2MLink(int id)
	{
		return "../../movies/"+id2Link(id);
	}
	
	private String makeM2PLink(int id)
	{
		return "../../people/"+id2Link(id);
	}
	
	// Wikipedia support (experimental)
	
	private String makeWikiLink(String key)
	{
	    while (key.length()<2) key+="_";
	    
	    key=key.trim();
	    key=key.toLowerCase();
	    
	    int comma=key.lastIndexOf(',');
	    if (comma!=-1)
	    {
	        String first=key.substring(0,comma).trim();
	        String last=key.substring(comma+1,key.length()).trim();

	        // make sure that we don't modify "100,000 dollars"
	        if (comma!=key.length()-1) // not the last character, so see what's next
	        {
	            if (key.charAt(comma+1)==' ')
	                key=last+" "+first;
	        }
	        else key=last+" "+first;
	    }
/*
	    if (key.endsWith(", the"))
	    {
	        key="the "+key.substring(0,key.length()-5);
	    }
	*/    
	    key=key.replaceAll("[/ #]","_");
	    
	    return wikiBaseUrl+key.substring(0,1)+"/"+key.substring(0,2)+"/"+key+".html";
	}

	private String makeWikiMovieLink(String key)
	{
		while (key.length()<2) key+="_";
	    
		key=key.trim();
		key=key.toLowerCase();
	    
		int comma=key.lastIndexOf(',');
		if (comma!=-1)
		{
			String first=key.substring(0,comma).trim();
			String last=key.substring(comma+1,key.length()).trim();

			// make sure that we don't modify "100,000 dollars"
			if (comma!=key.length()-1) // not the last character, so see what's next
			{
				if (key.charAt(comma+1)==' ')
					key=last+" "+first;
			}
			else key=last+" "+first;
		}
/*
		if (key.endsWith(", the"))
		{
			key="the "+key.substring(0,key.length()-5);
		}
	*/    
		key+=" (movie)";
		
		key=key.replaceAll("[/ #()]","_");
	    
		return wikiBaseUrl+key.substring(0,1)+"/"+key.substring(0,2)+"/"+key+".html";
	}
	
	private PreparedStatement getWikiLink;
	
	private String conditionalGenerateWikiLink(String key)
	{
	    String link=makeWikiLink(key);
	    String returnedLink="";
	    
	    boolean exists=false;
	    
	    try
	    {
	        getWikiLink.setString(1,link);
	        
	        ResultSet rs=getWikiLink.executeQuery();
	        if (rs.next()) exists=true;
	        rs.close();
	    }
	    catch(Exception e)
	    {
	        System.out.println("cannot test wiki link "+link+"\n"+e);
	    }

	    if (exists) System.out.println("test wiki link "+link+" "+exists);
	    
	    if (exists) returnedLink+="\t<wiki_link xmlns:xlink=\""+XML.nsXLink+"\" xlink:type=\"simple\" xlink:href=\""+link+"\"/>\n";
	    
		link=makeWikiMovieLink(key);
	    exists=false;

		try
		{
			getWikiLink.setString(1,link);
	        
			ResultSet rs=getWikiLink.executeQuery();
			if (rs.next()) exists=true;
			rs.close();
		}
		catch(Exception e)
		{
			System.out.println("cannot test wiki link "+link+"\n"+e);
		}

		if (exists) System.out.println("test wiki link "+link+" "+exists);
	    
		if (exists) returnedLink+="\t<wiki_link xmlns:xlink=\""+XML.nsXLink+"\" xlink:type=\"simple\" xlink:href=\""+link+"\"/>\n";
	    
	    return returnedLink;
	}
}
