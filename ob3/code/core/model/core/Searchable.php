<?php
/**
 * Atire search history
 * 
 * @author Yang LIU
 */

define('ATIRE_INDEXER_PATH', '/home/yann/src/ant/main/bin/index');

class OB3_Searchable extends DataObject {

	public static $db = array(
		'XML' => 'Text',
	);
	public static $has_one = array(
		'Document' => 'OB3_Document',
	
	);
	
	/**
	* Before writing into database, we find out what is the id of the document 
	* that the resource this searchable represents belongs to.
	*
	* @return void
	*/
	public function onBeforeWrite(){
			parent::onBeforeWrite();
			if($this->DocumentID == 0){
				$this->DocumentID = OB3_DataComponent::get_by_id($this->ID)->getNode()->getDocument()->ID;
			}

	}
	/**
	* Setter for Document to Searchable relationship
	*
	* @return void
	*/
	public function setDocument($id) {
		if(is_numeric($id)) {
			$this->setField('DocumentID', $id);
		}
		else throw new Exception('Expected a number');
	}
}

/**
 ******************************************************************************* 
 *
 * Below, there are some functions that are used to index and parses the result 
 * return by the search engine. 
 * 
 ******************************************************************************* 
 */

/**
 * Tell the Atire search host to construct the OB3 Searchable index at given location.
 *
 * @param string $filename_prefix The path to store the search index and a prefix
 *                                to the index filename.
 * @param string $atire_hostname The ip address and port number of the host Atire
 *                               search is listening on.
 * @return boolean False if failed to construct index. 
 */
function ob3_search_index($filename_prefix,$atire_hostname){
	$query = 'SELECT ID,XML FROM OB3_Searchable';
	$database = 'ob3';
	return search_index_reindex ($query,$database,$filename_prefix,$atire_hostname);
}

/**
 * Tell Atire to construct index or reconstruct and switch.
 * 
 * @param string $query The sql query to perform to retrieve the data. First column
 *                      of the select query is the ID that Atire returns as search 
 *                      results.
 * @param string $database The name of the database Atire talking to.
 * @param string $filename_prefix The path to store the search index and a prefix
 *                                to the index filename.
 * @param string $atire_hostname The ip address and port number of the host Atire
 *                               search is listening on.
 * @return boolean False if failed to construct index.
 */
function search_index_reindex($query, $database, $filename_prefix, $atire_hostname){
	$atire = new atire_api_remote();

	//Check which index is currently being served.
	if ($atire->open($atire_hostname)) {
		$result = $atire->describe_index();

		/* We are holding the connection open to Atire, so the filename of the index
		 * that's currently in use will remain true until we disconnect (no concurrent
		 * index switches).
		 */
		$doclist_filename = $index_filename = '';
		search_index_validate_filenames($filename_prefix, $doclist_filename, $index_filename);

		$index = simplexml_load_string($result);

		if ($index && $index->doclist['filename']==$doclist_filename) {
			//We're serving copy #1, write to copy #2
			$filename_prefix .= '.2';

			search_index_validate_filenames($filename_prefix, $doclist_filename, $index_filename);
		} else {
			//We'll just try to write on any of the indexes then.
			//Fall through.
		}

		/* Still holding Atire open. We SHOULD be disconnecting from Atire after
		 * the indexer locks the index successfully (so that other clients can
		 * continue to use the Atire index while it is being reindexed). TODO!
		 */
		if (!search_index_construct_internal($query, $database, $doclist_filename, $index_filename)) {
			$atire->close();

			return false;
		}

		$result = $atire->load_index($doclist_filename, $index_filename);

		$atire->close();

		return $result;
	} else {
		/* Can't connect to Atire? Just try to blindly write on the index and don't tell
		 * atire about the new version. Decidedly sub-optimal, of course.
		 */
		search_index_construct($query, $database, $filename_prefix);

		return false;
	}
}

/**
 * Helper function to validate if the index file we construct is accessible.
 * 
 * @param string $filename_prefix The path to store the search index and a prefix
 *                                to the index filename.
 * @param string $doclist_filename The document list filename with the path to be returned.
 * @param string  $index_filename The index filename with the path to be returned.
 * 
 */
function search_index_validate_filenames($filename_prefix, &$doclist_filename, &$index_filename) {
	$destdir = realpath(dirname($filename_prefix));

	if (!is_dir($destdir)) {
		die('Bad destination.');
	}

	//Filter out boneheads
	if (strpos($destdir, realpath($_SERVER['DOCUMENT_ROOT']))!==false) {
		die('Did you really want to save that to the webroot??');
	}


	$doclist_filename = $destdir . DIRECTORY_SEPARATOR . basename($filename_prefix.'.doclist.aspt');
	$index_filename = $destdir . DIRECTORY_SEPARATOR . basename($filename_prefix.'.index.aspt');
}

/**
 * Perform filename validation before calling search_index_validate_filenames.
 * 
 * @param string $query The sql query to perform to retrieve the data. First column
 *                      of the select query is the ID that Atire returns as search 
 *                      results.
 * @param string $database The name of the database Atire talking to.
 * @param string $filename_prefix The path to store the search index and a prefix
 *                                to the index filename.
 * @return boolean False if failed to construct index.
 * 
 */
function search_index_construct($query, $database, $filename_prefix) {
	$doclist_filename = $index_filename = '';

	search_index_validate_filenames($filename_prefix, $doclist_filename, $index_filename);

	return search_index_construct_internal($query, $database, $doclist_filename, $index_filename);
}

/**
 * Call Atire search's index excutable.
 * 
 * @param string $query The sql query to perform to retrieve the data. First column
 *                      of the select query is the ID that Atire returns as search 
 *                      results.
 * @param string $database The name of the database Atire talking to.
 * @param string $doclist_filename The document list filename including path. 
 * @param string $index_filename  The index filename including path.
 * @return boolean False if failed to construct index.
 */
function search_index_construct_internal($query, $database, $doclist_filename, $index_filename) {
	assert($query && $database && $doclist_filename && $index_filename);

	if (!file_exists(ATIRE_INDEXER_PATH)) {
		die('Missing Atire indexer');
	}

	$command = ATIRE_INDEXER_PATH
			.' -nologo'
			.' -fdoclist '.escapeshellarg($doclist_filename)
			.' -findex '.escapeshellarg($index_filename)
			.' -rmysql '.escapeshellarg('root')
				.' '.escapeshellarg('xagU4ece')
				.' '.escapeshellarg('localhost')
				.' '.escapeshellarg($database)
				.' '.escapeshellarg($query);

	$retval = -1;

	$result = system($command, $retval);

	return $result!==FALSE && $retval==0;
}

/**
 * Helper function that parses result that Atire search returns.
 * 
 * @param string $msg The results in xml Atire search returns. 
 * @return array The array of IDs. The ID is the first column of the result for the query we give to Atire. For example, select id, some_column, other_column for Searchable_table.
 */
function search_parse_search_result($msg) {
	$result = @simplexml_load_string($msg);

	if (!isset($result->hits))
		return array();

	$docids = array();

	foreach ($result->hits->hit as $hit) {
    	$docids[] = (int) $hit->name;
    }

    return $docids;
}