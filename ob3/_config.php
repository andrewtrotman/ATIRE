<?php
/**
 * Main config file
 * 
 * @copyright OceanBrowser Ltd 2010, All Rights Reserved
 * @package ob3
 * @subpackage none
 */

global $project;
$project = 'ob3';

global $databaseConfig;
$databaseConfig = array(
        "type" => "MySQLDatabase",
        "server" => "localhost",
        "username" => "root",
        "password" => "xagU4ece",
        "database" => "ob3",
);

Director::set_environment_type('dev');

Security::setDefaultAdmin('admin', 'password');

/**
 * Extensions
 */
Object::add_extension('Member', 'OB3_Member');

/**
 * OB3 Director Rules
 */
Director::addRules(100, array('browser/$Action' => 'OB3_Browser'));
Director::addRules(100, array('api/ext-direct/$Action' => 'OB3_API_ExtDirect'));
