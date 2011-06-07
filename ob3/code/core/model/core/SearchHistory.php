<?php
/**
 * Atire search query history
 * 
 * @author Yang LIU
 */

class OB3_SearchHistory extends DataObject {

	public static $db = array(
		'Term' => 'Text', //The entire query, this may be multiple words seperated by a space.
		'Occurrence' => 'Int'
	);
	/**
 	 * See if the query has been encountered before. 
	 * 
 	 * @param string $query The query a user has entered upto this point.
	 * @param int $start The starting position of the entire result. It counts from 0.
	 * @param int $limit The page size of the result to be returned.
	 * @return DataObject A set of DataObjects of class OB3_SearchHistory 
	 */
	public static function lookup($query,$start,$limit) {

		$whereExp = '"Term" LIKE '."'".$query."%'";
		$limitExp = $limit . ' '.' OFFSET '. $start;
		$orderbyExp = 'Occurrence DESC';

		return DataObject::get(__CLASS__,$whereExp,$orderbyExp,'',$limitExp);
	}
	/**
 	 * Increase the occurrence of the query.
	 * 
 	 * @param string $query The successful search query to add to the search history.
	 * @return DataObject The newly created or retrieved DataObject of class OB3_SearchHistory
	 */
	public static function increase_term_occurrence($query) {
	 	$obj = DataObject::get_one(__CLASS__, sprintf('"'.__CLASS__.'"."Term" = '."'".'%s'."'", Convert::raw2sql($query)),false);
	 	if(!$obj){
	 		$c = __CLASS__;
	 		$obj = new $c(array('Term' => $query, 'Occurrence' => 0));
	 	}
	 	$obj->Occurrence +=1;
	 	$obj->write();
	 	return $obj;
	}
}