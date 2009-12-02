package de.mpii.clix.imdb;

import java.io.*;
import java.sql.*;
import java.util.zip.*;
import java.util.*;


import de.mpii.clix.support.*;

public class IMDB2DBNew
{

	private static String DIR="c:\\projects\\clix\\imdb\\lists-270309\\"; // source directory
	private static String dbUser="imdb2009"; // insert your DB user here
	private static String dbPassword="imdb2009"; // insert your DB password here
	private static String dbHost="mpiat5316"; // insert your DB host here
	private static String dbServiceName="destiny.mpi"; // insert your DB service name here

	private Database db;
	
	private PreparedStatement selectPersonID;
	private PreparedStatement selectMaxPersonID;
	private PreparedStatement insertPerson;
	private PreparedStatement updatePerson;
	private PreparedStatement updatePersonSex;
	private PreparedStatement insertAlternativeName;
	private PreparedStatement selectAlternativePersonID;
	
	private PreparedStatement insertMovie;
	private PreparedStatement selectMovieID;
	private PreparedStatement selectMovieIDAdditionNULL;
	private PreparedStatement selectMovieIDNULLEpisode;
	private PreparedStatement selectMovieIDNULLNULL;
	private PreparedStatement selectAlternativeMovieID;
	private PreparedStatement selectAlternativeMovieIDAdditionNULL;
	private PreparedStatement selectAlternativeMovieIDNULLEpisode;
	private PreparedStatement selectAlternativeMovieIDNULLNULL;
	
	private PreparedStatement insertActsInMovie;
	private PreparedStatement insertDirectsMovie;
	private PreparedStatement insertComposesForMovie;
	private PreparedStatement insertEditsMovie;
	private PreparedStatement insertProducesMovie;
	private PreparedStatement insertCinematographsMovie;
	private PreparedStatement insertCostumeDesignsMovie;

	private PreparedStatement insertSoundMix;
	private PreparedStatement insertColourInfo;
	private PreparedStatement insertGenre;
	private PreparedStatement insertKeyword;
	private PreparedStatement insertMovieLink;
	private PreparedStatement insertTagline;
	private PreparedStatement insertPlot;
	private PreparedStatement insertLocation;
	private PreparedStatement insertLanguage;
	private PreparedStatement insertCertificate;
	private PreparedStatement insertCountry;

	private PreparedStatement insertFullBiography;
	private PreparedStatement insertBiographicalBook;
	private PreparedStatement insertBiographicalFilm;
	private PreparedStatement insertPortrait;
	private PreparedStatement insertOtherWork;
	private PreparedStatement insertActorTrivia;
	private PreparedStatement insertSalary;
	private PreparedStatement insertWhereNow;
	private PreparedStatement insertActorQuote;
	private PreparedStatement insertAgent;
	private PreparedStatement insertSpouse;
	private PreparedStatement insertTrademark;
	private PreparedStatement insertInterview;
	private PreparedStatement insertArticle;
	private PreparedStatement insertPictorial;
	private PreparedStatement insertCoverPhoto;
	private PreparedStatement insertGuestAppearance;
	private PreparedStatement insertAlternativeTitle;
	private PreparedStatement insertMovieTrivia;
	private PreparedStatement insertGoof;
	private PreparedStatement insertMiscellaneous;
	private PreparedStatement insertAlternateVersion;

	public static void main(String[] args)
	{
		System.out.println("CLiX/IMDB2DB $Revision: 1.2 $\n");
		
		System.out.println("CLiX/IMDB2DB started.");

		IMDB2DBNew converter=new IMDB2DBNew();
		
		converter.initDatabase();
//		converter.dropTables();
		converter.createTables();
		
		converter.createMovieTitleIndex();
		converter.readMoviesList();
		
		converter.readAlternativeTitles();
		converter.readGermanAlternativeTitles();
		converter.readISOAlternativeTitles();
		converter.readItalianAlternativeTitles();
		
		converter.createPersonNameIndex();
		
		converter.readActorsList(true);
		converter.readActorsList(false);
		
		converter.readDirectorsList();
		converter.readComposersList();
		converter.readEditorsList();
		converter.readProducersList();
		converter.readCinematographersList();
		converter.readCostumeDesignersList();
		converter.readMiscellaneousList();
						
		converter.readAlternativeNames();
		
		converter.readSoundMixList();
		converter.readColourInfoList();
		converter.readGenresList();
		converter.readKeywordsList();
		converter.readMovieLinks();
		converter.readTaglines();
		converter.readGoofList();
		converter.readTriviaList();
		converter.readPlots();
		converter.readAlternateVersionList();
		converter.readLocationsList();
		converter.readLanguagesList();
		converter.readCertificatesList();
		converter.readCountriesList();
		
		converter.readBiographies();
		
		converter.createIndexes();
		
		System.out.println("CLiX/IMDB2DB finished.");
	}
	
	private void initDatabase()
	{
	    db=new Database();
	    db.openDatabase(dbUser,dbPassword,dbHost,dbServiceName);
	    
	  try
	  {		
			insertMovie=db.prepareStatement("INSERT INTO MOVIES VALUES (?,?,?,?,?,?)");
			selectMovieID=db.prepareStatement("SELECT MovieID FROM Movies WHERE title=? and year=? and addition=? and type=? and episode=?");
			selectMovieIDNULLEpisode=db.prepareStatement("SELECT MovieID FROM Movies WHERE title=? and year=? and addition IS NULL and type=? and episode=?");		
			selectMovieIDAdditionNULL=db.prepareStatement("SELECT MovieID FROM Movies WHERE title=? and year=? and addition=? and type=? and episode IS NULL");		
			selectMovieIDNULLNULL=db.prepareStatement("SELECT MovieID FROM Movies WHERE title=? and year=? and addition IS NULL and type=? and episode IS NULL");		
			selectAlternativeMovieID=db.prepareStatement("SELECT MovieID FROM AlternativeTitles WHERE alternativetitle=? and year=? and addition=? and type=? and episode=?");
			selectAlternativeMovieIDNULLEpisode=db.prepareStatement("SELECT MovieID FROM AlternativeTitles WHERE alternativetitle=? and year=? and addition IS NULL and type=? and episode=?");		
			selectAlternativeMovieIDAdditionNULL=db.prepareStatement("SELECT MovieID FROM AlternativeTitles WHERE alternativetitle=? and year=? and addition=? and type=? and episode IS NULL");		
			selectAlternativeMovieIDNULLNULL=db.prepareStatement("SELECT MovieID FROM AlternativeTitles WHERE alternativetitle=? and year=? and addition IS NULL and type=? and episode IS NULL");		

			selectPersonID=db.prepareStatement("SELECT PersonID FROM Persons WHERE name=?");
			selectMaxPersonID=db.prepareStatement("SELECT MAX(PersonID) FROM Persons");
			insertPerson=db.prepareStatement("INSERT INTO Persons VALUES (?,?,?,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
			updatePerson=db.prepareStatement("UPDATE Persons SET RealName=?, NickName=?, DateBirth=?, PlaceBirth=?, DateDeath=?, PlaceDeath=?, CauseDeath=?, Height=? WHERE PersonID=?");
			updatePersonSex=db.prepareStatement("UPDATE Persons SET Sex=? WHERE PersonID=?");
			selectAlternativePersonID=db.prepareStatement("SELECT PersonID FROM AlternativeNames WHERE alternativename=?");
			insertAlternativeName=db.prepareStatement("INSERT INTO AlternativeNames VALUES (?,?)");
			
			insertActsInMovie=db.prepareStatement("INSERT INTO ActsInMovie VALUES (?,?,?,?)");
			insertDirectsMovie=db.prepareStatement("INSERT INTO DirectsMovie VALUES (?,?,?)");
			insertComposesForMovie=db.prepareStatement("INSERT INTO ComposesForMovie VALUES (?,?,?)");
			insertEditsMovie=db.prepareStatement("INSERT INTO EditsMovie VALUES (?,?,?)");
			insertProducesMovie=db.prepareStatement("INSERT INTO ProducesMovie VALUES(?,?,?)");
			insertCinematographsMovie=db.prepareStatement("INSERT INTO CinematographsMovie VALUES(?,?)");
			insertCostumeDesignsMovie=db.prepareStatement("INSERT INTO CostumeDesignsMovie VALUES(?,?)");
			insertMiscellaneous=db.prepareStatement("INSERT INTO Miscellaneous VALUES(?,?,?,?)");

			insertSoundMix=db.prepareStatement("INSERT INTO SoundMix VALUES(?,?)");
			insertColourInfo=db.prepareStatement("INSERT INTO ColourInfo VALUES(?,?)");
			insertGenre=db.prepareStatement("INSERT INTO Genres VALUES(?,?)");
			insertKeyword=db.prepareStatement("INSERT INTO Keywords VALUES(?,?)");
			insertMovieLink=db.prepareStatement("INSERT INTO MovieLinks VALUES(?,?,?)");
			insertTagline=db.prepareStatement("INSERT INTO Taglines VALUES (?,?)");
			insertPlot=db.prepareStatement("INSERT INTO Plots VALUES (?,?,?)");
			insertLocation=db.prepareStatement("INSERT INTO Locations VALUES(?,?,?)");
			insertGoof=db.prepareStatement("INSERT INTO Goofs VALUES(?,?,?)");
			insertMovieTrivia=db.prepareStatement("INSERT INTO MovieTrivia VALUES(?,?)");
			insertLanguage=db.prepareStatement("INSERT INTO Languages VALUES(?,?,?)");
			insertCertificate=db.prepareStatement("INSERT INTO Certificates VALUES(?,?)");
			insertCountry=db.prepareStatement("INSERT INTO Countries VALUES(?,?,?)");

			insertFullBiography=db.prepareStatement("INSERT INTO FullBiographies VALUES(?,?,?)");
			insertBiographicalBook=db.prepareStatement("INSERT INTO BiographicalBooks VALUES(?,?)");
			insertBiographicalFilm=db.prepareStatement("INSERT INTO BiographicalFilms VALUES(?,?)");
			insertPortrait=db.prepareStatement("INSERT INTO Portraits VALUES(?,?)");
			insertOtherWork=db.prepareStatement("INSERT INTO OtherWorks VALUES(?,?)");
			insertActorTrivia=db.prepareStatement("INSERT INTO ActorTrivia VALUES(?,?)");
			insertSalary=db.prepareStatement("INSERT INTO Salaries VALUES(?,?,?)");
			insertWhereNow=db.prepareStatement("INSERT INTO WhereNow VALUES(?,?)");
			insertActorQuote=db.prepareStatement("INSERT INTO ActorQuotes VALUES(?,?)");
			insertAgent=db.prepareStatement("INSERT INTO Agents VALUES(?,?)");
			insertSpouse=db.prepareStatement("INSERT INTO Spouses VALUES(?,?)");
			insertTrademark=db.prepareStatement("INSERT INTO Trademarks VALUES(?,?)");
			insertInterview=db.prepareStatement("INSERT INTO Interviews VALUES(?,?)");
			insertArticle=db.prepareStatement("INSERT INTO Articles VALUES(?,?)");
			insertPictorial=db.prepareStatement("INSERT INTO Pictorials VALUES(?,?)");
			insertCoverPhoto=db.prepareStatement("INSERT INTO CoverPhotos VALUES(?,?)");
			insertGuestAppearance=db.prepareStatement("INSERT INTO GuestAppearances VALUES(?,?)");
			insertAlternativeTitle=db.prepareStatement("INSERT INTO AlternativeTitles VALUES(?,?,?,?,?,?,?)");
			insertAlternateVersion=db.prepareStatement("INSERT INTO AlternateVersions VALUES(?,?)");
	  	}
	  	catch(Exception e)
	  	{
	  	    e.printStackTrace();
	  	    System.exit(1);
	  	}
	} // end creatConnection
	
	private void dropTables()
	{
		try
		{
			db.execUpdate("DROP TABLE Movies");
			db.execUpdate("DROP TABLE AlternativeTitles");
			
			db.execUpdate("DROP TABLE Persons");
			db.execUpdate("DROP TABLE AlternativeNames");
			
			db.execUpdate("DROP TABLE ActsInMovie");

			db.execUpdate("DROP TABLE DirectsMovie");
			db.execUpdate("DROP TABLE ComposesForMovie");
			db.execUpdate("DROP TABLE EditsMovie");
			db.execUpdate("DROP TABLE ProducesMovie");
			db.execUpdate("DROP TABLE CinematographsMovie");
			db.execUpdate("DROP TABLE CostumeDesignsMovie");
			db.execUpdate("DROP TABLE Miscellaneous");

			db.execUpdate("DROP TABLE SoundMix");
			db.execUpdate("DROP TABLE ColourInfo");
			db.execUpdate("DROP TABLE Genres");
			db.execUpdate("DROP TABLE Keywords");
			db.execUpdate("DROP TABLE MovieLinks");
			db.execUpdate("DROP TABLE Taglines");
			db.execUpdate("DROP TABLE Plots");
			db.execUpdate("DROP TABLE Locations");
			db.execUpdate("DROP TABLE Goofs");
			db.execUpdate("DROP TABLE MovieTrivia");
			db.execUpdate("DROP TABLE AlternateVersions");
			db.execUpdate("DROP TABLE Languages");
			db.execUpdate("DROP TABLE Certificates");
			db.execUpdate("DROP TABLE Countries");
			
			db.execUpdate("DROP TABLE FullBiographies");
			db.execUpdate("DROP TABLE BiographicalBooks");
			db.execUpdate("DROP TABLE BiographicalFilms");
			db.execUpdate("DROP TABLE Portraits");
			db.execUpdate("DROP TABLE OtherWorks");
			db.execUpdate("DROP TABLE ActorTrivia");
			db.execUpdate("DROP TABLE Salaries");
			db.execUpdate("DROP TABLE WhereNow");
			db.execUpdate("DROP TABLE ActorQuotes");
			db.execUpdate("DROP TABLE Agents");
			db.execUpdate("DROP TABLE Spouses");
			db.execUpdate("DROP TABLE TradeMarks");
			db.execUpdate("DROP TABLE Interviews");
			db.execUpdate("DROP TABLE Articles"); 
			db.execUpdate("DROP TABLE Pictorials");
			db.execUpdate("DROP TABLE CoverPhotos");
			db.execUpdate("DROP TABLE GuestAppearances");
		}
		catch(Exception e)
		{
			System.out.println("Exception while dropping tables (skipped remaining drops):\n"+e);
		}
	}

	private void createTables()
	{
		try
		{
			db.execUpdate("CREATE TABLE Movies(MovieID number(10) PRIMARY KEY, Title VARCHAR(500), Year number(4), Addition VARCHAR(10), Type NUMBER(1), Episode VARCHAR2(500))");
			db.execUpdate("CREATE TABLE AlternativeTitles(MovieID number(10), AlternativeTitle varchar(500), Year Number(30), addition varchar(10), Type Number(1), Episode VARCHAR2(500), Commentary VARCHAR(300))");

			db.execUpdate("CREATE TABLE Persons(PersonID number(10) PRIMARY KEY, name varchar(500), sex number(1), RealName VARCHAR(500), NickName VARCHAR(500), DateBirth VARCHAR(200), PlaceBirth VARCHAR(500), DateDeath VARCHAR(200), PlaceDeath VARCHAR(500), CauseDeath VARCHAR(500), Height VARCHAR(30))");
			db.execUpdate("CREATE TABLE AlternativeNames(PersonID number(10), AlternativeName varchar(500))");
			
			db.execUpdate("CREATE TABLE ActsInMovie(ActorID number(10), MovieID number(10), Role VARCHAR(1000), Position Number(10))");
			db.execUpdate("CREATE TABLE DirectsMovie(DirectorID number(10), MovieID number(10), extra VARCHAR(500))");
			db.execUpdate("CREATE TABLE ComposesForMovie(ComposerID number(10), MovieID number(10), extra VARCHAR(500))");
			db.execUpdate("CREATE TABLE EditsMovie(EditorID number(10), MovieID number(10), extra VARCHAR(500))");
			db.execUpdate("CREATE TABLE ProducesMovie(ProducerID number(10), MovieID number(10), extra VARCHAR(500))");
			db.execUpdate("CREATE TABLE CinematographsMovie(CinematographerID number(10), MovieID number(10))");
			db.execUpdate("CREATE TABLE CostumeDesignsMovie(DesignerID number(10), MovieID number(10))");
			db.execUpdate("CREATE TABLE Miscellaneous(PersonID number(10), MovieID number(10), job VARCHAR(500), extra VARCHAR(500))");

			db.execUpdate("CREATE TABLE SoundMix(MovieID number(10), SoundMix VARCHAR(100))");
			db.execUpdate("CREATE TABLE ColourInfo(MovieID number(10), ColourInfo VARCHAR(100))");
			db.execUpdate("CREATE TABLE Genres(MovieID number(10), Genre VARCHAR(100))");
			db.execUpdate("CREATE TABLE Keywords(MovieID number(10), Keyword VARCHAR(100))");
			db.execUpdate("CREATE TABLE MovieLinks(sourceID number(10), targetID number(10), type varchar(100))");
			db.execUpdate("CREATE TABLE Taglines(MovieID number(10), tagline varchar(2000))");
			db.execUpdate("CREATE TABLE Plots(MovieID number(10), author varchar(100), plot LONG)");
			db.execUpdate("CREATE TABLE Locations(MovieID number(10), location VARCHAR(500), extra varchar(500))");
			db.execUpdate("CREATE TABLE Goofs(MovieID number(10), type VARCHAR(10),goof LONG)");
			db.execUpdate("CREATE TABLE MovieTrivia(MovieID number(10), trivia LONG)");
			db.execUpdate("CREATE TABLE AlternateVersions(MovieID number(10), version LONG)");
			db.execUpdate("CREATE TABLE Languages(MovieID number(10), language varchar(100), position number(2))");
			db.execUpdate("CREATE TABLE Certificates(MovieID number(10), certificate varchar(1000))");
			db.execUpdate("CREATE TABLE Countries(MovieID number(10), country varchar(100), position number(2))");
			
			db.execUpdate("CREATE TABLE FullBiographies(PersonID number(10), Author VARCHAR(200), Biography LONG)");
			db.execUpdate("CREATE TABLE BiographicalBooks(PersonID number(10), Book varchar(500))");
			db.execUpdate("CREATE TABLE BiographicalFilms(PersonID number(10), Film varchar(500))");
			db.execUpdate("CREATE TABLE Portraits(PersonID number(10), Portrait varchar(500))");
			db.execUpdate("CREATE TABLE OtherWorks(PersonID number(10), Work LONG)");
			db.execUpdate("CREATE TABLE ActorTrivia(PersonID number(10), Trivia LONG)");
			db.execUpdate("CREATE TABLE Salaries(PersonID number(10), MovieID number(10), Salary varchar(500))");
			db.execUpdate("CREATE TABLE WhereNow(PersonID number(10), WhereNow LONG)");
			db.execUpdate("CREATE TABLE ActorQuotes(PersonID number(10), Quote LONG)");
			db.execUpdate("CREATE TABLE Agents(PersonID number(10), Agent varchar(500))");
			db.execUpdate("CREATE TABLE Spouses(PersonID number(10), Spouse varchar(500))");
			db.execUpdate("CREATE TABLE TradeMarks(PersonID number(10), TradeMark LONG)");
			db.execUpdate("CREATE TABLE Interviews(PersonID number(10), Interview varchar(500))");
			db.execUpdate("CREATE TABLE Articles(PersonID number(10), Article LONG)");
			db.execUpdate("CREATE TABLE Pictorials(PersonID number(10), Pictorial varchar(2000))");
			db.execUpdate("CREATE TABLE CoverPhotos(PersonID number(10), CoverPhoto varchar(500))");
			db.execUpdate("CREATE TABLE GuestAppearances(PersonID number(10), GuestAppearance varchar(500))");
		}
		catch(Exception e)
		{
			System.out.println("Exception while creating tables: "+e);
		}
	}
	
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
		String episode=null;
		
		public Title(String title,int year, int type, String addition, String episode)
		{
			this.title=title;
			this.year=year;
			this.type=type;
			this.addition=addition;
			this.episode=episode;
		}
		
		public void print()
		{
			System.out.println("title="+title+"; year="+year+"; type="+type+"; addition="+addition);
		}
		public String toString()
		{
			return title+" ("+year+") "+(episode!=null?"{"+episode+"} ":"")+"["+type+"] ["+addition+"]";
		}
	}
	
