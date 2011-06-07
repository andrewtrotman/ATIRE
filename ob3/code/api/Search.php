<?php
/**
 * Atire search API
 * 
 * @author Yang LIU
 */

define('ATIRE_SEARCH_HOST_AND_PORT', 'localhost:8088');

class OB3_API_Search {

	/**
	 * @remotable
	 *
	 * params {
	 * 		terms: Required
	 * 		start: The start of the result list. Counting from 1.
	 * 		limit: The number of result to be returned.
	 * }
	 */
	public function doSearch($terms,$start=1,$limit=10) {
			
			//We try to connect to Atire Search.				
			$atire = new atire_api_remote();
			if (!$atire->open(ATIRE_SEARCH_HOST_AND_PORT)) {
				return new Exception('Can not connect to atire');
			}
			
			//Constructs a search query. It contains words seperated by a space and the last word is followed by no space.
			$query = '';
			foreach ($terms as $term){
				$query .= $term . ' ';
			}
			$query = rtrim($query);
			
			

			//We send the query to atire and parse the results returned by Atire. $resourceIDs is an array of IDs.
			require_once("../ob3/code/core/model/core/Searchable.php");
			$resourceIDs = search_parse_search_result($atire->search($query,$start,$limit));
			$atire->close();

			//If our result list is not empty, we want to record the query for auto completion purpose.
			if(!empty($resourceIDs)) OB3_SearchHistory::increase_term_occurrence($query);

			try{
				$resluts = array();
				$resluts['values'] = array();
	
				//We go through the result list and fetch each DataComponent in the result list from database.  			
				foreach ($resourceIDs as $id) {
						
					$tmp = OB3_DataComponent::get_by_id($id);
					//FIXME: What we will do if we could not find the datacomponent?  Searchable and DataComponent tables are in inconsistent state? Is there any way to reconstruct Searchable table?
					
					$documentID = $tmp->getNode()->getDocument()->ID;
					$attributes = $tmp->getAttributes();
					
					//We also want to set up the documentID, so we can retrieve the document in the browser.
					$attributes['values'][] = array('field'=> 'DocumentID', 'value' => $documentID);
		
					$resluts['values'][] = $attributes;
				}
				
				return $resluts;
			}
			catch(Exception $e){
				$atire->close();
				throw new Exception($e->getMessage());
			}
	}

	/**
	 * @remotable
	 *
	 * params {
	 * 		query: Required
	 * 		start: Required, passed by Ext.ComboBox. It starts from 0.  
	 * 		limit: Required, it is the page size. 
	 * }
	 *
	 */
	public function getSuggestions($query, $start, $limit) {

		$tmp = OB3_SearchHistory::lookup($query,$start,$limit);

		if (!$tmp) return $reslut = array('count'=> 0);

		$results['count'] = $tmp->TotalItems();

		$tmp = $tmp->toArray();
		$count = 0;
		
		foreach ($tmp as $item){
				$results['values'][]= array('ID'=>$count++, 'term' => $item->Term);
		}
		
		return $results;
	}
}