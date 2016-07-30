<?php
/**
 * Accounts module admin toolbox
 *
 * @package    HNG2
 * @subpackage accounts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @var account           $account
 * @var template          $template
 * @var settings          $settings
 * @var \SimpleXMLElement $language
 * 
 * $_GET params:
 * @param mode
 */

use hng2_base\account;
use hng2_base\settings;
use hng2_base\template;

include "../../config.php";
include "../../includes/bootstrap.inc";
if( ! $account->_is_admin ) throw_fake_404();

header("Content-Type: text/plain; charset=utf-8");
switch( $_REQUEST["mode"] )
{
    case "enable_register":
        
        $settings->set("modules:accounts.register_enabled", "true");
        send_notification(
            $account->id_account,
            "information",
            $current_module->language->admin->record_nav->action_messages->registering_enabled
        );
        
        die("OK");
        break;
    
    case "disable_register":
        
        $settings->set("modules:accounts.register_enabled", "false");
        send_notification(
            $account->id_account,
            "information",
            $current_module->language->admin->record_nav->action_messages->registering_disabled
        );
        
        die("OK");
        break;
    
    case "promote_admin":
        
        $user_account = new account($_REQUEST["id_account"]);
        if( ! $user_account->_exists )
            die($current_module->language->admin->record_nav->action_messages->target_not_exists);
        
        if($account->id_account == $user_account->id_account)
            die($current_module->language->admin->record_nav->action_messages->no_self_promote_demote);
    
        if( $user_account->_is_admin )
            die($current_module->language->admin->record_nav->action_messages->user_is_already_admin);
    
        $user_account->set_admin();
        send_notification(
            $account->id_account,
            "information",
            $current_module->language->admin->record_nav->action_messages->promoted_ok
        );
        
        die("OK");
        break;
    
    case "demote_admin":
        
        $user_account = new account($_REQUEST["id_account"]);
        if( ! $user_account->_exists )
            die($current_module->language->admin->record_nav->action_messages->target_not_exists);
    
        if($account->id_account == $user_account->id_account)
            die( $current_module->language->admin->record_nav->action_messages->no_self_promote_demote );
    
        if( ! $user_account->_is_admin )
            die( $current_module->language->admin->record_nav->action_messages->user_is_not_admin );
        
        $user_account->unset_admin();
        send_notification(
            $account->id_account,
            "information",
            $current_module->language->admin->record_nav->action_messages->demoted_ok
        );
        
        die("OK");
        break;
    
    case "enable":
        
        $user_account = new account($_REQUEST["id_account"]);
        if( ! $user_account->_exists )
            die( $current_module->language->admin->record_nav->action_messages->target_not_exists );
    
        if( $account->id_account == $user_account->id_account )
            die( $current_module->language->admin->record_nav->action_messages->no_self_enable_disable );
    
        $user_account->enable();
        send_notification(
            $account->id_account,
            "information",
            $current_module->language->admin->record_nav->action_messages->enabled_ok
        );
        
        die("OK");
        break;
    
    case "disable":
        
        $user_account = new account($_REQUEST["id_account"]);
        if( ! $user_account->_exists )
            die( $current_module->language->admin->record_nav->action_messages->target_not_exists );
    
        if( $account->id_account == $user_account->id_account )
            die( $current_module->language->admin->record_nav->action_messages->no_self_enable_disable );
    
        $user_account->disable();
        send_notification(
            $account->id_account,
            "information",
            $current_module->language->admin->record_nav->action_messages->enabled_ok
        );
        
        die("OK");
        break;
}

echo $current_module->language->admin->record_nav->action_messages->invalid_mode_specified;