	private void createIndexes()
	{
		try
		{
			db.execUpdate("CREATE INDEX IDX_ActorQuotes_PersonID ON ActorQuotes(PersonID)");
			db.execUpdate("CREATE INDEX IDX_ActorTrivia_PersonID ON ActorTrivia(PersonID)");
			db.execUpdate("CREATE INDEX IDX_Agents_PersonID ON Agents(PersonID)");
			db.execUpdate("CREATE INDEX IDX_Articles_PersonID ON Articles(PersonID)");
			db.execUpdate("CREATE INDEX IDX_BioBooks_PersonID ON BiographicalBooks(PersonID)");
			db.execUpdate("CREATE INDEX IDX_BioFilms_PersonID ON BiographicalFilms(PersonID)");
			db.execUpdate("CREATE INDEX IDX_CoverPhotos_PersonID ON CoverPhotos(PersonID)");
			db.execUpdate("CREATE INDEX IDX_FullBiographies_PersonID ON FullBiographies(PersonID)");
			db.execUpdate("CREATE INDEX IDX_GuestAppearances_PersonID ON GuestAppearances(PersonID)");
			db.execUpdate("CREATE INDEX IDX_Interviews_PersonID ON Interviews(PersonID)");
			db.execUpdate("CREATE INDEX IDX_Misc_PersonID ON Miscellaneous(PersonID)");
			db.execUpdate("CREATE INDEX IDX_OtherWorks_PersonID ON OtherWorks(PersonID)");
			db.execUpdate("CREATE INDEX IDX_Pictorials_PersonID ON Pictorials(PersonID)");
			db.execUpdate("CREATE INDEX IDX_Portraits_PersonID ON Portraits(PersonID)");
			db.execUpdate("CREATE INDEX IDX_Salaries_PersonIDMovieID ON Salaries(PersonID,MovieID)");
			db.execUpdate("CREATE INDEX IDX_Spouses_PersonID ON Spouses(PersonID)");
			db.execUpdate("CREATE INDEX IDX_Trademarks_PersonID ON Trademarks(PersonID)");
			db.execUpdate("CREATE INDEX IDX_WhereNow_PersonID ON WhereNow(PersonID)");

//			db.execUpdate("CREATE INDEX IDX_AltNames_PersonID ON AlternativeNames(PersonID)");
//			db.execUpdate("CREATE INDEX IDX_AltNames_Name ON AlternativeNames(AlternativeName)");

//			db.execUpdate("CREATE INDEX IDX_AltTitles_MovieID ON AlternativeTitles(MovieID)");
//			db.execUpdate("CREATE INDEX IDX_AltTitles_Title ON AlternativeTitles(AlternativeTitle)");

			db.execUpdate("CREATE INDEX IDX_AltVersions_MovieID ON AlternateVersions(MovieID)");
			db.execUpdate("CREATE INDEX IDX_ColourInfo_MovieID ON ColourInfo(MovieID)");
			db.execUpdate("CREATE INDEX IDX_Countries_MovieID ON Countries(MovieID)");
			db.execUpdate("CREATE INDEX IDX_Genres_MovieID ON Genres(MovieID)");
			db.execUpdate("CREATE INDEX IDX_Goofs_MovieID ON Goofs(MovieID)");
			db.execUpdate("CREATE INDEX IDX_Keywords_MovieID ON Keywords(MovieID)");
			db.execUpdate("CREATE INDEX IDX_Languages_MovieID ON Languages(MovieID)");
			db.execUpdate("CREATE INDEX IDX_Locations_MovieID ON Locations(MovieID)");
			db.execUpdate("CREATE INDEX IDX_Misc_MovieID ON Miscellaneous(MovieID)");
			db.execUpdate("CREATE INDEX IDX_MovieTrivia_MovieID ON MovieTrivia(MovieID)");
			db.execUpdate("CREATE INDEX IDX_Plots_MovieID ON Plots(MovieID)");
			db.execUpdate("CREATE INDEX IDX_SoundMix_MovieID ON SoundMix(MovieID)");
			db.execUpdate("CREATE INDEX IDX_TagLines_MovieID ON TagLines(MovieID)");

			db.execUpdate("CREATE INDEX IDX_Composes4Movie_MovieID ON ComposesForMovie(MovieID)");
			db.execUpdate("CREATE INDEX IDX_CostumeDesignsMovie_MID ON CostumeDesignsMovie(MovieID)");
			db.execUpdate("CREATE INDEX IDX_CinematographsMovie_MID ON CinematographsMovie(MovieID)");
			db.execUpdate("CREATE INDEX IDX_DirectsMovie_MovieID ON DirectsMovie(MovieID)");
			db.execUpdate("CREATE INDEX IDX_ProducesMovie_MovieID ON ProducesMovie(MovieID)");

			db.execUpdate("CREATE INDEX IDX_Composes4Movie_CID ON ComposesForMovie(ComposerID)");
			db.execUpdate("CREATE INDEX IDX_CostumeDesignsMovie_DID ON CostumeDesignsMovie(DesignerID)");
			db.execUpdate("CREATE INDEX IDX_CinematographsMovie_CID ON CinematographsMovie(CinematographerID)");
			db.execUpdate("CREATE INDEX IDX_DirectsMovie_DID ON DirectsMovie(DirectorID)");
			db.execUpdate("CREATE INDEX IDX_ProducesMovie_PID ON ProducesMovie(ProducerID)");

			db.execUpdate("CREATE INDEX IDX_ActsInMovie_MovieID ON ActsinMovie(MovieID)");
			db.execUpdate("CREATE INDEX IDX_ActsInMovie_ActorIDMovieID ON ActsinMovie(ActorID,MovieID)");

			db.execUpdate("CREATE INDEX IDX_MovieLinks_SourceID ON MovieLinks(SourceID)");

//			db.execUpdate("CREATE INDEX IDX_Movies_Title ON Movies(Title)");

//			db.execUpdate("CREATE INDEX IDX_Persons_Name ON Persons(Name)");
		}
		catch(Exception e)
		{
			System.out.println("Exception while creating indexes: "+e);
			e.printStackTrace();
		}
	}
	
	private void createMovieTitleIndex()
	{
		try
		{
			db.execUpdate("CREATE INDEX IDX_AltTitles_MovieID ON AlternativeTitles(MovieID)");
			db.execUpdate("CREATE INDEX IDX_AltTitles_Title ON AlternativeTitles(AlternativeTitle)");

			db.execUpdate("CREATE INDEX IDX_Movies_Title ON Movies(Title)");
		}
		catch(Exception e)
		{
			System.out.println("Exception while creating indexes: "+e);
			e.printStackTrace();
		}
	}

	private void createPersonNameIndex()
	{
		try
		{
			db.execUpdate("CREATE INDEX IDX_AltNames_PersonID ON AlternativeNames(PersonID)");
			db.execUpdate("CREATE INDEX IDX_AltNames_Name ON AlternativeNames(AlternativeName,PersonID)");

			db.execUpdate("CREATE INDEX IDX_Persons_Name ON Persons(Name,PersonID)");
		}
		catch(Exception e)
		{
			System.out.println("Exception while creating indexes: "+e);
			e.printStackTrace();
		}
	}
	
	private Title demangleComplexTitle(String complexTitle)
	{
//		System.out.println("demangleComplexTitle("+complexTitle+")");
		
		try
		{
		if (complexTitle.endsWith("{{SUSPENDED}}"))
				complexTitle=complexTitle.substring(0,complexTitle.length()-"{{SUSPENDED}}".length()).trim();

		if (complexTitle.indexOf("{{")!=-1)
			complexTitle=complexTitle.substring(0,complexTitle.indexOf("{{")).trim();

		String episode=null;
		int idx=complexTitle.indexOf('{');
		if (idx!=-1)
		{
			episode=complexTitle.substring(idx+1);
			complexTitle=complexTitle.substring(0,idx).trim();
			if (episode.endsWith("}")) episode=episode.substring(0,episode.length()-1);
		}
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

//		System.out.println("stripped title is #"+complexTitle+"#");
//		System.out.println("year="+year);
//		System.out.println("addition="+addition);
//		System.out.println("type="+type);
		
		return new Title(complexTitle,year,type,addition,episode);
		}
		catch(Exception e)
		{
			System.err.println("cannot demangle "+complexTitle);
			e.printStackTrace();
			return null;
		}
	}
	
