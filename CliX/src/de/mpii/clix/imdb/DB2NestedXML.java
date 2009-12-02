package de.mpii.clix.imdb;
import java.io.*;
import java.sql.*;

import oracle.jdbc.driver.*;

// this is a preliminary quick-and-dirty version that
// includes limited actor information in movie files

public class DB2NestedXML
{
	private static String DIR="c:\\Projects\\IMDB\\NestedXML\\";
	private static String dbUser="imdb2";
	private static String dbPassword="imdb2";
	private static String dbHost="mpiat5316.ag5.mpi-sb.mpg.de";
	private static String dbServiceName="(SERVICE_NAME=destiny.mpi)";
	
	private Connection connection;
	private Statement statement;

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
		System.out.println("DB2XML [nested] started.");
		
		DB2NestedXML exporter=new DB2NestedXML();
		
		exporter.initDatabase();
		
		exporter.initFileSystem();

		int maxMovieID=exporter.getMaxMovieID();		
		
		for (int i=1; i<=maxMovieID; i++)
//		for (int i=1; i<=99; i++)
			exporter.exportMovie(i);

//		int maxPersonID=exporter.getMaxPersonID();

//		for (int p=1; p<=maxPersonID; p++)
//			exporter.exportPerson(p);

//		String tmp=exporter.insertPersonLinks("When Scott first walks into the room and Emily ('Sara Gilbert' (qv)) tells him she will be right out, _\"Little House on the Prairie\" (1974)_ (qv) is on the TV, featuring Gilbert's sister, 'Melissa Gilbert' (qv), as Laura Ingalls.");
//		System.out.println(exporter.insertMovieLinks(tmp));

		exporter.closeDatabase();
		
