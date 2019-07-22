<?php
/**
 * User account editor
 *
 * @package    HNG2
 * @subpackage accounts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *             
 * @var template $template
 * @var account  $account
 */

use hng2_base\account;
use hng2_base\accounts_repository;
use hng2_base\config;
use hng2_base\template;

include "../config.php";
include "../includes/bootstrap.inc";
include "../includes/guncs.php";

if( ! $account->_exists ) throw_fake_401();
$origin_account = clone $account;
$xaccount       = clone $account;
$repository     = new accounts_repository();

$errors = $messages = array();
if( $_POST["mode"] == "save" )
{
    # This keeps the whole posted data prepped for output on the form
    $user_name = $account->user_name;
    $xaccount->assign_from_posted_form();
    $xaccount->user_name = $user_name;
    $config->globals["accounts:processing_account"] = $xaccount;
    $current_module->load_extensions("profile_editor", "after_init");
    
    # Validations: missing fields
    foreach( array("display_name", "country", "email" ) as $field )
        if( trim(stripslashes($_POST[$field])) == "" ) $errors[] = $current_module->language->errors->registration->missing->{$field};
    
    # Blacklist validations
    if( $account->level < config::MODERATOR_USER_LEVEL )
    {
        if( $account->display_name != $xaccount->display_name )
        {
            $blacklist = trim($settings->get("modules:accounts.displaynames_blacklist"));
            if( ! empty($blacklist) )
            {
                foreach(explode("\n", $blacklist) as $line)
                {
                    $pattern = "@^" . str_replace(array("*", "?"), array(".+", ".?"), trim($line)) . "@i";
                    if( preg_match($pattern, $xaccount->display_name) )
                    {
                        $errors[] = $current_module->language->errors->registration->invalid->display_name_blacklisted;
                        
                        break;
                    }
                }
            }
        }
        
        $blacklist = trim($settings->get("modules:accounts.email_domains_blacklist"));
        if( ! empty($blacklist) )
        {
            $main_domain = end(explode("@", $xaccount->email));
            $alt_domain  = end(explode("@", $xaccount->alt_email));
            foreach(explode("\n", $blacklist) as $line)
            {
                $line = trim($line);
                if( empty($line) ) continue;
                if( substr($line, 0, 1) == "#" ) continue;
            
                if( $line == $main_domain )
                {
                    $errors[] = $current_module->language->errors->registration->invalid->mail_domain;
                
                    break;
                }
            
                if( $line == $alt_domain )
                {
                    $errors[] = $current_module->language->errors->registration->invalid->alt_mail_domain;
                
                    break;
                }
            }
        }
    }
    
    if(
        $settings->get("modules:accounts.automatic_user_names") == "true"
        && $origin_account->display_name != $xaccount->display_name
    ) {
        $count = $repository->get_record_count(array(
            "display_name" => $xaccount->display_name,
            "id_account <> '$xaccount->id_account'"
        ));
        
        if( $count > 0 )
            $errors[] = $current_module->language->errors->registration->display_name_taken;
    }
    
    # Validations: invalid entries
    if( ! filter_var(trim(stripslashes($_POST["email"])), FILTER_VALIDATE_EMAIL) )
        $errors[] = $current_module->language->errors->registration->invalid->email;
    
    if( trim(stripslashes($_POST["alt_email"])) != "" )
        if( ! filter_var(trim(stripslashes($_POST["alt_email"])), FILTER_VALIDATE_EMAIL) )
            $errors[] = $current_module->language->errors->registration->invalid->alt_email;
    
    if( trim(stripslashes($_POST["alt_email"])) != "" )
        if( trim(stripslashes($_POST["email"])) == trim(stripslashes($_POST["alt_email"])) )
            $errors[] = $current_module->language->errors->registration->invalid->same_emails;
    
    if( trim(stripslashes($_POST["password"])) != "" && trim(stripslashes($_POST["password2"])) == "" )
        $errors[] = $current_module->language->errors->registration->invalid->passwords_mismatch;
    
    if( trim(stripslashes($_POST["password"])) == "" && trim(stripslashes($_POST["password2"])) != "" )
        $errors[] = $current_module->language->errors->registration->invalid->passwords_mismatch;
    
    if( trim(stripslashes($_POST["password"])) != "" && trim(stripslashes($_POST["password2"])) != "" )
        if( trim(stripslashes($_POST["password"])) != trim(stripslashes($_POST["password2"])) )
            $errors[] = $current_module->language->errors->registration->invalid->passwords_mismatch;
    
    if( $settings->get("modules:accounts.hide_birthdate_input") != "true" )
    {
        if( empty($_POST["birthdate"]) )
            $errors[] = $current_module->language->errors->registration->invalid->birthdate;
        elseif( ! @checkdate(substr($_POST["birthdate"], 5, 2), substr($_POST["birthdate"], 8, 2), substr($_POST["birthdate"], 0, 4)) )
            $errors[] = $current_module->language->errors->registration->invalid->birthdate;
    }
    
    # Impersonation tries
    if( $_POST["email"] != $origin_account->email && $xaccount->level < config::MODERATOR_USER_LEVEL )
    {
        $res = $database->query("select * from account where id_account <> '$xaccount->id_account' and (email = '".trim(stripslashes($_POST["email"]))."' or alt_email = '".trim(stripslashes($_POST["email"]))."')");
        if( $database->num_rows($res) > 0 ) $errors[] = $current_module->language->errors->registration->invalid->main_email_exists;
    }
    
    if( $_POST["alt_email"] != "" && $_POST["alt_email"] != $origin_account->alt_email && $xaccount->level < config::MODERATOR_USER_LEVEL )
    {
        $query = "select * from account where id_account <> '$xaccount->id_account' and (email = '".trim(stripslashes($_POST["alt_email"]))."' or alt_email = '".trim(stripslashes($_POST["alt_email"]))."')";
        $res = $database->query($query);
        if( $database->num_rows($res) > 0 ) $errors[] = $current_module->language->errors->registration->invalid->alt_email_exists;
    }
    
    if( count($errors) == 0 )
    {
        if( trim(stripslashes($_POST["password"])) != "" ) $xaccount->password = md5($xaccount->_raw_password);
        $xaccount->set_avatar_from_post();
        $xaccount->set_banner_from_post();
    }
    
    if( count($errors) == 0 ) $current_module->load_extensions("profile_editor", "before_saving");
    
    # Actual save
    if( count($errors) == 0 )
    {
        $xaccount->save();
        
        if( ! empty($_POST["api_keys"]) )
        {
            $api_keys = array();
            foreach($_POST["api_keys"]["enabled"] as $index => $enabled )
            {
                $app_name = stripslashes($_POST["api_keys"]["app_name"][$index]);
                if( empty($app_name) ) $app_name = trim($current_module->language->api_keys->unnamed);
                
                $secret_key = trim(stripslashes($_POST["api_keys"]["secret_key"][$index]));
                $secret_key = three_layer_encrypt($secret_key, $config->encryption_key, $account->id_account, $account->creation_date);
                $api_keys[] = array(
                    "enabled"     => $enabled,
                    "app_name"    => $app_name,
                    "public_key"  => $_POST["api_keys"]["public_key"][$index],
                    "secret_key"  => $secret_key,
                );
            }
        }
        $xaccount->set_engine_pref("api_keys", $api_keys);
        if( ! empty($_POST["deleted_api_keys"]) )
            foreach(explode(",", $_POST["deleted_api_keys"]) as $deleted_key)
                $xaccount->set_engine_pref("api_access.last:{$deleted_key}", "");
        
        $current_module->load_extensions("profile_editor", "after_saving");
        
        $messages[] = $current_module->language->edit_account_form->saved_ok;
        $account = $xaccount;
    }
}

# Country list preload
$current_user_country = $xaccount->country;
$countries            = array();
$res                  = $database->query("select * from countries order by name asc");
while( $row = $database->fetch_object($res) ) $countries[$row->alpha_2] = $row->name;

$_errors               = $errors;
$_messages             = $messages;
$_country_list         = $countries;
$_current_user_country = strtolower($current_user_country);

$template->set_page_title($current_module->language->page_titles->edit_account);
$template->set("page_tag", "user_home");
$template->page_contents_include = "edit_account_form.tpl.inc";
include "{$template->abspath}/main.php";