	private void readMoviesList()
	{
		System.out.println("import movies.");

		boolean inMovie=false;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"movies.list.gz"))));
			String inputStr;
			
			int index=1;
			
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inMovie && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("MOVIES LIST")) 
				{
					inMovie = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
				}
				
				else if (inputStr.indexOf('(')==-1)
					continue;
					
				else if (inMovie)
				{
					// we are reading a line for a movie
					//System.out.println("["+index+"] read line:" +inputStr);
					int tab=inputStr.indexOf("\t");
					String title=inputStr.substring(0,tab);
					Title t=demangleComplexTitle(title);
					
					try
					{
					insertMovie.setInt(1,index);
					insertMovie.setString(2,t.title);
					insertMovie.setInt(3,t.year);
					if (t.addition==null) insertMovie.setNull(4,Types.VARCHAR);
					else insertMovie.setString(4,t.addition);
					insertMovie.setInt(5,t.type);
					if (t.episode!=null)
						insertMovie.setString(6,t.episode);
					else
						insertMovie.setNull(6,Types.VARCHAR);
					
//					System.out.println("title=#"+t.title+"#");
//					System.out.println("year=#"+t.year+"#");
//					System.out.println("addition=#"+t.addition+"#");
//					System.out.println("type=#"+t.type+"#");

					insertMovie.execute();
					}
					catch(Exception e)
					{
						System.err.println("cannot dump "+t);
						e.printStackTrace();
					}
					index++;
				}
				
				if (index%1000==0) System.out.println(index);
				
				//if (index>100) break;
			}  
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);	
		}
	}

	private void readActorsList(boolean male)
	{
		if (male) System.out.println("import actors.");
		else System.out.println("import actresses.");

		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		int cnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+(male?"actors.list.gz":"actresses.list.gz")))));
			String inputStr;
			
			int index=0;
			String name=null; // name of the current actor
			int title=-1;
			String role=null;
			String position=null;

			// get current max person id
			
			ResultSet rs=db.execQuery("SELECT MAX(PersonID) FROM Persons");
			rs.next();
			index=rs.getInt(1);
			rs.close();

			while ((inputStr=buffer.readLine()) != null)
			{
				inputStr=inputStr.replaceAll("\"\\[scrubs\\]\"", "\"scrubs\"");
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith(male?"THE ACTORS LIST":"THE ACTRESSES LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
					buffer.readLine(); // skip Name ...
					buffer.readLine(); // skip "----"
				}
				
				else if (inputStr.indexOf('(')==-1)
					continue;
					
				else if (inActor)
				{
					// we are reading a line for an actor
//					System.out.println("["+index+":"+actorcnt+"] read line:" +inputStr);

					totalcnt++;
					
					title=-1;
					role=null;
					position=null;
					
					int tab=inputStr.indexOf("\t");
					if (tab>0) // we have an actor's name
					{
						name=inputStr.substring(0,tab).trim();
//						System.out.println("["+index+"] found actor "+name+"");
						index=findOrCreatePerson(name,male?1:2);
//						if (male) updatePersonSex.setInt(1,1);
//						else updatePersonSex.setInt(1,2);
//						updatePersonSex.setInt(2,index);
//						
//						updatePersonSex.execute();
						actorcnt++;
						inputStr=inputStr.substring(tab); // remove the actor name and continue with the movie
					}
					
					inputStr=inputStr.trim();
					int lessthan=inputStr.lastIndexOf("<");
					int greaterthan=inputStr.lastIndexOf(">");
					
					
					if ((lessthan!=-1)&&(greaterthan!=-1)&&(greaterthan>lessthan))
					{
						position=inputStr.substring(lessthan+1,greaterthan);
						inputStr=inputStr.substring(0,lessthan).trim();
					}
					
					int squarebracketopen=inputStr.indexOf("[");
					int squarebracketclose=inputStr.lastIndexOf("]");
					if ((squarebracketopen!=-1)&&(squarebracketclose!=-1)&&(squarebracketopen<squarebracketclose))
					{
						role=inputStr.substring(squarebracketopen+1,squarebracketclose);
						inputStr=inputStr.substring(0,squarebracketopen).trim();
					}

					if (inputStr.indexOf("(as ")!=-1)
					{
						// we don't care if the actor appeared under a slightly different name (at least not for now)
						inputStr=inputStr.substring(0,inputStr.indexOf("(as ")).trim();
					}

					if (inputStr.endsWith("(uncredited)"))
					{
						inputStr=inputStr.substring(0,inputStr.lastIndexOf("(uncredited)")).trim();
					}

					if (inputStr.endsWith("(unconfirmed)"))
					{
						inputStr=inputStr.substring(0,inputStr.lastIndexOf("(unconfirmed)")).trim();
					}

					if (inputStr.endsWith("(archive footage)"))
					{
						// we don't care about archive footage
						inputStr=inputStr.substring(0,inputStr.lastIndexOf("(archive footage)")).trim();
					}

					if (inputStr.endsWith("(uncredited)"))
					{
						inputStr=inputStr.substring(0,inputStr.lastIndexOf("(uncredited)")).trim();
					}

					if (inputStr.endsWith("(also archive footage)"))
					{
						// we don't care about archive footage
						inputStr=inputStr.substring(0,inputStr.lastIndexOf("(also archive footage)")).trim();
					}

					if (inputStr.endsWith("(voice)"))
					{
						// we don't care about archive footage
						inputStr=inputStr.substring(0,inputStr.lastIndexOf("(voice)")).trim();
					}

					if (inputStr.endsWith("(rumored)"))
					{
						// we don't care about archive footage
						inputStr=inputStr.substring(0,inputStr.lastIndexOf("(rumored)")).trim();
					}

					if (inputStr.endsWith("(credit only)"))
					{
						// we don't care about archive footage
						inputStr=inputStr.substring(0,inputStr.lastIndexOf("(credit only)")).trim();
					}

					if (inputStr.endsWith("(archive sound)"))
					{
						// we don't care about archive footage
						inputStr=inputStr.substring(0,inputStr.lastIndexOf("(archive sound)")).trim();
					}

					if (inputStr.endsWith("(voice)"))
					{
						// we don't care about archive footage
						inputStr=inputStr.substring(0,inputStr.lastIndexOf("(voice)")).trim();
					}

					if (inputStr.endsWith("(scenes deleted)"))
					{
						// we don't care about archive footage
						inputStr=inputStr.substring(0,inputStr.lastIndexOf("(scenes deleted)")).trim();
					}

					if (inputStr.endsWith("(singing voice)"))
					{
						// we don't care about archive footage
						inputStr=inputStr.substring(0,inputStr.lastIndexOf("(singing voice)")).trim();
					}

					if (inputStr.indexOf("(as ")!=-1)
					{
						// we don't care if the actor appeared under a slightly different name (at least not for now)
						inputStr=inputStr.substring(0,inputStr.indexOf("(as ")).trim();
					}

					if (inputStr.endsWith("()"))
					{
						// we don't care if the actor appeared under a slightly different name (at least not for now)
						inputStr=inputStr.substring(0,inputStr.lastIndexOf("()")).trim();
					}

					if (inputStr.indexOf("(voice: ")!=-1)
					{
						// we don't care if the actor appeared under a slightly different name (at least not for now)
						inputStr=inputStr.substring(0,inputStr.indexOf("(voice: ")).trim();
					}

					int parenthesisclose=inputStr.lastIndexOf(')');
					if (parenthesisclose!=-1) title=findMovie(inputStr.substring(0,Math.min(parenthesisclose+1,inputStr.length())).trim());
					
					if (title==-1)
					{
						System.out.println("\t*** ignored an entry***");
						ignoredcnt++;
						continue;
					}
					
					if (position!=null)
					{
						if (Character.isDigit(position.charAt(0))==false) position=null; // fix: we probably read a wrong position
					}
					
//					System.out.println("\t"+index+" acted in movie "+title+") as "+role+" (pos "+position+" in casting list)");

					insertActsInMovie.setInt(1,index);
					insertActsInMovie.setInt(2,title);
					if (role!=null) insertActsInMovie.setString(3,role);
					else insertActsInMovie.setNull(3, Types.VARCHAR);
					if (position!=null) insertActsInMovie.setString(4,position);
					else insertActsInMovie.setNull(4, Types.VARCHAR);
						
					insertActsInMovie.addBatch();
					cnt++;
					
					if (cnt%1000==0) insertActsInMovie.executeBatch();
					
//					if (actorcnt%1000==0) 
//						System.out.println(actorcnt);
				}
			}			
			insertActsInMovie.executeBatch();
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		if (male) System.out.println("imported "+actorcnt+" actors.");
		else System.out.println("imported "+actorcnt+" actresses.");
		System.out.println("imported "+(totalcnt-ignoredcnt)+" movies, ignored "+ignoredcnt+".");
	}
	
	private void readDirectorsList()
	{
		System.out.println("import directors.");

		StringBuffer sb = new StringBuffer();
		
		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"directors.list.gz"))));
			String inputStr;
			
			int index=0;
			String name=null; // name of the current actor
			int title=-1;
			
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("THE DIRECTORS LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
					buffer.readLine(); // skip Name ...
					buffer.readLine(); // skip "----"
				}
				
				else if (inputStr.indexOf('(')==-1)
					continue;
					
				else if (inActor)
				{
					// we are reading a line for an actor
					//System.out.println("["+index+"] read line:" +inputStr);

					totalcnt++;
					
					title=-1;
					
					int tab=inputStr.indexOf("\t");
					if (tab>0) // we have an actor's name
					{
						name=inputStr.substring(0,tab).trim();
						//System.out.println("["+index+"] found actor "+name+"");
						index=findOrCreatePerson(name);
						actorcnt++;
					}
					
					inputStr=inputStr.trim();

					if (inputStr.endsWith("(uncredited)"))
					{
						inputStr=inputStr.substring(0,inputStr.lastIndexOf("(uncredited)"));
					}
					
					int parenthesisclose=inputStr.lastIndexOf(')');
					if (parenthesisclose!=-1) title=findMovie(inputStr.substring(tab,min(parenthesisclose+1,inputStr.length())).trim());
					
					if (title==-1)
					{
						//System.out.println("\t*** ignored an entry***");
						ignoredcnt++;
						continue;
					}
					
					//System.out.println("\tacted in movie "+title.title+" ("+title.year+"/"+title.addition+"/"+title.type+") as "+role+" (pos "+position+" in casting list)");

					insertDirectsMovie.setInt(1,index);
					insertDirectsMovie.setInt(2,title);
					insertDirectsMovie.setNull(3,Types.VARCHAR);
						
					insertDirectsMovie.execute();
				}
				
				//if (index>100) break;
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("imported "+actorcnt+" directors.");
		System.out.println("imported "+(totalcnt-ignoredcnt)+" movies, ignored "+ignoredcnt+".");
	}

	private void readProducersList()
	{
		System.out.println("import producers.");

		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"producers.list.gz"))));
			String inputStr;
			
			int index=0;
			String name=null; // name of the current actor
			int movieID=-1;
			String extra=null;
			
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("THE PRODUCERS LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
					buffer.readLine(); // skip Name ...
					buffer.readLine(); // skip "----"
				}
				
				else if (inputStr.indexOf('(')==-1)
					continue;
					
				else if (inActor)
				{
					// we are reading a line for an actor
					//System.out.println("["+index+"] read line:" +inputStr);

					totalcnt++;
					
					movieID=-1;
					extra=null;

					inputStr=inputStr.trim();
				
					int tab=inputStr.indexOf("\t");
					if (tab>0) // we have an actor's name
					{
						name=inputStr.substring(0,tab).trim();
						//System.out.println("["+index+"] found producer "+name+"");
						index=findOrCreatePerson(name);
						actorcnt++;
						inputStr=inputStr.substring(tab).trim();
					}
					
					int paropen=inputStr.lastIndexOf('(');
					if (inputStr.substring(paropen).startsWith("(as"))
					{
						inputStr=inputStr.substring(0,paropen).trim();
						paropen=inputStr.lastIndexOf('(');
					}
					
					// what follows is extra information
					
					extra=inputStr.substring(paropen+1,inputStr.length()-1);
					inputStr=inputStr.substring(0,paropen).trim();
					
					//System.out.println("inputStr="+inputStr+"; extra="+extra);
					
					movieID=findMovie(inputStr);
										
					if (movieID==-1)
					{
						//System.out.println("\t*** ignored an entry***");
						ignoredcnt++;
						continue;
					}
					
					//System.out.println("\tproduced movie "+movieID+" as "+extra);

					insertProducesMovie.setInt(1,index);
					insertProducesMovie.setInt(2,movieID);
					insertProducesMovie.setString(3,extra);
						
					insertProducesMovie.execute();
				}
				
				//if (index>100) break;
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("imported "+actorcnt+" producers.");
		System.out.println("imported "+(totalcnt-ignoredcnt)+" movies, ignored "+ignoredcnt+".");
	}

	private void readMiscellaneousList()
	{
		System.out.println("import miscellaneous entries.");

		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"miscellaneous.list.gz"))));
			String inputStr;
			
			int index=0;
			String name=null; // name of the current actor
			int movieID=-1;
			String job=null;
			String extra=null;
			
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("THE MISCELLANEOUS FILMOGRAPHY LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
					buffer.readLine(); // skip blank line
					buffer.readLine(); // skip Name ...
					buffer.readLine(); // skip "----"
				}
				
				else if (inputStr.indexOf('(')==-1)
					continue;
					
				else if (inActor)
				{
					// we are reading a line for an actor
					//System.out.println("["+index+"] read line:" +inputStr);

					totalcnt++;
					
					movieID=-1;
					job=null;
					extra=null;

					inputStr=inputStr.trim();
				
					int tab=inputStr.indexOf("\t");
					if (tab>0) // we have an actor's name
					{
						name=inputStr.substring(0,tab).trim();
						index=findOrCreatePerson(name);
						//System.out.println("["+index+"] found person "+name+"");
						actorcnt++;
						inputStr=inputStr.substring(tab).trim();
					}
					
					int paropen=inputStr.lastIndexOf('(');
					if (inputStr.substring(paropen).startsWith("(as "))
					{
						extra=inputStr.substring(paropen+1,inputStr.length()-1);
						inputStr=inputStr.substring(0,paropen).trim();
						paropen=inputStr.lastIndexOf('(');
					}

					if (inputStr.substring(paropen).startsWith("(episode"))
					{
						if (extra==null) extra=inputStr.substring(paropen+1,inputStr.length()-1);
						else extra+=inputStr.substring(paropen+1,inputStr.length()-1);
						inputStr=inputStr.substring(0,paropen).trim();
						paropen=inputStr.lastIndexOf('(');
					}

					if (inputStr.substring(paropen).startsWith("(uncredited)"))
					{
						if (extra==null) extra=inputStr.substring(paropen+1,inputStr.length()-1);
						else extra+=inputStr.substring(paropen+1,inputStr.length()-1);
						inputStr=inputStr.substring(0,paropen).trim();
						paropen=inputStr.lastIndexOf('(');
					}					
					// what follows is extra information
					
					job=inputStr.substring(paropen+1,inputStr.length()-1);
					inputStr=inputStr.substring(0,paropen).trim();
					
					//System.out.println("inputStr="+inputStr+"; job="+job+"; extra="+extra);
					
					movieID=findMovie(inputStr);
										
					if (movieID==-1)
					{
						//System.out.println("\t*** ignored movie "+inputStr+" ***");
						ignoredcnt++;
						continue;
					}
					
					//System.out.println("\t"+job+" in movie "+movieID+" ("+extra+")");

					insertMiscellaneous.setInt(1,index);
					insertMiscellaneous.setInt(2,movieID);
					insertMiscellaneous.setString(3,job);
					if (extra!=null) insertMiscellaneous.setString(4,extra);
					else insertMiscellaneous.setNull(4,Types.VARCHAR);						
					
					insertMiscellaneous.execute();
				}
				
				//if (index>100) break;
			}
		}
		catch(Exception e)
		{
			System.out.println("readMiscellaneousList(): "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("imported misc entries for "+actorcnt+" people.");
		System.out.println("imported "+(totalcnt-ignoredcnt)+" movies, ignored "+ignoredcnt+".");
	}

	private void readComposersList()
	{
		System.out.println("import composers.");

		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"composers.list.gz"))));
			String inputStr;
			
			int index=0;
			String name=null; // name of the current actor
			int title=-1;
			
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("THE COMPOSERS LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
					buffer.readLine(); // skip Name ...
					buffer.readLine(); // skip "----"
				}
				
				else if (inputStr.indexOf('(')==-1)
					continue;
					
				else if (inActor)
				{
					// we are reading a line for an actor
					//System.out.println("["+index+"] read line:" +inputStr);

					totalcnt++;
					
					title=-1;
					
					int tab=inputStr.indexOf("\t");
					if (tab>0) // we have an actor's name
					{
						name=inputStr.substring(0,tab).trim();
						//System.out.println("["+index+"] found actor "+name+"");
						index=findOrCreatePerson(name);
						actorcnt++;
					}
					
					inputStr=inputStr.trim();

					if (inputStr.endsWith("(uncredited)"))
					{
						inputStr=inputStr.substring(0,inputStr.lastIndexOf("(uncredited)"));
					}
					
					int parenthesisclose=inputStr.lastIndexOf(')');
					if (parenthesisclose!=-1) title=findMovie(inputStr.substring(tab,min(parenthesisclose+1,inputStr.length())).trim());
					
					if (title==-1)
					{
						//System.out.println("\t*** ignored an entry***");
						ignoredcnt++;
						continue;
					}
					
					//System.out.println("\tacted in movie "+title.title+" ("+title.year+"/"+title.addition+"/"+title.type+") as "+role+" (pos "+position+" in casting list)");

					insertComposesForMovie.setInt(1,index);
					insertComposesForMovie.setInt(2,title);
					insertComposesForMovie.setNull(3,Types.VARCHAR);
					
					insertComposesForMovie.execute();
				}
				
				//if (index>100) break;
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("imported "+actorcnt+" composers.");
		System.out.println("imported "+(totalcnt-ignoredcnt)+" movies, ignored "+ignoredcnt+".");
	}

	private void readEditorsList()
	{
		System.out.println("import editors.");

		StringBuffer sb = new StringBuffer();
		
		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"editors.list.gz"))));
			String inputStr;
			
			int index=0;
			String name=null; // name of the current actor
			int title=-1;
			String role=null;
			String position=null;
			boolean uncredited=false;
			
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("THE EDITORS LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
					buffer.readLine(); // skip Name ...
					buffer.readLine(); // skip "----"
				}
				
				else if (inputStr.indexOf('(')==-1)
					continue;
					
				else if (inActor)
				{
					// we are reading a line for an actor
					//System.out.println("["+index+"] read line:" +inputStr);

					totalcnt++;
					
					title=-1;
					role=null;
					position=null;
					uncredited=false;
					
					int tab=inputStr.indexOf("\t");
					if (tab>0) // we have an actor's name
					{
						name=inputStr.substring(0,tab).trim();
						//System.out.println("["+index+"] found actor "+name+"");
						index=findOrCreatePerson(name);
						actorcnt++;
					}
					
					inputStr=inputStr.trim();

					if (inputStr.endsWith("(uncredited)"))
					{
						uncredited=true;
						inputStr=inputStr.substring(0,inputStr.lastIndexOf("(uncredited)"));
					}
					
					int parenthesisclose=inputStr.lastIndexOf(')');
					if (parenthesisclose!=-1) title=findMovie(inputStr.substring(tab,min(parenthesisclose+1,inputStr.length())).trim());
					
					if (title==-1)
					{
						//System.out.println("\t*** ignored an entry***");
						ignoredcnt++;
						continue;
					}
					
					//System.out.println("\tacted in movie "+title.title+" ("+title.year+"/"+title.addition+"/"+title.type+") as "+role+" (pos "+position+" in casting list)");

					insertEditsMovie.setInt(1,index);
					insertEditsMovie.setInt(2,title);
					insertEditsMovie.setNull(3,Types.VARCHAR);
						
					insertEditsMovie.execute();
				}
				
				//if (index>100) break;
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("imported "+actorcnt+" editors.");
		System.out.println("imported "+(totalcnt-ignoredcnt)+" movies, ignored "+ignoredcnt+".");
	}

	private void readCinematographersList()
	{
		System.out.println("import cinematographers.");

		StringBuffer sb = new StringBuffer();
		
		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"cinematographers.list.gz"))));
			String inputStr;
			
			int index=0;
			String name=null; // name of the current actor
			int title=-1;
			
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("THE CINEMATOGRAPHERS LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
					buffer.readLine(); // skip Name ...
					buffer.readLine(); // skip "----"
				}
				
				else if (inputStr.indexOf('(')==-1)
					continue;
					
				else if (inActor)
				{
					// we are reading a line for an actor
					//System.out.println("["+index+"] read line:" +inputStr);

					totalcnt++;
					
					title=-1;
					
					int tab=inputStr.indexOf("\t");
					if (tab>0) // we have an actor's name
					{
						name=inputStr.substring(0,tab).trim();
						//System.out.println("["+index+"] found actor "+name+"");
						index=findOrCreatePerson(name);
						actorcnt++;
					}
					
					inputStr=inputStr.trim();

					int parenthesisclose=inputStr.lastIndexOf(')');
					if (parenthesisclose!=-1) title=findMovie(inputStr.substring(tab,min(parenthesisclose+1,inputStr.length())).trim());
					
					if (title==-1)
					{
						//System.out.println("\t*** ignored an entry***");
						ignoredcnt++;
						continue;
					}
					
					//System.out.println("\tacted in movie "+title.title+" ("+title.year+"/"+title.addition+"/"+title.type+") as "+role+" (pos "+position+" in casting list)");

					insertCinematographsMovie.setInt(1,index);
					insertCinematographsMovie.setInt(2,title);
						
					insertCinematographsMovie.execute();
				}
				
				//if (index>100) break;
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("imported "+actorcnt+" cinematographs.");
		System.out.println("imported "+(totalcnt-ignoredcnt)+" movies, ignored "+ignoredcnt+".");
	}

	private void readCostumeDesignersList()
	{
		System.out.println("import costume designers.");

		StringBuffer sb = new StringBuffer();
		
		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"costume-designers.list.gz"))));
			String inputStr;
			
			int index=0;
			String name=null; // name of the current actor
			int title=-1;
			
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("THE COSTUME DESIGNERS LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
					buffer.readLine(); // skip Name ...
					buffer.readLine(); // skip "----"
				}
				
				else if (inputStr.indexOf('(')==-1)
					continue;
					
				else if (inActor)
				{
					// we are reading a line for an actor
					//System.out.println("["+index+"] read line:" +inputStr);

					totalcnt++;
					
					title=-1;
					
					int tab=inputStr.indexOf("\t");
					if (tab>0) // we have an actor's name
					{
						name=inputStr.substring(0,tab).trim();
						//System.out.println("["+index+"] found actor "+name+"");
						index=findOrCreatePerson(name);
						actorcnt++;
					}
					
					inputStr=inputStr.trim();

					int parenthesisclose=inputStr.lastIndexOf(')');
					if (parenthesisclose!=-1) title=findMovie(inputStr.substring(tab,min(parenthesisclose+1,inputStr.length())).trim());
					
					if (title==-1)
					{
						//System.out.println("\t*** ignored an entry***");
						ignoredcnt++;
						continue;
					}
					
					//System.out.println("\tacted in movie "+title.title+" ("+title.year+"/"+title.addition+"/"+title.type+") as "+role+" (pos "+position+" in casting list)");

					insertCostumeDesignsMovie.setInt(1,index);
					insertCostumeDesignsMovie.setInt(2,title);
						
					insertCostumeDesignsMovie.execute();
				}
				
				//if (index>100) break;
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("imported "+actorcnt+" costume designers.");
		System.out.println("imported "+(totalcnt-ignoredcnt)+" movies, ignored "+ignoredcnt+".");
	}
	
	private void readSoundMixList()
	{
		System.out.println("import sound mix information.");

		StringBuffer sb = new StringBuffer();
		
		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"sound-mix.list.gz"))));
			String inputStr;
			
			int index=0;
			int title=-1;
			
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("SOUND-MIX LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
					buffer.readLine(); // skip Name ...
					buffer.readLine(); // skip "----"
				}
				
				else if (inputStr.indexOf('(')==-1)
					continue;
					
				else if (inActor)
				{
					// we are reading a line for an actor
					//System.out.println("["+totalcnt+"] read line:" +inputStr);

					totalcnt++;
					
					title=-1;
					
					int tab=inputStr.indexOf("\t");
					if (tab==-1) 
					{
						continue;
					}
					
					String mix=inputStr.substring(tab).trim();
					
					inputStr=inputStr.substring(0,tab).trim();

					title=findMovie(inputStr);
					
					if (title==-1)
					{
						//System.out.println("\t*** ignored an entry***");
						ignoredcnt++;
						continue;
					}
					
					//System.out.println(mix+" for movieID "+title);

					insertSoundMix.setInt(1,title);
					insertSoundMix.setString(2,mix);

					insertSoundMix.execute();
				}
				
				//if (index>100) break;
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("imported "+(totalcnt-ignoredcnt)+" sound mixes, ignored "+ignoredcnt+".");
	}

	private void readColourInfoList()
	{
		System.out.println("import colour information.");

		StringBuffer sb = new StringBuffer();
		
		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"color-info.list.gz"))));
			String inputStr;
			
			int index=0;
			int title=-1;
			
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("COLOR INFO LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
				}
				
				else if (inputStr.indexOf('(')==-1)
					continue;
					
				else if (inActor)
				{
					// we are reading a line for an actor
					//System.out.println("["+totalcnt+"] read line:" +inputStr);

					totalcnt++;
					
					title=-1;
					
					int tab=inputStr.indexOf("\t");
					if (tab==-1) 
					{
						continue;
					}
					
					String mix=inputStr.substring(tab).trim();
					
					inputStr=inputStr.substring(0,tab).trim();

					title=findMovie(inputStr);
					
					if (title==-1)
					{
						//System.out.println("\t*** ignored an entry***");
						ignoredcnt++;
						continue;
					}
					
					//System.out.println(mix+" for movie "+title.title+" ("+title.year+"/"+title.addition+"/"+title.type+")");

					insertColourInfo.setInt(1,title);
					insertColourInfo.setString(2,mix);

					insertColourInfo.execute();
				}
				
				//if (index>100) break;
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("imported "+(totalcnt-ignoredcnt)+" colour infos, ignored "+ignoredcnt+".");
	}

	private void readGenresList()
	{
		System.out.println("import genres.");

		StringBuffer sb = new StringBuffer();
		
		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"genres.list.gz"))));
			String inputStr;
			
			int index=0;
			int title=-1;
			
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("8: THE GENRES LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
				}
				
				else if (inputStr.indexOf('(')==-1)
					continue;
					
				else if (inActor)
				{
					// we are reading a line for an actor
					//System.out.println("["+totalcnt+"] read line:" +inputStr);

					totalcnt++;
					
					title=-1;
					
					int tab=inputStr.indexOf("\t");
					if (tab==-1) 
					{
						continue;
					}
					
					String mix=inputStr.substring(tab).trim();
					
					inputStr=inputStr.substring(0,tab).trim();

					title=findMovie(inputStr);
					
					if (title==-1)
					{
						//System.out.println("\t*** ignored an entry***");
						ignoredcnt++;
						continue;
					}
					
					//System.out.println(mix+" for movie "+title.title+" ("+title.year+"/"+title.addition+"/"+title.type+")");

					insertGenre.setInt(1,title);
					insertGenre.setString(2,mix);
						
					insertGenre.execute();
				}
				
				//if (index>100) break;
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("imported "+(totalcnt-ignoredcnt)+" genres, ignored "+ignoredcnt+".");
	}

	private void readLocationsList()
	{
		System.out.println("import locations.");

		StringBuffer sb = new StringBuffer();
		
		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"locations.list.gz"))));
			String inputStr;
			
			int movieID=0;
			String title=null;
			String location=null;
			String extra=null;

			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("LOCATIONS LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
				}
				
				else if (inputStr.indexOf('(')==-1)
					continue;
					
				else if (inActor)
				{
					// we are reading a line for a movie
					//System.out.println("["+totalcnt+"] read line:" +inputStr);

					totalcnt++;
					
					title=null;
					extra=null;
					location=null;
					
					int tab=inputStr.indexOf("\t");
					if (tab==-1) 
					{
						continue;
					}
					
					title=inputStr.substring(0,tab).trim();
					inputStr=inputStr.substring(tab).trim();

					movieID=findMovie(title);
					
					if (movieID==-1)
					{
						//System.out.println("\t*** ignored an entry***");
						ignoredcnt++;
						continue;
					}

					int paropen=inputStr.lastIndexOf('(');
					if (paropen!=-1)
					{			
						extra=inputStr.substring(paropen+1,inputStr.length()-1);
						inputStr=inputStr.substring(0,paropen).trim();
					}
					
					//System.out.println("movie "+movieID+" was filmed at "+inputStr+"; extra="+extra);

					insertLocation.setInt(1,movieID);
					insertLocation.setString(2,inputStr);
					if (extra==null) insertLocation.setNull(3,Types.VARCHAR);
					else insertLocation.setString(3,extra);
						
					insertLocation.execute();

				}
				
				//if (index>100) break;
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("imported "+(totalcnt-ignoredcnt)+" locations, ignored "+ignoredcnt+".");
	}

	private void readKeywordsList()
	{
		System.out.println("import keywords.");

		StringBuffer sb = new StringBuffer();
		
		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"keywords.list.gz"))));
			String inputStr;
			
			int index=0;
			int title=-1;
			
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("8: THE KEYWORDS LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
				}
				
				else if (inputStr.indexOf('(')==-1)
					continue;
					
				else if (inActor)
				{
					// we are reading a line for an actor
					//System.out.println("["+totalcnt+"] read line:" +inputStr);

					totalcnt++;
					
					title=-1;
					
					int tab=inputStr.indexOf("\t");
					if (tab==-1) 
					{
						continue;
					}
					
					String mix=inputStr.substring(tab).trim();
					
					inputStr=inputStr.substring(0,tab).trim();

					title=findMovie(inputStr);
					
					if (title==-1)
					{
						//System.out.println("\t*** ignored an entry***");
						ignoredcnt++;
						continue;
					}
					
					//System.out.println(mix+" for movie "+title.title+" ("+title.year+"/"+title.addition+"/"+title.type+")");

					insertKeyword.setInt(1,title);
					insertKeyword.setString(2,mix);

					insertKeyword.execute();
				}
				
				//if (index>100) break;
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("imported "+(totalcnt-ignoredcnt)+" genres, ignored "+ignoredcnt+".");
	}

	private void readLanguagesList()
	{
		System.out.println("import languages.");

		StringBuffer sb = new StringBuffer();
		
		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"language.list.gz"))));
			String inputStr;
			
			int index=0;
			int title=-1;
			int previous=-1;
			int order=-1;
			
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("LANGUAGE LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
				}
				
				else if (inputStr.indexOf('(')==-1)
					continue;
					
				else if (inActor)
				{
					// we are reading a line for an actor
					//System.out.println("["+totalcnt+"] read line:" +inputStr);

					totalcnt++;
					
					title=-1;
					
					int tab=inputStr.indexOf("\t");
					if (tab==-1) 
					{
						continue;
					}
					
					String mix=inputStr.substring(tab).trim();
					
					inputStr=inputStr.substring(0,tab).trim();

					title=findMovie(inputStr);
					
					if (title==-1)
					{
						//System.out.println("\t*** ignored an entry***");
						ignoredcnt++;
						continue;
					}
					
					if (previous==title)
					{
						order++;
					}
					else
					{
						order=1;
						previous=title;
					}
					//System.out.println(mix+" for movie "+title.title+" ("+title.year+"/"+title.addition+"/"+title.type+")");

					insertLanguage.setInt(1,title);
					insertLanguage.setString(2,mix);
					insertLanguage.setInt(3,order);
						
					insertLanguage.execute();
				}
				
				//if (index>100) break;
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("imported "+(totalcnt-ignoredcnt)+" languages, ignored "+ignoredcnt+".");
	}

	private void readCertificatesList()
	{
		System.out.println("import certificates.");

		StringBuffer sb = new StringBuffer();
		
		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"certificates.list.gz"))));
			String inputStr;
			
			int index=0;
			int title=-1;
			int previous=-1;
			int order=-1;
			
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("CERTIFICATES LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
				}
				
				else if (inputStr.indexOf('(')==-1)
					continue;
					
				else if (inActor)
				{
					// we are reading a line for an actor
					//System.out.println("["+totalcnt+"] read line:" +inputStr);

					totalcnt++;
					
					title=-1;
					
					int tab=inputStr.indexOf("\t");
					if (tab==-1) 
					{
						continue;
					}
					
					String mix=inputStr.substring(tab).trim();
					
					inputStr=inputStr.substring(0,tab).trim();

					title=findMovie(inputStr);
					
					if (title==-1)
					{
						//System.out.println("\t*** ignored an entry***");
						ignoredcnt++;
						continue;
					}
					
					//System.out.println(mix+" for movie "+title.title+" ("+title.year+"/"+title.addition+"/"+title.type+")");

					insertCertificate.setInt(1,title);
					insertCertificate.setString(2,mix);
						
					insertCertificate.execute();
				}
				
				//if (index>100) break;
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("imported "+(totalcnt-ignoredcnt)+" languages, ignored "+ignoredcnt+".");
	}

	private void readCountriesList()
	{
		System.out.println("import countries.");

		StringBuffer sb = new StringBuffer();
		
		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"countries.list.gz"))));
			String inputStr;
			
			int index=0;
			int title=-1;
			int previous=-1;
			int order=-1;
			
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("COUNTRIES LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
				}
				
				else if (inputStr.indexOf('(')==-1)
					continue;
					
				else if (inActor)
				{
					// we are reading a line for an actor
					//System.out.println("["+totalcnt+"] read line:" +inputStr);

					totalcnt++;
					
					title=-1;
					
					int tab=inputStr.indexOf("\t");
					if (tab==-1) 
					{
						continue;
					}
					
					String mix=inputStr.substring(tab).trim();
					
					inputStr=inputStr.substring(0,tab).trim();

					title=findMovie(inputStr);
					
					if (title==-1)
					{
						//System.out.println("\t*** ignored an entry***");
						ignoredcnt++;
						continue;
					}
					
					if (previous==title)
					{
						order++;
					}
					else
					{
						order=1;
						previous=title;
					}
					//System.out.println(mix+" for movie "+title.title+" ("+title.year+"/"+title.addition+"/"+title.type+")");

					insertCountry.setInt(1,title);
					insertCountry.setString(2,mix);
					insertCountry.setInt(3,order);
						
					insertCountry.execute();
				}
				
				//if (index>100) break;
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("imported "+(totalcnt-ignoredcnt)+" countries, ignored "+ignoredcnt+".");
	}

	private void readMovieLinks()
	{
		System.out.println("import movie links.");

		StringBuffer sb = new StringBuffer();
		
		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"movie-links.list.gz"))));
			String inputStr;
			
			int index=0;
			String name=null; // name of the current actor
			int title=-1;
			int sourceID=-1;
			
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("MOVIE LINKS LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
				}
				
				else if (inputStr.indexOf('(')==-1)
					continue;
					
				else if (inActor)
				{
					// we are reading a line for a movie
					//System.out.println("["+index+"] read line:" +inputStr);

					totalcnt++;
					
					title=-1;
					
					//int tab=inputStr.indexOf("\t");
					if (!(inputStr.startsWith(" "))) // we have an actor's name
					{
						index++;
						name=inputStr.trim();
						//System.out.println("["+index+"] found movie #"+name+"#");
						sourceID=findMovie(name);
						
						actorcnt++;
						continue;
					}
					
					inputStr=inputStr.trim();
					String movie=null;
					String link=null;
					if (inputStr.startsWith("(referenced in"))
					{
						movie=inputStr.substring(new String("(referenced in ").length(),inputStr.length()-1);
						link="REFERENCED_IN";
					}
					if (inputStr.startsWith("(references"))
					{
						movie=inputStr.substring(new String("(references ").length(),inputStr.length()-1);
						link="REFERENCES";
					}
					else if (inputStr.startsWith("(featured in"))
					{
						movie=inputStr.substring(new String("(featured in ").length(),inputStr.length()-1);
						link="FEATURED_IN";
					}
					else if (inputStr.startsWith("(spin off"))
					{
						movie=inputStr.substring(new String("(spin off ").length(),inputStr.length()-1);
						link="SPIN_OFF";
					}
					else if (inputStr.startsWith("(spin off from"))
					{
						movie=inputStr.substring(new String("(spin off from ").length(),inputStr.length()-1);
						link="SPIN_OFF";
					}
					else if (inputStr.startsWith("(spoofed in"))
					{
						movie=inputStr.substring(new String("(spoofed in ").length(),inputStr.length()-1);
						link="SPOOFED_IN";
					}
					else if (inputStr.startsWith("(spoofs"))
					{
						movie=inputStr.substring(new String("(spoofs ").length(),inputStr.length()-1);
						link="SPOOFS";
					}
					else if (inputStr.startsWith("(version of"))
					{
						movie=inputStr.substring(new String("(version of ").length(),inputStr.length()-1);
						link="VERSION_OF";
					}
					else if (inputStr.startsWith("(follows"))
					{
						movie=inputStr.substring(new String("(follows ").length(),inputStr.length()-1);
						link="FOLLOWS";
					}
					else if (inputStr.startsWith("(remade as"))
					{
						movie=inputStr.substring(new String("(remade as ").length(),inputStr.length()-1);
						link="REMADE_AS";
					}
					else if (inputStr.startsWith("(alternate language version of"))
					{
						movie=inputStr.substring(new String("(alternate language version of ").length(),inputStr.length()-1);
						link="ALTERNATE_LANGUAGE_VERSION_OF";
					}
					else if (inputStr.startsWith("(remake of"))
					{
						movie=inputStr.substring(new String("(remake of ").length(),inputStr.length()-1);
						link="REMAKE_OF";
					}
					else if (inputStr.startsWith("(edited into"))
					{
						movie=inputStr.substring(new String("(edited into ").length(),inputStr.length()-1);
						link="EDITED_INTO";
					}
					else if (inputStr.startsWith("(edited from"))
					{
						movie=inputStr.substring(new String("(edited from ").length(),inputStr.length()-1);
						link="EDITED_FROM";
					}
					else if (inputStr.startsWith("(followed by"))
					{
						movie=inputStr.substring(new String("(followed by ").length(),inputStr.length()-1);
						link="FOLLOWED_BY";
					}
					else if (inputStr.startsWith("(features"))
					{
						movie=inputStr.substring(new String("(features ").length(),inputStr.length()-1);
						link="FEATURES";
					}
										
					if (movie!=null) title=findMovie(movie);
					
					if ((title==-1)||(sourceID==-1))
					{
						System.out.println("\t*** ignored an entry*** (title="+title+", sourceID="+sourceID+")");
						ignoredcnt++;
						continue;
					}
					
					//System.out.println("link from movieID "+sourceID+" to movie "+title.title+" ("+title.year+"/"+title.addition+"/"+title.type+") of type "+link);

					insertMovieLink.setInt(1,sourceID);
					insertMovieLink.setInt(2,title);
					insertMovieLink.setString(3,link);		
						
					insertMovieLink.execute();
				}
				
				//if (index>100) break;
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("imported "+actorcnt+" source movies.");
		System.out.println("imported "+(totalcnt-ignoredcnt)+" target movies, ignored "+ignoredcnt+".");
	}
	
	private void readTaglines()
	{
		System.out.println("import taglines.");

		StringBuffer sb = new StringBuffer();
		
		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"taglines.list.gz"))));
			String inputStr;
			
			int index=0;
			String name=null; // name of the current actor
			int sourceID=-1;
			
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("TAG LINES LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
					buffer.readLine(); // skip blank line
				}
				
				else if (inputStr.indexOf('(')==-1)
					continue;
					
				else if (inActor)
				{
					// we are reading a line for a movie
					//System.out.println("["+index+"] read line:" +inputStr);

					totalcnt++;
					
					if (inputStr.startsWith("#")) // we have a movie name
					{
						index++;
						name=inputStr.substring(2,inputStr.length()).trim();
						//System.out.println("["+index+"] found movie #"+name+"#");
						sourceID=findMovie(name);
						actorcnt++;
						continue;
					}
					
					inputStr=inputStr.trim();

					if (sourceID==-1)
					{
						System.out.println("\t*** ignored an entry*** (sourceID="+sourceID+")");
						ignoredcnt++;
						continue;
					}
					
					System.out.println("tagline for movieID "+sourceID+" is "+inputStr);

					insertTagline.setInt(1,sourceID);
					insertTagline.setString(2,inputStr);
					
					insertTagline.execute();
				}
				
				//if (index>100) break;
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("imported "+actorcnt+" source movies.");
		System.out.println("imported "+(totalcnt-ignoredcnt)+" taglines, ignored "+ignoredcnt+".");
	}

	private void readPlots()
	{
		System.out.println("import plots.");

		StringBuffer sb = new StringBuffer();
		
		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"plot.list.gz"))));
			String inputStr;
			
			int index=0;
			String name=null; // name of the current actor
			Title title=null;
			int sourceID=-1;
			String plot=null;
			String author=null;
			
			while ((inputStr=buffer.readLine()) != null)
			{
				if ( inputStr.startsWith("PLOT SUMMARIES LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
					buffer.readLine(); // skip blank line
				}
				
				else if (inActor)
				{
					// we are reading a line for a movie
					//System.out.println("["+index+"] read line:" +inputStr);

					//int tab=inputStr.indexOf("\t");
					if (inputStr.startsWith("---"))
					{
						plot=null; author=null;
						sourceID=-1; continue;
					}
					
					else if (inputStr.startsWith("MV:")) // we have a movie name
					{
						index++;
						name=inputStr.substring(3,inputStr.length()).trim();
						//System.out.println("["+index+"] found movie #"+name+"#");
						sourceID=findMovie(name);
						actorcnt++;
						continue;
					}
					else if (inputStr.startsWith("PL:"))
					{
						if (plot!=null) plot=plot+" "+inputStr.substring(3,inputStr.length()).trim();
						else plot=inputStr.substring(3,inputStr.length()).trim();
						continue;
					}
					else if (inputStr.startsWith("BY:"))
					{
						totalcnt++;
						
						author=inputStr.substring(3,inputStr.length()).trim();
						if (author.length()>100) // the underlying data is broken, so skip this
							plot=null;
													
						if ((sourceID!=-1)&&(plot!=null))
						{
							//System.out.println("plot for movieID "+sourceID+" by "+author+" is "+plot);

							StringReader sr=new StringReader(plot);
							
							insertPlot.setInt(1,sourceID);
							insertPlot.setString(2,author);
							insertPlot.setCharacterStream(3,sr,plot.length());
							
							insertPlot.execute();
						}
						else ignoredcnt++;
						
						plot=null; author=null;
					}
				}
				
				//if (index>100) break;
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("imported "+actorcnt+" source movies.");
		System.out.println("imported "+(totalcnt-ignoredcnt)+" plots, ignored "+ignoredcnt+".");
	}

	private void readGoofList()
	{
		System.out.println("import goofs.");

		StringBuffer sb = new StringBuffer();
		
		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"goofs.list.gz"))));
			String inputStr;
			
			int index=0;
			String name=null; // name of the current actor
			int sourceID=-1;
			String goof=null;
			String type=null;
			
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("GOOFS LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
				}
				
				else if (inActor)
				{
					// we are reading a line for a movie
					//System.out.println("["+index+"] read line:" +inputStr);

					totalcnt++;
					
					//int tab=inputStr.indexOf("\t");
					if (inputStr.startsWith("#")) // we have a movie name
					{
						if (goof!=null) // there is one line left to flush, so do it now
						{
							//System.out.println("goof for movieID "+sourceID+" of type "+type+" is "+goof);

							StringReader sr=new StringReader(goof);
							insertGoof.setInt(1,sourceID);
							if (type!=null) insertGoof.setString(2,type);
							else insertGoof.setNull(2,Types.VARCHAR);
							insertGoof.setCharacterStream(3,sr,goof.length());
							
							insertGoof.execute();
							goof=null;
						}
						
						index++;
						name=inputStr.substring(2,inputStr.length()).trim();
						//System.out.println("["+index+"] found movie #"+name+"#");
						sourceID=findMovie(name);
						actorcnt++;
						continue;
					}
					
					inputStr=inputStr.trim();

					if (sourceID==-1)
					{
						//System.out.println("\t*** ignored an entry*** (sourceID="+sourceID+")");
						ignoredcnt++;
						continue;
					}
					
					if (inputStr.startsWith("-")) // this is a new goof
					{
						if (goof!=null)
						{
							//System.out.println("goof for movieID "+sourceID+" of type "+type+" is "+goof);

							StringReader sr=new StringReader(goof);
							insertGoof.setInt(1,sourceID);
							if (type!=null) insertGoof.setString(2,type);
							else insertGoof.setNull(2,Types.VARCHAR);
							insertGoof.setCharacterStream(3,sr,goof.length());
							
							insertGoof.execute();
							goof=null;
						}
						inputStr=inputStr.substring(1).trim();
						
						int colonpos=inputStr.indexOf(':');
						if (colonpos!=-1)
						{
							type=inputStr.substring(0,colonpos);
							inputStr=inputStr.substring(colonpos+1).trim();
						}
					}
					
					if (goof==null) goof=inputStr;
					else goof=goof+" "+inputStr;
				}
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("imported "+actorcnt+" source movies.");
		System.out.println("imported "+(totalcnt-ignoredcnt)+" goofs, ignored "+ignoredcnt+".");
	}

	private void readTriviaList()
	{
		System.out.println("import trivia.");

		StringBuffer sb = new StringBuffer();
		
		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"trivia.list.gz"))));
			String inputStr;
			
			int index=0;
			String name=null; // name of the current actor
			int sourceID=-1;
			String trivia=null;
			
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("FILM TRIVIA")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
				}
				
				else if (inActor)
				{
					// we are reading a line for a movie
					//System.out.println("["+index+"] read line:" +inputStr);

					totalcnt++;
					
					//int tab=inputStr.indexOf("\t");
					if (inputStr.startsWith("#")) // we have a movie name
					{
						if (trivia!=null) // there is one line left to flush, so do it now
						{
							System.out.println("trivia for movieID "+sourceID+" is "+trivia);

							StringReader sr=new StringReader(trivia);
							insertMovieTrivia.setInt(1,sourceID);
							insertMovieTrivia.setCharacterStream(2,sr,trivia.length());
							
							insertMovieTrivia.execute();
							trivia=null;
						}
						
						index++;
						name=inputStr.substring(2,inputStr.length()).trim();
						//System.out.println("["+index+"] found movie #"+name+"#");
						sourceID=findMovie(name);
						actorcnt++;
						continue;
					}
					
					inputStr=inputStr.trim();

					if (sourceID==-1)
					{
						//System.out.println("\t*** ignored an entry*** (sourceID="+sourceID+")");
						ignoredcnt++;
						continue;
					}
					
					if (inputStr.startsWith("-")) // this is a new goof
					{
						if (trivia!=null)
						{
							System.out.println("trivia for movieID "+sourceID+" is "+trivia);

							StringReader sr=new StringReader(trivia);
							insertMovieTrivia.setInt(1,sourceID);
							insertMovieTrivia.setCharacterStream(2,sr,trivia.length());
							
							insertMovieTrivia.execute();
							trivia=null;
						}
						inputStr=inputStr.substring(1).trim();
					}
					
					if (trivia==null) trivia=inputStr;
					else trivia=trivia+" "+inputStr;
				}
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("imported "+actorcnt+" source movies.");
		System.out.println("imported "+(totalcnt-ignoredcnt)+" goofs, ignored "+ignoredcnt+".");
	}

	private void readAlternateVersionList()
	{
		System.out.println("import alternate versions.");

		StringBuffer sb = new StringBuffer();
		
		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"alternate-versions.list.gz"))));
			String inputStr;
			
			int index=0;
			String name=null; // name of the current actor
			int sourceID=-1;
			String version=null;
			
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("ALTERNATE VERSIONS LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
				}
				
				else if (inActor)
				{
					// we are reading a line for a movie
					//System.out.println("["+index+"] read line:" +inputStr);

					totalcnt++;
					
					//int tab=inputStr.indexOf("\t");
					if (inputStr.startsWith("#")) // we have a movie name
					{
						if (version!=null) // there is one line left to flush, so do it now
						{
							System.out.println("alternate version for movieID "+sourceID+" is "+version);

							StringReader sr=new StringReader(version);
							insertAlternateVersion.setInt(1,sourceID);
							insertAlternateVersion.setCharacterStream(2,sr,version.length());
							
							insertAlternateVersion.execute();
							version=null;
						}
						
						index++;
						name=inputStr.substring(2,inputStr.length()).trim();
						//System.out.println("["+index+"] found movie #"+name+"#");
						sourceID=findMovie(name);
						actorcnt++;
						continue;
					}
					
					inputStr=inputStr.trim();

					if (sourceID==-1)
					{
						//System.out.println("\t*** ignored an entry*** (sourceID="+sourceID+")");
						ignoredcnt++;
						continue;
					}
					
					if (inputStr.startsWith("-")) // this is a new goof
					{
						if (version!=null)
						{
							System.out.println("alternate version for movieID "+sourceID+" is "+version);

							StringReader sr=new StringReader(version);
							insertAlternateVersion.setInt(1,sourceID);
							insertAlternateVersion.setCharacterStream(2,sr,version.length());
							
							insertAlternateVersion.execute();
							version=null;
						}
						inputStr=inputStr.substring(1).trim();
					}
					
					if (version==null) version=inputStr;
					else version=version+" "+inputStr;
				}
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("considered "+actorcnt+" movies.");
		System.out.println("imported "+(totalcnt-ignoredcnt)+" alternate versions, ignored "+ignoredcnt+".");
	}

	private void readAlternativeTitles()
	{
		System.out.println("import alternative titles.");

		StringBuffer sb = new StringBuffer();
		
		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"aka-titles.list.gz"))));
			String inputStr;
			
			int index=0;
			String name=null; // name of the current actor
			String movie=null;
			String comment=null;
			int movieID=-1;
			Title title=null;
						
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("AKA TITLES LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
					buffer.readLine(); // skip blank line
				}
				
				else if (inputStr.indexOf('(')==-1)
					continue;
					
				else if (inActor)
				{
					// we are reading a line for an actor
					//System.out.println("["+index+"] read line:" +inputStr);

					movie=null;
					title=null;
					comment=null;
					
					inputStr=inputStr.trim();
					
					int tab=inputStr.indexOf("\t");
					if (!(inputStr.startsWith("(aka"))) // we have a movie title
					{
						if (tab>0) name=inputStr.substring(0,tab).trim();
						else name=inputStr.trim();
						movieID=findMovie(name);
						actorcnt++;
						//System.out.println("["+movieID+"] found movie "+name);
						continue;
					}
					
					inputStr=inputStr.trim();

					int parcnt=0;
					int i;
					for (i=0; i<inputStr.length(); i++)
					{
						if (inputStr.charAt(i)=='(') parcnt++;
						else if (inputStr.charAt(i)==')') parcnt--;
						if (parcnt==0) break;					
					}
					
					if (i!=0) // we found a movie (and possibly more)
					{
						totalcnt++;

						movie=inputStr.substring(4,i).trim();
						if (i<inputStr.length()-1)
							comment=inputStr.substring(i+1).trim();
						else
							comment=null;
						//System.out.println("movie="+movie+"; i="+i+"; input="+inputStr.length());
						title=demangleComplexTitle(movie);
					}
					
					if (title==null)
					{
						//System.out.println("\t*** ignored an entry***");
						if (i!=0) ignoredcnt++;
						continue;
					}
					
					//System.out.println("\talternative title is "+title.title+" ("+title.year+"/"+title.addition+"/"+title.type+"), comment="+comment);

					insertAlternativeTitle.setInt(1,movieID);
					insertAlternativeTitle.setString(2,title.title);
					insertAlternativeTitle.setInt(3,title.year);
					if (title.addition!=null)
						insertAlternativeTitle.setString(4,title.addition);
					else
						insertAlternativeTitle.setNull(4,Types.VARCHAR);
					insertAlternativeTitle.setInt(5,title.type);
					if (title.episode!=null)
						insertAlternativeTitle.setString(6,title.episode);
					else
						insertAlternativeTitle.setNull(6,Types.VARCHAR);

					if (comment!=null)
						insertAlternativeTitle.setString(7,comment);
					else
						insertAlternativeTitle.setNull(7,Types.VARCHAR);

						if (movieID!=-1) insertAlternativeTitle.execute();
				}
				
				//if (index>100) break;
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("considered "+actorcnt+" movies.");
		System.out.println("imported "+(totalcnt-ignoredcnt)+" alternative titles, ignored "+ignoredcnt+".");
	}

	private void readGermanAlternativeTitles()
	{
		System.out.println("import german alternative titles.");

		StringBuffer sb = new StringBuffer();
		
		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"german-aka-titles.list.gz"))));
			String inputStr;
			
			int index=0;
			String name=null; // name of the current actor
			String movie=null;
			String comment=null;
			int movieID=-1;
			Title title=null;
						
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("AKA TITLES LIST GERMAN")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
				}
				
				else if (inputStr.indexOf('(')==-1)
					continue;
					
				else if (inActor)
				{
					// we are reading a line for an actor
					//System.out.println("["+index+"] read line:" +inputStr);

					movie=null;
					title=null;
					comment=null;
					
					inputStr=inputStr.trim();
					
					int tab=inputStr.indexOf("\t");
					if (!(inputStr.startsWith("(aka"))) // we have a movie title
					{
						if (tab>0) name=inputStr.substring(0,tab).trim();
						else name=inputStr.trim();
						movieID=findMovie(name);
						actorcnt++;
						//System.out.println("["+movieID+"] found movie "+name);
						continue;
					}
					
					inputStr=inputStr.trim();

					int parcnt=0;
					int i;
					for (i=0; i<inputStr.length(); i++)
					{
						if (inputStr.charAt(i)=='(') parcnt++;
						else if (inputStr.charAt(i)==')') parcnt--;
						if (parcnt==0) break;					
					}
					
					if (i!=0) // we found a movie (and possibly more)
					{
						totalcnt++;

						movie=inputStr.substring(4,i).trim();
						if (i<inputStr.length()-1)
							comment=inputStr.substring(i+1).trim();
						else
							comment=null;
						//System.out.println("movie="+movie+"; i="+i+"; input="+inputStr.length());
						title=demangleComplexTitle(movie);
					}
					
					if (title==null)
					{
						//System.out.println("\t*** ignored an entry***");
						if (i!=0) ignoredcnt++;
						continue;
					}
					
					//System.out.println("\talternative title is "+title.title+" ("+title.year+"/"+title.addition+"/"+title.type+"), comment="+comment);

					insertAlternativeTitle.setInt(1,movieID);
					insertAlternativeTitle.setString(2,title.title);
					insertAlternativeTitle.setInt(3,title.year);
					if (title.addition!=null)
						insertAlternativeTitle.setString(4,title.addition);
					else
						insertAlternativeTitle.setNull(4,Types.VARCHAR);
					insertAlternativeTitle.setInt(5,title.type);
					if (title.episode!=null)
						insertAlternativeTitle.setString(6,title.episode);
					else
						insertAlternativeTitle.setNull(6,Types.VARCHAR);

					if (comment!=null)
						insertAlternativeTitle.setString(7,comment);
					else
						insertAlternativeTitle.setNull(7,Types.VARCHAR);
	
						insertAlternativeTitle.execute();
				}
				
				//if (index>100) break;
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("considered "+actorcnt+" movies.");
		System.out.println("imported "+(totalcnt-ignoredcnt)+" alternative titles, ignored "+ignoredcnt+".");
	}

	private void readISOAlternativeTitles()
	{
		System.out.println("import iso alternative titles.");

		StringBuffer sb = new StringBuffer();
		
		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"iso-aka-titles.list.gz"))));
			String inputStr;
			
			int index=0;
			String name=null; // name of the current actor
			String movie=null;
			String comment=null;
			int movieID=-1;
			Title title=null;
						
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("AKA TITLES LIST ISO")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
				}
				
				else if (inputStr.indexOf('(')==-1)
					continue;
					
				else if (inActor)
				{
					// we are reading a line for an actor
					//System.out.println("["+index+"] read line:" +inputStr);

					movie=null;
					title=null;
					comment=null;
					
					inputStr=inputStr.trim();
					
					int tab=inputStr.indexOf("\t");
					if (!(inputStr.startsWith("(aka"))) // we have a movie title
					{
						if (tab>0) name=inputStr.substring(0,tab).trim();
						else name=inputStr.trim();
						movieID=findMovie(name);
						actorcnt++;
						//System.out.println("["+movieID+"] found movie "+name);
						continue;
					}
					
					inputStr=inputStr.trim();

					int parcnt=0;
					int i;
					for (i=0; i<inputStr.length(); i++)
					{
						if (inputStr.charAt(i)=='(') parcnt++;
						else if (inputStr.charAt(i)==')') parcnt--;
						if (parcnt==0) break;					
					}
					
					if (i!=0) // we found a movie (and possibly more)
					{
						totalcnt++;

						movie=inputStr.substring(4,i).trim();
						if (i<inputStr.length()-1)
							comment=inputStr.substring(i+1).trim();
						else
							comment=null;
						//System.out.println("movie="+movie+"; i="+i+"; input="+inputStr.length());
						title=demangleComplexTitle(movie);
					}
					
					if (title==null)
					{
						//System.out.println("\t*** ignored an entry***");
						if (i!=0) ignoredcnt++;
						continue;
					}
					
					//System.out.println("\talternative title is "+title.title+" ("+title.year+"/"+title.addition+"/"+title.type+"), comment="+comment);

					insertAlternativeTitle.setInt(1,movieID);
					insertAlternativeTitle.setString(2,title.title);
					insertAlternativeTitle.setInt(3,title.year);
					if (title.addition!=null)
						insertAlternativeTitle.setString(4,title.addition);
					else
						insertAlternativeTitle.setNull(4,Types.VARCHAR);
					insertAlternativeTitle.setInt(5,title.type);
					if (title.episode!=null)
						insertAlternativeTitle.setString(6,title.episode);
					else
						insertAlternativeTitle.setNull(6,Types.VARCHAR);

					if (comment!=null)
						insertAlternativeTitle.setString(7,comment);
					else
						insertAlternativeTitle.setNull(7,Types.VARCHAR);
	
						insertAlternativeTitle.execute();
				}
				
				//if (index>100) break;
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("considered "+actorcnt+" movies.");
		System.out.println("imported "+(totalcnt-ignoredcnt)+" alternative titles, ignored "+ignoredcnt+".");
	}

	private void readItalianAlternativeTitles()
	{
		System.out.println("import italian alternative titles.");

		StringBuffer sb = new StringBuffer();
		
		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"italian-aka-titles.list.gz"))));
			String inputStr;
			
			int index=0;
			String name=null; // name of the current actor
			String movie=null;
			String comment=null;
			int movieID=-1;
			Title title=null;
						
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("AKA TITLES LIST ITALIAN")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
				}
				
				else if (inputStr.indexOf('(')==-1)
					continue;
					
				else if (inActor)
				{
					// we are reading a line for an actor
					//System.out.println("["+index+"] read line:" +inputStr);

					movie=null;
					title=null;
					comment=null;
					
					inputStr=inputStr.trim();
					
					int tab=inputStr.indexOf("\t");
					if (!(inputStr.startsWith("(aka"))) // we have a movie title
					{
						if (tab>0) name=inputStr.substring(0,tab).trim();
						else name=inputStr.trim();
						movieID=findMovie(name);
						actorcnt++;
						//System.out.println("["+movieID+"] found movie "+name);
						continue;
					}
					
					inputStr=inputStr.trim();

					int parcnt=0;
					int i;
					for (i=0; i<inputStr.length(); i++)
					{
						if (inputStr.charAt(i)=='(') parcnt++;
						else if (inputStr.charAt(i)==')') parcnt--;
						if (parcnt==0) break;					
					}
					
					if (i!=0) // we found a movie (and possibly more)
					{
						totalcnt++;

						movie=inputStr.substring(4,i).trim();
						if (i<inputStr.length()-1)
							comment=inputStr.substring(i+1).trim();
						else
							comment=null;
						//System.out.println("movie="+movie+"; i="+i+"; input="+inputStr.length());
						title=demangleComplexTitle(movie);
					}
					
					if (title==null)
					{
						//System.out.println("\t*** ignored an entry***");
						if (i!=0) ignoredcnt++;
						continue;
					}
					
					//System.out.println("\talternative title is "+title.title+" ("+title.year+"/"+title.addition+"/"+title.type+"), comment="+comment);

					insertAlternativeTitle.setInt(1,movieID);
					insertAlternativeTitle.setString(2,title.title);
					insertAlternativeTitle.setInt(3,title.year);
					if (title.addition!=null)
						insertAlternativeTitle.setString(4,title.addition);
					else
						insertAlternativeTitle.setNull(4,Types.VARCHAR);
					insertAlternativeTitle.setInt(5,title.type);
					if (title.episode!=null)
						insertAlternativeTitle.setString(6,title.episode);
					else
						insertAlternativeTitle.setNull(6,Types.VARCHAR);

					if (comment!=null)
						insertAlternativeTitle.setString(7,comment);
					else
						insertAlternativeTitle.setNull(7,Types.VARCHAR);
	
						insertAlternativeTitle.execute();
				}
				
				//if (index>100) break;
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("considered "+actorcnt+" movies.");
		System.out.println("imported "+(totalcnt-ignoredcnt)+" alternative titles, ignored "+ignoredcnt+".");
	}

	
	private void readAlternativeNames()
	{
		System.out.println("import alternative names.");

		StringBuffer sb = new StringBuffer();
		
		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"aka-names.list.gz"))));
			String inputStr;
			
			int index=0;
			String name=null; // name of the current actor
			String actor=null;
			int actorID=-1;
			Title title=null;
						
			while ((inputStr=buffer.readLine()) != null)
			{
				// we finished reading the movies, so break
				if ( inActor && ( inputStr.startsWith("-----")) ) 
				  break ;

				else if ( inputStr.startsWith("AKA NAMES LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip blank line
				}
				
				else if (inputStr.length()<1) continue;
				
				else if (inActor)
				{
					// we are reading a line for an actor
					//System.out.println("["+index+"] read line:" +inputStr);

					actor=null;
					
					inputStr=inputStr.trim();
					
					int tab=inputStr.indexOf("\t");
					if (!(inputStr.startsWith("(aka"))) // we have a movie title
					{
						if (tab>0) name=inputStr.substring(0,tab).trim();
						else name=inputStr.trim();
						actorID=this.findPerson(name);
						actorcnt++;
						//System.out.println("["+actorID+"] found actor "+name);
						continue;
					}
					
					int parcnt=0;
					int i;
					for (i=0; i<inputStr.length(); i++)
					{
						if (inputStr.charAt(i)=='(') parcnt++;
						else if (inputStr.charAt(i)==')') parcnt--;
						if (parcnt==0) break;					
					}
					
					if (i!=0) // we found an alternative actor name
					{
						totalcnt++;

						actor=inputStr.substring(4,i).trim();

						insertAlternativeName.setInt(1,actorID);
						insertAlternativeName.setString(2,actor);

						if (actorID!=-1) insertAlternativeName.execute();

						//System.out.println("\talternative name is "+actor);
					}
				}
				
				//if (index>100) break;
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("considered "+actorcnt+" actors.");
		System.out.println("imported "+(totalcnt-ignoredcnt)+" alternative names, ignored "+ignoredcnt+".");
	}

	private void readBiographies()
	{
		System.out.println("import biographies.");

		boolean inActor=false;
		
		int totalcnt=0;
		int ignoredcnt=0;
		int actorcnt=0;
		
		try
		{      
			BufferedReader buffer = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(DIR+"biographies.list.gz"))));
			String inputStr;
			
			int index=0;
			String name=null; // name of the current actor
			int sourceID=-1;
			String datebirth=null;
			String placebirth=null;
			String datedeath=null;
			String placedeath=null;
			String causedeath=null;
			String realName=null;
			String nickName=null;
			String height=null;
			String bg=null;
			String by=null;
			String bo=null;
			String bt=null;
			String ga=null;
			String ow=null;
			String pi=null;
			String tr=null;
			String qu=null;
			String sa=null;
			String wn=null;
			String sp=null;
			String tm=null;
			String it=null;
			String at=null;
			String pt=null;
			String cv=null;
			String ag=null;
			
			while ((inputStr=buffer.readLine()) != null)
			{
				if ( inputStr.startsWith("BIOGRAPHY LIST")) 
				{
					inActor = true ; 
					buffer.readLine(); // skip "==========="
					buffer.readLine(); // skip "-----------"
//					buffer.readLine(); // skip blank line
				}
				
				else if (inActor)
				{
					// we are reading a line for a movie
					//System.out.println("\tread line:" +inputStr);

					//int tab=inputStr.indexOf("\t");
					if (inputStr.startsWith("---"))
					{
						if (sourceID!=-1)
						{	
							flushBiographies(sourceID,bg,bo,bt,pi,ow,tr,sa,wn,qu,ag,sp,tm,it,at,pt,cv,ga,by);
							flushActorBiography(sourceID,realName,nickName,datebirth,placebirth,datedeath,placedeath,causedeath,height);
						}
						
						datebirth=null;
						placebirth=null;
						datedeath=null;
						placedeath=null;
						causedeath=null;
						realName=null;
						nickName=null;
						height=null;
						bg=null;
						by=null;
						bo=null;
						bt=null;
						ga=null;
						ow=null;
						pi=null;
						tr=null;
						qu=null;
						sa=null;
						wn=null;
						sp=null;
						tm=null;
						it=null;
						at=null;
						pt=null;
						cv=null;
						ag=null;
						sourceID=-1; continue;
					}
					
					else if (inputStr.startsWith("NM:")) // we have an actor's name
					{
						index++;
						name=inputStr.substring(3,inputStr.length()).trim();
						//System.out.println("["+index+"] found actor "+name);

						sourceID=findOrCreatePerson(name);

						actorcnt++;
						continue;
					}
					else if (inputStr.startsWith("RN:"))
					{
						realName=inputStr.substring(3,inputStr.length()).trim();
						continue;
					}
					else if (inputStr.startsWith("NK:"))
					{
						nickName=inputStr.substring(3,inputStr.length()).trim();
						continue;
					}
					else if (inputStr.startsWith("HT:"))
					{
						height=inputStr.substring(3,inputStr.length()).trim();
						continue;
					}
					else if (inputStr.startsWith("DB:"))
					{
						datebirth=inputStr.substring(3,inputStr.length()).trim();
						int commapos=datebirth.indexOf(',');
						if (commapos!=-1)
						{
							placebirth=datebirth.substring(commapos+1).trim();
							datebirth=datebirth.substring(0,commapos).trim();
						}
						continue;
					}
					else if (inputStr.startsWith("DD:"))
					{
						datedeath=inputStr.substring(3,inputStr.length()).trim();
						int commapos=datedeath.indexOf(',');
						if (commapos!=-1)
						{
							placedeath=datedeath.substring(commapos+1).trim();
							datedeath=datedeath.substring(0,commapos).trim();
							
							int parpos=placedeath.indexOf('(');
							if (parpos!=-1)
							{
								causedeath=placedeath.substring(parpos+1,placedeath.length()-1).trim();
								placedeath=placedeath.substring(0,parpos).trim();								
							}
						}
							
						int parpos=datedeath.indexOf('(');
						if ((parpos!=-1)&&(causedeath==null))
						{
							causedeath=datedeath.substring(parpos+1,datedeath.length()-1).trim();
							datedeath=datedeath.substring(0,parpos).trim();								
						}
						
						if ((datedeath!=null)&&(datedeath.endsWith("."))) datedeath=datedeath.substring(0,datedeath.length()-1);
						if ((datebirth!=null)&&(datebirth.endsWith("."))) datebirth=datebirth.substring(0,datebirth.length()-1);

						if ((placebirth!=null)&&(placebirth.endsWith("."))) placebirth=placebirth.substring(0,placebirth.length()-1);
						if ((placedeath!=null)&&(placedeath.endsWith("."))) placedeath=placedeath.substring(0,placedeath.length()-1);

						continue;
					}
					else if (inputStr.startsWith("BG:"))
					{
						if (bg!=null) bg=bg+" "+inputStr.substring(3,inputStr.length()).trim();
						else bg=inputStr.substring(3,inputStr.length()).trim();
						continue;
					}
					else if (inputStr.startsWith("BY:"))
					{
						totalcnt++;
						
						by=inputStr.substring(3,inputStr.length()).trim();
													
						if ((sourceID!=-1)&&(bg!=null))
						{
							System.out.println("biography for actorID "+sourceID+" by "+by+" is "+bg);

							StringReader sr=new StringReader(bg);
							
							insertFullBiography.setInt(1,sourceID);
							insertFullBiography.setString(2,by);
							insertFullBiography.setCharacterStream(3,sr,bg.length());
							
							insertFullBiography.execute();
						}
						else ignoredcnt++;
						
						bg=null; by=null;
					}
					else if (inputStr.startsWith("BO:"))
					{
						flushBiographies(sourceID,bg,null,bt,pi,ow,tr,sa,wn,qu,ag,sp,tm,it,at,pt,cv,ga,by);
						by=null;ga=null;bg=null;by=null;bt=null;ga=null;ow=null;pi=null;tr=null;qu=null;sa=null;wn=null;sp=null;tm=null;it=null;at=null;pt=null;cv=null;ag=null;

						inputStr=inputStr.substring(3,inputStr.length()).trim();
						if (inputStr.startsWith("*")) // new entry
						{
							if ((sourceID!=-1)&&(bo!=null))
							{
								//System.out.println("biography book for actorID "+sourceID+" is "+bo);
							
								insertBiographicalBook.setInt(1,sourceID);
								insertBiographicalBook.setString(2,bo);
								
								insertBiographicalBook.execute();
							}
							if (inputStr.length()>2) inputStr=inputStr.substring(2,inputStr.length()).trim();
							else if (inputStr.length()==1) continue;
							bo=null;
						}
						if (bo!=null) bo=bo+" "+inputStr;
						else bo=inputStr;
						continue;
					}
					else if (inputStr.startsWith("BT:"))
					{
						flushBiographies(sourceID,bg,bo,null,pi,ow,tr,sa,wn,qu,ag,sp,tm,it,at,pt,cv,ga,by);
						by=null;ga=null;bg=null;by=null;bo=null;ga=null;ow=null;pi=null;tr=null;qu=null;sa=null;wn=null;sp=null;tm=null;it=null;at=null;pt=null;cv=null;ag=null;

						inputStr=inputStr.substring(3,inputStr.length()).trim();
						if (inputStr.startsWith("*")) // new entry
						{
							if ((sourceID!=-1)&&(bt!=null))
							{
								//System.out.println("biographical film for actorID "+sourceID+" is "+bt); 
							
								insertBiographicalFilm.setInt(1,sourceID);
								insertBiographicalFilm.setString(2,bt);
								
								insertBiographicalFilm.execute();
							}
							if (inputStr.length()>2) inputStr=inputStr.substring(2,inputStr.length()).trim();
							else if (inputStr.length()==1) continue;
							bt=null;
						}
						if (bt!=null) bt=bt+" "+inputStr;
						else bt=inputStr;
						continue;
					}
					else if (inputStr.startsWith("PI:"))
					{
						flushBiographies(sourceID,bg,bo,bt,null,ow,tr,sa,wn,qu,ag,sp,tm,it,at,pt,cv,ga,by);
						by=null;ga=null;bg=null;by=null;bo=null;ga=null;ow=null;bt=null;tr=null;qu=null;sa=null;wn=null;sp=null;tm=null;it=null;at=null;pt=null;cv=null;ag=null;

						inputStr=inputStr.substring(3,inputStr.length()).trim();
						if (inputStr.startsWith("*")) // new entry
						{
							if ((sourceID!=-1)&&(pi!=null))
							{
								//System.out.println("portrait for actorID "+sourceID+" is "+pi);
							
								insertPortrait.setInt(1,sourceID);
								insertPortrait.setString(2,pi);
								
								insertPortrait.execute();
							}
							if (inputStr.length()>2) inputStr=inputStr.substring(2,inputStr.length()).trim();
							else if (inputStr.length()==1) continue;
							pi=null;
						}
						if (pi!=null) pi=pi+" "+inputStr;
						else pi=inputStr;
						continue;
					}
					else if (inputStr.startsWith("OW:"))
					{
						flushBiographies(sourceID,bg,bo,bt,pi,null,tr,sa,wn,qu,ag,sp,tm,it,at,pt,cv,ga,by);
						by=null;ga=null;bg=null;by=null;bo=null;ga=null;pi=null;bt=null;tr=null;qu=null;sa=null;wn=null;sp=null;tm=null;it=null;at=null;pt=null;cv=null;ag=null;

						inputStr=inputStr.substring(3,inputStr.length()).trim();
						if (inputStr.startsWith("*")) // new entry
						{
							if ((sourceID!=-1)&&(ow!=null))
							{
								//System.out.println("other works for actorID "+sourceID+" is "+ow);
							
								StringReader sr=new StringReader(ow);
								insertOtherWork.setInt(1,sourceID);
								insertOtherWork.setCharacterStream(2,sr,ow.length());
								
								insertOtherWork.execute();
							}
							if (inputStr.length()>2) inputStr=inputStr.substring(2,inputStr.length()).trim();
							else if (inputStr.length()==1) continue;
							ow=null;
						}
						if (ow!=null) ow=ow+" "+inputStr;
						else ow=inputStr;
						continue;
					}
					else if (inputStr.startsWith("TR:"))
					{
						flushBiographies(sourceID,bg,bo,bt,pi,ow,null,sa,wn,qu,ag,sp,tm,it,at,pt,cv,ga,by);
						by=null;ga=null;bg=null;by=null;bo=null;ga=null;ow=null;bt=null;pi=null;qu=null;sa=null;wn=null;sp=null;tm=null;it=null;at=null;pt=null;cv=null;ag=null;

						inputStr=inputStr.substring(3,inputStr.length()).trim();
						if (inputStr.startsWith("*")) // new entry
						{
							if ((sourceID!=-1)&&(tr!=null))
							{
								//System.out.println("trivia for actorID "+sourceID+" is "+tr);
							
								StringReader sr=new StringReader(tr);
								insertActorTrivia.setInt(1,sourceID);
								insertActorTrivia.setCharacterStream(2,sr,tr.length());
								
								insertActorTrivia.execute();
							}
							if (inputStr.length()>2) inputStr=inputStr.substring(2,inputStr.length()).trim();
							else if (inputStr.length()==1) continue;
							tr=null;
						}
						if (tr!=null) tr=tr+" "+inputStr;
						else tr=inputStr;
						continue;
					}
					else if (inputStr.startsWith("SA:"))
					{
						flushBiographies(sourceID,bg,bo,bt,pi,ow,tr,null,wn,qu,ag,sp,tm,it,at,pt,cv,ga,by);
						by=null;ga=null;bg=null;by=null;bo=null;ga=null;ow=null;bt=null;pi=null;qu=null;tr=null;wn=null;sp=null;tm=null;it=null;at=null;pt=null;cv=null;ag=null;

						inputStr=inputStr.substring(3,inputStr.length()).trim();
						if (inputStr.startsWith("*")) // new entry
						{
							if ((sourceID!=-1)&&(sa!=null))
							{
								String salary=null;
								String movie=null;
								int movieID=-1;
				
								int pos=sa.indexOf("->");
								if (pos!=-1)
								{
									salary=sa.substring(pos+2).trim();
									movie=sa.substring(0,pos).trim();
									int fp=movie.indexOf('_');
									int lp=movie.lastIndexOf('_');
									if ((fp==0)&&(lp!=-1))
										movie=movie.substring(fp+1,lp);
								}
								else
								{
									salary=sa;
								}

								//System.out.println("sa="+sa+"; movie="+movie+"; salary="+salary);
				
								insertSalary.setInt(1,sourceID);
				
								if (movie!=null)
								{
									movieID=findMovie(movie);
					
									if (movieID==-1) insertSalary.setNull(2,Types.INTEGER);
									else insertSalary.setInt(2,movieID);
									insertSalary.setString(3,salary);
					
									insertSalary.execute();
					
									//System.out.println("salary for actorID "+sourceID+" in movie "+movieID+" is "+salary);
								}
							}
							if (inputStr.length()>2) inputStr=inputStr.substring(2,inputStr.length()).trim();
							else if (inputStr.length()==1) continue;
							sa=null;
						}
						if (sa!=null) sa=sa+" "+inputStr;
						else sa=inputStr;
						continue;
					}
					else if (inputStr.startsWith("WN:"))
					{
						flushBiographies(sourceID,bg,bo,bt,pi,ow,tr,sa,null,qu,ag,sp,tm,it,at,pt,cv,ga,by);
						by=null;ga=null;bg=null;by=null;bo=null;ga=null;ow=null;bt=null;pi=null;qu=null;tr=null;sa=null;sp=null;tm=null;it=null;at=null;pt=null;cv=null;ag=null;

						inputStr=inputStr.substring(3,inputStr.length()).trim();
						if (inputStr.startsWith("*")) // new entry
						{
							if ((sourceID!=-1)&&(wn!=null))
							{
								//System.out.println("where is he now for actorID "+sourceID+" is "+wn);
							
								StringReader sr=new StringReader(wn);
								insertWhereNow.setInt(1,sourceID);
								insertWhereNow.setCharacterStream(2,sr,wn.length());
								
								insertWhereNow.execute();
							} 
							if (inputStr.length()>2) inputStr=inputStr.substring(2,inputStr.length()).trim();
							else if (inputStr.length()==1) continue;
							wn=null;
						}
						if (wn!=null) wn=wn+" "+inputStr;
						else wn=inputStr;
						continue;
					}
					else if (inputStr.startsWith("QU:"))
					{
						flushBiographies(sourceID,bg,bo,bt,pi,ow,tr,sa,wn,null,ag,sp,tm,it,at,pt,cv,ga,by);
						by=null;ga=null;bg=null;by=null;bo=null;ga=null;ow=null;bt=null;pi=null;wn=null;tr=null;sa=null;sp=null;tm=null;it=null;at=null;pt=null;cv=null;ag=null;

						inputStr=inputStr.substring(3,inputStr.length()).trim();
						if (inputStr.startsWith("*")) // new entry
						{
							if ((sourceID!=-1)&&(qu!=null))
							{
								//System.out.println("quote for actorID "+sourceID+" is "+qu);
							
								StringReader sr=new StringReader(qu);
								insertActorQuote.setInt(1,sourceID);
								insertActorQuote.setCharacterStream(2,sr,qu.length());
								
								insertActorQuote.execute();
							}
							if (inputStr.length()>2) inputStr=inputStr.substring(2,inputStr.length()).trim();
							else if (inputStr.length()==1) continue;
							qu=null;
						}
						if (qu!=null) qu=qu+" "+inputStr;
						else qu=inputStr;
						continue;
					}
					else if (inputStr.startsWith("AG:"))
					{
						flushBiographies(sourceID,bg,bo,bt,pi,ow,tr,sa,wn,qu,null,sp,tm,it,at,pt,cv,ga,by);
						by=null;ga=null;bg=null;by=null;bo=null;ga=null;ow=null;bt=null;pi=null;wn=null;tr=null;sa=null;sp=null;tm=null;it=null;at=null;pt=null;cv=null;qu=null;

						inputStr=inputStr.substring(3,inputStr.length()).trim();
						if (inputStr.startsWith("*")) // new entry
						{
							if ((sourceID!=-1)&&(ag!=null))
							{
								//System.out.println("agent for actorID "+sourceID+" is "+ag);
							
								insertAgent.setInt(1,sourceID);
								insertAgent.setString(2,ag);
								
								insertAgent.execute();
							}
							if (inputStr.length()>2) inputStr=inputStr.substring(2,inputStr.length()).trim();
							else if (inputStr.length()==1) continue;
							ag=null;
						}
						if (ag!=null) ag=ag+" "+inputStr;
						else ag=inputStr;
						continue;
					}
					else if (inputStr.startsWith("SP:"))
					{
						flushBiographies(sourceID,bg,bo,bt,pi,ow,tr,sa,wn,qu,ag,null,tm,it,at,pt,cv,ga,by);
						by=null;ga=null;bg=null;by=null;bo=null;ga=null;ow=null;bt=null;pi=null;wn=null;tr=null;sa=null;ag=null;tm=null;it=null;at=null;pt=null;cv=null;qu=null;

						inputStr=inputStr.substring(3,inputStr.length()).trim();
						if (inputStr.startsWith("*")) // new entry
						{
							if ((sourceID!=-1)&&(sp!=null))
							{
								//System.out.println("spouse for actorID "+sourceID+" is "+sp);
							
								insertSpouse.setInt(1,sourceID);
								insertSpouse.setString(2,sp);
								
								insertSpouse.execute();
							}
							if (inputStr.length()>2) inputStr=inputStr.substring(2,inputStr.length()).trim();
							else if (inputStr.length()==1) continue;
							sp=null;
						}
						if (sp!=null) sp=sp+" "+inputStr;
						else sp=inputStr;
						continue;
					}
					else if (inputStr.startsWith("TM:"))
					{
						flushBiographies(sourceID,bg,bo,bt,pi,ow,tr,sa,wn,qu,ag,sp,null,it,at,pt,cv,ga,by);
						by=null;ga=null;bg=null;by=null;bo=null;ga=null;ow=null;bt=null;pi=null;wn=null;tr=null;sa=null;ag=null;sp=null;it=null;at=null;pt=null;cv=null;qu=null;

						inputStr=inputStr.substring(3,inputStr.length()).trim();
						if (inputStr.startsWith("*")) // new entry
						{
							if ((sourceID!=-1)&&(tm!=null))
							{
								//System.out.println("trademark for actorID "+sourceID+" is "+tm);
							
								StringReader sr=new StringReader(tm);
								insertTrademark.setInt(1,sourceID);
								insertTrademark.setCharacterStream(2,sr,tm.length());
								
								insertTrademark.execute();
							}
							if (inputStr.length()>2) inputStr=inputStr.substring(2,inputStr.length()).trim();
							else if (inputStr.length()==1) continue;
							tm=null;
						}
						if (tm!=null) tm=tm+" "+inputStr;
						else tm=inputStr;
						continue;
					}
					else if (inputStr.startsWith("IT:"))
					{
						flushBiographies(sourceID,bg,bo,bt,pi,ow,tr,sa,wn,qu,ag,sp,tm,null,at,pt,cv,ga,by);
						by=null;ga=null;bg=null;by=null;bo=null;ga=null;ow=null;bt=null;pi=null;wn=null;tr=null;sa=null;ag=null;sp=null;tm=null;at=null;pt=null;cv=null;qu=null;

						inputStr=inputStr.substring(3,inputStr.length()).trim();
						if (inputStr.startsWith("*")) // new entry
						{
							if ((sourceID!=-1)&&(it!=null))
							{
								//System.out.println("interview for actorID "+sourceID+" is "+it);
							
								insertInterview.setInt(1,sourceID);
								insertInterview.setString(2,it);
								
								insertInterview.execute();
							}
							if (inputStr.length()>2) inputStr=inputStr.substring(2,inputStr.length()).trim();
							else if (inputStr.length()==1) continue;
							it=null;
						}
						if (it!=null) it=it+" "+inputStr;
						else it=inputStr;
						continue;
					}
					else if (inputStr.startsWith("AT:"))
					{
						flushBiographies(sourceID,bg,bo,bt,pi,ow,tr,sa,wn,qu,ag,sp,tm,it,null,pt,cv,ga,by);
						by=null;ga=null;bg=null;by=null;bo=null;ga=null;ow=null;bt=null;pi=null;wn=null;tr=null;sa=null;ag=null;sp=null;tm=null;it=null;pt=null;cv=null;qu=null;

						inputStr=inputStr.substring(3,inputStr.length()).trim();
						if (inputStr.startsWith("*")) // new entry
						{
							if ((sourceID!=-1)&&(at!=null))
							{
								//System.out.println("article for actorID "+sourceID+" is "+at);
							
								StringReader sr=new StringReader(at);
								insertArticle.setInt(1,sourceID);
								insertArticle.setCharacterStream(2,sr,at.length());
								
								insertArticle.execute();
							}
							if (inputStr.length()>2) inputStr=inputStr.substring(2,inputStr.length()).trim();
							else if (inputStr.length()==1) continue;
							at=null;
						}
						if (at!=null) at=at+" "+inputStr;
						else at=inputStr;
						continue;
					}
					else if (inputStr.startsWith("PT:"))
					{
						flushBiographies(sourceID,bg,bo,bt,pi,ow,tr,sa,wn,qu,ag,sp,tm,it,at,null,cv,ga,by);
						by=null;ga=null;bg=null;by=null;bo=null;ga=null;ow=null;bt=null;pi=null;wn=null;tr=null;sa=null;ag=null;sp=null;tm=null;it=null;at=null;cv=null;qu=null;

						inputStr=inputStr.substring(3,inputStr.length()).trim();
						if (inputStr.startsWith("*")) // new entry
						{
							if ((sourceID!=-1)&&(pt!=null))
							{
//								System.out.println("pictorial for actorID "+sourceID+" is "+pt);
							
								insertPictorial.setInt(1,sourceID);
								insertPictorial.setString(2,pt);
								
								insertPictorial.execute();
							}
							if (inputStr.length()>2) inputStr=inputStr.substring(2,inputStr.length()).trim();
							else if (inputStr.length()==1) continue;
							pt=null;
						}
						if (pt!=null) pt=pt+" "+inputStr;
						else pt=inputStr;
						continue;
					}
					else if (inputStr.startsWith("CV:"))
					{
						flushBiographies(sourceID,bg,bo,bt,pi,ow,tr,sa,wn,qu,ag,sp,tm,it,at,pt,null,ga,by);
						by=null;ga=null;bg=null;by=null;bo=null;ga=null;ow=null;bt=null;pi=null;wn=null;tr=null;sa=null;ag=null;sp=null;tm=null;it=null;at=null;pt=null;qu=null;

						inputStr=inputStr.substring(3,inputStr.length()).trim();
						if (inputStr.startsWith("*")) // new entry
						{
							if ((sourceID!=-1)&&(cv!=null))
							{
								//System.out.println("cover photo for actorID "+sourceID+" is "+cv);
							
								insertCoverPhoto.setInt(1,sourceID);
								insertCoverPhoto.setString(2,cv);
								
								insertCoverPhoto.execute();
							}
							if (inputStr.length()>2) inputStr=inputStr.substring(2,inputStr.length()).trim();
							else if (inputStr.length()==1) continue;
							cv=null;
						}
						if (cv!=null) cv=cv+" "+inputStr;
						else cv=inputStr;
						continue;
					}
					else if (inputStr.startsWith("GA:"))
					{
						flushBiographies(sourceID,bg,bo,bt,pi,ow,tr,sa,wn,qu,ag,sp,tm,it,at,pt,cv,null,by);
						by=null;bg=null;by=null;bo=null;ow=null;bt=null;pi=null;wn=null;tr=null;sa=null;ag=null;sp=null;tm=null;it=null;at=null;pt=null;qu=null;

						inputStr=inputStr.substring(3,inputStr.length()).trim();
						if (inputStr.startsWith("*")) // new entry
						{
							if ((sourceID!=-1)&&(ga!=null))
							{
								//System.out.println("guest appearance for actorID "+sourceID+" is "+ga);
							
								insertGuestAppearance.setInt(1,sourceID);
								insertGuestAppearance.setString(2,ga);
								
								insertGuestAppearance.execute();
							}
							if (inputStr.length()>2) inputStr=inputStr.substring(2,inputStr.length()).trim();
							else if (inputStr.length()==1) continue;
							ga=null;
						}
						if (ga!=null) ga=ga+" "+inputStr;
						else ga=inputStr;
						continue;
					}
				}
				
				//if (index>100) break;
			}
			flushBiographies(sourceID,bg,bo,bt,pi,ow,tr,sa,wn,qu,ag,sp,tm,it,at,pt,cv,ga,by);
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("considered "+actorcnt+" persons.");
		System.out.println("imported "+(totalcnt-ignoredcnt)+" biographies, ignored "+ignoredcnt+".");
	}
	
	private void flushBiographies(int sourceID, String bg, String bo, String bt, String pi, String ow, String tr, String sa, String wn, String qu, String ag, String sp, String tm, String it, String at, String pt, String cv, String ga, String by)
	{
		if (sourceID==-1) return;
		
		try
		{
			if (bg!=null)
			{
				//System.out.println("biography for actorID "+sourceID+" is "+bg);
	
				StringReader sr=new StringReader(bg);
								
				insertFullBiography.setInt(1,sourceID);
				if (by==null) insertFullBiography.setNull(2,Types.VARCHAR);
				else insertFullBiography.setString(2,by);
				insertFullBiography.setCharacterStream(3,sr,bg.length());
								
				insertFullBiography.execute();
			}
			if (bo!=null)
			{
				//System.out.println("biography book for actorID "+sourceID+" is "+bo);
				
				insertBiographicalBook.setInt(1,sourceID);
				insertBiographicalBook.setString(2,bo);
								
				insertBiographicalBook.execute();
			}
			if (bt!=null)
			{
				//System.out.println("biographical film for actorID "+sourceID+" is "+bt);
				
				insertBiographicalFilm.setInt(1,sourceID);
				insertBiographicalFilm.setString(2,bt);
								
				insertBiographicalFilm.execute();
			}
			if (pi!=null)
			{
				//System.out.println("portrait film for actorID "+sourceID+" is "+pi);
				
				insertPortrait.setInt(1,sourceID);
				insertPortrait.setString(2,pi);
								
				insertPortrait.execute();
			}
			if (ow!=null)
			{
				//System.out.println("other works for actorID "+sourceID+" is "+ow);
				
				StringReader sr=new StringReader(ow);
				insertOtherWork.setInt(1,sourceID);
				insertOtherWork.setCharacterStream(2,sr,ow.length());
								
				insertOtherWork.execute();
			}
			if (tr!=null)
			{
				//System.out.println("trivia for actorID "+sourceID+" is "+tr);
				
				StringReader sr=new StringReader(tr);
				insertActorTrivia.setInt(1,sourceID);
				insertActorTrivia.setCharacterStream(2,sr,tr.length());
								
				insertActorTrivia.execute();
			}
			if (sa!=null)
			{
				String salary=null;
				String movie=null;
				int movieID=-1;
		
				//System.out.println("sa="+sa);
						
				int pos=sa.indexOf("->");
				if (pos!=-1)
				{
					salary=sa.substring(pos+2).trim();
					movie=sa.substring(0,pos).trim();
					int fp=movie.indexOf('_');
					int lp=movie.lastIndexOf('_');
					if ((fp==0)&&(lp!=-1))
						movie=movie.substring(fp+1,lp);
				}
				else
				{
					salary=sa;
				}

				//System.out.println("sa="+sa+"; movie="+movie+"; salary="+salary);
				
				insertSalary.setInt(1,sourceID);
				
				if (movie!=null)
				{
					movieID=findMovie(movie);
					
					if (movieID==-1) insertSalary.setNull(2,Types.INTEGER);
					else insertSalary.setInt(2,movieID);
					insertSalary.setString(3,salary);
					
					insertSalary.execute();
					
					//System.out.println("salary for actorID "+sourceID+" in movie "+movieID+" is "+salary);
				}
				
			}
			if (wn!=null)
			{
				//System.out.println("where is he for actorID "+sourceID+" is "+wn);
				
				StringReader sr=new StringReader(wn);

				insertWhereNow.setInt(1,sourceID);
				insertWhereNow.setCharacterStream(2,sr,wn.length());
								
				insertWhereNow.execute();
			}
			if (qu!=null)
			{
				//System.out.println("quote for actorID "+sourceID+" is "+qu);
				
				StringReader sr=new StringReader(qu);

				insertActorQuote.setInt(1,sourceID);
				insertActorQuote.setCharacterStream(2,sr,qu.length());
								
				insertActorQuote.execute();
			}
			if (ag!=null)
			{
				//System.out.println("agent's address for actorID "+sourceID+" is "+ag);
				
				insertAgent.setInt(1,sourceID);
				insertAgent.setString(2,ag);
								
				insertAgent.execute();
			}
			if (sp!=null)
			{
				//System.out.println("spouse for actorID "+sourceID+" is "+sp);
				
				insertSpouse.setInt(1,sourceID);
				insertSpouse.setString(2,sp);
								
				insertSpouse.execute();
			}
			if (tm!=null)
			{
				//System.out.println("trade mark for actorID "+sourceID+" is "+tm);
				
				StringReader sr=new StringReader(tm);
				insertTrademark.setInt(1,sourceID);
				insertTrademark.setCharacterStream(2,sr,tm.length());
								
				insertTrademark.execute();
			}
			if (it!=null)
			{
				//System.out.println("interview for actorID "+sourceID+" is "+it);
				
				insertInterview.setInt(1,sourceID);
				insertInterview.setString(2,it);
								
				insertInterview.execute();
			}
			if (at!=null)
			{
				//System.out.println("article for actorID "+sourceID+" is "+at);
				
				StringReader sr=new StringReader(at);
				insertArticle.setInt(1,sourceID);
				insertArticle.setCharacterStream(2,sr,at.length());
								
				insertArticle.execute();
			}
			if (pt!=null)
			{
				//System.out.println("pictorial for actorID "+sourceID+" is "+pt);
				
				insertPictorial.setInt(1,sourceID);
				insertPictorial.setString(2,pt);
								
				insertPictorial.execute();
			}
			if (cv!=null)
			{
				//System.out.println("cover photo for actorID "+sourceID+" is "+cv);
				
				insertCoverPhoto.setInt(1,sourceID);
				insertCoverPhoto.setString(2,cv);
								
				insertCoverPhoto.execute();
			}
			if (ga!=null)
			{
				//System.out.println("guest appearance for actorID "+sourceID+" is "+ga);
				
				insertGuestAppearance.setInt(1,sourceID);
				insertGuestAppearance.setString(2,ga);
								
				insertGuestAppearance.execute();
			}
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
	}
	
	private void flushActorBiography(int sourceID, String realname, String nickname, String datebirth, String placebirth, String datedeath, String placedeath, String causedeath, String height)
	{
		if (sourceID==-1) return;
		
		try
		{
			//System.out.print("flush Actor "+sourceID);
			
			if (realname!=null)
			{
				//System.out.print("; real name "+realname);
				updatePerson.setString(1,realname);
			}
			else updatePerson.setNull(1,Types.VARCHAR);

			if (nickname!=null)
			{
				//System.out.print("; nick name "+nickname);
				updatePerson.setString(2,nickname);
			}
			else updatePerson.setNull(2,Types.VARCHAR);

			if (datebirth!=null)
			{
				//System.out.print("; born "+datebirth+" in "+placebirth);
				updatePerson.setString(3,datebirth);
			}
			else updatePerson.setNull(3,Types.VARCHAR);
			
			if (placebirth!=null)
			updatePerson.setString(4,placebirth);
			else updatePerson.setNull(4,Types.VARCHAR);
			
			if (datedeath!=null)
			{
				//System.out.print("; died "+datedeath+" in "+placedeath+ " of "+causedeath);
				updatePerson.setString(5,datedeath);
			}
			else updatePerson.setNull(5,Types.VARCHAR);

			if (placedeath!=null)
			updatePerson.setString(6,placedeath);
			else updatePerson.setNull(6,Types.VARCHAR);
			
			if (causedeath!=null)
			updatePerson.setString(7,causedeath);
			else updatePerson.setNull(7,Types.VARCHAR);
			
			if (height!=null)
			{
				//System.out.print("; height "+height);
				updatePerson.setString(8,height);
			}
			else updatePerson.setNull(8,Types.VARCHAR);
			
			updatePerson.setInt(9,sourceID);
			
			updatePerson.execute();
			
			//System.out.println();
		}
		catch(Exception e)
		{
			System.out.println("flushActorBiography("+sourceID+"): "+e);
			e.printStackTrace();
			System.exit(1);
		}
	}
	private int min(int i1, int i2)
	{
		if (i1<i2) return i1;
		else return i2;
	}
	
	private HashMap<String,Integer> name2personid=new HashMap<String,Integer>();
	
	private int maxPersonID=0;
	
	int fPAllCnt=0; int fPHits=0;
	
	private int findOrCreatePerson(String name)
	{
		return findOrCreatePerson(name,0);
	}

	private int findOrCreatePerson(String name, int sex)
	{
		Integer id=name2personid.get(name);
		
		fPAllCnt++;
		if (id!=null) fPHits++;
		
		if (fPAllCnt%10000==0)
		{
			System.out.println("find*Person(): "+fPAllCnt+" calls, "+(100.0*fPHits/10000.0)+"% hit ratio in last 10000 calls");
			fPHits=0;
		}

		if (id!=null) return id;
		
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

			if (myID==-1)
			{
				if (maxPersonID==0)
				{
					rs=selectMaxPersonID.executeQuery();
					rs.next();
					maxPersonID=rs.getInt(1);
					myID=rs.getInt(1)+1;
					rs.close();
				}
				myID=++maxPersonID;
				
				insertPerson.setInt(1,myID);
				insertPerson.setString(2,name);
				if (sex>0) insertPerson.setInt(3, sex);
				else insertPerson.setNull(3, Types.INTEGER);
				insertPerson.execute();
			}
			
			name2personid.put(name, myID);
		}
		catch(Exception e)
		{
			System.out.println("findOrCreatePersion("+name+"): "+e);
			e.printStackTrace();
		}
		
		return myID;
	}

	private int findPerson(String name)
	{
		Integer id=name2personid.get(name);
		
		fPAllCnt++;
		if (id!=null) fPHits++;
		
		if (fPAllCnt%10000==0)
		{
			System.out.println("find*Person(): "+fPAllCnt+" calls, "+(100.0*fPHits/10000.0)+"% hit ratio in last 10000 calls");
			fPHits=0;
		}

		if (id!=null) return id;

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
	
	private HashMap<String,Integer> title2movieid=new HashMap<String,Integer>();
	
	int fMHits=0; int fMAllCnt=0;
	
	long fMts=System.currentTimeMillis();
	
	private int findMovie(String fullName)
	{
//		System.out.println("findMovie("+fullName+")");
		
		Integer id=title2movieid.get(fullName);

		fMAllCnt++;
		if (id!=null) fMHits++;
		
		if (fMAllCnt%10000==0)
		{
			long ts=System.currentTimeMillis();
			double secs=(ts-fMts)/1000.0;
			System.out.println("findMovie(): "+fMAllCnt+" calls, "+(100.0*fMHits/10000.0)+"% hit ratio in last 10000 calls, "+10000.0/secs+" calls/sec");
			fMHits=0; fMts=ts;
		}
		if (id!=null) return id;
		
		int movieID=-1;
		
		try
		{
			Title t=demangleComplexTitle(fullName);
			if (t!=null)
			{
//				System.out.println(t.title);
				// unfix fix
				if (t.title.compareTo("scrubs")==0)
					t.title="[scrubs]";
				
				if ((t.addition!=null)&&(t.episode!=null))
				{
					selectMovieID.setString(1,t.title);
					selectMovieID.setInt(2,t.year);
					selectMovieID.setString(3,t.addition);
					selectMovieID.setInt(4,t.type);
					selectMovieID.setString(5, t.episode);
					ResultSet rs=selectMovieID.executeQuery();
					if (rs.next())
					{
						movieID=rs.getInt(1);
					}
					rs.close();
					if (movieID==-1)
					{
						selectAlternativeMovieID.setString(1,t.title);
						selectAlternativeMovieID.setInt(2,t.year);
						selectAlternativeMovieID.setString(3,t.addition);
						selectAlternativeMovieID.setInt(4,t.type);
						selectAlternativeMovieID.setString(5, t.episode);
						rs=selectAlternativeMovieID.executeQuery();
						if (rs.next())
						{
							movieID=rs.getInt(1);
						}
						rs.close();
					}
				}
				else if ((t.addition==null)&&(t.episode==null))
				{
					selectMovieIDNULLNULL.setString(1,t.title);
					selectMovieIDNULLNULL.setInt(2,t.year);
					selectMovieIDNULLNULL.setInt(3,t.type);
					ResultSet rs=selectMovieIDNULLNULL.executeQuery();
					if (rs.next())
					{
						movieID=rs.getInt(1);
					}
					rs.close();
					if (movieID==-1)
					{
						selectAlternativeMovieIDNULLNULL.setString(1,t.title);
						selectAlternativeMovieIDNULLNULL.setInt(2,t.year);
						selectAlternativeMovieIDNULLNULL.setInt(3,t.type);
						rs=selectAlternativeMovieIDNULLNULL.executeQuery();
						if (rs.next())
						{
							movieID=rs.getInt(1);
						}
						rs.close();
					}
				}
				else if (t.addition==null)
				{
					selectMovieIDNULLEpisode.setString(1,t.title);
					selectMovieIDNULLEpisode.setInt(2,t.year);
					selectMovieIDNULLEpisode.setInt(3,t.type);
					selectMovieIDNULLEpisode.setString(4, t.episode);
					ResultSet rs=selectMovieIDNULLEpisode.executeQuery();
					if (rs.next())
					{
						movieID=rs.getInt(1);
					}
					rs.close();
					if (movieID==-1)
					{
						selectAlternativeMovieIDNULLEpisode.setString(1,t.title);
						selectAlternativeMovieIDNULLEpisode.setInt(2,t.year);
						selectAlternativeMovieIDNULLEpisode.setInt(3,t.type);
						selectAlternativeMovieIDNULLEpisode.setString(4,t.episode);
						rs=selectAlternativeMovieIDNULLEpisode.executeQuery();
						if (rs.next())
						{
							movieID=rs.getInt(1);
						}
						rs.close();
					}					
				}
				else 
				{
					selectMovieIDAdditionNULL.setString(1,t.title);
					selectMovieIDAdditionNULL.setInt(2,t.year);
					selectMovieIDAdditionNULL.setInt(4,t.type);
					selectMovieIDAdditionNULL.setString(3, t.addition);
					ResultSet rs=selectMovieIDAdditionNULL.executeQuery();
					if (rs.next())
					{
						movieID=rs.getInt(1);
					}
					rs.close();
					if (movieID==-1)
					{
						selectAlternativeMovieIDAdditionNULL.setString(1,t.title);
						selectAlternativeMovieIDAdditionNULL.setInt(2,t.year);
						selectAlternativeMovieIDAdditionNULL.setInt(4,t.type);
						selectAlternativeMovieIDAdditionNULL.setString(3,t.addition);
						rs=selectAlternativeMovieIDAdditionNULL.executeQuery();
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
		
		if (movieID!=-1)
			title2movieid.put(fullName, movieID);
		
		return movieID;
	}
}
