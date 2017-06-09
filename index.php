<?php
/**
 * Accounts module admin index - db navigator
 *
 * @package    HNG2
 * @subpackage accounts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *             
 * @var account           $account
 * @var template          $template
 * @var settings          $settings
 * @var \SimpleXMLElement $language
 */

use hng2_base\account;
use hng2_base\accounts_repository;
use hng2_base\config;
use hng2_base\settings;
use hng2_base\template;

include "../config.php";
include "../includes/bootstrap.inc";
include "../includes/guncs.php";
if( ! $account->has_admin_rights_to_module("accounts") ) throw_fake_404();

$repository = new accounts_repository();

$template->page_contents_include = "contents/index.nav.inc";

$messages = $errors = array();
switch( $_REQUEST["mode"] )
{
    case "edit":
        
        $xaccount      = new account($_REQUEST["id_account"]);
        $_country_list = array();
        
        if( ! $xaccount->_exists )
        {
            $errors[] = $current_module->language->admin->record_nav->action_messages->target_not_exists;
            $template->set_page_title($current_module->language->admin->record_nav->page_title);
        }
        else
        {
            $res = $database->query("select * from countries order by name asc");
            while( $row = $database->fetch_object($res) ) $_country_list[$row->alpha_2] = $row->name;
            
            $_form_title = replace_escaped_vars(
                $current_module->language->edit_account_form->alt_title,
                '{$name}',
                $xaccount->user_name
            );
            
            $_include_account_id    = true;
            $_current_user_country  = $xaccount->country;
            $_cancelation_redirect  = $_SERVER["PHP_SELF"] . "?wasuuup=" . md5(mt_rand(1, 65535));
            $_submit_button_caption = $language->save;
            $template->page_contents_include  = "contents/edit_account_form.tpl.inc";
            $template->set_page_title($current_module->language->admin->edit_account->page_title);
        }
        
        include "{$template->abspath}/admin.php";
        break;
    
    case "save":
        
        if( empty($_POST["id_account"]) )
        {
            $errors[] = $current_module->language->edit_account_form->no_account_id_specified;
            $template->set_page_title($current_module->language->admin->record_nav->page_title);
            include "{$template->abspath}/admin.php";
            break;
        }
        
        $xaccount = new account($_POST["id_account"]);
        if( ! $xaccount->_exists )
        {
            $errors[] = $current_module->language->edit_account_form->account_not_found;
            $template->set_page_title($current_module->language->admin->record_nav->page_title);
            include "{$template->abspath}/admin.php";
            break;
        }
        
        $config->globals["accounts:processing_account"] = $xaccount;
        $current_module->load_extensions("account_admin_editor", "after_init");
        
        # This keeps the whole posted data prepped for output on the form
        $user_name = $xaccount->user_name;
        $xaccount->assign_from_posted_form();
        $xaccount->user_name = $user_name;
        
        # Validations: missing fields
        foreach( array("display_name", "country", "email" ) as $field )
            if( trim(stripslashes($_POST[$field])) == "" )
                $errors[] = $current_module->language->errors->registration->missing->{$field};
        
        if( $settings->get("modules:accounts.automatic_user_names") == "true" )
        {
            $count = $repository->get_record_count(array(
                "display_name" => $xaccount->display_name,
                "id_account <> '$xaccount->id_account'"
            ));
            
            if( $count > 0 )
                $errors[] = $current_module->language->errors->registration->display_name_taken;
        }
        
        # Validations: invalid entries
        if( ! filter_var(trim(stripslashes($_POST["email"])), FILTER_VALIDATE_EMAIL) )
            if( ! $account->_is_admin )
                $errors[] = $current_module->language->errors->registration->invalid->email;
        
        if( trim(stripslashes($_POST["alt_email"])) != "" )
            if( ! filter_var(trim(stripslashes($_POST["alt_email"])), FILTER_VALIDATE_EMAIL) )
                if( ! $account->_is_admin )
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
        
        if( $account->level < config::MODERATOR_USER_LEVEL
            && $settings->get("modules:accounts.hide_birthdate_input") != "true" )
        {
            if( empty($_POST["birthdate"]) )
                $errors[] = $current_module->language->errors->registration->invalid->birthdate;
            elseif( ! @checkdate(substr($_POST["birthdate"], 5, 2), substr($_POST["birthdate"], 8, 2), substr($_POST["birthdate"], 0, 4)) )
                $errors[] = $current_module->language->errors->registration->invalid->birthdate;
        }
        
        # Impersonation tries
        if(
            trim(stripslashes($_POST["email"])) != $xaccount->email
            && $account->level < config::MODERATOR_USER_LEVEL
        ) {
            $query = "
                select * from account
                where id_account <> '$xaccount->id_account'
                and (
                    email = '".trim(stripslashes($_POST["email"]))."'
                    or
                    alt_email = '".trim(stripslashes($_POST["email"]))."'
                )
            ";
            $res = $database->query($query);
            if( $database->num_rows($res) > 0 ) $errors[] = $current_module->language->errors->registration->invalid->main_email_exists;
        }
        
        if(
            trim(stripslashes($_POST["alt_email"])) != ""
            && $_POST["alt_email"] != $xaccount->alt_email
            && $account->level < config::MODERATOR_USER_LEVEL
        ) {
            $query = "
                select * from account
                where id_account <> '$xaccount->id_account'
                and (
                    email = '".trim(stripslashes($_POST["alt_email"]))."'
                    or
                    alt_email = '".trim(stripslashes($_POST["alt_email"]))."'
                )
            ";
            $res = $database->query($query);
            if( $database->num_rows($res) > 0 ) $errors[] = $current_module->language->errors->registration->invalid->alt_email_exists;
        }
        
        # Actual save
        if( count($errors) == 0 )
        {
            if( trim(stripslashes($_POST["password"])) != "" ) $xaccount->password = md5($xaccount->_raw_password);
            $xaccount->set_avatar_from_post();
            $xaccount->set_banner_from_post();
            $xaccount->save();
            $messages[] = $current_module->language->edit_account_form->saved_ok;
            
            $template->set_page_title($current_module->language->admin->record_nav->page_title);
            include "{$template->abspath}/admin.php";
            break;
        }
        else
        {
            $_country_list = array();
            $res = $database->query("select * from countries order by name asc");
            while( $row = $database->fetch_object($res) ) $_country_list[$row->alpha_2] = $row->name;
            
            $_form_title = replace_escaped_vars(
                $current_module->language->edit_account_form->alt_title,
                '{$name}',
                $xaccount->user_name
            );
            
            $_include_account_id    = true;
            $_current_user_country  = $xaccount->country;
            $_cancelation_redirect  = $_SERVER["PHP_SELF"] . "?wasuuup=" . md5(mt_rand(1, 65535));
            $_submit_button_caption = $language->save;
            $_messages              = $messages;
            $_errors                = $errors;
            
            $template->page_contents_include  = "contents/edit_account_form.tpl.inc";
            $template->set_page_title($current_module->language->admin->edit_account->page_title);
            include "{$template->abspath}/admin.php";
            break;
        }
    
    case "show_creation_form":
        
        $xaccount      = new account();
        $_country_list = array();
        
        $res = $database->query("select * from countries order by name asc");
        while( $row = $database->fetch_object($res) ) $_country_list[$row->alpha_2] = $row->name;
        
        $_form_title = replace_escaped_vars(
            $current_module->language->edit_account_form->alt_title,
            '{$name}',
            $xaccount->user_name
        );
        
        $_include_account_id    = true;
        $_current_user_country  = strtolower(get_geoip_location_data(get_remote_address()));
        $_cancelation_redirect  = $_SERVER["PHP_SELF"] . "?wasuuup=" . md5(mt_rand(1, 65535));
        $_hide_captcha          = true;
        $_form_title            = $current_module->language->register_form->creation;
        $_no_flag_check         = true;
        $_hide_infos            = true;
        
        $template->page_contents_include = "contents/register_form.tpl.inc";
        $template->set_page_title($current_module->language->admin->create_account->page_title);
        include "{$template->abspath}/admin.php";
        break;
    
    case "create":
        
        $xaccount = new account($_POST["user_name"]);
        if( $xaccount->_exists )
        {
            $errors[] = $current_module->language->errors->registration->invalid->user_name_taken;
            $xaccount = new account($_POST["user_name"]);
            $xaccount->assign_from_posted_form();
        }
        
        if( empty($errors) && $settings->get("modules:accounts.automatic_user_names") == "true" ) {
            $count = $repository->get_record_count(array("display_name" => $_POST["display_name"]));
            if( $count > 0 )
                $errors[] = $current_module->language->errors->registration->display_name_taken;
        }
        
        if( empty($errors) )
        {
            $xaccount->assign_from_posted_form();
            
            # Validations: missing fields
            foreach( array("user_name", "display_name", "country", "email" ) as $field )
                if( trim(stripslashes($_POST[$field])) == "" )
                    $errors[] = $current_module->language->errors->registration->missing->{$field};
            
            # Validations: invalid entries
            if( preg_match('/[^a-z0-9\-_]/i', $xaccount->user_name) )
                $errors[] = $current_module->language->errors->registration->invalid->chars_in_user_name;
            
            if( ! filter_var(trim(stripslashes($_POST["email"])), FILTER_VALIDATE_EMAIL) )
                $errors[] = $current_module->language->errors->registration->invalid->email;
            
            if( trim(stripslashes($_POST["alt_email"])) != "" )
                if( ! filter_var(trim(stripslashes($_POST["alt_email"])), FILTER_VALIDATE_EMAIL) )
                    $errors[] = $current_module->language->errors->registration->invalid->alt_email;
            
            if( trim(stripslashes($_POST["alt_email"])) != "" )
                if( trim(stripslashes($_POST["email"])) == trim(stripslashes($_POST["alt_email"])) )
                    $errors[] = $current_module->language->errors->registration->invalid->same_emails;
            
            if( trim(stripslashes($_POST["password"])) == "" )
                $errors[] = $current_module->language->errors->registration->missing->password;
                
            if( trim(stripslashes($_POST["password2"])) == "" )
                $errors[] = $current_module->language->errors->registration->missing->password2;
                
            if( trim(stripslashes($_POST["password"])) != "" && trim(stripslashes($_POST["password2"])) != "" )
                if( trim(stripslashes($_POST["password"])) != trim(stripslashes($_POST["password2"])) )
                    $errors[] = $current_module->language->errors->registration->invalid->passwords_mismatch;
        }
        
        # Actual save
        if( count($errors) == 0 )
        {
            $xaccount->password = md5(trim(stripslashes($_POST["password"])));
            $xaccount->set_new_id();
            $xaccount->save();
            $xaccount->enable();
            $xaccount->set_level(config::NEWCOMER_USER_LEVEL);
            $messages[] = $current_module->language->register_form->account_manually_created;
            
            $template->set_page_title($current_module->language->admin->record_nav->page_title);
            include "{$template->abspath}/admin.php";
            break;
        }
        else
        {
            $_country_list = array();
            
            $res = $database->query("select * from countries order by name asc");
            while( $row = $database->fetch_object($res) ) $_country_list[$row->alpha_2] = $row->name;
            
            $_form_title = replace_escaped_vars(
                $current_module->language->edit_account_form->alt_title,
                '{$name}',
                $xaccount->user_name
            );
            
            $_include_account_id    = true;
            $_current_user_country  = strtolower(get_geoip_location_data(get_remote_address()));
            $_cancelation_redirect  = $_SERVER["PHP_SELF"] . "?wasuuup=" . md5(mt_rand(1, 65535));
            $_hide_captcha          = true;
            $_form_title            = $current_module->language->register_form->creation;
            $_no_flag_check         = true;
            $_hide_infos            = true;
            $_messages              = $messages;
            $_errors                = $errors;
            
            $template->page_contents_include  = "contents/register_form.tpl.inc";
            $template->set_page_title($current_module->language->admin->create_account->page_title);
            include "{$template->abspath}/admin.php";
            break;
        }
    
    default:
    
        $template->set_page_title($current_module->language->admin->record_nav->page_title);
        include "{$template->abspath}/admin.php";
        break;
}
