<?php
/**
 * User account confirmation page
 *
 * @package    HNG2
 * @subpackage accounts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 */

use hng2_base\account;
use hng2_base\config;
use hng2_base\device;

include "../config.php";
include "../includes/bootstrap.inc";

$id_account = "";
$limit      = "0000-00-00 00:00:00";
$errors     = array();

if( trim($_REQUEST["token"]) == "" )
    $errors[] = $current_module->language->errors->confirmation->missing_token;

if( count($errors) == 0 )
{
    $token = decrypt( trim(stripslashes($_REQUEST["token"])), $config->encryption_key );
    
    list($id_account, $limit) = explode("\t", $token);
    if( preg_match('/[0-9]{4}\-[0-9]{2}\-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}/', $limit) == 0 )
        $errors[] = $current_module->language->errors->confirmation->invalid_token;
}

if( count($errors) == 0 )
{
    if( date("Y-m-d H:i:s") > $limit ) $errors[] = $current_module->language->errors->confirmation->expired_token;
}

if( ! is_numeric($id_account) ) $errors[] = $current_module->language->errors->confirmation->invalid_token;

if( count($errors) == 0 )
{
    $xaccount = new account($id_account);
    $current_module->load_extensions("registration", "before_state_check");
}

if( count($errors) == 0 )
{
    if($xaccount->state == "enabled")
    {
        header("Location: {$config->full_root_url}");
        
        die("<a href='{$config->full_root_url}'>{$current_module->language->click_to_continue}</a>");
    }
    
    if($xaccount->state == "disabled")
    {
        $errors[] = $current_module->language->errors->account_disabled;
    }
}

if( count($errors) == 0 )
{
    # Let's activate the account
    $xaccount->activate(config::NEWCOMER_USER_LEVEL);
    
    # Let's register this device
    $device = new device($id_account);
    $device->set_new($xaccount);
    $device->state        = "enabled";
    $device->device_label = "Primary device";
    $device->save();
}

$current_module->load_extensions("registration", "after_confirmation");

$_errors = $errors;
$template->set_page_title($current_module->language->page_titles->account_confirmation);
$template->page_contents_include = "confirm_account.tpl.inc";
include "{$template->abspath}/main.php";
