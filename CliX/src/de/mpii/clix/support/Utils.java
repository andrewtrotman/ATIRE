package de.mpii.clix.support;

import java.util.Calendar;
import java.util.TimeZone;

public class Utils
{
    static public String getCurrentDate()
    {
        Calendar cal = Calendar.getInstance(TimeZone.getDefault());
        
        String DATE_FORMAT = "dd.MM.yyyy HH:mm:ss";
        java.text.SimpleDateFormat sdf = 
              new java.text.SimpleDateFormat(DATE_FORMAT);

        sdf.setTimeZone(TimeZone.getDefault());          
              
        return sdf.format(cal.getTime());
    }
}