		System.out.println("DB2XML [nested] finished.");
	}
	
	private int getMaxMovieID()
	{
		int max=-1;
		
		try
		{
			String query="SELECT MAX(movieid) FROM MOVIES";
			
			ResultSet rs=this.statement.executeQuery(query);
			if (rs.next())
			{
				max=rs.getInt(1);
			}
		}
		catch(Exception e)
		{
			System.out.println("getMaxMovieID(): "+e);
			e.printStackTrace();
		}
		return max;
	}

	private int getMaxPersonID()
	{
		int max=-1;
		
		try
		{
			String query="SELECT MAX(personid) FROM Persons";
			
			ResultSet rs=this.statement.executeQuery(query);
			if (rs.next())
			{
				max=rs.getInt(1);
			}
		}
		catch(Exception e)
		{
			System.out.println("getMaxPersonID(): "+e);
			e.printStackTrace();
		}
		return max;
	}
	
	private void initDatabase()
	{
	 	try
	  	{
			DriverManager.registerDriver( new OracleDriver() );
			connection = DriverManager.getConnection( 
				"jdbc:oracle:thin:@(DESCRIPTION=(ADDRESS_LIST=(ADDRESS=(PROTOCOL=TCP)(HOST="+dbHost+")(PORT=1521)))(CONNECT_DATA= " + dbServiceName + " ))",
				dbUser,dbPassword);
		
			connection.setAutoCommit(true);
		
			selectMovieWithIDLimit=connection.prepareStatement("SELECT MovieID,Title,Year,Addition,Type FROM Movies WHERE MovieID<?");
			selectMovieByID=connection.prepareStatement("SELECT Title,Year,Addition,Type FROM Movies WHERE MovieID=?");

			selectDirectorByMovieID=connection.prepareStatement("SELECT Name,DirectorID From DirectsMovie D, Persons P WHERE D.MovieID=? AND D.DirectorID=P.PersonID");	
			selectProducerByMovieID=connection.prepareStatement("SELECT Name,ProducerID,Extra From producesMovie D, Persons P WHERE D.MovieID=? AND D.ProducerID=P.PersonID");	

			selectCastByMovieIDOrderByCredits=connection.prepareStatement("SELECT Name,PersonID,role,position FROM Persons P, ActsInMovie A WHERE P.PersonID=A.ActorID AND A.MovieID=? ORDER BY A.Position ASC");
			selectCastCountByMovieID=connection.prepareStatement("SELECT count(*) FROM ActsInMovie A WHERE A.MovieID=?");

			selectPlotByMovieID=connection.prepareStatement("SELECT author,plot FROM Plots WHERE MovieID=?");
			selectKeywordsByMovieID=connection.prepareStatement("SELECT keyword FROM Keywords WHERE MovieID=?");
			selectTaglinesByMovieID=connection.prepareStatement("SELECT tagline FROM taglines WHERE MovieID=?");
			selectGoofByMovieID=connection.prepareStatement("SELECT type,goof FROM goofs WHERE MovieID=?");
			selectMovieTriviaByMovieID=connection.prepareStatement("SELECT trivia FROM movietrivia WHERE MovieID=?");
			selectGenresByMovieID=connection.prepareStatement("SELECT genre FROM Genres WHERE MovieID=?");
			selectAlternateVersionsByMovieID=connection.prepareStatement("SELECT version FROM AlternateVersions WHERE MovieID=?");
			selectAlternativeTitlesByMovieID=connection.prepareStatement("SELECT alternativetitle FROM AlternativeTitles WHERE MovieID=?");
			selectCountryByMovieID=connection.prepareStatement("SELECT country FROM countries WHERE MovieID=? order by position");
			selectColourInfoByMovieID=connection.prepareStatement("SELECT colourinfo FROM colourinfo WHERE MovieID=?");
			selectSoundMixByMovieID=connection.prepareStatement("SELECT soundmix FROM soundmix WHERE MovieID=?");
			selectLanguageByMovieID=connection.prepareStatement("SELECT language FROM languages WHERE MovieID=? order by position");
			selectLocationByMovieID=connection.prepareStatement("SELECT location FROM locations WHERE MovieID=?");

			selectKeywordsCountByMovieID=connection.prepareStatement("SELECT COUNT(*) FROM Keywords WHERE MovieID=?");
			selectTaglinesCountByMovieID=connection.prepareStatement("SELECT COUNT(*) FROM Taglines WHERE MovieID=?");
			selectGoofCountByMovieID=connection.prepareStatement("SELECT count(*) FROM goofs WHERE MovieID=?");
			selectMovieTriviaCountByMovieID=connection.prepareStatement("SELECT count(*) FROM movietrivia WHERE MovieID=?");
			selectGenresCountByMovieID=connection.prepareStatement("SELECT COUNT(*) FROM Genres WHERE MovieID=?");
			selectAlternateVersionsCountByMovieID=connection.prepareStatement("SELECT COUNT(*) FROM AlternateVersions WHERE MovieID=?");
			selectAlternativeTitlesCountByMovieID=connection.prepareStatement("SELECT COUNT(*) FROM AlternativeTitles WHERE MovieID=?");
			selectCountryCountByMovieID=connection.prepareStatement("SELECT count(*) FROM countries WHERE MovieID=?");

			selectMovieLinksByMovieID=connection.prepareStatement("SELECT targetID,L.type,title,year,addition,M.type FROM MovieLinks L, Movies M WHERE SourceID=? AND TargetID=MovieID");

			selectMovieLinksCountByMovieID=connection.prepareStatement("SELECT COUNT(*) FROM MovieLinks WHERE SourceID=?");

			selectPersonByID=connection.prepareStatement("SELECT name,sex,RealName,NickName,DateBirth,PlaceBirth,DateDeath,PlaceDeath,CauseDeath,Height FROM Persons WHERE PersonID=?");

			selectFullBiographyByPersonID=connection.prepareStatement("SELECT author,biography FROM FullBiographies WHERE PersonID=?");
			selectActorTriviaByPersonID=connection.prepareStatement("SELECT trivia FROM actortrivia WHERE personID=?");
			selectActorTriviaCountByPersonID=connection.prepareStatement("SELECT count(*) FROM actortrivia WHERE personID=?");
			selectActorQuotesByPersonID=connection.prepareStatement("SELECT quote FROM actorquotes WHERE personID=?");
			selectActorQuotesCountByPersonID=connection.prepareStatement("SELECT count(*) FROM actorquotes WHERE personID=?");
			selectAgentsByPersonID=connection.prepareStatement("SELECT agent FROM agents WHERE personID=?");
			selectAgentsCountByPersonID=connection.prepareStatement("SELECT count(*) FROM agents WHERE personID=?");
			selectAlternativeNamesByPersonID=connection.prepareStatement("SELECT alternativename FROM alternativenames WHERE personID=?");
			selectAlternativeNamesCountByPersonID=connection.prepareStatement("SELECT count(*) FROM alternativenames WHERE personID=?");
			selectArticlesByPersonID=connection.prepareStatement("SELECT article FROM articles WHERE personID=?");
			selectArticlesCountByPersonID=connection.prepareStatement("SELECT count(*) FROM articles WHERE personID=?");
			selectBiographicalBooksByPersonID=connection.prepareStatement("SELECT book FROM biographicalbooks WHERE personID=?");
			selectBiographicalBooksCountByPersonID=connection.prepareStatement("SELECT count(*) FROM biographicalbooks WHERE personID=?");
			selectBiographicalFilmsByPersonID=connection.prepareStatement("SELECT film FROM biographicalfilms WHERE personID=?");
			selectBiographicalFilmsCountByPersonID=connection.prepareStatement("SELECT count(*) FROM biographicalfilms WHERE personID=?");
			selectCoverPhotosByPersonID=connection.prepareStatement("SELECT coverphoto FROM coverphotos WHERE personID=?");
			selectCoverPhotosCountByPersonID=connection.prepareStatement("SELECT count(*) FROM coverphotos WHERE personID=?");
			selectGuestAppearancesByPersonID=connection.prepareStatement("SELECT guestappearance FROM guestappearances WHERE personID=?");
			selectGuestAppearancesCountByPersonID=connection.prepareStatement("SELECT count(*) FROM guestappearances WHERE personID=?");
			selectInterviewsByPersonID=connection.prepareStatement("SELECT interview FROM interviews WHERE personID=?");
			selectInterviewsCountByPersonID=connection.prepareStatement("SELECT count(*) FROM interviews WHERE personID=?");
			selectOtherWorksByPersonID=connection.prepareStatement("SELECT work FROM otherworks WHERE personID=?");
			selectOtherWorksCountByPersonID=connection.prepareStatement("SELECT count(*) FROM otherworks WHERE personID=?");
			selectPictorialsByPersonID=connection.prepareStatement("SELECT pictorial FROM pictorials WHERE personID=?");
			selectPictorialsCountByPersonID=connection.prepareStatement("SELECT count(*) FROM pictorials WHERE personID=?");
			selectPortraitsByPersonID=connection.prepareStatement("SELECT portrait FROM portraits WHERE personID=?");
			selectPortraitsCountByPersonID=connection.prepareStatement("SELECT count(*) FROM portraits WHERE personID=?");
			selectSalariesByPersonID=connection.prepareStatement("SELECT movieid,salary FROM salaries WHERE personID=?");
			selectSalariesByPersonIDMovieID=connection.prepareStatement("SELECT salary FROM salaries WHERE personID=? AND movieID=?");
			selectSalariesCountByPersonID=connection.prepareStatement("SELECT count(*) FROM salaries WHERE personID=?");
			selectSpousesByPersonID=connection.prepareStatement("SELECT spouse FROM spouses WHERE personID=?");
			selectSpousesCountByPersonID=connection.prepareStatement("SELECT count(*) FROM spouses WHERE personID=?");
			selectTrademarksByPersonID=connection.prepareStatement("SELECT trademark FROM trademarks WHERE personID=?");
			selectTrademarksCountByPersonID=connection.prepareStatement("SELECT count(*) FROM trademarks WHERE personID=?");
			selectWhereNowByPersonID=connection.prepareStatement("SELECT wherenow FROM wherenow WHERE personID=?");
			selectWhereNowCountByPersonID=connection.prepareStatement("SELECT count(*) FROM wherenow WHERE personID=?");
			
			selectActsInMovieCountByPersonID=connection.prepareStatement("SELECT COUNT(*) FROM actsinmovie WHERE ActorID=?");
			selectActsInMovieByPersonID=connection.prepareStatement("SELECT movieid,role FROM ActsInMovie WHERE ActorID=?");
			selectDirectsMovieCountByPersonID=connection.prepareStatement("SELECT COUNT(*) FROM directsmovie WHERE directorID=?");
			selectDirectsMovieByPersonID=connection.prepareStatement("SELECT movieid FROM directsMovie WHERE directorID=?");
			selectProducesMovieCountByPersonID=connection.prepareStatement("SELECT COUNT(*) FROM producesmovie WHERE producerID=?");
			selectProducesMovieByPersonID=connection.prepareStatement("SELECT movieid,extra FROM producesMovie WHERE producerID=?");
			selectComposesMovieCountByPersonID=connection.prepareStatement("SELECT COUNT(*) FROM composesformovie WHERE composerID=?");
			selectComposesMovieByPersonID=connection.prepareStatement("SELECT movieid FROM composesforMovie WHERE composerID=?");
			selectCostumeDesignsMovieCountByPersonID=connection.prepareStatement("SELECT COUNT(*) FROM costumedesignsmovie WHERE designerID=?");
			selectCostumeDesignsMovieByPersonID=connection.prepareStatement("SELECT movieid FROM costumedesignsMovie WHERE designerID=?");
			selectEditsMovieCountByPersonID=connection.prepareStatement("SELECT COUNT(*) FROM editsmovie WHERE editorID=?");
			selectEditsMovieByPersonID=connection.prepareStatement("SELECT movieid FROM editsMovie WHERE editorID=?");
			selectCinematographsMovieCountByPersonID=connection.prepareStatement("SELECT COUNT(*) FROM cinematographsmovie WHERE cinematographerID=?");
			selectCinematographsMovieByPersonID=connection.prepareStatement("SELECT movieid FROM cinematographsMovie WHERE cinematographerID=?");
			
			selectPersonID=connection.prepareStatement("SELECT PersonID FROM Persons WHERE name=?");
			selectAlternativePersonID=connection.prepareStatement("SELECT PersonID FROM AlternativeNames WHERE alternativename=?");

			selectMovieID=connection.prepareStatement("SELECT MovieID FROM Movies WHERE title=? and year=? and addition=? and type=?");
			selectMovieIDNULL=connection.prepareStatement("SELECT MovieID FROM Movies WHERE title=? and year=? and addition IS NULL and type=?");		
			selectAlternativeMovieID=connection.prepareStatement("SELECT MovieID FROM AlternativeTitles WHERE alternativetitle=? and year=? and addition=? and type=?");
			selectAlternativeMovieIDNULL=connection.prepareStatement("SELECT MovieID FROM AlternativeTitles WHERE alternativetitle=? and year=? and addition IS NULL and type=?");		

			selectMiscByMovieID=connection.prepareStatement("SELECT P.PersonID,job,M.extra,P.name FROM Miscellaneous M, Persons P WHERE MovieID=? AND M.PersonID=P.PersonID");
			selectMiscCountByMovieID=connection.prepareStatement("SELECT COUNT(*) FROM Miscellaneous WHERE MovieID=?");
			selectMiscByPersonID=connection.prepareStatement("SELECT MovieID,job,extra FROM Miscellaneous WHERE PersonID=?");
			selectMiscCountByPersonID=connection.prepareStatement("SELECT COUNT(*) FROM Miscellaneous WHERE PersonID=?");
			
			statement=connection.createStatement();
			
		}
	  	catch(Exception e)
	  	{
		  	try
		  	{
				e.printStackTrace();
				connection.close();      
		 	}
		  	catch( Exception e2 )
		  	{
				System.out.println( "Error in createConnection to close the connection: " + e2);
		  	}
		  	System.exit(1);
	  	}
	} 
	
	private void closeDatabase()
	{
		try
		{
			statement.close();
			connection.close();
		}
		catch(Exception e)
		{
			  System.out.println("Exception while closing the database connection: " + e);
		}
	}
	
	private void initFileSystem()
	{
		int moviecnt=0;
		int personcnt=0;
		
		try
		{
			ResultSet rs=statement.executeQuery("SELECT MAX(MovieID) FROM Movies");
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

/*
		try
		{
			ResultSet rs=statement.executeQuery("SELECT MAX(PersonID) FROM Persons");
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
*/	
	}
	
	private void exportMovie(int id)
	{
		String title=null;
		int year=-1;
		String addition=null;
		int type=-1;
		
		File xmlfile=null;
		FileWriter xml=null;
	
		if (id%1000==0) System.out.println("exportMovie("+id+")");
			
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
			}
			rs.close();
			
			xmlfile=new File(DIR+"movies\\"+id2Filename(id));
			xml=new FileWriter(xmlfile);
			
			xml.write("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n\n");
			//xml.write("DOCTYPE movie SYSTEM \"..\\imdb.dtd\">");
			
			xml.write("<movie id=\""+id+"\">\n");
			
			xml.write("\t<title>"+XMLify(title)+"</title>\n");

			selectAlternativeTitlesCountByMovieID.setInt(1,id);
			rs=selectAlternativeTitlesCountByMovieID.executeQuery();
			int alttCount=0;
			if (rs.next()) alttCount=rs.getInt(1);
			rs.close();
			
			if (alttCount>0)
			{
//				xml.write("\t<taglines>\n");
				
				selectAlternativeTitlesByMovieID.setInt(1,id);
				rs=selectAlternativeTitlesByMovieID.executeQuery();
				while (rs.next())
				{
					String tagline=rs.getString(1);
					xml.write("\t<alternative_title>"+XMLify(tagline)+"</alternative_title>\n");
				}
				rs.close();					
	
//				xml.write("\t</taglines>\n");
			}

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
					xml.write("\t\t<production_country>"+XMLify(country)+"</production_country>\n");
				}
				rs.close();					
	
				if (countryCount>1) xml.write("\t</production_countries>\n");
			}
