<?php
/**
 * Accounts module admin toolbox
 *
 * @package    HNG2
 * @subpackage accounts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @var config            $config
 * @var account           $account
 * @var template          $template
 * @var settings          $settings
 * @var \SimpleXMLElement $language
 * 
 * $_GET params:
 * @param mode
 */

use hng2_base\account;
use hng2_base\config;
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
    
    case "enable":
        
        $user_account = new account($_REQUEST["id_account"]);
        if( ! $user_account->_exists )
            die( $current_module->language->admin->record_nav->action_messages->target_not_exists );
    
        if( $account->id_account == $user_account->id_account )
            die( $current_module->language->admin->record_nav->action_messages->no_self_enable_disable );
    
        $user_account->enable();
        if( $user_account->level < config::NEWCOMER_USER_LEVEL ) $user_account->set_level(config::NEWCOMER_USER_LEVEL);
        
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
    
    case "change_level":
        
        if( $account->level < $config::MODERATOR_USER_LEVEL )
            die( $current_module->language->admin->record_nav->action_messages->level_change_denied );
        
        $user_account = new account($_REQUEST["id_account"]);
        
        if( ! $user_account->_exists )
            die( $current_module->language->admin->record_nav->action_messages->target_not_exists );
        
        if( $account->id_account == $user_account->id_account )
            die( $current_module->language->admin->record_nav->action_messages->no_self_level_change );
        
        if( $user_account->level > $account->level )
            die( $current_module->language->admin->record_nav->action_messages->no_upper_level_allowed );
        
        if( ! is_numeric($_GET["level"]) )
            die( $current_module->language->admin->record_nav->action_messages->invalid_level_specified );
        
        if( ! isset($config->user_levels_by_level[$_GET["level"]]))
            die( $current_module->language->admin->record_nav->action_messages->invalid_level_specified );
        
        $user_account->set_level( $_GET["level"] );
        send_notification(
            $account->id_account,
            "information",
            replace_escaped_vars(
                $current_module->language->admin->record_nav->action_messages->level_change_ok,
                array('{$user}', '{$level}', '{$level_name}'),
                array($user_account->display_name, $user_account->level, $config->user_levels_by_level[$user_account->level])
            )
        );
        
        die("OK");
        break;
}

echo $current_module->language->admin->record_nav->action_messages->invalid_mode_specified;
