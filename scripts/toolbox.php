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
 * @var module            $current_module
 * 
 * $_GET params:
 * @param mode
 */

use hng2_base\account;
use hng2_base\accounts_repository;
use hng2_base\config;
use hng2_base\module;
use hng2_base\settings;
use hng2_base\template;

include "../../config.php";
include "../../includes/bootstrap.inc";
if( $account->level < $config::MODERATOR_USER_LEVEL ) throw_fake_404();

header("Content-Type: text/plain; charset=utf-8");
switch( $_REQUEST["mode"] )
{
    case "enable_register":
    {
        $settings->set("modules:accounts.register_enabled", "true");
        send_notification(
            $account->id_account,
            "information",
            $current_module->language->admin->record_nav->action_messages->registering_enabled
        );
        
        die("OK");
        break;
    }
    case "disable_register":
    {
        $settings->set("modules:accounts.register_enabled", "false");
        send_notification(
            $account->id_account,
            "information",
            $current_module->language->admin->record_nav->action_messages->registering_disabled
        );
        
        die("OK");
        break;
    }
    case "enable":
    {
        $user_account = new account($_REQUEST["id_account"]);
        if( ! $user_account->_exists )
            die( $current_module->language->admin->record_nav->action_messages->target_not_exists );
    
        if( $account->id_account == $user_account->id_account )
            die( $current_module->language->admin->record_nav->action_messages->no_self_enable_disable );
        
        $user_account->enable();
        $current_module->load_extensions("toolbox", "enable_account");
        
        if( $user_account->level < config::NEWCOMER_USER_LEVEL ) $user_account->set_level(config::NEWCOMER_USER_LEVEL);
        
        send_notification(
            $account->id_account,
            "information",
            $current_module->language->admin->record_nav->action_messages->enabled_ok
        );
        
        die("OK");
        break;
    }
    case "disable":
    {
        $user_account = new account($_REQUEST["id_account"]);
        if( ! $user_account->_exists )
            die( $current_module->language->admin->record_nav->action_messages->target_not_exists );
        
        if( $account->id_account == $user_account->id_account )
            die( $current_module->language->admin->record_nav->action_messages->no_self_enable_disable );
        
        $user_account->disable();
        $current_module->load_extensions("toolbox", "disable_account");
        
        send_notification(
            $account->id_account,
            "information",
            $current_module->language->admin->record_nav->action_messages->disabled_ok
        );
        
        die("OK");
        break;
    }
    case "change_level":
    {
        # if( $account->level < $config::MODERATOR_USER_LEVEL )
        #     die( $current_module->language->admin->record_nav->action_messages->level_change_denied );
        
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
        
        $current_module->load_extensions("toolbox", "before_level_change");
        $previous_level = $user_account->level;
        $user_account->set_level( $_GET["level"] );
        if( $previous_level != $_GET["level"] ) $current_module->load_extensions("toolbox", "account_level_changed");
        
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
    case "delete":
    {
        $repository   = new accounts_repository();
        $user_account = $repository->get($_GET["id_account"]);
        
        if( ! $user_account->_exists )
            die( $current_module->language->admin->record_nav->action_messages->target_not_exists );
        
        if( $account->id_account == $user_account->id_account )
            die( $current_module->language->admin->record_nav->action_messages->no_self_delete );
        
        if( $account->level < $user_account->level )
            die( $current_module->language->admin->record_nav->action_messages->deletion_above_level_denied );
        
        if( $_GET["id_account"] == "100000000000000" )
            die( $current_module->language->admin->record_nav->action_messages->system_account_cannot_be_deleted );
        
        $current_module->load_extensions("toolbox", "before_delete");
        $config->globals["deletions_log"] = array();
        $config->globals["notify_deletion_progress"] = false;
        //send_notification($account->id_account, "information", $current_module->language->admin->record_nav->action_messages->deletion_progress->start);
        //sleep(1);
        $repository->delete($_GET["id_account"]);
        
        if( ! empty($config->globals["deletions_log"]) )
        {
            $ops_log = implode("</li>\n<li>", $config->globals["deletions_log"]);
            $subject = "[{$settings->get("engine.website_name")}] Account #{$user_account->id_account} deletion operations report";
            $body    = unindent("
                <p><b>Account details:</b></p>
                <ul>
                    <li><b>Display name:</b> {$user_account->display_name}</li>
                    <li><b>Main email:</b> {$user_account->email}</li>
                    <li><b>Created on:</b> {$user_account->creation_date}</li>
                    <li><b>Issued by:</b> {$account->display_name}</li>
                </ul>
                
                <p><b>Operations log:</b></p>
                <ul>
                    <li>$ops_log</li>
                </ul>
            ");
            
            # @file_put_contents("/tmp/bc_user_deletion_log.html", $body);
            broadcast_mail_between_levels( $config::ADMIN_USER_LEVEL, $config::ADMIN_USER_LEVEL, $subject, $body );
        }
        send_notification($account->id_account, "information", $current_module->language->admin->record_nav->action_messages->deletion_progress->end);
        
        die("OK");
        break;
    }
}

echo $current_module->language->admin->record_nav->action_messages->invalid_mode_specified;