/*
			selectLanguageByMovieID.setInt(1,id);
			rs=selectLanguageByMovieID.executeQuery();
			while (rs.next())
			{
				String language=rs.getString(1);
				xml.write("\t\t<production_language>"+XMLify(language)+"</production_language>\n");
			}
			rs.close();					
*/
			
			selectLocationByMovieID.setInt(1,id);
			rs=selectLocationByMovieID.executeQuery();
			while (rs.next())
			{
				String location=rs.getString(1);
				xml.write("\t\t<production_location>"+XMLify(location)+"</production_location>\n");
			}
			rs.close();				

/*			
			selectDirectorByMovieID.setInt(1,id);
			rs=selectDirectorByMovieID.executeQuery();
			if (rs.next())
			{
				String director=rs.getString(1);
				int directorID=rs.getInt(2);
			
				xml.write("\t<director xmlns:xlink=\"http://www.w3.org/1999/xlink/namespace/\" xlink:type=\"simple\" xlink:href=\""+makeM2PLink(directorID)+"\">\n");	
				xml.write("\t\t"+XMLify(director)+"\n\t</director>\n");
			}
			rs.close();
*/

/*			selectProducerByMovieID.setInt(1,id);
			rs=selectProducerByMovieID.executeQuery();
			if (rs.next())
			{
				String producer=rs.getString(1);
				int producerID=rs.getInt(2);
				String job=rs.getString(3);
			
				xml.write("\t<producer xmlns:xlink=\"http://www.w3.org/1999/xlink/namespace/\" xlink:type=\"simple\" xlink:href=\""+makeM2PLink(producerID)+"\">\n");	
				xml.write("\t\t<name>"+XMLify(producer)+"</name>\n");
				if (job!=null) xml.write("\t\t<job>"+XMLify(job)+"</job>\n");
				xml.write("\t</producer>\n");
			}
			rs.close();
*/
			
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
					exportActorInlined(actorID,xml);
					if (role!=null) xml.write("\t\t\t<role>"+XMLify(role)+"</role>\n");
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
					xml.write("\t\t\t<person xmlns:xlink=\"http://www.w3.org/1999/xlink/namespace/\" xlink:type=\"simple\" xlink:href=\""+makeM2PLink(personID)+"\">"+XMLify(name)+"</person>\n");	
					xml.write("\t\t\t<job>"+XMLify(job)+"</job>\n");
					if (extra!=null) xml.write("\t\t\t<extra>"+XMLify(extra)+"</extra>\n");
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
					String blubb=new String(plotA,0,read);
					if (plot==null) plot=blubb;
					else plot+=blubb;
				}
				
				xml.write("\t<plot");
				if (author!=null) xml.write(" author=\""+XMLify(author)+"\"");
				xml.write(">\n");
				xml.write("\t\t"+XMLify(plot)+"\n");
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
					xml.write("\t\t<genre>"+XMLify(genre)+"</genre>\n");
				}
				rs.close();					
	
				xml.write("\t</genres>\n");

				selectGenresByMovieID.setInt(1,id);
				rs=selectGenresByMovieID.executeQuery();
				while (rs.next())
				{
					String genre=rs.getString(1);
					xml.write("\t<"+XMLify(genre).trim()+"/>\n");
				}
				rs.close();					
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
					xml.write("\t\t<tagline>"+XMLify(tagline)+"</tagline>\n");
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
					xml.write("\t\t<keyword>"+XMLify(keyword)+"</keyword>\n");
				}
				rs.close();					
	
				xml.write("\t</keywords>\n");
			}
