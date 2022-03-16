<?php
/**
 * User registration page
 *
 * @package    HNG2
 * @subpackage accounts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * @var module $current_module
 */

use hng2_base\account;
use hng2_base\accounts_repository;
use hng2_base\module;

include "../config.php";
include "../includes/bootstrap.inc";

$repository = new accounts_repository();

try
{
    check_sql_injection($_POST);
}
catch(\Exception $e)
{
    throw_fake_501();
}

$errors = array();
if( $_POST["mode"] == "create" )
{
    # This keeps the whole posted data prepped for output on the form
    $xaccount = new account();
    $xaccount->assign_from_posted_form();
    $config->globals["accounts:processing_account"] = $xaccount;
    $current_module->load_extensions("registration", "after_init");
    
    # Validations: missing fields
    foreach( array("display_name", "email", "password", "password2") as $field )
        if( trim(stripslashes($_POST[$field])) == "" ) $errors[] = $current_module->language->errors->registration->missing->{$field};
    
    $user_name = trim(stripslashes($_POST["user_name"]));
    $country   = trim(stripslashes($_POST["country"]));
    
    if( $settings->get("modules:accounts.automatic_user_names") == "true" )
    {
        $count = $repository->get_record_count(array("display_name" => addslashes($xaccount->display_name)));
        if( $count > 0 )
            $errors[] = $current_module->language->errors->registration->display_name_taken;
    }
    else
    {
        if( empty($user_name) )
            $errors[] = $current_module->language->errors->registration->missing->user_name;
        
        if( preg_match('/[^a-z0-9\-_\.]/i', $user_name) )
            $errors[] = $current_module->language->errors->registration->invalid->chars_in_user_name;
        
        if( strlen($user_name) < 3 )
            $errors[] = $current_module->language->errors->registration->invalid->user_name_length;
        
        if( is_numeric($user_name) )
            $errors[] = $current_module->language->errors->registration->invalid->numeric_user_name;
        
        if( ! preg_match('/[a-z0-9]/i', $user_name) )
            $errors[] = $current_module->language->errors->registration->invalid->not_only_special_symbols;
        
        if( strlen($xaccount->display_name) < 3 )
            $errors[] = $current_module->language->errors->registration->invalid->display_name_length;
        
        if( is_numeric($xaccount->display_name) )
            $errors[] = $current_module->language->errors->registration->invalid->numeric_display_name;
        
        if( ! preg_match('/[a-z0-9]/i', $xaccount->display_name) )
            $errors[] = $current_module->language->errors->registration->invalid->not_only_special_symbols2;
    }

    # Check for accounts created from the same IP during the last N hours
    $hours = (int) $settings->get("modules:accounts.registration_from_same_ip_threshold");
    if( $hours > 0 )
    {
        $ip       = get_remote_address();
        $boundary = date("Y-m-d H:i:s", strtotime("now - $hours hours"));
        $filter   = array("(creation_date >= '{$boundary}' and creation_host like '{$ip}%')");
        $count    = $repository->get_record_count($filter);
        if( $count > 0 ) $errors[] = $current_module->language->errors->registration->invalid->created_from_same_ip;
    }
    
    # Blacklist validations
    $blacklist = trim($settings->get("modules:accounts.usernames_blacklist"));
    if( ! empty($blacklist) )
    {
        foreach(explode("\n", $blacklist) as $line)
        {
            $line = trim($line);
            if( empty($line) ) continue;
            if( substr($line, 0, 1) == "#" ) continue;
            
            $pattern = "@^" . str_replace(array("*", "?"), array(".+", ".?"), trim($line)) . "@i";
            if( preg_match($pattern, $user_name) )
            {
                $errors[] = $current_module->language->errors->registration->invalid->user_name_blacklisted;
                
                break;
            }
        }
    }
    $blacklist = trim($settings->get("modules:accounts.displaynames_blacklist"));
    if( ! empty($blacklist) )
    {
        foreach(explode("\n", $blacklist) as $line)
        {
            $line = trim($line);
            if( empty($line) ) continue;
            if( substr($line, 0, 1) == "#" ) continue;
            
            $pattern = "@^" . str_replace(array("*", "?"), array(".+", ".?"), trim($line)) . "@i";
            if( preg_match($pattern, $xaccount->display_name) )
            {
                $errors[] = $current_module->language->errors->registration->invalid->display_name_blacklisted;
                
                break;
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
    
    if( $settings->get("modules:accounts.non_mandatory_country") != "true" && empty($country) )
        $errors[] = $current_module->language->errors->registration->missing->country;
    
    # Validations: invalid entries
    if( ! filter_var(trim(stripslashes($_POST["email"])), FILTER_VALIDATE_EMAIL) )
        $errors[] = $current_module->language->errors->registration->invalid->email;
    
    if( trim(stripslashes($_POST["alt_email"])) != "" )
        if( ! filter_var(trim(stripslashes($_POST["alt_email"])), FILTER_VALIDATE_EMAIL) )
            $errors[] = $current_module->language->errors->registration->invalid->alt_email;
    
    if( trim(stripslashes($_POST["alt_email"])) != "" )
        if( trim(stripslashes($_POST["email"])) == trim(stripslashes($_POST["alt_email"])) )
            $errors[] = $current_module->language->errors->registration->invalid->mails_must_be_different;
    
    if( trim(stripslashes($_POST["password"])) != trim(stripslashes($_POST["password2"])) )
        $errors[] = $current_module->language->errors->registration->invalid->passwords_mismatch;
    
    # Validations: captcha
    if( $settings->get("engine.recaptcha_private_key") != "" )
    {
        if( ! isset($_POST['g-recaptcha-response']) )
        {
            $errors[] = $current_module->language->errors->registration->invalid->captcha_invalid;
        }
        else
        {
            $cap = trim(stripslashes($_POST['g-recaptcha-response']));
            $ch  = curl_init("https://www.google.com/recaptcha/api/siteverify?secret={$settings->get("engine.recaptcha_private_key")}&response={$cap}");
            curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "GET");
            curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
            $res = curl_exec($ch);
            
            if( curl_error($ch) )
            {
                $errors[] = replace_escaped_objects(
                    $current_module->language->errors->registration->invalid->captcha_api_error,
                    array('{$error}' => curl_error($ch))
                );
            }
            else
            {
                $obj = json_decode($res);
                if( empty($obj) )
                {
                    $errors[] = replace_escaped_objects(
                        $current_module->language->errors->registration->invalid->captcha_api_error,
                        array('{$error}' => print_r($res))
                    );
                }
                else
                {
                    if( ! $obj->success )
                        $errors[] = $current_module->language->errors->registration->invalid->captcha_invalid;
                }
            }
            curl_close($ch);
        }
    }
    
    # Pre-check for double accounts (by display name)
    if( count($errors) == 0 )
    {
        $count = $repository->get_record_count(array("display_name" => addslashes($_POST["display_name"])) );
        if( $count > 0 ) $errors[] = $current_module->language->errors->registration->invalid->display_name_taken;
    }
    
    # User name forging if enabled
    if( count($errors) == 0 )
    {
        if( $settings->get("modules:accounts.automatic_user_names") == "true" )
        {
            if( strlen($xaccount->display_name) < 3 )
                $errors[] = $current_module->language->errors->registration->invalid->display_name_length;
            
            if( is_numeric($xaccount->display_name) )
                $errors[] = $current_module->language->errors->registration->invalid->numeric_display_name;
            
            if( preg_match('/[^a-z0-9 \-_\.]/i', $xaccount->display_name) )
                $errors[] = $current_module->language->errors->registration->invalid->not_only_special_symbols2;
            
            if( count($errors) == 0 )
            {
                $user_name = str_replace("_", "-", wp_sanitize_filename($xaccount->display_name));
                
                if( is_numeric($user_name) )
                    $errors[] = $current_module->language->errors->registration->invalid->numeric_user_name;
                
                if( strlen($user_name) < 3 )
                    $errors[] = $current_module->language->errors->registration->invalid->user_name_length;
                
                if( ! preg_match('/[a-z0-9]/i', $user_name) )
                    $errors[] = $current_module->language->errors->registration->invalid->not_only_special_symbols;
                
                if( ! preg_match('/[a-z0-9 \-_\.]/i', $user_name) )
                    $errors[] = $current_module->language->errors->registration->user_name_cant_be_forged;
            }
            
            if( count($errors) == 0 )
            {
                $count = $repository->get_record_count(array("user_name like '$user_name%'"));
                if( $count == 0 )
                {
                    $xaccount->user_name = $user_name;
                }
                else
                {
                    $user_name .= ($count + 1);
                    $res   = $repository->get($user_name);
                    if( ! is_null($res) )
                        $errors[] = replace_escaped_vars(
                            $current_module->language->errors->registration->similar_account_exists,
                            '{$user_name}',
                            $user_name
                        );
                    else
                        $xaccount->user_name = $user_name;
                }
            }
        }
    }
    
    # Post check for duplicate account
    if( count($errors) == 0 )
    {
        $yaccount = new account($user_name);
        if( $yaccount->_exists ) $errors[] = $current_module->language->errors->registration->invalid->user_name_taken;
    }
    
    $allow_duplicate_emails = $settings->get("modules:accounts.allow_duplicate_emails") == "true";
    
    # Check for existing main email
    if( count($errors) == 0 && ! $allow_duplicate_emails )
    {
        $res = $database->query("
            select * from account 
            where email = '".trim(stripslashes($_POST["email"]))."' 
            or alt_email = '".trim(stripslashes($_POST["email"]))."'
        ");
        if( $database->num_rows($res) > 0 ) $errors[] = $current_module->language->errors->registration->invalid->main_email_exists;
    }
    
    # Check for existing alt email
    if( count($errors) == 0 && trim(stripslashes($_POST["alt_email"])) != "" && ! $allow_duplicate_emails )
    {
        $res = $database->query("
            select * from account 
            where email = '".trim(stripslashes($_POST["alt_email"]))."' 
            or alt_email = '".trim(stripslashes($_POST["alt_email"]))."'
        ");
        if( $database->num_rows($res) > 0 ) $errors[] = $current_module->language->errors->registration->invalid->alt_email_exists;
    }
    
    # Final assignments
    if( $settings->get("modules:accounts.non_mandatory_country") == "true" && empty($xaccount->country) )
        $xaccount->country = get_geoip_country_code();
    
    if( empty($xaccount->country) )
    {
        $country = $settings->get("modules:accounts.default_country");
        if( empty($country) ) $country = "us";
        $xaccount->country = $country;
    }
    
    if( count($errors) == 0 ) $current_module->load_extensions("registration", "before_insertion");
    
    # Proceed to insert the account and notify the user to confirm it
    if( count($errors) == 0 )
    {
        $xaccount->password = md5($xaccount->_raw_password);
        $xaccount->set_new_id();
        $xaccount->save();
        $xaccount->send_new_account_confirmation_email();
        
        $current_module->load_extensions("registration", "after_sending_confirmation_email");
        
        if( ! empty($_REQUEST["redir_url"]) )
        {
            header("Location: " . $_REQUEST["redir_url"]);
            die("<a href='".$_REQUEST["redir_url"]."'>".$current_module->language->click_to_continue."</a>");
        }
        
        $template->set_page_title($current_module->language->page_titles->registration_form_submitted);
        $template->page_contents_include = "register_form_submitted.tpl.inc";
        include "{$template->abspath}/main.php";
        die();
    }
}

# Country list preload
$current_user_country = empty($xaccount->country) ? get_geoip_country_code() : $xaccount->country;
$countries            = array();
$query                = "select * from countries order by name asc";
$res                  = $database->query("select * from countries order by name asc");
while( $row = $database->fetch_object($res) ) $countries[$row->alpha_2] = $row->name;

$_errors               = $errors;
$_country_list         = $countries;
$_current_user_country = strtolower($current_user_country);

$template->set_page_title($current_module->language->page_titles->registration_form);
$template->page_contents_include = "register_form.tpl.inc";
include "{$template->abspath}/main.php";
