<?php
/**
 * Account engine pref saver
 *
 * @package    HNG2
 * @subpackage accounts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @param string "id_account"
 * @param string "key"
 * @param string "value"
 * 
 * @return string "OK" on success | error message
 */

use hng2_base\account;

include "../../config.php";
include "../../includes/bootstrap.inc";

$_current_page_requires_login = true;
if( ! $account->_exists ) die("OK");

$key = trim(stripslashes($_REQUEST["key"]));
if( empty($key) ) die($current_module->language->errors->prefs_setting->empty_key);

$val = trim(stripslashes($_REQUEST["value"]));

if( isset($_REQUEST["id_account"]) && $_REQUEST["id_account"] != $account->id_account )
{
    if( ! $account->_is_admin )
        die( $language->errors->access_denied );
    
    $xaccount = new account($_REQUEST["id_account"]);
    if( ! $xaccount->_exists ) die($current_module->language->admin->record_nav->action_messages->target_not_exists);
    
    $xaccount->set_engine_pref($key, $val);
    die("OK");
}

$account->set_engine_pref($key, $val);
echo "OK";
