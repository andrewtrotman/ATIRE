<?php

/** ExtDirect_Router */
require_once('../ob3/3rdparty/ext-direct/ExtDirect/ExtDirect_Router.php');

/** ExtDirect_API */
require_once('../ob3/3rdparty/ext-direct/ExtDirect/ExtDirect_API.php');

class OB3_API_ExtDirect extends Controller {
	
	public function index() {
		$this->direct();
	}
	
	public function direct($output = true) {
		$api = new ExtDirect_API();
		
		$config = array(
			'id' => 'ob3',
			'type' => 'ob3-remoting',
			'maxRetries' => 0,
			'enableBuffer' => 250
		);
		
		$url = Director::protocolAndHost().Director::baseURL().'api/ext-direct/router';
		
		//enable zend debugger for requests
		if(isset($_GET['start_debug'])) {
			$url .= '?start_debug=1&debug_host='.$_GET['debug_host'].'&debug_port='.$_GET['debug_port'].'&debug_stop=1';
			if(isset($_COOKIE['start_profile'])) $url .= '&start_profile=1';
			$config['timeout'] = 24*60*60*1000; //really long timeout limit
		}
		
		$api->setRouterUrl($url);
		
		$api->setNamespace('OB3.API');
		$api->setDescriptor('OB3.API.discriptor');
		
		$api->setConfig($config);
		
		$api->add(array(
			'Auth' => array('prefix' => 'OB3_API_'),
			'File' => array('prefix' => 'OB3_API_'),
			'DocumentTree' => array('prefix' => 'OB3_API_'),
			'DataComponent' => array('prefix' => 'OB3_API_'),
			'Search' => array('prefix' => 'OB3_API_')
		));
		
		if($output) $api->output();
		
		Session::set('Ext.Direct.state', $api->getState());
	}
	
	public function router($http) {
		if(!Session::get('Ext.Direct.state')) {
			$this->direct(false);
		}

		if($this->auth($http->getBody())) {
			if(isset($_GET['ajaxUpload'])) {
				$tmpfile = tempnam(sys_get_temp_dir(), uniqid());
				$input = fopen('php://input', 'r');
				$temp = fopen($tmpfile, 'w');
				$size = stream_copy_to_stream($input, $temp);
				fclose($input);
				fclose($temp);
				
				$_FILES[] = array(
					'name' => $_GET['fileName'],
					'type' => $_GET['fileType'],
					'tmp_name' => $tmpfile,
					'error' => 0,
					'size' => $size
				);
			}

			$api = new ExtDirect_API();
			$api->setState(Session::get('Ext.Direct.state'));
			  
			$router = new ExtDirect_Router($api);
			$router->dispatch();
			$router->getResponse(true); // true to print the response instantly
		}
		else {
			header('HTTP/1.1 401 Unauthorized');
			header('Content-Type: text/plain');
			echo 'Unauthorized'; exit();
		}
	}
	
	public function auth($http) {
		if(($member = Member::currentUser())) {
			return true;
		}
		else {
			if(($http = json_decode($http))) {
				if(isset($http->action) && $http->action === 'Auth' && isset($http->method) && $http->method === 'login') {
					return true;
				}
			}
		}

		return false;
	}
}