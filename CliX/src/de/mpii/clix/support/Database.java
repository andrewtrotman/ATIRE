/*
 * Created on 10.02.2005
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package de.mpii.clix.support;

import java.sql.*;

import oracle.jdbc.driver.OracleDriver;

public class Database {

    private Connection dbConn=null;
    private Statement dbStmt=null;
    
    // ------- database support methods --------
    
    public boolean openDatabase(String dbUser,String dbPasswd,String dbHost,String dbService)
    {
		String sql = "";

//		System.out.println("connecting to database...");
		
		try {
			sql =
				"jdbc:oracle:thin:@(DESCRIPTION=(ADDRESS_LIST=(ADDRESS=(PROTOCOL=TCP)(HOST="
					+ dbHost
					+ ")(PORT=1521)))(CONNECT_DATA= (SERVICE_NAME="
					+ dbService
					+ " )))";

			//System.out.println("sql="+sql);
			
			DriverManager.registerDriver(new OracleDriver());
			dbConn = DriverManager.getConnection(sql, dbUser, dbPasswd);
			dbConn.setAutoCommit(true);
		} catch (Exception e) {
			try {
				System.out.println("cannot open connection to service "+dbService+" on host "+dbHost+" (user="+dbUser+"):\n"+e);
				dbConn.close();
				return false;
			} catch (Exception e1) {
				System.out.println("nested exception when closing connection:\n"+ e1);
				return false;
			}
		}
		
//		System.out.println("connected.");
		
		try
		{
		    dbStmt=dbConn.createStatement();
		}
		catch(Exception e)
		{
		    System.out.println("cannot create statement:\n"+e);
		    try
		    {
		        dbConn.close();
		        return false;
		    }
		    catch(Exception e1)
		    {
		        System.out.println("nested exception when closing connection:\n"+e1);
		        return false;
		    }
		}
		return true;
    }

    public ResultSet execQuery(String query)
    {
        ResultSet rs=null;
        try
        {
            rs=dbStmt.executeQuery(query);
        }
        catch(Exception e)
        {
            System.out.println("cannot exec query "+query+":\n"+e);
        }
        return rs;
    }

    public PreparedStatement prepareStatement(String statement)
    {
        PreparedStatement ps=null;
        try
        {
            ps=dbConn.prepareStatement(statement);
        }
        catch(Exception e)
        {
            System.out.println("cannot prepare statement "+statement+":\n"+e);
        }
        return ps;
    }
    
    public void execUpdate(String query)
    {
        try
        {
            dbStmt.execute(query);
        }
        catch(Exception e)
        {
            System.out.println("cannot exec update "+query+":\n"+e);
        }
    }

    public Statement createPrivateStatement()
    {
        Statement s=null;
        try
        {
            s=dbConn.createStatement();
        }
        catch(Exception e)
        {
            System.out.println(e);
        }
        return s;
    }
}