/*
			selectColourInfoByMovieID.setInt(1,id);
			rs=selectColourInfoByMovieID.executeQuery();
			while (rs.next())
			{
				String colourinfo=rs.getString(1);
				xml.write("\t\t<colourinfo>"+XMLify(colourinfo)+"</colourinfo>\n");
			}
			rs.close();					
			
			selectSoundMixByMovieID.setInt(1,id);
			rs=selectSoundMixByMovieID.executeQuery();
			while (rs.next())
			{
				String soundmix=rs.getString(1);
				xml.write("\t\t<soundmix>"+XMLify(soundmix)+"</soundmix>\n");
			}
			rs.close();					
*/			
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
/*			
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
					
					xml.write("\t\t<link xmlns:xlink=\"http://www.w3.org/1999/xlink/namespace/\" xlink:type=\"simple\" xlink:href=\""+makeM2MLink(targetid)+"\"");	
					xml.write(" linktype=\""+XMLify(linkType)+"\">\n");
					xml.write("\t\t\t<movie>\n");
					xml.write("\t\t\t\t<title>"+XMLify(targetTitle)+"</title>\n");
					xml.write("\t\t\t\t<production_year>"+targetYear+"</production_year>\n");
					if (targetAddition!=null) xml.write("\t\t\t\t<uniqueid>"+targetAddition+"</uniqueid>\n");
					if (targetType>1) xml.write("\t\t\t\t<type>"+movieTypes[targetType]+"</type>\n");
					xml.write("\t\t\t</movie>\n");
					xml.write("\t\t</link>\n");
				}
				rs.close();					
	
				xml.write("\t</links>\n");
			}
*/			
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
					xml.write("\t\t\t"+XMLify(goof)+"\n");
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
	
	private void exportActorInlined(int id,FileWriter xml)
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
			
			xml.write("<actor id=\""+id+"\">\n");
			
			xml.write("\t<name>"+XMLify(name)+"</name>\n");
			if (realName!=null) xml.write("\t<realname>"+XMLify(realName)+"</realname>\n");
			if (nickName!=null) xml.write("\t<nickname>"+XMLify(nickName)+"</nickname>\n");

			selectAlternativeNamesByPersonID.setInt(1,id);
			rs=selectAlternativeNamesByPersonID.executeQuery();
			while (rs.next())
			{
				String aka=rs.getString(1);
					
				xml.write("\t\t<aka>\n");
				xml.write("\t\t\t"+XMLify(aka)+"\n");
				xml.write("\t\t</aka>\n");
			}
			rs.close();

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
				if (dateBirth.length()>=4)
				{
					String s=dateBirth.substring(dateBirth.length()-4);
					int year=0;
					try
					{year=Integer.parseInt(s);}
					catch (Exception e){year=0;}
					if (year>0) 
						xml.write("\t<birthdate>"+year+"</birthdate>\n");			
				}
			}

			if (placeBirth!=null) xml.write("<birthplace>"+XMLify(placeBirth)+"</birthplace>\n");

			if (dateDeath!=null)
			{
				xml.write("\t<died");
				if (placeDeath!=null) xml.write(" place=\""+XMLify(placeDeath)+"\"");
				if (causeDeath!=null) xml.write(" cause=\""+XMLify(causeDeath)+"\"");
				xml.write(">"+dateDeath+"</died>\n");
			}
			
			/*
			if (height!=null) xml.write("\t<height>"+XMLify(height)+"</height>\n");
			
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
					String blubb=new String(bioA,0,read);
					if (bio==null) bio=blubb;
					else bio+=blubb;
				}
				
				xml.write("\t<biography");
				if (author!=null) xml.write(" author=\""+XMLify(author)+"\"");
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
					
						xml.write("\t\t<acted_in xmlns:xlink=\"http://www.w3.org/1999/xlink/namespace/\" xlink:type=\"simple\" xlink:href=\""+makeP2MLink(movieID)+"\"");
						if (role!=null) xml.write(" role=\""+XMLify(role)+"\"");
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
					
						xml.write("\t\t<directed xmlns:xlink=\"http://www.w3.org/1999/xlink/namespace/\" xlink:type=\"simple\" xlink:href=\""+makeP2MLink(movieID)+"\"/>\n");
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
					
						xml.write("\t\t<produced xmlns:xlink=\"http://www.w3.org/1999/xlink/namespace/\" xlink:type=\"simple\" xlink:href=\""+makeP2MLink(movieID)+"\"");
						if (role!=null) xml.write(" role=\""+XMLify(role)+"\"");
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
					
						xml.write("\t\t<acted_in xmlns:xlink=\"http://www.w3.org/1999/xlink/namespace/\" xlink:type=\"simple\" xlink:href=\""+makeP2MLink(movieID)+"\"/>\n");
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
					
						xml.write("\t\t<composed_for xmlns:xlink=\"http://www.w3.org/1999/xlink/namespace/\" xlink:type=\"simple\" xlink:href=\""+makeP2MLink(movieID)+"\"/>\n");
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
					
						xml.write("\t\t<cinematographed xmlns:xlink=\"http://www.w3.org/1999/xlink/namespace/\" xlink:type=\"simple\" xlink:href=\""+makeP2MLink(movieID)+"\"/>\n");
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
					
						xml.write("\t\t<designed_costumes_for xmlns:xlink=\"http://www.w3.org/1999/xlink/namespace/\" xlink:type=\"simple\" xlink:href=\""+makeP2MLink(movieID)+"\"/>\n");
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
					
						xml.write("\t\t<misc xmlns:xlink=\"http://www.w3.org/1999/xlink/namespace/\" xlink:type=\"simple\" xlink:href=\""+makeP2MLink(movieID)+"\" job=\""+XMLify(job)+"\"");
						if (extra!=null) xml.write(" extra=\""+XMLify(extra)+"\"");
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
					xml.write("\t\t\t"+XMLify(book)+"\n");
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
					xml.write("\t\t\t"+XMLify(agent)+"\n");
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
					xml.write("\t\t\t"+XMLify(cv)+"\n");
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
					xml.write("\t\t\t"+XMLify(it)+"\n");
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
					xml.write("\t\t\t"+XMLify(pt)+"\n");
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
//				if (wnCount>1) xml.write("\t<where_nows>\n");
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
//				if (wnCount>1) xml.write("\t</where_nows>\n");
			}
*/
			xml.write("</actor>\n");
						
		}
		catch(Exception e)
		{
			System.out.println("export actor "+id+": "+e);
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
				if (out==null) out=XMLify(input.substring(i));
				else out+=XMLify(input.substring(i));
				break;
			}
			int epos=input.substring(i+apos+1).indexOf('\'');
			//System.out.println("epos="+epos);
			if (epos==-1)
			{
				if (out==null) out=XMLify(input.substring(i));
				else out+=XMLify(input.substring(i));
				break;
			}
			//System.out.println("substring="+input.substring(i+apos+epos+2));
			if (!input.substring(i+apos+epos+2).startsWith(" (qv)"))
			{
				if (out==null) out=XMLify(input.substring(i,i+apos));
				else out+=XMLify(input.substring(i,i+apos));
				i+=apos+1;
				continue;
			}
			if (out==null) out=XMLify(input.substring(i,i+apos));
			else out+=XMLify(input.substring(i,i+apos));
			//System.out.println("added "+input.substring(i,i+apos));
			String link=input.substring(i+apos+1,i+apos+epos+1);
			System.out.println("link="+link);
			int personID=findPerson(link);
			if (personID==-1)
			{
				if (out==null) out=XMLify(link);
				else out+=XMLify(link);
			}
			else
			{
				if (out==null) out="<person xmlns:xlink=\"http://www.w3.org/1999/xlink/namespace/\" xlink:type=\"simple\" xlink:href=\"../people/"+personID+".xml\">"+XMLify(link)+"</person>";
				else out+="<person xmlns:xlink=\"http://www.w3.org/1999/xlink/namespace/\" xlink:type=\"simple\" xlink:href=\"../people/"+personID+".xml\">"+XMLify(link)+"</person>";
			}
			i+=apos+epos+2+4;
		}
		return out;
	}

	private String XMLify(String input)
	{
		//System.out.print("XMLify("+input+")=");
		input=input.replaceAll("&","&amp;");
		input=input.replaceAll("<","&lt;");
		input=input.replaceAll(">","&gt;");
		input=input.replaceAll("\"","&quot;");
		input=input.replaceAll("'","&apos;");
		//System.out.println(input);
		return input;
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
				if (out==null) out=XMLify(input.substring(i));
				else out+=XMLify(input.substring(i));
				break;
			}
			int epos=input.substring(i+apos+1).indexOf('_');
			//System.out.println("epos="+epos);
			if (epos==-1)
			{
				if (out==null) out=XMLify(input.substring(i));
				else out+=XMLify(input.substring(i));
				break;
			}
			//System.out.println("substring="+input.substring(i+apos+epos+2));
			if (!input.substring(i+apos+epos+2).startsWith(" (qv)"))
			{
				if (out==null) out=XMLify(input.substring(i,i+apos));
				else out+=XMLify(input.substring(i,i+apos));
				i+=apos+1;
				continue;
			}
			if (out==null) out=XMLify(input.substring(i,i+apos));
			else out+=XMLify(input.substring(i,i+apos));
			//System.out.println("added "+input.substring(i,i+apos));
			String link=input.substring(i+apos+1,i+apos+epos+1);
			System.out.println("movie link="+link);
			int movieID=findMovie(link);
			if (movieID==-1)
			{
				if (out==null) out=XMLify(link);
				else out+=XMLify(link);
			}
			else
			{
				if (out==null) out="<movie xmlns:xlink=\"http://www.w3.org/1999/xlink/namespace/\" xlink:type=\"simple\" xlink:href=\""+movieID+".xml\">"+XMLify(link)+"</movie>";
				else out+="<movie xmlns:xlink=\"http://www.w3.org/1999/xlink/namespace/\" xlink:type=\"simple\" xlink:href=\""+movieID+".xml\">"+XMLify(link)+"</movie>";
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
				if (out==null) out=XMLify(input.substring(i));
				else out+=XMLify(input.substring(i));
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
						if (out==null) out=XMLify(input.substring(i));
						else out+=XMLify(input.substring(i));
						break;
					}
					else
					{
						if (out==null) out=XMLify(input.substring(i,i+Apos));
						else out+=XMLify(input.substring(i,i+Apos));
						i+=Apos-1;
						continue;
					}
				}
				//System.out.println("substring="+input.substring(i+apos+epos+2));
				if (!input.substring(i+apos+epos+2).startsWith(" (qv)"))
				{
					if (out==null) out=XMLify(input.substring(i,i+apos));
					else out+=XMLify(input.substring(i,i+apos));
					i+=apos;
					continue;
				}
				
				if (out==null) out=XMLify(input.substring(i,i+apos));
				else out+=XMLify(input.substring(i,i+apos));
				//System.out.println("added "+input.substring(i,i+apos));
				String link=input.substring(i+apos+1,i+apos+epos+1);
				//System.out.print("person link="+link);
				int personID=findPerson(link);
				if (personID==-1)
				{
					//System.out.println(" (not found)");
					if (out==null) out=XMLify(link);
					else out+=XMLify(link);
				}
				else
				{
					//System.out.println(" ("+personID+")");
					if (out==null) out="<person xmlns:xlink=\"http://www.w3.org/1999/xlink/namespace/\" xlink:type=\"simple\" xlink:href=\""+makeM2PLink(personID)+"\">"+XMLify(link)+"</person>";
					else out+="<person xmlns:xlink=\"http://www.w3.org/1999/xlink/namespace/\" xlink:type=\"simple\" xlink:href=\""+makeM2PLink(personID)+"\">"+XMLify(link)+"</person>";
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
						if (out==null) out=XMLify(input.substring(i));
						else out+=XMLify(input.substring(i));
						break;
					}
					else
					{
						if (out==null) out=XMLify(input.substring(i,i+apos));
						else out+=XMLify(input.substring(i,i+apos));
						i+=apos-1;
						continue;
					}
				}
				//System.out.println("substring="+input.substring(i+apos+epos+2));
				if (!input.substring(i+Apos+Epos+2).startsWith(" (qv)"))
				{
					if (out==null) out=XMLify(input.substring(i,i+Apos));
					else out+=XMLify(input.substring(i,i+Apos));
					i+=(Apos);
					continue;
				}
				if (out==null) out=XMLify(input.substring(i,i+Apos));
				else out+=XMLify(input.substring(i,i+Apos));
				//System.out.println("added "+input.substring(i,i+apos));
				String link=input.substring(i+Apos+1,i+Apos+Epos+1);
				//System.out.print("movie link="+link);
				int movieID=findMovie(link);
				if (movieID==-1)
				{
					//System.out.println(" (not found)");
					if (out==null) out=XMLify(link);
					else out+=XMLify(link);
				}
				else
				{
					//System.out.println(" ("+movieID+")");
					if (out==null) out="<movie xmlns:xlink=\"http://www.w3.org/1999/xlink/namespace/\" xlink:type=\"simple\" xlink:href=\""+makeP2MLink(movieID)+"\">"+XMLify(link)+"</movie>";
					else out+="<movie xmlns:xlink=\"http://www.w3.org/1999/xlink/namespace/\" xlink:type=\"simple\" xlink:href=\""+makeP2MLink(movieID)+"\">"+XMLify(link)+"</movie>";
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

}
