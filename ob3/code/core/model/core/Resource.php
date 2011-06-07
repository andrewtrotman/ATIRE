<?php
/**
 * OB3 Resource
 * 
 * Provides a building block for all OB3 Resource types
 * 
 * @copyright GHC Holdings Ltd 2010, All Rights Reserved
 * @package ob3
 * @subpackage none
 */

//define('ATIRE_SEARCH_HOST_AND_PORT', 'localhost:8088');
 
class OB3_Resource extends OB3_DataComponent {
	
		
	public static $readable = array(
		'values' => array(
			'Type' => 'Text'
		)
	);
	
	public static $writeable = array(
		'NodeID' => 'Int',
	);
	
	public static $searchable = false;
	
	public function getNode() {
		if(($Node = DataObject::get_one('OB3_DocumentNode', 'ResourceID = '.$this->ID))) {
			return $Node;
		}
	}
	
	public function getSearchBody() {}
	
	public function buildBody($body, &$xml) {
		if(is_array($body)) {
			$body['tag'] = isset($body['tag']) ? $body['tag'] : 'span';
			
			if(isset($body['children'])) {
				foreach($body['children'] as $child) {
					$this->buildBody($child, $xml->{$body['tag']});
				}
			}
			else $xml->{$body['tag']}[] = isset($body['text']) ? $body['text'] : null;
		}
	}
	
	//write xml for search engines
	public function writeSearchable() {
		if(($Node = $this->getNode())) {
			$Document = $Node->getDocument();
			
			$xml = new SimpleXMLElement('<root></root>');
			$xml->resource['id'] = $this->ID;
			$xml->resource['type'] = $this->getType();
			$xml->document['id'] = $Document->ID;
			
			$this->buildBody($this->getSearchBody(), $xml->body);
			
			if(($Searchable = DataObject::get_by_id('OB3_Searchable', $this->ID)) == false) {
				$Searchable = new OB3_Searchable();
				$Searchable->ID = $this->ID;
			}
			
			if($Node->Deleted || $Document->Deleted) {
				$Searchable->Delete;
			}
			else {
				$Searchable->XML = $xml->asXML();
				$Searchable->write();
			}
		}	
	}
	
	public function afterWrite() {
		if(isset($this->NodeID)) {
			if(($Node = OB3_DocumentNode::get_by_id($this->NodeID))) {
				$Node->ResourceID = $this->ID;
				$Node->write();	
			}
		}
		
		if(Object::combined_static($this->ClassName, 'searchable')) $this->writeSearchable();
		
		//Tell Atire search to construct index. 
		require_once('../ob3/code/core/model/core/Searchable.php');
		ob3_search_index('/tmp/atire/ob3',ATIRE_SEARCH_HOST_AND_PORT);
	}
	
}